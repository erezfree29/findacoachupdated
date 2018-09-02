/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2010-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#ifndef _PASSENGER_APACHE2_MODULE_HOOKS_H_
#define _PASSENGER_APACHE2_MODULE_HOOKS_H_

#include <apr_pools.h>

namespace Passenger {
namespace Apache2Module {


void registerHooks(apr_pool_t *p);


} // namespace Apache2Module
} // namespace Passenger

#endif /* _PASSENGER_APACHE2_MODULE_HOOKS_H_ */
