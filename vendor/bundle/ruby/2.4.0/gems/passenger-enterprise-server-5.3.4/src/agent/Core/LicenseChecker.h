/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2011-2016 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#ifndef _PASSENGER_LICENSE_CHECKER_H_
#define _PASSENGER_LICENSE_CHECKER_H_

#include <string>
#include <oxt/thread.hpp>
#include <oxt/backtrace.hpp>

namespace Passenger {

using namespace std;
using namespace oxt;

/**
 * When start()ed, this class verifies the license. If the license has an expiration
 * date, then it also starts a thread to periodically re-check it (the entire license)
 * and notify through a callback if no longer valid.
 */
class LicenseChecker {

private:
	oxt::thread *validityCheckThread;
	long checkIntervalSec;
	void (*licenseInvalidCallback)(const string &reason);

public:

	LicenseChecker() {
		licenseInvalidCallback = NULL;
		validityCheckThread = NULL;
		checkIntervalSec = 0;
	}

	/**
	 * throws RuntimeException with an appropriate error message if the license is not valid.
	 *
	 * If (and only if) the license has an expiration date, it also starts a thread that
	 * periodically re-checks, notifying the callback if any of the checks fail. If
	 * notification occurs, the check thread automatically stops checking (because
	 * the re-check frequency is probably too slow for the user to wait for anyway).
	 */
	void start(long checkIntervalSec, void (*licenseInvalidCallback)(const string &reason)) {
		this->checkIntervalSec = checkIntervalSec;
		this->licenseInvalidCallback = licenseInvalidCallback;

		passenger_enterprise_license_init();

		char *error = passenger_enterprise_license_recheck();
		if (error != NULL) {
			string message = error;
			free(error);
			throw RuntimeException(message);
		}

		if (passenger_enterprise_license_has_expiration()) {
			assert(licenseInvalidCallback != NULL && checkIntervalSec > 0);
			validityCheckThread = new oxt::thread(
					boost::bind(&LicenseChecker::threadMain, this),
					"License validity checker",
					1024 * 512
				);
		}
	}

	~LicenseChecker() {
		if (validityCheckThread != NULL) {
			validityCheckThread->interrupt_and_join();
			delete validityCheckThread;
			validityCheckThread = NULL;
		}
	}

	void threadMain() {
		TRACE_POINT();
		while (!boost::this_thread::interruption_requested()) {
			UPDATE_TRACE_POINT();
			try {
				char *error = passenger_enterprise_license_recheck();
				if (error != NULL) {
					string reason = error;
					free(error);
					licenseInvalidCallback(reason);
					break;
				}
			} catch (const tracable_exception &e) {
				P_ERROR(e.what() << "\n" << e.backtrace());
				licenseInvalidCallback("License check thread failed.");
				break;
			}
			UPDATE_TRACE_POINT();
			boost::this_thread::sleep_for(boost::chrono::seconds(checkIntervalSec));
		}
	}

};
}

#endif /* _PASSENGER_LICENSE_CHECKER_H_ */
