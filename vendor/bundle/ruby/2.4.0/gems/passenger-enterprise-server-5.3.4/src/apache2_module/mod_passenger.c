/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2010-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */

// This file MUST be named mod_passenger.c so so that
// <IfModule mod_passenger.c> works.

#include <httpd.h>
#include <http_config.h>

#ifdef VISIBILITY_ATTRIBUTE_SUPPORTED
	#define PUBLIC_SYMBOL __attribute__ ((visibility("default")))
#else
	#define PUBLIC_SYMBOL
#endif

extern const command_rec passenger_commands[];

void *passenger_create_dir_config(apr_pool_t *p, char *dirspec);
void *passenger_merge_dir_config(apr_pool_t *p, void *basev, void *addv);
void passenger_register_hooks(apr_pool_t *p);

PUBLIC_SYMBOL module AP_MODULE_DECLARE_DATA passenger_module = {
	STANDARD20_MODULE_STUFF,
	passenger_create_dir_config,  // create per-dir config structs
	passenger_merge_dir_config,   // merge per-dir config structs
	NULL,                         // create per-server config structs
	NULL,                         // merge per-server config structs
	passenger_commands,           // table of config file commands
	passenger_register_hooks      // register hooks
};
