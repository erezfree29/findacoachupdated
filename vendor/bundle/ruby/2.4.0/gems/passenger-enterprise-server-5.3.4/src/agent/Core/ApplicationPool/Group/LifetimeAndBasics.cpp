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
 * Functions for ApplicationPool2::Group for handling life time, basic info,
 * backreferences and related objects
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


// Thread-safe.
bool
Group::isAlive() const {
	return getLifeStatus() == ALIVE;
}

// Thread-safe.
OXT_FORCE_INLINE
Group::LifeStatus
Group::getLifeStatus() const {
	return (LifeStatus) lifeStatus.load(boost::memory_order_seq_cst);
}

StaticString
Group::getName() const {
	return info.name;
}

const BasicGroupInfo &
Group::getInfo() {
	return info;
}

const ApiKey &
Group::getApiKey() const {
	return info.apiKey;
}

/**
 * Thread-safe.
 * @pre getLifeState() != SHUT_DOWN
 * @post result != NULL
 */
OXT_FORCE_INLINE Pool *
Group::getPool() const {
	return pool;
}

Context *
Group::getContext() const {
	return info.context;
}

psg_pool_t *
Group::getPallocPool() const {
	return getPool()->palloc;
}

const ResourceLocator &
Group::getResourceLocator() const {
	return *getPool()->getSpawningKitContext()->resourceLocator;
}


} // namespace ApplicationPool2
} // namespace Passenger
