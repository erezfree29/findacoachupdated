/*
 *  Phusion Passenger - http://www.modrails.com/
 *  Copyright (c) 2012-2016 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "MD5.h"
#include "License.h"

#ifdef __cplusplus
namespace Passenger {
#endif

#define MAX_LICENSE_LINES 30
#define LICENSE_SECRET "An error occurred while fetching this page. Please contact an administrator if this problem persists."
/* N.B. there is a legacy field named "Valid until:" that used to signify the fastspring license expiration and might still be present
 * in old certificates, so "Expires after:" was deliberately chosen to avoid that legacy.
 */
#define EXPIRES_AFTER_KEY "Expires after:"

/* Workaround to shut up compiler warnings about return values */
#define IGNORE_RETVAL(val) \
	do { \
		if (val == 0) { \
			printf("\n"); \
		} \
	} while (0)

char *licenseKey = (char *) 0;
/**
 * 2	Checksum OK, has expiry date
 * 1	Checksum OK, valid forever
 * 0	Not initialized
 * -1	License file not found or openable
 * -2	I/O error while reading
 * -3	Corrupted file
 * -4	Invalid checksum (corrupt or hack)
 * -10	Checksum OK, but expired
 */
int licenseState = 0;

static md5_byte_t
hexNibbleToByte(char hexNibble) {
	if (hexNibble >= '0' && hexNibble <= '9') {
		return hexNibble - '0';
	} else {
		return hexNibble - 'A' + 10;
	}
}

/*
 * Searches licenseKey for EXPIRES_AFTER_KEY and returns date value (yyyy-mm-dd)
 * as new char *, or NULL otherwise.
 */
char *
copyExpiresAfterDate(const char *licenseKey) {
	const char *valueStart = strstr(licenseKey, EXPIRES_AFTER_KEY);
	if (valueStart == NULL) {
		return NULL;
	}

	valueStart += sizeof(EXPIRES_AFTER_KEY);
	while (*valueStart == ' ') {
		valueStart++;
	}

	const char *valueEnd = valueStart;
	while (*valueEnd != '\n') {
		valueEnd++;
	}
	int len = valueEnd - valueStart;
	char *expiresAfter = (char *) malloc(len + 1);
	strncpy(expiresAfter, valueStart, len);
	expiresAfter[len] = '\0';
	return expiresAfter;
}

/*
 * Returns 0 if expired, 1 otherwise
 */
int
compareDates(const char *expiresAfterChars, struct tm checkDate) {
	// A license without Expires after is valid forever.
	if (expiresAfterChars == NULL) {
		return 1;
	}

	// Otherwise, valid until the date specified in the license is in the past
	char checkDateChars[20];
	snprintf(checkDateChars, sizeof(checkDateChars), "%d-%02d-%02d", checkDate.tm_year + 1900, checkDate.tm_mon + 1, checkDate.tm_mday);
	if (strcmp(expiresAfterChars, checkDateChars) >= 0) {
		return 1;
	}

	return 0;
}

void
passenger_enterprise_license_init() {
	/* Because of bugs on certain operating systems, we cannot assume that
	 * `licenseKey` is NULL before initialization. This init function
	 * initializes global variables. See email by Jason Rust,
	 * "Not loading after installing 4.0.7", July 5 2013.
	 */
	licenseKey = NULL;
	licenseState = 0;
}

#ifndef __cplusplus
// Copy of Utils.cpp::getSystemTempDir() to avoid dependency
// when we are included from the Nginx module.
static const char *
getSystemTempDir() {
	const char *temp_dir = getenv("TMPDIR");
	if (temp_dir == NULL || *temp_dir == '\0') {
		temp_dir = "/tmp";
	}
	return temp_dir;
}
#endif

static FILE *
open_license_file() {
	const char *licenseData = getenv("PASSENGER_ENTERPRISE_LICENSE_DATA");
	if (licenseData != NULL && *licenseData != '\0') {
		char path[PATH_MAX];
		snprintf(path, PATH_MAX, "%s/passenger.XXXXXXXX", getSystemTempDir());
		int fd = mkstemp(path);
		FILE *f;

		if (fd == -1) {
			int e = errno;
			fprintf(stderr, "Error: " PROGRAM_NAME " Enterprise license detected "
				"in environment variable PASSENGER_ENTERPRISE_LICENSE_DATA, "
				"but unable to create a temporary file: %s (errno=%d)\n",
				strerror(e), e);
			return NULL;
		}

		unlink(path);
		f = fdopen(fd, "r+");
		if (f != NULL) {
			size_t len = strlen(licenseData);
			size_t ret;

			ret = fwrite(licenseData, 1, len, f);
			IGNORE_RETVAL(ret);
			if (len > 0 && licenseData[len - 1] != '\n') {
				ret = fwrite("\n", 1, 1, f);
				IGNORE_RETVAL(ret);
			}
			ret = fseek(f, 0, SEEK_SET);
			IGNORE_RETVAL(ret);
		}
		return f;
	} else {
		return fopen("/etc/passenger-enterprise-license", "r");
	}
}

/**
 * Convenience method to read the license file contents.
 *
 * Returns contents of the license file in a newly malloc'd char *, or NULL if anything went wrong.
 */
char *blind_load_license_file() {
	FILE *f;
	char line[1024];
	char *lines[MAX_LICENSE_LINES];
	unsigned int count = 0, i;
	size_t len, totalSize = 0;
	char *result = NULL;
	char *walk;

	f = open_license_file();
	if (f == NULL) {
		return NULL;
	}

	while (1) {
		if (fgets(line, sizeof(line), f) == NULL) {
			if (ferror(f)) {
				goto finish;
			} else {
				break;
			}
		}

		len = strlen(line);
		if (len == 0 || line[len - 1] != '\n' || count >= MAX_LICENSE_LINES) {
			goto finish;
		}

		lines[count] = strdup(line);
		count++;
		totalSize += len;
	}

	result = (char *) malloc(totalSize + 1);
	if (result == NULL) {
		goto finish;
	}
	walk = result;
	for (i = 0; i < count; i++) {
		len = strlen(lines[i]);
		memcpy(walk, lines[i], len);
		walk  += len;
	}
	*walk = '\0';

	finish:
	fclose(f);
	for (i = 0; i < count; i++) {
		free(lines[i]);
	}

	return result;
}

char *
passenger_enterprise_license_recheck() {
	FILE *f;
	char *lines[MAX_LICENSE_LINES];
	char line[1024];
	unsigned int count = 0, i;
	char *message = NULL;
	size_t len, totalSize = 0;
	struct md5_state_s md5;
	md5_byte_t digest[MD5_SIZE], readDigest[MD5_SIZE], *readDigestCursor;
	const char *data, *dataEnd;
	char *dataEnd2;
	time_t t = time(NULL);
	struct tm dateTimeNow;
	char *expiresAfter;

	if (licenseKey != NULL) {
		free(licenseKey);
		passenger_enterprise_license_init();
	}

	f = open_license_file();
	if (f == NULL) {
		licenseState = -1;
		return strdup("Could not open the " PROGRAM_NAME " Enterprise license file. "
			"Please check whether it's installed correctly and whether it's world-readable.\n"
			APPEAL_MESSAGE);
	}

	// Read all lines in the license key file and store them in a string array.
	while (1) {
		if (fgets(line, sizeof(line), f) == NULL) {
			if (ferror(f)) {
				licenseState = -2;
				message = strdup(
					"An I/O error occurred while reading the " PROGRAM_NAME " Enterprise license file.\n"
					APPEAL_MESSAGE);
				goto finish;
			} else {
				break;
			}
		}

		len = strlen(line);
		if (len == 0 || line[len - 1] != '\n' || count >= MAX_LICENSE_LINES) {
			licenseState = -3;
			message = strdup("The " PROGRAM_NAME " Enterprise license file appears to be corrupted. Please reinstall it.\n"
				APPEAL_MESSAGE);
			goto finish;
		}

		lines[count] = strdup(line);
		count++;
		totalSize += len;
	}

	if (count == 0) {
		licenseState = -3;
		message = strdup("The " PROGRAM_NAME " Enterprise license file appears to be corrupted. Please reinstall it.\n"
			APPEAL_MESSAGE);
		goto finish;
	}

	// Calculate MD5 of the license key file.
	md5_init(&md5);
	for (i = 0; i < count - 1; i++) {
		md5_append(&md5, (const md5_byte_t *) lines[i], strlen(lines[i]));
	}
	md5_append(&md5, (const md5_byte_t *) LICENSE_SECRET, sizeof(LICENSE_SECRET) - 1);
	md5_finish(&md5, digest);

	// Read the last line of the license key file as binary MD5 data.
	readDigestCursor = readDigest;
	data = lines[count - 1];
	dataEnd = data + strlen(data);
	while (data < dataEnd && *data != '\n' && readDigestCursor < readDigest + MD5_SIZE) {
		while (*data == ' ') {
			data++;
		}
		*readDigestCursor = (hexNibbleToByte(data[0]) << 4) | hexNibbleToByte(data[1]);
		readDigestCursor++;
		data += 2;
	}

	// Validate MD5 checksum.
	if (memcmp(digest, readDigest, MD5_SIZE) != 0) {
		licenseState = -4;
		message = strdup("The " PROGRAM_NAME " Enterprise license file is invalid.\n"
			APPEAL_MESSAGE);
		goto finish;
	}

	// Create null-terminated buffer containing license key file data,
	// excluding last line.
	licenseKey = (char *) malloc(totalSize + 1);
	dataEnd2 = licenseKey;
	for (i = 0; i < count - 1; i++) {
		len = strlen(lines[i]);
		memcpy(dataEnd2, lines[i], len);
		dataEnd2 += len;
	}
	*dataEnd2 = '\0';

	// If there is a validity limit, check it
	expiresAfter = copyExpiresAfterDate(licenseKey);
	if (expiresAfter == NULL) {
		licenseState = 1;
	} else {
		dateTimeNow = *localtime(&t);
		if (compareDates(expiresAfter, dateTimeNow) == 0) {
			licenseState = -10;
			free(licenseKey);
			licenseKey = NULL;
			int messageLen = sizeof("The " PROGRAM_NAME " Enterprise license file is invalid: expired since .\n") + strlen(expiresAfter) + sizeof(EXPIRED_APPEAL_MESSAGE) + 1;
			message = (char *) malloc(messageLen);
			snprintf(message, messageLen, "The " PROGRAM_NAME " Enterprise license file is invalid: expired since %s.\n%s", expiresAfter, EXPIRED_APPEAL_MESSAGE);
		} else {
			licenseState = 2;
		}
		free(expiresAfter);
	}

	finish:
	fclose(f);
	for (i = 0; i < count; i++) {
		free(lines[i]);
	}
	return message;
}

int
passenger_enterprise_license_invalid() {
	return licenseState <= 0;
}

int
passenger_enterprise_license_has_expiration() {
	return licenseState == 2 || licenseState == -10;
}

static int
passenger_enterprise_on_cloud_license() {
	return strstr(licenseKey, "Cloud license") != NULL;
}

int
passenger_enterprise_should_track_usage() {
	// This is a somewhat confusing check: we actually don't really care if someone is on a cloud infra or not,
	// as we would like usage statistics from anyone (who has not opted out). So at the moment, the "cloud" license
	// is used as "OK with usage tracking, if not, can download a different license which doesn't mention cloud to disable tracking"
	// Could be done with a default on / disable option instead, but then existing customers would be affected when they upgrade Passenger.
	return passenger_enterprise_on_cloud_license();
}

#ifdef __cplusplus
} // namespace Passenger
#endif
