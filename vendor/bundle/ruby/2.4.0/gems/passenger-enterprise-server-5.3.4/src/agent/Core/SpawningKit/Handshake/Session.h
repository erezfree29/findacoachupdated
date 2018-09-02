/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2016-2018 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#ifndef _PASSENGER_SPAWNING_KIT_HANDSHAKE_SESSION_H_
#define _PASSENGER_SPAWNING_KIT_HANDSHAKE_SESSION_H_

#include <boost/scoped_ptr.hpp>
#include <string>
#include <map>

#include <Utils.h>
#include <Core/SpawningKit/Context.h>
#include <Core/SpawningKit/Config.h>
#include <Core/SpawningKit/Journey.h>
#include <Core/SpawningKit/Result.h>
#include <Core/SpawningKit/Handshake/WorkDir.h>


namespace Passenger {
namespace SpawningKit {

using namespace std;


struct HandshakeSession {
	Context *context;
	Config *config;

	boost::scoped_ptr<HandshakeWorkDir> workDir;
	string responseDir;
	string envDumpDir;
	int workDirFd;
	int responseDirFd;
	int responseErrorDirFd;
	int envDumpDirFd;
	int envDumpAnnotationsDirFd;
	map<JourneyStep, int> stepDirFds;
	Journey journey;
	Result result;

	uid_t uid;
	gid_t gid;
	string homedir;
	string shell;

	unsigned long long timeoutUsec;

	/**
	 * The port that the application is expected to start on. Only meaningful
	 * if `config->genericApp || config->findFreePort`.
	 */
	unsigned int expectedStartPort;

	HandshakeSession(Context &_context, Config &_config, JourneyType journeyType)
		: context(&_context),
		  config(&_config),
		  workDirFd(-1),
		  responseDirFd(-1),
		  responseErrorDirFd(-1),
		  envDumpDirFd(-1),
		  envDumpAnnotationsDirFd(-1),
		  journey(journeyType, !_config.genericApp && _config.startsUsingWrapper),
		  uid(USER_NOT_GIVEN),
		  gid(GROUP_NOT_GIVEN),
		  timeoutUsec(_config.startTimeoutMsec * 1000),
		  expectedStartPort(0)
		{ }

	~HandshakeSession() {
		if (workDirFd != -1) {
			safelyClose(workDirFd, true);
		}
		if (responseDirFd != -1) {
			safelyClose(responseDirFd, true);
		}
		if (responseErrorDirFd != -1) {
			safelyClose(responseErrorDirFd, true);
		}
		if (envDumpDirFd != -1) {
			safelyClose(envDumpDirFd, true);
		}
		if (envDumpAnnotationsDirFd != -1) {
			safelyClose(envDumpAnnotationsDirFd, true);
		}

		map<JourneyStep, int>::iterator it, end = stepDirFds.end();
		for (it = stepDirFds.begin(); it != end; it++) {
			safelyClose(it->second);
		}

		if (config->debugWorkDir && workDir != NULL) {
			string path = workDir->dontRemoveOnDestruction();
			P_NOTICE("Work directory " << path << " preserved for debugging");
		}
	}
};


} // namespace SpawningKit
} // namespace Passenger

#endif /* _PASSENGER_SPAWNING_KIT_HANDSHAKE_SESSION_H_ */
