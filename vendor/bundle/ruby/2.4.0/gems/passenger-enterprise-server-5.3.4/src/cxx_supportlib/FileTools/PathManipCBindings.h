/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2018 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#ifndef _PASSENGER_FILE_TOOLS_PATH_MANIP_C_BINDINGS_H_
#define _PASSENGER_FILE_TOOLS_PATH_MANIP_C_BINDINGS_H_

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


char *psg_absolutize_path(const char *path, size_t path_len,
	const char *working_dir, size_t working_dir_len,
	size_t *result_len);
char *psg_resolve_symlink(const char *path, size_t path_len,
	size_t *result_len);
const char *psg_extract_dir_name_static(const char *path,
	size_t path_len, size_t *result_len);


#ifdef __cplusplus
}
#endif

#endif /* _PASSENGER_FILE_TOOLS_PATH_MANIP_C_BINDINGS_H_ */
