/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2013-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */

#include <Exceptions.h>
#include <stdlib.h>
#include <string.h>


void
pp_error_init(PP_Error *error) {
	error->message = NULL;
	error->errnoCode = PP_NO_ERRNO;
	error->messageIsStatic = 0;
}

void
pp_error_destroy(PP_Error *error) {
	if (!error->messageIsStatic) {
		free(static_cast<void *>(const_cast<char *>(error->message)));
		error->message = NULL;
		error->messageIsStatic = 0;
	}
}

void
pp_error_set(const std::exception &ex, PP_Error *error) {
	const Passenger::SystemException *sys_e;

	if (error == NULL) {
		return;
	}

	if (error->message != NULL && !error->messageIsStatic) {
		free(static_cast<void *>(const_cast<char *>(error->message)));
	}

	error->message = strdup(ex.what());
	error->messageIsStatic = error->message == NULL;
	if (error->message == NULL) {
		error->message = "Unknown error message (unable to allocate memory for the message)";
	}

	sys_e = dynamic_cast<const Passenger::SystemException *>(&ex);
	if (sys_e != NULL) {
		error->errnoCode = sys_e->code();
	} else {
		error->errnoCode = PP_NO_ERRNO;
	}
}
