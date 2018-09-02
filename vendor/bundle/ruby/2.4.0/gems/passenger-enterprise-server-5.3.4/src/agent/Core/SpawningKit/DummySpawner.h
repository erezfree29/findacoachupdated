/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2011-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#ifndef _PASSENGER_SPAWNING_KIT_DUMMY_SPAWNER_H_
#define _PASSENGER_SPAWNING_KIT_DUMMY_SPAWNER_H_

#include <boost/shared_ptr.hpp>
#include <boost/atomic.hpp>
#include <vector>

#include <StaticString.h>
#include <Utils/StrIntUtils.h>
#include <Core/SpawningKit/Spawner.h>
#include <Core/SpawningKit/Exceptions.h>

namespace Passenger {
namespace SpawningKit {

using namespace std;
using namespace boost;
using namespace oxt;


class DummySpawner: public Spawner {
private:
	boost::atomic<unsigned int> count;

	void setConfigFromAppPoolOptions(Config *config, Json::Value &extraArgs,
		const AppPoolOptions &options)
	{
		Spawner::setConfigFromAppPoolOptions(config, extraArgs, options);
		config->spawnMethod = P_STATIC_STRING("dummy");
	}

public:
	unsigned int cleanCount;

	DummySpawner(Context *context)
		: Spawner(context),
		  count(1),
		  cleanCount(0)
		{ }

	virtual Result spawn(const AppPoolOptions &options) {
		TRACE_POINT();
		possiblyRaiseInternalError(options);

		if (context->debugSupport != NULL) {
			syscalls::usleep(context->debugSupport->dummySpawnDelay);
		}

		Config config;
		Json::Value extraArgs;
		setConfigFromAppPoolOptions(&config, extraArgs, options);

		unsigned int number = count.fetch_add(1, boost::memory_order_relaxed);
		Result result;
		Result::Socket socket;

		socket.address = "tcp://127.0.0.1:1234";
		socket.protocol = "session";
		socket.concurrency = 1;
		socket.acceptHttpRequests = true;
		if (context->debugSupport != NULL) {
			socket.concurrency = context->debugSupport->dummyConcurrency;
		}

		result.initialize(*context, &config);
		result.pid = number;
		result.dummy = true;
		result.gupid = "gupid-" + toString(number);
		result.spawnEndTime = result.spawnStartTime;
		result.spawnEndTimeMonotonic = result.spawnStartTimeMonotonic;
		result.sockets.push_back(socket);

		vector<StaticString> internalFieldErrors;
		vector<StaticString> appSuppliedFieldErrors;
		if (!result.validate(internalFieldErrors, appSuppliedFieldErrors)) {
			Journey journey(SPAWN_DIRECTLY, !config.genericApp && config.startsUsingWrapper);
			journey.setStepErrored(SPAWNING_KIT_HANDSHAKE_PERFORM, true);
			SpawnException e(INTERNAL_ERROR, journey, &config);
			e.setSummary("Error spawning the web application:"
				" a bug in " SHORT_PROGRAM_NAME " caused the"
				" spawn result to be invalid: "
				+ toString(internalFieldErrors)
				+ ", " + toString(appSuppliedFieldErrors));
			e.setProblemDescriptionHTML(
				"Bug: the spawn result is invalid: "
				+ toString(internalFieldErrors)
				+ ", " + toString(appSuppliedFieldErrors));
			throw e.finalize();
		}

		return result;
	}

	virtual bool cleanable() const {
		return true;
	}

	virtual void cleanup() {
		cleanCount++;
	}
};

typedef boost::shared_ptr<DummySpawner> DummySpawnerPtr;


} // namespace SpawningKit
} // namespace Passenger

#endif /* _PASSENGER_SPAWNING_KIT_DUMMY_SPAWNER_H_ */
