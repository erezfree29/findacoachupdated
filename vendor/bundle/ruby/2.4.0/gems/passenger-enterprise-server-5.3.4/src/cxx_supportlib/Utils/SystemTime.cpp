/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2010-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */

#include <Utils/SystemTime.h>

namespace Passenger {
	namespace SystemTimeData {
		bool initialized = false;
		bool hasForcedValue = false;
		time_t forcedValue = 0;
		bool hasForcedUsecValue = false;
		unsigned long long forcedUsecValue = 0;

		#if BOOST_OS_MACOS
			mach_timebase_info_data_t timeInfo;
		#elif defined(SYSTEM_TIME_HAVE_MONOTONIC_CLOCK)
			#ifdef SYSTEM_TIME_HAVE_CLOCK_MONOTONIC_COARSE
				unsigned long long monotonicCoarseResolutionNs = 0;
			#endif
			#ifdef SYSTEM_TIME_HAVE_CLOCK_MONOTONIC_FAST
				unsigned long long monotonicFastResolutionNs = 0;
			#endif
			unsigned long long monotonicResolutionNs;
		#endif
	}
}
