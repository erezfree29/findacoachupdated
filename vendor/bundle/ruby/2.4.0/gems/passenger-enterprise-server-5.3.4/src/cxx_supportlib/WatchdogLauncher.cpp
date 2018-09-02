/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2010-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#include <oxt/thread.hpp>
#include <set>
#include <cerrno>
#include <cstring>
#include <string.h>
#include <WatchdogLauncher.h>
#include <Exceptions.h>

using namespace std;
using namespace boost;
using namespace oxt;

extern "C" {


PsgWatchdogLauncher *
psg_watchdog_launcher_new(PsgIntegrationMode mode, char **error_message) {
	return (PsgWatchdogLauncher *) new Passenger::WatchdogLauncher(mode);
}

int
psg_watchdog_launcher_start(PsgWatchdogLauncher *l,
	const char *passengerRoot,
	PsgJsonValue *extraConfig,
	const PsgAfterForkCallback afterFork,
	void *callbackArgument,
	char **errorMessage)
{
	Passenger::WatchdogLauncher *launcher = (Passenger::WatchdogLauncher *) l;
	boost::this_thread::disable_syscall_interruption dsi;
	try {
		boost::function<void ()> afterForkFunctionObject;

		if (afterFork != NULL) {
			afterForkFunctionObject = boost::bind(afterFork, callbackArgument, extraConfig);
		}
		launcher->start(passengerRoot,
			*static_cast<Passenger::Json::Value *>(extraConfig),
			afterForkFunctionObject);
		return 1;
	} catch (const Passenger::SystemException &e) {
		errno = e.code();
		*errorMessage = strdup(e.what());
		return 0;
	} catch (const std::exception &e) {
		errno = -1;
		*errorMessage = strdup(e.what());
		return 0;
	}
}

const char *
psg_watchdog_launcher_get_core_address(PsgWatchdogLauncher *l, unsigned int *size) {
	Passenger::WatchdogLauncher *launcher = (Passenger::WatchdogLauncher *) l;
	if (size != NULL) {
		*size = launcher->getCoreAddress().size();
	}
	return launcher->getCoreAddress().c_str();
}

const char *
psg_watchdog_launcher_get_core_password(PsgWatchdogLauncher *l, unsigned int *size) {
	Passenger::WatchdogLauncher *launcher = (Passenger::WatchdogLauncher *) l;
	if (size != NULL) {
		*size = launcher->getCorePassword().size();
	}
	return launcher->getCorePassword().c_str();
}

const char *
psg_watchdog_launcher_get_instance_dir(PsgWatchdogLauncher *l, unsigned int *size) {
	Passenger::WatchdogLauncher *launcher = (Passenger::WatchdogLauncher *) l;
	if (size != NULL) {
		*size = launcher->getInstanceDir().size();
	}
	return launcher->getInstanceDir().c_str();
}

pid_t
psg_watchdog_launcher_get_pid(PsgWatchdogLauncher *l) {
	Passenger::WatchdogLauncher *launcher = (Passenger::WatchdogLauncher *) l;
	return launcher->getPid();
}

void
psg_watchdog_launcher_detach(PsgWatchdogLauncher *l) {
	Passenger::WatchdogLauncher *launcher = (Passenger::WatchdogLauncher *) l;
	launcher->detach();
}

void
psg_watchdog_launcher_free(PsgWatchdogLauncher *l) {
	Passenger::WatchdogLauncher *launcher = (Passenger::WatchdogLauncher *) l;
	delete launcher;
}


} // extern "C"
