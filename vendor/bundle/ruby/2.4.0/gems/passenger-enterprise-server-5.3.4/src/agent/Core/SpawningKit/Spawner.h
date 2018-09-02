/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2011-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#ifndef _PASSENGER_SPAWNING_KIT_SPAWNER_H_
#define _PASSENGER_SPAWNING_KIT_SPAWNER_H_

#include <boost/shared_ptr.hpp>
#include <oxt/system_calls.hpp>

#include <modp_b64.h>

#include <LoggingKit/Logging.h>
#include <Utils/SystemTime.h>
#include <Core/SpawningKit/Context.h>
#include <Core/SpawningKit/Result.h>
#include <Core/SpawningKit/UserSwitchingRules.h>

namespace Passenger {
namespace SpawningKit {

using namespace std;
using namespace boost;
using namespace oxt;


class Spawner {
private:
	StringKeyTable<StaticString> decodeEnvironmentVariables(const StaticString &envvarsData) {
		StringKeyTable<StaticString> result;
		string::size_type keyStart = 0;

		while (keyStart < envvarsData.size()) {
			string::size_type keyEnd = envvarsData.find('\0', keyStart);
			string::size_type valueStart = keyEnd + 1;
			if (valueStart >= envvarsData.size()) {
				break;
			}

			string::size_type valueEnd = envvarsData.find('\0', valueStart);
			if (valueEnd >= envvarsData.size()) {
				break;
			}

			StaticString key = envvarsData.substr(keyStart, keyEnd - keyStart);
			StaticString value = envvarsData.substr(valueStart, valueEnd - valueStart);
			result.insert(key, value, true);
			keyStart = valueEnd + 1;
		}

		result.compact();
		return result;
	}

protected:
	Context *context;

	void setConfigFromAppPoolOptions(Config *config, Json::Value &extraArgs,
		const AppPoolOptions &options)
	{
		string startCommand = options.getStartCommand(*context->resourceLocator);
		string envvarsData;
		try {
			envvarsData = modp::b64_decode(options.environmentVariables.data(),
				options.environmentVariables.size());
		} catch (const std::runtime_error &) {
			P_WARN("Unable to decode base64-encoded environment variables: " <<
				options.environmentVariables);
			envvarsData.clear();
		}

		config->appGroupName = options.getAppGroupName();
		config->appRoot = options.appRoot;
		config->logLevel = options.logLevel;
		config->genericApp = false;
		config->startsUsingWrapper = true;
		config->wrapperSuppliedByThirdParty = false;
		config->findFreePort = false;
		config->loadShellEnvvars = options.loadShellEnvvars;
		config->startCommand = startCommand;
		config->startupFile = options.getStartupFile();
		config->appType = options.appType;
		config->appEnv = options.environment;
		config->baseURI = options.baseURI;
		config->environmentVariables = decodeEnvironmentVariables(
			envvarsData);
		config->logFile = options.appLogFile;
		config->apiKey = options.apiKey;
		config->groupUuid = options.groupUuid;
		config->lveMinUid = options.lveMinUid;
		config->fileDescriptorUlimit = options.fileDescriptorUlimit;
		config->startTimeoutMsec = options.startTimeout;

		UserSwitchingInfo info = prepareUserSwitching(options);
		config->user = info.username;
		config->group = info.groupname;

		extraArgs["spawn_method"] = options.spawnMethod.toString();

		/******************/

		config->concurrencyModel = options.concurrencyModel;
		config->threadCount = options.threadCount;
		config->memoryLimit = options.memoryLimit;
		config->debuggerSupport = options.debugger;

		/******************/

		config->internStrings();
	}

	static void nonInterruptableKillAndWaitpid(pid_t pid) {
		boost::this_thread::disable_syscall_interruption dsi;
		syscalls::kill(pid, SIGKILL);
		syscalls::waitpid(pid, NULL, 0);
	}

	static void possiblyRaiseInternalError(const AppPoolOptions &options) {
		if (options.raiseInternalError) {
			throw RuntimeException("An internal error!");
		}
	}

public:
	/**
	 * Timestamp at which this Spawner was created. Microseconds resolution.
	 */
	const unsigned long long creationTime;

	Spawner(Context *_context)
		: context(_context),
		  creationTime(SystemTime::getUsec())
		{ }

	virtual ~Spawner() { }

	virtual Result spawn(const AppPoolOptions &options) = 0;

	virtual bool cleanable() const {
		return false;
	}

	virtual void cleanup() {
		// Do nothing.
	}

	virtual unsigned long long lastUsed() const {
		return 0;
	}

	Context *getContext() const {
		return context;
	}
};
typedef boost::shared_ptr<Spawner> SpawnerPtr;


} // namespace SpawningKit
} // namespace Passenger

#endif /* _PASSENGER_SPAWNING_KIT_SPAWNER_H_ */
