/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2014-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */

// Implementation is in its own file so that we can enable compiler optimizations for these functions only.

#include <Utils/Hasher.h>

namespace Passenger {

void
JenkinsHash::update(const char *data, unsigned int size) {
	const char *end = data + size;

	while (data < end) {
		hash += *data;
		hash += (hash << 10);
		hash ^= (hash >> 6);
		data++;
	}
}

boost::uint32_t
JenkinsHash::finalize() {
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return hash;
}

} // namespace Passenger
