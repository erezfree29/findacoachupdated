/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2010-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#include "CachedFileStat.h"
#include "CachedFileStat.hpp"

extern "C" {

PP_CachedFileStat *
pp_cached_file_stat_new(unsigned int max_size) {
	return (PP_CachedFileStat *) new Passenger::CachedFileStat(max_size);
}

void
pp_cached_file_stat_free(PP_CachedFileStat *cstat) {
	delete (Passenger::CachedFileStat *) cstat;
}

int
pp_cached_file_stat_perform(PP_CachedFileStat *cstat,
                            const char *filename,
                            struct stat *buf,
                            unsigned int throttle_rate)
{
	try {
		return ((Passenger::CachedFileStat *) cstat)->stat(filename, buf, throttle_rate);
	} catch (const Passenger::TimeRetrievalException &e) {
		errno = e.code();
		return -1;
	} catch (const boost::thread_interrupted &) {
		errno = EINTR;
		return -1;
	}
}

} // extern "C"
