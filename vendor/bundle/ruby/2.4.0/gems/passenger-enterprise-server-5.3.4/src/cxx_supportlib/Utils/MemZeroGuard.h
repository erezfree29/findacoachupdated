/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2010-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#ifndef _PASSENGER_MEM_ZERO_GUARD_H_
#define _PASSENGER_MEM_ZERO_GUARD_H_

#include <string>

namespace Passenger {

using namespace std;


/**
 * Fills the given memory space or string with zeroes when a MemoryZeroGuard object
 * is destroyed. Useful for ensuring that buffers containing password data or
 * other sensitive information is cleared when it goes out of scope.
 */
class MemZeroGuard {
private:
	void *data;
	unsigned int size;
	string *str;

	static void securelyZeroMemory(volatile void *data, unsigned int size) {
		/* We do not use memset() here because the compiler may
		 * optimize out memset() calls. Instead, the following
		 * code is guaranteed to zero the memory.
		 * http://www.dwheeler.com/secure-programs/Secure-Programs-HOWTO/protect-secrets.html
		 */
		volatile char *p = (volatile char *) data;
		while (size--) {
			*p++ = 0;
		}
	}

public:
	/**
	 * Creates a new MemZeroGuard object with a memory region to zero.
	 *
	 * @param data The data to zero after destruction.
	 * @param size The size of the data.
	 * @pre data != NULL
	 */
	MemZeroGuard(void *data, unsigned int size) {
		this->data = data;
		this->size = size;
		this->str  = NULL;
	}

	/**
	 * Creates a new MemoryZeroGuard object with a string to zero.
	 *
	 * @param str The string to zero after destruction.
	 */
	MemZeroGuard(string &str) {
		this->data = NULL;
		this->size = 0;
		this->str  = &str;
	}

	/**
	 * Zero the data immediately. The data will still be zeroed after
	 * destruction of this object.
	 */
	void zeroNow() {
		if (str == NULL) {
			securelyZeroMemory(data, size);
		} else {
			securelyZeroMemory((volatile void *) str->c_str(), str->size());
		}
	}

	~MemZeroGuard() {
		zeroNow();
	}
};


} // namespace Passenger

#endif /* _PASSENGER_MEM_ZERO_GUARD_H_ */
