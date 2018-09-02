/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2016-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#include <DataStructures/LString.h>

namespace Passenger {


LString::Part emptyLStringPart = {
	NULL,
	NULL,
	"",
	0
};


}
