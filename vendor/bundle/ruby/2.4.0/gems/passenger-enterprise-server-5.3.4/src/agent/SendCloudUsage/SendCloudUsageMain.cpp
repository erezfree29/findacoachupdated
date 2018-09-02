/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2014-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */

#include <curl/curl.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>
#include <oxt/initialize.hpp>
#include <oxt/system_calls.hpp>
#include <LoggingKit/LoggingKit.h>
#include <LoggingKit/Context.h>
#include <Constants.h>
#include <Exceptions.h>
#include <Core/CloudUsageTracker.h>
#include <Utils.h>
#include <Utils/License.h>

using namespace std;
using namespace Passenger;

namespace {
	struct Options {
		string passengerRoot;
		string datadir;
		string url;
		bool sslCertCheck;
		bool dumpMachineProperties;
		bool verbose;
		bool help;

		Options()
			: sslCertCheck(true),
			  dumpMachineProperties(false),
			  verbose(false),
			  help(false)
			{ }
	};
}

static void usage();


static bool
isFlag(const char *arg, char shortFlagName, const char *longFlagName) {
	return strcmp(arg, longFlagName) == 0
		|| (shortFlagName != '\0' && arg[0] == '-'
			&& arg[1] == shortFlagName && arg[2] == '\0');
}

static bool
isValueFlag(int argc, int i, const char *arg, char shortFlagName, const char *longFlagName) {
	if (isFlag(arg, shortFlagName, longFlagName)) {
		if (argc >= i + 2) {
			return true;
		} else {
			fprintf(stderr, "ERROR: extra argument required for %s\n", arg);
			usage();
			exit(1);
			return false; // Never reached
		}
	} else {
		return false;
	}
}

static void
usage() {
	printf("Usage: passenger-config send-cloud-usage [OPTIONS]\n");
	printf("Send cloud usage points to the Phusion Passenger licensing server.\n");
	printf("\n");
	printf("Options:\n");
	printf("      --passenger-root PATH   The location to the " PROGRAM_NAME " source\n");
	printf("                              directory\n");
	printf("      --no-ssl-cert-check     Do not verify SSL certificate\n");
	printf("      --url                   URL to contact. Default:\n");
	printf("                              https://www.phusionpassenger.com/cloud_service/track_usage\n");
	printf("      --dump-machine-properties\n");
	printf("                              Don't send anything. Just dump machine\n");
	printf("                              properties\n");
	printf("  -v, --verbose               Verbose output\n");
	printf("  -h, --help                  Show this help\n");
}

static Options
parseOptions(int argc, char *argv[]) {
	Options options;
	int i = 2;

	while (i < argc) {
		if (isValueFlag(argc, i, argv[i], '\0', "--passenger-root")) {
			options.passengerRoot = argv[i + 1];
			i += 2;
		} else if (isFlag(argv[i], '\0', "--no-ssl-cert-check")) {
			options.sslCertCheck = false;
			i++;
		} else if (isValueFlag(argc, i, argv[i], '\0', "--url")) {
			options.url = argv[i + 1];
			i += 2;
		} else if (isFlag(argv[i], '\0', "--dump-machine-properties")) {
			options.dumpMachineProperties = true;
			i++;
		} else if (isFlag(argv[i], 'v', "--verbose")) {
			options.verbose = true;
			i++;
		} else if (isFlag(argv[i], 'h', "--help")) {
			options.help = true;
			i++;
		} else {
			fprintf(stderr, "ERROR: unrecognized argument %s\n", argv[i]);
			usage();
			exit(1);
		}
	}
	return options;
}

static void
initialize(Options &options) {
	if (options.help) {
		usage();
		exit(0);
	} else if (options.passengerRoot.empty()) {
		fprintf(stderr, "ERROR: please set --passenger-root.\n");
		usage();
		exit(1);
	}

	Json::Value loggingConfig(Json::objectValue);
	if (options.verbose) {
		loggingConfig["level"] = "debug";
	}

	passenger_enterprise_license_init();
	char *error = passenger_enterprise_license_recheck();
	if (error != NULL) {
		string message = error;
		free(error);
		P_CRITICAL(message);
		exit(1);
	}

	CURLcode code = curl_global_init(CURL_GLOBAL_ALL);
	if (code != CURLE_OK) {
		P_CRITICAL("Could not initialize libcurl: " << curl_easy_strerror(code));
		exit(1);
	}

	oxt::initialize();
	setup_syscall_interruption_support();
	LoggingKit::initialize(loggingConfig);
	SystemTime::initialize();
}

static Json::Value
createConfig(const Options &options) {
	Json::Value result;
	if (!options.datadir.empty()) {
		result["data_dir"] = options.datadir;
	}
	if (!options.url.empty()) {
		result["url"] = options.url;
	}
	result["check_certificate"] = options.sslCertCheck;
	return result;
}

static void
licenseErrorHandler(const string &message) {
	P_CRITICAL(message);
	exit(1);
}

int
sendCloudUsageMain(int argc, char *argv[]) {
	CloudUsageTracker::Schema schema;
	Options options = parseOptions(argc, argv);
	initialize(options);

	CloudUsageTracker tracker(schema, createConfig(options));
	if (options.dumpMachineProperties) {
		cout << tracker.dumpMachineProperties().toStyledString();
		return 0;
	} else {
		tracker.licenseErrorHandler = licenseErrorHandler;
		if (tracker.runOneCycle()) {
			P_INFO("Success!");
			return 0;
		} else {
			P_INFO("Failed!");
			return 1;
		}
	}
}
