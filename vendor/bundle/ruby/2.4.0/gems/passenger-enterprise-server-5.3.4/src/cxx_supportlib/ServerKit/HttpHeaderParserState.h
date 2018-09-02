/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2014-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#ifndef _PASSENGER_SERVER_KIT_HTTP_HEADER_PARSER_STATE_H_
#define _PASSENGER_SERVER_KIT_HTTP_HEADER_PARSER_STATE_H_

#include <DataStructures/LString.h>
#include <Utils/Hasher.h>

namespace Passenger {
namespace ServerKit {


struct HttpHeaderParserState {
	enum State {
		PARSING_NOT_STARTED,
		PARSING_URL,
		PARSING_FIRST_HEADER_FIELD,
		PARSING_FIRST_HEADER_VALUE,
		PARSING_HEADER_FIELD,
		PARSING_HEADER_VALUE,
		ERROR_SECURITY_PASSWORD_MISMATCH,
		ERROR_SECURITY_PASSWORD_DUPLICATE,
		ERROR_SECURE_HEADER_NOT_ALLOWED,
		ERROR_NORMAL_HEADER_NOT_ALLOWED_AFTER_SECURITY_PASSWORD
	};

	State state;
	bool secureMode;
	http_parser parser;
	Header *currentHeader;
	Hasher hasher;
};


} // namespace ServerKit
} // namespace Passenger

#endif /* _PASSENGER_SERVER_KIT_HTTP_HEADER_PARSER_STATE_H_ */
