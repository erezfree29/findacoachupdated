/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2014-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#include <Core/Controller.h>

/*************************************************************************
 *
 * Initialization and shutdown-related code for Core::Controller
 *
 *************************************************************************/

namespace Passenger {
namespace Core {

using namespace std;
using namespace boost;


/****************************
 *
 * Public methods
 *
 ****************************/

Controller::~Controller() {
	ev_check_stop(getLoop(), &checkWatcher);
	delete singleAppModeConfig;
}

void
Controller::preinitialize() {
	ev_check_init(&checkWatcher, onEventLoopCheck);
	ev_set_priority(&checkWatcher, EV_MAXPRI);
	ev_check_start(getLoop(), &checkWatcher);
	checkWatcher.data = this;

	#ifdef DEBUG_CC_EVENT_LOOP_BLOCKING
		ev_prepare_init(&prepareWatcher, onEventLoopPrepare);
		ev_prepare_start(getLoop(), &prepareWatcher);
		prepareWatcher.data = this;

		timeBeforeBlocking = 0;
	#endif

	PASSENGER_APP_GROUP_NAME = "!~PASSENGER_APP_GROUP_NAME";
	PASSENGER_ENV_VARS = "!~PASSENGER_ENV_VARS";
	PASSENGER_MAX_REQUESTS = "!~PASSENGER_MAX_REQUESTS";
	PASSENGER_SHOW_VERSION_IN_HEADER = "!~PASSENGER_SHOW_VERSION_IN_HEADER";
	PASSENGER_STICKY_SESSIONS = "!~PASSENGER_STICKY_SESSIONS";
	PASSENGER_STICKY_SESSIONS_COOKIE_NAME = "!~PASSENGER_STICKY_SESSIONS_COOKIE_NAME";
	PASSENGER_REQUEST_OOB_WORK = "!~Request-OOB-Work";
	REMOTE_ADDR = "!~REMOTE_ADDR";
	REMOTE_PORT = "!~REMOTE_PORT";
	REMOTE_USER = "!~REMOTE_USER";
	FLAGS = "!~FLAGS";
	HTTP_COOKIE = "cookie";
	HTTP_DATE = "date";
	HTTP_HOST = "host";
	HTTP_CONTENT_LENGTH = "content-length";
	HTTP_CONTENT_TYPE = "content-type";
	HTTP_EXPECT = "expect";
	HTTP_CONNECTION = "connection";
	HTTP_STATUS = "status";
	HTTP_TRANSFER_ENCODING = "transfer-encoding";

	/**************************/

	PASSENGER_MAX_REQUEST_TIME = "!~PASSENGER_MAX_REQUEST_TIME";
}

void
Controller::initialize() {
	TRACE_POINT();
	if (resourceLocator == NULL) {
		throw RuntimeException("ResourceLocator not initialized");
	}
	if (appPool == NULL) {
		throw RuntimeException("AppPool not initialized");
	}

	ParentClass::initialize();
	turboCaching.initialize(config["turbocaching"].asBool());

	if (mainConfig.singleAppMode) {
		boost::shared_ptr<Options> options = boost::make_shared<Options>();
		fillPoolOptionsFromConfigCaches(*options, mainConfig.pool, requestConfig);

		string appRoot = singleAppModeConfig->get("app_root").asString();
		string environment = config["default_environment"].asString();
		string appType = singleAppModeConfig->get("app_type").asString();
		string startupFile = singleAppModeConfig->get("startup_file").asString();

		options->appRoot = appRoot;
		options->environment = environment;
		options->appType = appType;
		options->startupFile = startupFile;
		*options = options->copyAndPersist();
		poolOptionsCache.insert(options->getAppGroupName(), options);
	}
}


} // namespace Core
} // namespace Passenger
