/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2014-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#ifndef _PASSENGER_SERVER_KIT_HTTP_CLIENT_H_
#define _PASSENGER_SERVER_KIT_HTTP_CLIENT_H_

#include <psg_sysqueue.h>
#include <ServerKit/Client.h>
#include <ServerKit/HttpRequest.h>

namespace Passenger {
namespace ServerKit {


template<typename Request = HttpRequest>
class BaseHttpClient: public BaseClient {
public:
	typedef Request RequestType;
	LIST_HEAD(RequestList, Request);

	/**
	 * @invariant
	 *     if currentRequest != NULL:
	 *         currentRequest->httpState != HttpRequest::WAITING_FOR_REFERENCES
	 *         currentRequest->httpState != HttpRequest::IN_FREELIST
	 */
	Request *currentRequest;
	unsigned int requestsBegun;

	BaseHttpClient(void *server)
		: BaseClient(server),
		  currentRequest(NULL),
		  requestsBegun(0)
		{ }
};


#define SERVER_KIT_BASE_HTTP_CLIENT_INIT() \
	LIST_INIT(&lingeringRequests); \
	lingeringRequestCount = 0

#define DEFINE_SERVER_KIT_BASE_HTTP_CLIENT_FOOTER(ClientType, RequestType) \
	DEFINE_SERVER_KIT_BASE_CLIENT_FOOTER(ClientType); \
	/* Last field from BASE_CLIENT_FOOTER is an int, so we put an */ \
	/* unsigned int here first to avoid an alignment hole on x86_64. */ \
	unsigned int lingeringRequestCount; \
	Passenger::ServerKit::BaseHttpClient<RequestType>::RequestList lingeringRequests

#define DEFINE_SERVER_KIT_BASE_HTTP_CLIENT_FOOTER_FOR_TEMPLATE_CLASS(ClientType, RequestType) \
	DEFINE_SERVER_KIT_BASE_CLIENT_FOOTER(ClientType); \
	/* Last field from BASE_CLIENT_FOOTER is an int, so we put an */ \
	/* unsigned int here first to avoid an alignment hole on x86_64. */ \
	unsigned int lingeringRequestCount; \
	typename Passenger::ServerKit::BaseHttpClient<RequestType>::RequestList lingeringRequests


template<typename Request = HttpRequest>
class HttpClient: public BaseHttpClient<Request> {
public:
	HttpClient(void *server)
		: BaseHttpClient<Request>(server)
	{
		SERVER_KIT_BASE_HTTP_CLIENT_INIT();
	}

	DEFINE_SERVER_KIT_BASE_HTTP_CLIENT_FOOTER_FOR_TEMPLATE_CLASS(HttpClient, Request);
};


} // namespace ServerKit
} // namespace Passenger

#endif /* _PASSENGER_SERVER_KIT_HTTP_CLIENT_H_ */
