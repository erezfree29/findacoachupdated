/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2010-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#ifndef _PASSENGER_OPTION_PARSING_H_
#define _PASSENGER_OPTION_PARSING_H_

#include <cstdio>
#include <cstring>

namespace Passenger {

using namespace std;


class OptionParser {
public:
	typedef void (*UsageFunction)();

private:
	UsageFunction usage;

public:
	OptionParser(UsageFunction _usage)
		: usage(_usage)
		{ }

	bool
	isFlag(const char *arg, char shortFlagName, const char *longFlagName) {
		return strcmp(arg, longFlagName) == 0
			|| (shortFlagName != '\0' && arg[0] == '-'
				&& arg[1] == shortFlagName && arg[2] == '\0');
	}

	inline bool
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
};


} // namespace Passenger

#endif /* _PASSENGER_OPTION_PARSING_H_ */
