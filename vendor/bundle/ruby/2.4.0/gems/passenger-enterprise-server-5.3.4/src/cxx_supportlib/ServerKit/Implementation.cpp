/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2014-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#include <DataStructures/HashedStaticString.h>

namespace Passenger {
namespace ServerKit {


// Define 'extern' so that the compiler doesn't output warnings.
extern const HashedStaticString HTTP_COOKIE;
extern const HashedStaticString HTTP_SET_COOKIE;
extern const HashedStaticString HTTP_CONTENT_LENGTH;
extern const HashedStaticString HTTP_TRANSFER_ENCODING;
extern const HashedStaticString HTTP_X_SENDFILE;
extern const HashedStaticString HTTP_X_ACCEL_REDIRECT;
extern const char DEFAULT_INTERNAL_SERVER_ERROR_RESPONSE[];
extern const unsigned int DEFAULT_INTERNAL_SERVER_ERROR_RESPONSE_SIZE;

const char DEFAULT_INTERNAL_SERVER_ERROR_RESPONSE[] =
	"Status: 500 Internal Server Error\r\n"
	"Content-Length: 22\r\n"
	"Content-Type: text/plain\r\n"
	"Connection: close\r\n"
	"\r\n"
	"Internal server error\n";
const unsigned int DEFAULT_INTERNAL_SERVER_ERROR_RESPONSE_SIZE =
	sizeof(DEFAULT_INTERNAL_SERVER_ERROR_RESPONSE) - 1;
const HashedStaticString HTTP_COOKIE("cookie");
const HashedStaticString HTTP_SET_COOKIE("set-cookie");
const HashedStaticString HTTP_CONTENT_LENGTH("content-length");
const HashedStaticString HTTP_TRANSFER_ENCODING("transfer-encoding");
const HashedStaticString HTTP_X_SENDFILE("x-sendfile");
const HashedStaticString HTTP_X_ACCEL_REDIRECT("x-accel-redirect");


} // namespace ServerKit
} // namespace
