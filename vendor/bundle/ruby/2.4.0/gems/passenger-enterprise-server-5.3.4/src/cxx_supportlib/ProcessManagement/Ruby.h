/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2010-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#ifndef _PASSENGER_PROCESS_MANAGEMENT_RUBY_H_
#define _PASSENGER_PROCESS_MANAGEMENT_RUBY_H_

#include <string>
#include <vector>
#include <cstddef>

namespace Passenger {

using namespace std;

class ResourceLocator;


/**
 * Run a Passenger-internal Ruby tool, e.g. passenger-config, and optionally capture
 * its stdout output. This function does not care whether the command fails.
 *
 * @param resourceLocator
 * @param ruby The Ruby interpreter to attempt to use for running the tool.
 * @param args The command as an array of strings, e.g. ["passenger-config", "system-properties"].
 * @param status The status of the child process will be stored here, if non-NULL.
 *               When unable to waitpid() the child process because of an ECHILD
 *               or ESRCH, this will be set to -1.
 * @param output The output of the child process will be stored here, if non-NULL.
 * @throws RuntimeException
 * @throws SystemException
 */
void runInternalRubyTool(const ResourceLocator &resourceLocator,
	const string &ruby, const vector<string> &args,
	int *status = NULL, string *output = NULL);


} // namespace Passenger

#endif /* _PASSENGER_PROCESS_MANAGEMENT_RUBY_H_ */
