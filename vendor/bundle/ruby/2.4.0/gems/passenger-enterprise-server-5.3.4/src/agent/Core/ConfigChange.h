/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */

#ifndef _PASSENGER_CORE_CONFIG_CHANGE_H_
#define _PASSENGER_CORE_CONFIG_CHANGE_H_

#include <boost/config.hpp>
#include <boost/function.hpp>
#include <ConfigKit/ConfigKit.h>

namespace Passenger {
namespace Core {

using namespace std;


struct ConfigChangeRequest;
typedef boost::function<void (const vector<ConfigKit::Error> &errors, ConfigChangeRequest *req)> PrepareConfigChangeCallback;
typedef boost::function<void (ConfigChangeRequest *req)> CommitConfigChangeCallback;

ConfigChangeRequest *createConfigChangeRequest();
void freeConfigChangeRequest(ConfigChangeRequest *req);
void asyncPrepareConfigChange(const Json::Value &updates, ConfigChangeRequest *req, const PrepareConfigChangeCallback &callback);
void asyncCommitConfigChange(ConfigChangeRequest *req, const CommitConfigChangeCallback &callback) BOOST_NOEXCEPT_OR_NOTHROW;
Json::Value inspectConfig();

Json::Value manipulateLoggingKitConfig(const ConfigKit::Store &coreConfig,
	const Json::Value &loggingKitConfig);


} // namespace Core
} // namespace Passenger

#endif /* _PASSENGER_CORE_CONFIG_CHANGE_H_ */
