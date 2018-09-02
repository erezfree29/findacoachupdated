/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#ifndef _PASSENGER_JSON_TOOLS_AUTOCAST_H_
#define _PASSENGER_JSON_TOOLS_AUTOCAST_H_

#include <boost/regex.hpp>
#include <jsoncpp/json.h>

#include <cstdlib>
#include <string>
#include <StaticString.h>

namespace Passenger {

using namespace std;


inline Json::Value
autocastValueToJson(const StaticString &value) {
	static const boost::regex intRegex("\\A-?[0-9]+\\z");
	static const boost::regex realRegex("\\A-?[0-9]+(\\.[0-9]+)?([eE][+\\-]?[0-9]+)?\\z");
	static const boost::regex boolRegex("\\A(true|false|on|off|yes|no)\\z",
		boost::regex::perl | boost::regex::icase);
	static const boost::regex trueRegex("\\A(true|on|yes)\\z",
		boost::regex::perl | boost::regex::icase);
	const char *valueData = value.data();
	const char *valueEnd = value.data() + value.size();
	boost::cmatch results;

	if (boost::regex_match(valueData, valueEnd, results, intRegex)) {
		#if __cplusplus >= 201103
			return (Json::Int64) atoll(value.toString().c_str());
		#else
			return (Json::Int64) atol(value.toString().c_str());
		#endif
	} else if (boost::regex_match(valueData, valueEnd, results, realRegex)) {
		return atof(value.toString().c_str());
	} else if (boost::regex_match(valueData, valueEnd, results, boolRegex)) {
		return boost::regex_match(valueData, valueEnd, results, trueRegex);
	} else if (value.size() > 0 && (valueData[0] == '{' || valueData[0] == '[')) {
		Json::Reader reader;
		Json::Value jValue;
		if (reader.parse(value, jValue)) {
			return jValue;
		} else {
			return Json::Value(valueData, valueEnd);
		}
	} else {
		return Json::Value(valueData, valueEnd);
	}
}


} // namespace Passenger

#endif /* _PASSENGER_JSON_TOOLS_AUTOCAST_H_ */
