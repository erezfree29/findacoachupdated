/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */

#include "Hooks.h"
#include "Config.h"
#include <httpd.h>
#include <http_config.h>


using namespace Passenger;


extern "C" void *
passenger_create_dir_config(apr_pool_t *p, char *dirspec) {
	return Apache2Module::createDirConfig(p, dirspec);
}

extern "C" void *
passenger_merge_dir_config(apr_pool_t *p, void *basev, void *addv) {
	return Apache2Module::mergeDirConfig(p, basev, addv);
}

extern "C" void
passenger_register_hooks(apr_pool_t *p) {
	Apache2Module::registerHooks(p);
}
