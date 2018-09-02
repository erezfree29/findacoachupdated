/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#ifndef _PASSENGER_APACHE2_MODULE_CONFIG_GENERAL_SETTER_FUNCS_H_
#define _PASSENGER_APACHE2_MODULE_CONFIG_GENERAL_SETTER_FUNCS_H_

#include <limits>
#include <cstdlib>
#include <http_config.h>

namespace Passenger {
namespace Apache2Module {

using namespace std;


inline const char *
setIntConfig(cmd_parms *cmd, const char *rawValue, int &parsedValue,
	int minValue = std::numeric_limits<int>::min())
{
	char *end;
	long result;

	result = strtol(rawValue, &end, 10);
	if (*end != '\0') {
		return apr_psprintf(cmd->temp_pool, "Invalid number specified for %s.",
			cmd->directive->directive);
	}

	if (minValue != std::numeric_limits<int>::min() && result < (long) minValue) {
		return apr_psprintf(cmd->temp_pool, "%s must be at least %d.",
			cmd->directive->directive, minValue);
	}

	parsedValue = (int) result;
	return NULL;
}


} // namespace Apache2Module
} // namespace Passenger

#endif /* _PASSENGER_APACHE2_MODULE_CONFIG_GENERAL_SETTER_FUNCS_H_ */
