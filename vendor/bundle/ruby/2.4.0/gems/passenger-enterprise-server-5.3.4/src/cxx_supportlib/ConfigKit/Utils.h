/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#ifndef _PASSENGER_CONFIG_KIT_UTILS_H_
#define _PASSENGER_CONFIG_KIT_UTILS_H_

#include <string>
#include <vector>

#include <ConfigKit/Common.h>
#include <StaticString.h>
#include <DataStructures/StringKeyTable.h>
#include <Utils/FastStringStream.h>

namespace Passenger {
namespace ConfigKit {

using namespace std;

class Error;


inline StaticString
getTypeString(Type type) {
	switch (type) {
	case STRING_TYPE:
		return P_STATIC_STRING("string");
	case INT_TYPE:
		return P_STATIC_STRING("integer");
	case UINT_TYPE:
		return P_STATIC_STRING("unsigned integer");
	case FLOAT_TYPE:
		return P_STATIC_STRING("float");
	case BOOL_TYPE:
		return P_STATIC_STRING("boolean");
	case ARRAY_TYPE:
		return P_STATIC_STRING("array");
	case STRING_ARRAY_TYPE:
		return P_STATIC_STRING("array of strings");
	case OBJECT_TYPE:
		return P_STATIC_STRING("object");
	case ANY_TYPE:
		return P_STATIC_STRING("any");
	default:
		return P_STATIC_STRING("unknown");
	}
}

inline vector<ConfigKit::Error>
deduplicateErrors(const vector<ConfigKit::Error> &errors) {
	StringKeyTable<bool> messagesSeen;
	vector<ConfigKit::Error>::const_iterator it, end = errors.end();
	vector<ConfigKit::Error> result;

	for (it = errors.begin(); it != end; it++) {
		bool *tmp;
		string message = it->getMessage();

		if (!messagesSeen.lookup(message, &tmp)) {
			messagesSeen.insert(message, true);
			result.push_back(*it);
		}
	}

	return result;
}

inline string
toString(const vector<Error> &errors) {
	FastStringStream<> stream;
	vector<Error>::const_iterator it, end = errors.end();

	for (it = errors.begin(); it != end; it++) {
		if (it != errors.begin()) {
			stream << "; ";
		}
		stream << it->getMessage();
	}
	return string(stream.data(), stream.size());
}


} // namespace ConfigKit
} // namespace Passenger

#endif /* _PASSENGER_CONFIG_KIT_UTILS_H_ */
