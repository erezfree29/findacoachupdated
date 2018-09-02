/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#ifndef _PASSENGER_CONFIG_KIT_COMMON_H_
#define _PASSENGER_CONFIG_KIT_COMMON_H_

#include <boost/function.hpp>
#include <string>
#include <vector>

#include <jsoncpp/json.h>

#include <StaticString.h>

namespace Passenger {
namespace ConfigKit {

using namespace std;


class Store;

enum Type {
	STRING_TYPE,
	INT_TYPE,
	UINT_TYPE,
	FLOAT_TYPE,
	BOOL_TYPE,

	ARRAY_TYPE,
	STRING_ARRAY_TYPE,

	OBJECT_TYPE,

	ANY_TYPE,

	UNKNOWN_TYPE
};

enum Flags {
	OPTIONAL = 0,
	REQUIRED = 1 << 0,
	CACHE_DEFAULT_VALUE = 1 << 1,
	READ_ONLY = 1 << 2,
	SECRET = 1 << 3,

	_DYNAMIC_DEFAULT_VALUE = 1 << 30,
	_FROM_SUBSCHEMA = 1 << 31
};

/** Represents a validation error. */
class Error {
private:
	static string dummyKeyProcessor(const StaticString &key) {
		return key.toString();
	}

	string rawMessage;

public:
	typedef boost::function<string (const StaticString &key)> KeyProcessor;

	Error() { }

	Error(const string &_rawMessage)
		: rawMessage(_rawMessage)
		{ }

	string getMessage() const {
		return getMessage(dummyKeyProcessor);
	}

	string getMessage(const KeyProcessor &processor) const {
		string result = rawMessage;
		string::size_type searchBegin = 0;
		bool done = false;

		while (!done) {
			string::size_type pos = result.find("{{", searchBegin);
			if (pos == string::npos) {
				done = true;
				break;
			}

			string::size_type endPos = result.find("}}", pos + 2);
			if (endPos == string::npos) {
				done = true;
				break;
			}

			string key = result.substr(pos + 2, endPos - pos - 2);
			string replacement = processor(key);
			result.replace(pos, endPos - pos + 2, replacement);
			searchBegin = pos + replacement.size();
		}
		return result;
	}

	bool operator<(const Error &other) const {
		return rawMessage < other.rawMessage;
	}
};

typedef boost::function<Json::Value (const Store &store)> ValueGetter;
typedef boost::function<Json::Value (const Json::Value &value)> ValueFilter;


} // namespace ConfigKit
} // namespace Passenger

#endif /* _PASSENGER_CONFIG_KIT_COMMON_H_ */
