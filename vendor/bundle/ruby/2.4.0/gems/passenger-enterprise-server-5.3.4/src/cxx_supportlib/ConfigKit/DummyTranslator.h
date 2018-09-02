/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#ifndef _PASSENGER_CONFIG_KIT_DUMMY_TRANSLATOR_H_
#define _PASSENGER_CONFIG_KIT_DUMMY_TRANSLATOR_H_

#include <vector>
#include <ConfigKit/Translator.h>
#include <StaticString.h>

namespace Passenger {
namespace ConfigKit {

using namespace std;


/**
 * A translator that does nothing.
 *
 * You can learn more about translators in the ConfigKit README, section
 * "The special problem of overlapping configuration names and translation".
 */
class DummyTranslator: public Translator {
public:
	virtual Json::Value translate(const Json::Value &doc) const {
		return doc;
	}

	virtual Json::Value reverseTranslate(const Json::Value &doc) const {
		return doc;
	}

	virtual vector<Error> translate(const vector<Error> &errors) const {
		return errors;
	}

	virtual vector<Error> reverseTranslate(const vector<Error> &errors) const {
		return errors;
	}

	virtual string translateOne(const StaticString &key) const {
		return key;
	}

	virtual string reverseTranslateOne(const StaticString &key) const {
		return key;
	}
};


} // namespace ConfigKit
} // namespace Passenger

#endif /* _PASSENGER_CONFIG_KIT_DUMMY_TRANSLATOR_H_ */
