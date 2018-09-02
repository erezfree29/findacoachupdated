/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2011-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#include <Core/ApplicationPool/Group.h>

/*************************************************************************
 *
 * Miscellaneous for ApplicationPool2::Group
 *
 *************************************************************************/

namespace Passenger {
namespace ApplicationPool2 {

using namespace std;
using namespace boost;


/****************************
 *
 * Public methods
 *
 ****************************/


void
Group::cleanupSpawner(boost::container::vector<Callback> &postLockActions) {
	assert(isAlive());
	postLockActions.push_back(boost::bind(doCleanupSpawner, spawner));
}

bool
Group::authorizeByUid(uid_t uid) const {
	return uid == 0 || SpawningKit::prepareUserSwitching(options).uid == uid;
}

bool
Group::authorizeByApiKey(const ApiKey &key) const {
	return key.isSuper() || key == getApiKey();
}


} // namespace ApplicationPool2
} // namespace Passenger
