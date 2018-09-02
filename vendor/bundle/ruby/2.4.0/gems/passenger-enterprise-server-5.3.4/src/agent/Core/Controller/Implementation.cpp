/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2015-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */

#include <Core/Controller.h>
#include <Core/Controller/InitRequest.cpp>
#include <Core/Controller/BufferBody.cpp>
#include <Core/Controller/CheckoutSession.cpp>
#include <Core/Controller/SendRequest.cpp>
#include <Core/Controller/ForwardResponse.cpp>
#include <Core/Controller/Hooks.cpp>
#include <Core/Controller/InitializationAndShutdown.cpp>
#include <Core/Controller/InternalUtils.cpp>
#include <Core/Controller/Miscellaneous.cpp>
#include <Core/Controller/Config.cpp>
#include <Core/Controller/StateInspection.cpp>
