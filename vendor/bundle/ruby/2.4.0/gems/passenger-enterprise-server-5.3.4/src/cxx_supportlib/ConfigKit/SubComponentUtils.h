/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#ifndef _PASSENGER_CONFIG_KIT_SUB_COMPONENT_UTILS_H_
#define _PASSENGER_CONFIG_KIT_SUB_COMPONENT_UTILS_H_

#include <string>
#include <vector>
#include <stdexcept>

#include <jsoncpp/json.h>
#include <ConfigKit/Common.h>
#include <ConfigKit/Translator.h>

namespace Passenger {
namespace ConfigKit {

using namespace std;


template<typename Component>
inline void
prepareConfigChangeForSubComponent(Component &component, const Translator &translator,
	const Json::Value &updates, vector<ConfigKit::Error> &errors,
	typename Component::ConfigChangeRequest &req)
{
	vector<Error> tempErrors;
	component.prepareConfigChange(translator.translate(updates),
		tempErrors, req);
	tempErrors = translator.reverseTranslate(tempErrors);
	errors.insert(errors.end(), tempErrors.begin(), tempErrors.end());
}


} // namespace ConfigKit
} // namespace Passenger

#endif /* _PASSENGER_CONFIG_KIT_SUB_COMPONENT_UTILS_H_ */
