/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2010-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#ifndef _PASSENGER_CACHED_FILE_STAT_H_
#define _PASSENGER_CACHED_FILE_STAT_H_

#include <sys/stat.h>

#ifdef __cplusplus
extern "C" {
#endif


/** C bindings for Passenger::CachedFileStat. */

typedef void PP_CachedFileStat;

PP_CachedFileStat *pp_cached_file_stat_new(unsigned int max_size);
void pp_cached_file_stat_free(PP_CachedFileStat *cstat);
int  pp_cached_file_stat_perform(PP_CachedFileStat *cstat,
                                 const char *filename,
                                 struct stat *buf,
                                 unsigned int throttle_rate);


#ifdef __cplusplus
}
#endif

#endif /* _PASSENGER_CACHED_FILE_STAT_H_ */
