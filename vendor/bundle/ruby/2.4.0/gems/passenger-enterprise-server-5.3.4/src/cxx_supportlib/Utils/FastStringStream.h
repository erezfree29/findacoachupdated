/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2015-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#ifndef _PASSENGER_FAST_STRING_STREAM_H_
#define _PASSENGER_FAST_STRING_STREAM_H_

#include <ostream>
#include <sstream>
#include <string>
#include <new>
#include <cstdlib>
#include <boost/cstdint.hpp>
#include <boost/static_assert.hpp>
#include <oxt/macros.hpp>

namespace Passenger {

using namespace std;


/**
 * An std::streambuf-compatible string buffer. It's similar to std::stringbuf,
 * with a few optimizations:
 *
 * - It uses an in-place storage area as long as the amount of data written
 *   fits inside.
 * - It allows direct read-only access to the storage area, in order to avoid
 *   copying data.
 */
template<size_t staticCapacity = 1024>
class FastStdStringBuf: public streambuf {
public:
	typedef char                  char_type;
	typedef char_traits<char>     traits_type;
	typedef traits_type::int_type int_type;
	typedef traits_type::pos_type pos_type;
	typedef traits_type::off_type off_type;
	typedef string                string_type;

	// Ensures that power-of-two doubling of staticCapacity works.
	BOOST_STATIC_ASSERT(staticCapacity >= 4);

private:
	mutable char_type *bufend;
	size_t dynamicCapacity;
	union {
		char staticBuffer[staticCapacity];
		char *dynamicBuffer;
	} u;

	bool usingStaticBuffer() const {
		return dynamicCapacity == 0;
	}

	boost::uint32_t nextPowerOf2(boost::uint32_t v) const {
		v--;
		v |= v >> 1;
		v |= v >> 2;
		v |= v >> 4;
		v |= v >> 8;
		v |= v >> 16;
		v++;
		return v;
	}

protected:
	virtual int_type overflow(int_type ch = traits_type::eof()) {
		size_t oldSize = size();
		size_t newDynamicCapacity;
		char *newDynamicBuffer;

		if (usingStaticBuffer()) {
			newDynamicCapacity = nextPowerOf2(2 * staticCapacity);
			newDynamicBuffer = (char *) malloc(newDynamicCapacity);
			if (OXT_UNLIKELY(newDynamicBuffer == NULL)) {
				return traits_type::eof();
			}
			memcpy(newDynamicBuffer, u.staticBuffer, oldSize);
		} else {
			newDynamicCapacity = 2 * dynamicCapacity;
			newDynamicBuffer = (char *) realloc(u.dynamicBuffer, newDynamicCapacity);
			if (OXT_UNLIKELY(newDynamicBuffer == NULL)) {
				return traits_type::eof();
			}
		}

		dynamicCapacity = newDynamicCapacity;
		u.dynamicBuffer = newDynamicBuffer;
		setp(u.dynamicBuffer, u.dynamicBuffer + dynamicCapacity);

		if (traits_type::eq_int_type(ch, traits_type::eof())) {
			pbump(oldSize);
		} else {
			u.dynamicBuffer[oldSize] = ch;
			pbump(oldSize + 1);
		}

		return traits_type::not_eof(ch);
	}

public:
	FastStdStringBuf(unsigned int initialCapacity = 0) {
		if (initialCapacity <= staticCapacity) {
			dynamicCapacity = 0;
			setp(u.staticBuffer, u.staticBuffer + staticCapacity);
		} else {
			dynamicCapacity = nextPowerOf2(initialCapacity);
			u.dynamicBuffer = (char *) malloc(dynamicCapacity);
			if (u.dynamicBuffer == NULL) {
				throw std::bad_alloc();
			}
			setp(u.dynamicBuffer, u.dynamicBuffer + dynamicCapacity);
		}
	}

	~FastStdStringBuf() {
		if (!usingStaticBuffer()) {
			free(u.dynamicBuffer);
		}
	}

	const char *data() const {
		return pbase();
	}

	size_t size() const {
		return pptr() - pbase();
	}

	size_t capacity() const {
		if (usingStaticBuffer()) {
			return staticCapacity;
		} else {
			return dynamicCapacity;
		}
	}
};

/**
 * An std::ostream-compatible output stream. It's similar to std::stringstream,
 * with a few optimizations:
 *
 * - It uses an in-place storage area as long as the amount of data written
 *   fits inside.
 * - It allows direct read-only access to the storage area, in order to avoid
 *   copying data.
 *
 * This class is implemented using FastStdStringBuf.
 */
#ifndef _PASSENGER_FAST_STRING_STREAM_FORWARD_DECLARED_
	#define _PASSENGER_FAST_STRING_STREAM_FORWARD_DECLARED_
	template<size_t staticCapacity = 1024>
#else
	template<size_t staticCapacity>
#endif
class FastStringStream: public FastStdStringBuf<staticCapacity>, public ostream {
public:
	FastStringStream(unsigned int initialCapacity = 0)
		: FastStdStringBuf<staticCapacity>(initialCapacity),
		  ostream(this)
		{ }
};


} // namespace Passenger

#endif /* _PASSENGER_FAST_STRING_STREAM_H_ */
