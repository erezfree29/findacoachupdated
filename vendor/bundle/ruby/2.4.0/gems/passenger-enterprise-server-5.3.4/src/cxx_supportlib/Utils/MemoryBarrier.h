/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2010-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#ifndef _PASSENGER_MEMORY_BARRIER_H_
#define _PASSENGER_MEMORY_BARRIER_H_

// Memory barrier macros. Also act as compiler barriers.

#if defined(__GNUC__) || defined(__INTEL_COMPILER)
	#if defined(__i386__)
		#if defined(HAS_SSE2) || (defined(HAS_LFENCE) && defined(HAS_SFENCE))
			#define P_READ_BARRIER() \
				do { __asm__ __volatile__ ("lfence" ::: "memory"); } while (false)
			#define P_WRITE_BARRIER() \
				do { __asm__ __volatile__ ("sfence" ::: "memory"); } while (false)
		#else
			#define P_READ_BARRIER() \
				do { __asm__ __volatile__ ("" ::: "memory"); } while (false)
			#define P_WRITE_BARRIER() \
				do { __asm__ __volatile__ ("lock; addl $0,0(%%esp)" ::: "memory"); } while (false)
		#endif

	#elif defined(__x86_64__)
		#define P_READ_BARRIER() \
			do { __asm__ __volatile__ ("lfence" ::: "memory"); } while (false)
		#define P_WRITE_BARRIER() \
			do { __asm__ __volatile__ ("sfence" ::: "memory"); } while (false)
	#endif
#endif

#endif /* _PASSENGER_MEMORY_BARRIER_H_ */
