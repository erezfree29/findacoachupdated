/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2011-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#ifndef _BACKGROUND_EVENT_LOOP_H_
#define _BACKGROUND_EVENT_LOOP_H_

#include <boost/shared_ptr.hpp>
#include <string>
#include <pthread.h>

extern "C" {
	struct ev_loop;
	struct uv_loop_s;
}

namespace Passenger {
	using namespace std;
	using namespace boost;

	class SafeLibev;
	struct BackgroundEventLoopPrivate;

	/**
	 * Implements a libev event loop that runs in a background thread.
	 */
	struct BackgroundEventLoop {
		struct ev_loop *libev_loop;
		struct uv_loop_s *libuv_loop;
		boost::shared_ptr<SafeLibev> safe;
		BackgroundEventLoopPrivate *priv;

		BackgroundEventLoop(bool scalable = false, bool usesLibuv = true);
		~BackgroundEventLoop();

		void start(const string &threadName = "", unsigned int stackSize = 1024 * 1024);
		void stop();
		bool isStarted() const;
		pthread_t getNativeHandle() const;
	};

}

#endif /* _BACKGROUND_EVENT_LOOP_H_ */
