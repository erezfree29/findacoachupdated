/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2010-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#ifndef _PASSENGER_AGENT_FUNDAMENTALS_INITIALIZATION_H_
#define _PASSENGER_AGENT_FUNDAMENTALS_INITIALIZATION_H_

/** Common initialization code for all agents. */

#include <cstddef>
#include <string>
#include <ConfigKit/Store.h>
#include <Shared/Fundamentals/AbortHandler.h>

namespace Passenger {
namespace Agent {
namespace Fundamentals {

using namespace std;


struct Context {
	ResourceLocator *resourceLocator;
	unsigned int randomSeed;
	int origArgc;
	char **origArgv;
	bool feedbackFdAvailable;
	AbortHandlerConfig abortHandlerConfig;
};

typedef void (*OptionParserFunc)(int argc, const char **argv, ConfigKit::Store &config);
typedef void (*LoggingKitPreInitFunc)(Json::Value &config);

extern Context *context;


void initializeAgent(int argc, char **argv[], const char *processName,
	ConfigKit::Store &config, const ConfigKit::Translator &loggingKitTranslator,
	OptionParserFunc optionParser = NULL,
	const LoggingKitPreInitFunc &loggingKitPreInitFunc = NULL,
	int argStartIndex = 1);
void shutdownAgent(ConfigKit::Schema *schema, ConfigKit::Store *config);

bool feedbackFdAvailable();
void restoreOomScore(const string &score);


} // namespace Fundamentals
} // namespace Agent
} // namespace Passenger

#endif /* _PASSENGER_AGENT_FUNDAMENTALS_INITIALIZATION_H_ */
