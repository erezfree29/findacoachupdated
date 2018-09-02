/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2015-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#ifndef _PASSENGER_CLASS_UTILS_H_
#define _PASSENGER_CLASS_UTILS_H_

#define P_DEFINE_GETTER(type, name) \
	type get ## name() const { \
		return m ## name; \
	}

#define P_DEFINE_GETTER_CONST_REF(type, name) \
	const type &get ## name() const { \
		return m ## name; \
	}

#define P_DEFINE_GETTER_REF(type, name) \
	type &get ## name() { \
		return m ## name; \
	}

#define P_DEFINE_SETTER(type, name) \
	void set ## name(const type &value) { \
		m ## name = value; \
	}

#define P_RO_PROPERTY(visibility, type, name) \
	public: \
		P_DEFINE_GETTER(type, name) \
	visibility: \
		type m ## name

#define P_PROPERTY_CONST_REF(visibility, type, name) \
	public: \
		P_DEFINE_GETTER_CONST_REF(type, name) \
		P_DEFINE_SETTER(type, name) \
	visibility: \
		type m ## name

#define P_RO_PROPERTY_REF(visibility, type, name) \
	public: \
		P_DEFINE_GETTER_REF(type, name) \
	visibility: \
		type m ## name

#define P_RO_PROPERTY_CONST_REF(visibility, type, name) \
	public: \
		P_DEFINE_GETTER_CONST_REF(type, name) \
	visibility: \
		type m ## name

#endif /* _PASSENGER_CLASS_UTILS_H_ */
