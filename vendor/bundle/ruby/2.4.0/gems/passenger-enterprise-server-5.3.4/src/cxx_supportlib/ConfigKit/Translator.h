/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#ifndef _PASSENGER_CONFIG_KIT_TRANSLATOR_H_
#define _PASSENGER_CONFIG_KIT_TRANSLATOR_H_

#include <boost/bind.hpp>
#include <string>
#include <vector>
#include <ConfigKit/Common.h>
#include <StaticString.h>
#include <jsoncpp/json.h>

namespace Passenger {
namespace ConfigKit {

using namespace std;


/**
 * An abstract base class for all translators.
 *
 * You can learn more about translators in the ConfigKit README, section
 * "The special problem of overlapping configuration names and translation".
 */
class Translator {
private:
	string translateErrorKey(const StaticString &key) const {
		return "{{" + translateOne(key) + "}}";
	}

	string reverseTranslateErrorKey(const StaticString &key) const {
		return "{{" + reverseTranslateOne(key) + "}}";
	}

public:
	virtual ~Translator() { }

	virtual Json::Value translate(const Json::Value &doc) const {
		Json::Value result(Json::objectValue);
		Json::Value::const_iterator it, end = doc.end();

		for (it = doc.begin(); it != end; it++) {
			const char *keyEnd;
			const char *key = it.memberName(&keyEnd);
			result[translateOne(StaticString(key, keyEnd - key))] = *it;
		}

		return result;
	}

	virtual Json::Value reverseTranslate(const Json::Value &doc) const {
		Json::Value result(Json::objectValue);
		Json::Value::const_iterator it, end = doc.end();

		for (it = doc.begin(); it != end; it++) {
			const char *keyEnd;
			const char *key = it.memberName(&keyEnd);
			result[reverseTranslateOne(StaticString(key, keyEnd - key))] = *it;
		}

		return result;
	}

	virtual vector<Error> translate(const vector<Error> &errors) const {
		vector<Error> result;
		vector<Error>::const_iterator it, end = errors.end();
		Error::KeyProcessor keyProcessor =
			boost::bind(&Translator::translateErrorKey, this,
				boost::placeholders::_1);

		for (it = errors.begin(); it != end; it++) {
			const Error &error = *it;
			result.push_back(Error(error.getMessage(keyProcessor)));
		}

		return result;
	}

	virtual vector<Error> reverseTranslate(const vector<Error> &errors) const {
		vector<Error> result;
		vector<Error>::const_iterator it, end = errors.end();
		Error::KeyProcessor keyProcessor =
			boost::bind(&Translator::reverseTranslateErrorKey, this,
				boost::placeholders::_1);

		for (it = errors.begin(); it != end; it++) {
			const Error &error = *it;
			result.push_back(Error(error.getMessage(keyProcessor)));
		}

		return result;
	}

	virtual string translateOne(const StaticString &key) const = 0;
	virtual string reverseTranslateOne(const StaticString &key) const = 0;
};


} // namespace ConfigKit
} // namespace Passenger

#endif /* _PASSENGER_CONFIG_KIT_TRANSLATOR_H_ */
