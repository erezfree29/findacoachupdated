/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#include <Core/Controller.h>

namespace Passenger {
namespace Core {

using namespace std;


/****************************
 *
 * Private methods
 *
 ****************************/


bool
Controller::prepareConfigChange(const Json::Value &updates,
	vector<ConfigKit::Error> &errors,
	ControllerConfigChangeRequest &req)
{
	if (ParentClass::prepareConfigChange(updates, errors, req.forParent)) {
		req.mainConfig.reset(new ControllerMainConfig(
			*req.forParent.forParent.config));
		req.requestConfig.reset(new ControllerRequestConfig(
			*req.forParent.forParent.config));
	}
	return errors.empty();
}

void
Controller::commitConfigChange(ControllerConfigChangeRequest &req)
	BOOST_NOEXCEPT_OR_NOTHROW
{
	ParentClass::commitConfigChange(req.forParent);
	mainConfig.swap(*req.mainConfig);
	requestConfig.swap(req.requestConfig);
}


} // namespace Core
} // namespace Passenger
