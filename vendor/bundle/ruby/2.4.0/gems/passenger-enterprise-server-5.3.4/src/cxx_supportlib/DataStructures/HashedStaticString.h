/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2014-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#ifndef _PASSENGER_HASHED_STATIC_STRING_H_
#define _PASSENGER_HASHED_STATIC_STRING_H_

#include <boost/cstdint.hpp>
#include <oxt/macros.hpp>
#include <string>
#include <StaticString.h>
#include <Utils/Hasher.h>

namespace Passenger {

using namespace std;


class HashedStaticString: public StaticString {
private:
	boost::uint32_t m_hash;

public:
	HashedStaticString()
		: StaticString(),
		  m_hash(Hasher::EMPTY_STRING_HASH)
		{ }

	HashedStaticString(const StaticString &b)
		: StaticString(b)
	{
		rehash();
	}

	HashedStaticString(const HashedStaticString &b)
		: StaticString(b),
		  m_hash(b.m_hash)
		{ }

	HashedStaticString(const string &s)
		: StaticString(s)
	{
		rehash();
	}

	HashedStaticString(const char *data)
		: StaticString(data)
	{
		rehash();
	}

	HashedStaticString(const char *data, string::size_type len)
		: StaticString(data, len)
	{
		rehash();
	}

	HashedStaticString(const char *data, string::size_type len,
		boost::uint32_t hash)
		: StaticString(data, len),
		  m_hash(hash)
		{ }

	void rehash() {
		Hasher h;
		h.update(data(), size());
		m_hash = h.finalize();
	}

	void setHash(boost::uint32_t value) {
		m_hash = value;
	}

	OXT_FORCE_INLINE
	boost::uint32_t hash() const {
		return m_hash;
	}
};


} // namespace Passenger

#endif /* _PASSENGER_HASHED_STATIC_STRING_H_ */
