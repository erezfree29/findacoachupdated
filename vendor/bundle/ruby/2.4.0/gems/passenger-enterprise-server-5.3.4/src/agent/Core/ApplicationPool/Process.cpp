/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2011-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */

#include <Core/ApplicationPool/Process.h>
#include <Core/ApplicationPool/Group.h>

namespace Passenger {
namespace ApplicationPool2 {

using namespace std;
using namespace boost;


string
Process::getAppGroupName(const BasicGroupInfo *info) const {
	if (info->group != NULL) {
		return info->group->options.getAppGroupName().toString();
	} else {
		return string();
	}
}

string
Process::getAppLogFile(const BasicGroupInfo *info) const {
	if (info->group != NULL) {
		return info->group->options.appLogFile.toString();
	} else {
		return string();
	}
}


} // namespace ApplicationPool2
} // namespace Passenger
