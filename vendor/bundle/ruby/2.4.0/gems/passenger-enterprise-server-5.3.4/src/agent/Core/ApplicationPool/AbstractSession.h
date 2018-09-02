/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2016-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#ifndef _PASSENGER_APPLICATION_POOL_ABSTRACT_SESSION_H_
#define _PASSENGER_APPLICATION_POOL_ABSTRACT_SESSION_H_

#include <sys/types.h>
#include <boost/atomic.hpp>
#include <boost/intrusive_ptr.hpp>
#include <StaticString.h>
#include <Shared/ApplicationPoolApiKey.h>

namespace Passenger {
namespace ApplicationPool2 {


/**
 * An abstract base class for Session so that unit tests can work with
 * a mocked version of it.
 */
class AbstractSession {
public:
	virtual ~AbstractSession() {}

	virtual void ref() const = 0;
	virtual void unref() const = 0;

	virtual pid_t getPid() const = 0;
	virtual StaticString getGupid() const = 0;
	virtual StaticString getProtocol() const = 0;
	virtual unsigned int getStickySessionId() const = 0;
	virtual const ApiKey &getApiKey() const = 0;
	virtual int fd() const = 0;
	virtual bool isClosed() const = 0;

	virtual void initiate(bool blocking = true) = 0;

	virtual void requestOOBW() { /* Do nothing */ }

	virtual void killProcess(int signo) {
		syscalls::kill(getPid(), signo);
	}

	/**
	 * This Session object becomes fully unsable after closing.
	 */
	virtual void close(bool success, bool wantKeepAlive = false) = 0;
};


inline void
intrusive_ptr_add_ref(const AbstractSession *session) {
	session->ref();
}

inline void
intrusive_ptr_release(const AbstractSession *session) {
	session->unref();
}


} // namespace ApplicationPool2
} // namespace Passenger

#endif /* _PASSENGER_APPLICATION_POOL2_ABSTRACT_SESSION_H_ */
