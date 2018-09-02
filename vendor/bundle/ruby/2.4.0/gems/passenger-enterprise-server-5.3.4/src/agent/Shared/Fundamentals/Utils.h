/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2010-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#ifndef _PASSENGER_AGENT_FUNDAMENTALS_UTILS_H_
#define _PASSENGER_AGENT_FUNDAMENTALS_UTILS_H_

#include <StaticString.h>

namespace Passenger {
namespace Agent {
namespace Fundamentals {


const char *getEnvString(const char *name, const char *defaultValue = NULL);
bool getEnvBool(const char *name, bool defaultValue = false);
void ignoreSigpipe();
int tryRestoreOomScore(const StaticString &score, bool &isLegacy);


} // namespace Fundamentals
} // namespace Agent
} // namespace Passenger

#endif /* _PASSENGER_AGENT_FUNDAMENTALS_UTILS_H_ */
