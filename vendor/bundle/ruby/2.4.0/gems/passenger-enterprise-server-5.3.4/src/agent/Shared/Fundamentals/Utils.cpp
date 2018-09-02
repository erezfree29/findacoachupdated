/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2010-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */

#include <Shared/Fundamentals/Utils.h>
#include <oxt/backtrace.hpp>
#include <cstdlib>
#include <cstring>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>

namespace Passenger {
namespace Agent {
namespace Fundamentals {

using namespace std;


const char *
getEnvString(const char *name, const char *defaultValue) {
	const char *value = getenv(name);
	if (value != NULL && *value != '\0') {
		return value;
	} else {
		return defaultValue;
	}
}

bool
getEnvBool(const char *name, bool defaultValue) {
	const char *value = getEnvString(name);
	if (value != NULL) {
		return strcmp(value, "yes") == 0
			|| strcmp(value, "y") == 0
			|| strcmp(value, "1") == 0
			|| strcmp(value, "on") == 0
			|| strcmp(value, "true") == 0;
	} else {
		return defaultValue;
	}
}

void
ignoreSigpipe() {
	struct sigaction action;
	action.sa_handler = SIG_IGN;
	action.sa_flags   = 0;
	sigemptyset(&action.sa_mask);
	sigaction(SIGPIPE, &action, NULL);
}

/**
 * Linux-only way to change OOM killer configuration for
 * current process. Requires root privileges, which we
 * should have.
 *
 * Returns 0 on success, or an errno code on error.
 *
 * This function is async signal-safe.
 */
int
tryRestoreOomScore(const StaticString &scoreString, bool &isLegacy) {
	TRACE_POINT();
	if (scoreString.empty()) {
		return 0;
	}

	int fd, ret, e;
	size_t written = 0;
	StaticString score;

	if (scoreString.at(0) == 'l') {
		isLegacy = true;
		score = scoreString.substr(1);
		fd = open("/proc/self/oom_adj", O_WRONLY | O_TRUNC, 0600);
	} else {
		isLegacy = false;
		score = scoreString;
		fd = open("/proc/self/oom_score_adj", O_WRONLY | O_TRUNC, 0600);
	}

	if (fd == -1) {
		return errno;
	}

	do {
		ret = write(fd, score.data() + written, score.size() - written);
		if (ret != -1) {
			written += ret;
		}
	} while (ret == -1 && errno != EAGAIN && written < score.size());
	e = errno;
	close(fd);
	if (ret == -1) {
		return e;
	} else {
		return 0;
	}
}


} // namespace Fundamentals
} // namespace Agent
} // namespace Passenger
