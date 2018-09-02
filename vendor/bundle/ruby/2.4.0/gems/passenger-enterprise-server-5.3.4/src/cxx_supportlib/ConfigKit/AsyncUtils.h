/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#ifndef _PASSENGER_CONFIG_KIT_ASYNC_UTILS_H_
#define _PASSENGER_CONFIG_KIT_ASYNC_UTILS_H_

#include <vector>
#include <boost/function.hpp>
#include <ConfigKit/Common.h>

namespace Passenger {
namespace ConfigKit {

using namespace std;


template<typename Component>
struct CallbackTypes {
	typedef
		boost::function<void (const vector<Error> &errors, typename Component::ConfigChangeRequest &req)>
		PrepareConfigChange;
	typedef
		boost::function<void (typename Component::ConfigChangeRequest &req)>
		CommitConfigChange;
	typedef
		boost::function<void (const Json::Value &config)>
		InspectConfig;
};


template<typename Component>
inline void
callPrepareConfigChangeAndCallback(Component *component, Json::Value updates,
	typename Component::ConfigChangeRequest *req,
	const typename CallbackTypes<Component>::PrepareConfigChange &callback)
{
	vector<Error> errors;
	component->prepareConfigChange(updates, errors, *req);
	callback(errors, *req);
}

template<typename Component>
inline void
callCommitConfigChangeAndCallback(Component *component,
	typename Component::ConfigChangeRequest *req,
	const typename CallbackTypes<Component>::CommitConfigChange &callback)
{
	component->commitConfigChange(*req);
	callback(*req);
}

template<typename Component>
inline void
callInspectConfigAndCallback(Component *component,
	const typename CallbackTypes<Component>::InspectConfig &callback)
{
	callback(component->inspectConfig());
}


} // namespace ConfigKit
} // namespace Passenger

#endif /* _PASSENGER_CONFIG_KIT_ASYNC_UTILS_H_ */
