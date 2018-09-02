/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2013-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#ifndef _PASSENGER_FILE_TOOLS_LARGE_FILES_H_
#define _PASSENGER_FILE_TOOLS_LARGE_FILES_H_

#include <stdio.h>

/* Allows access to Large File Support APIs, if the OS supports them. */

namespace Passenger {


::FILE *lfs_fopen(const char *filename, const char *mode);
::FILE *lfs_fdopen(int filedes, const char *mode);
int lfs_mkstemp(char *templ);


} // namespace Passenger

#endif /* _PASSENGER_FILE_TOOLS_LARGE_FILES_H_ */
