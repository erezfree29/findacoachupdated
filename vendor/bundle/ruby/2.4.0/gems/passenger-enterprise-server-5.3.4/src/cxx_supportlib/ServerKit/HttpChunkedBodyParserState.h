/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2012-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#ifndef _PASSENGER_SERVER_KIT_CHUNKED_BODY_PARSER_STATE_H_
#define _PASSENGER_SERVER_KIT_CHUNKED_BODY_PARSER_STATE_H_

#include <boost/cstdint.hpp>

namespace Passenger {
namespace ServerKit {

using namespace std;


struct HttpChunkedBodyParserState {
	/***** Types and constants *****/

	// (2^32-1)/10 (409 MB), because `remainingDataSize` is 32-bit. Divided by 10 to
	// prevent overflow during parsing of the chunk size.
	static const unsigned int MAX_CHUNK_SIZE = 429496729;
	static const char CR = '\x0D';
	static const char LF = '\x0A';

	enum State {
		EXPECTING_SIZE_FIRST_DIGIT,
		EXPECTING_SIZE,
		EXPECTING_CHUNK_EXTENSION,
		EXPECTING_HEADER_LF,
		EXPECTING_DATA,
		EXPECTING_NON_FINAL_CR,
		EXPECTING_NON_FINAL_LF,
		EXPECTING_FINAL_CR,
		EXPECTING_FINAL_LF,
		DONE,
		ERROR
	};


	/***** Working state *****/

	State state;
	boost::uint32_t remainingDataSize;
};


} // namespace ServerKit
} // namespace Passenger

#endif /* _PASSENGER_SERVER_KIT_CHUNKED_BODY_PARSER_STATE_H_ */
