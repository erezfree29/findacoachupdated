/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2010-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#ifndef _PASSENGER_AGENT_FUNDAMENTALS_ABORT_HANDLER_H_
#define _PASSENGER_AGENT_FUNDAMENTALS_ABORT_HANDLER_H_

namespace Passenger {
	class ResourceLocator;
}

namespace Passenger {
namespace Agent {
namespace Fundamentals {


struct AbortHandlerConfig {
	typedef void (*DiagnosticsDumper)(void *userData);

	char *ruby;
	char **origArgv;
	unsigned int randomSeed;
	bool dumpWithCrashWatch;
	bool beep;
	bool stopProcess;
	ResourceLocator *resourceLocator;
	DiagnosticsDumper diagnosticsDumper;
	void *diagnosticsDumperUserData;
};

void installAbortHandler(const AbortHandlerConfig *config);
bool abortHandlerInstalled();
void abortHandlerLogFds();
void abortHandlerConfigChanged();
void shutdownAbortHandler();


} // namespace Fundamentals
} // namespace Agent
} // namespace Passenger

#endif /* _PASSENGER_AGENT_FUNDAMENTALS_ABORT_HANDLER_H_ */
