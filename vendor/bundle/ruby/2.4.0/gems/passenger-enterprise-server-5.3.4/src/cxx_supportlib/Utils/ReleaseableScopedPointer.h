/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2015-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#ifndef _PASSENGER_RELEASEABLE_SCOPED_POINTER_H_
#define _PASSENGER_RELEASEABLE_SCOPED_POINTER_H_

#include <boost/noncopyable.hpp>
#include <cstddef>

namespace Passenger {


using namespace std;

/**
 * This is like std::auto_ptr, but does not raise deprecation warnings on newer
 * compilers. We cannot replace std::auto_ptr with boost::scoped_ptr or
 * boost::shared_ptr because have a few use cases for std::auto_ptr.release().
 */
template<typename T>
class ReleaseableScopedPointer: public boost::noncopyable {
private:
	T *p;

public:
	ReleaseableScopedPointer(T *_p)
		: p(_p)
		{ }

	~ReleaseableScopedPointer() {
		delete p;
	}

	T *get() const {
		return p;
	}

	T *release() {
		T *tmp = p;
		p = NULL;
		return tmp;
	}
};


} // namespace Passenger

#endif /* _PASSENGER_RELEASEABLE_SCOPED_POINTER_H_ */
