/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2011-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#ifndef _PASSENGER_SPAWNING_KIT_FACTORY_H_
#define _PASSENGER_SPAWNING_KIT_FACTORY_H_

#include <Core/SpawningKit/Context.h>
#include <Core/SpawningKit/SmartSpawner.h>
#include <Core/SpawningKit/DirectSpawner.h>
#include <Core/SpawningKit/DummySpawner.h>

namespace Passenger {
namespace SpawningKit {

using namespace std;
using namespace boost;
using namespace oxt;


class Factory {
private:
	boost::mutex syncher;
	Context *context;
	DummySpawnerPtr dummySpawner;

	SpawnerPtr tryCreateSmartSpawner(const AppPoolOptions &options) {
		string dir = context->resourceLocator->getHelperScriptsDir();
		vector<string> preloaderCommand;
		if (options.appType == "rack") {
			preloaderCommand.push_back(options.ruby);
			preloaderCommand.push_back(dir + "/rack-preloader.rb");
		} else {
			return SpawnerPtr();
		}
		return boost::make_shared<SmartSpawner>(context,
			preloaderCommand, options);
	}

public:
	unsigned int spawnerCreationSleepTime;

	Factory(Context *_context)
		: context(_context),
		  spawnerCreationSleepTime(0)
	{
		if (context->debugSupport != NULL) {
			spawnerCreationSleepTime = context->debugSupport->spawnerCreationSleepTime;
		}
	}

	virtual ~Factory() { }

	virtual SpawnerPtr create(const AppPoolOptions &options) {
		if (options.spawnMethod == "smart" || options.spawnMethod == "smart-lv2") {
			SpawnerPtr spawner = tryCreateSmartSpawner(options);
			if (spawner == NULL) {
				spawner = boost::make_shared<DirectSpawner>(context);
			}
			return spawner;
		} else if (options.spawnMethod == "direct" || options.spawnMethod == "conservative") {
			boost::shared_ptr<DirectSpawner> spawner = boost::make_shared<DirectSpawner>(
				context);
			return spawner;
		} else if (options.spawnMethod == "dummy") {
			syscalls::usleep(spawnerCreationSleepTime);
			return getDummySpawner();
		} else {
			throw ArgumentException("Unknown spawn method '" + options.spawnMethod + "'");
		}
	}

	/**
	 * SpawnerFactory always returns the same DummyFactory object upon
	 * creating a dummy spawner. This allows unit tests to easily
	 * set debugging options on the spawner.
	 */
	DummySpawnerPtr getDummySpawner() {
		boost::lock_guard<boost::mutex> l(syncher);
		if (dummySpawner == NULL) {
			dummySpawner = boost::make_shared<DummySpawner>(context);
		}
		return dummySpawner;
	}

	/**
	 * All created Spawner objects share the same Context object.
	 */
	Context *getContext() const {
		return context;
	}
};

typedef boost::shared_ptr<Factory> FactoryPtr;


} // namespace SpawningKit
} // namespace Passenger

#endif /* _PASSENGER_SPAWNING_KIT_FACTORY_H_ */
