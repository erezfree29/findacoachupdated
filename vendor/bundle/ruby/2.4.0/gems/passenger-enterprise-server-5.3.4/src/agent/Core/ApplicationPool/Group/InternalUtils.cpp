/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2011-2018 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#include <Core/ApplicationPool/Group.h>

/*************************************************************************
 *
 * Internal utility functions for ApplicationPool2::Group
 *
 *************************************************************************/

namespace Passenger {
namespace ApplicationPool2 {

using namespace std;
using namespace boost;


/****************************
 *
 * Private methods
 *
 ****************************/


void
Group::runAllActions(const boost::container::vector<Callback> &actions) {
	boost::container::vector<Callback>::const_iterator it, end = actions.end();
	for (it = actions.begin(); it != end; it++) {
		(*it)();
	}
}

void
Group::interruptAndJoinAllThreads(GroupPtr self) {
	self->interruptableThreads.interrupt_and_join_all();
}

void
Group::doCleanupSpawner(SpawningKit::SpawnerPtr spawner) {
	spawner->cleanup();
}

/**
 * Persists options into this Group. Called at creation time and at restart time.
 * Values will be persisted into `destination`. Or if it's NULL, into `this->options`.
 */
void
Group::resetOptions(const Options &newOptions, Options *destination) {
	if (destination == NULL) {
		destination = &this->options;
	}
	*destination = newOptions;
	destination->persist(newOptions);
	destination->clearPerRequestFields();
	destination->apiKey    = getApiKey().toStaticString();
	destination->groupUuid = uuid;
}

/**
 * Merges some of the new options from the latest get() request into this Group.
 */
void
Group::mergeOptions(const Options &other) {
	options.maxRequests      = other.maxRequests;
	options.minProcesses     = other.minProcesses;
	options.statThrottleRate = other.statThrottleRate;
	options.maxPreloaderIdleTime = other.maxPreloaderIdleTime;
	options.maxProcesses     = other.maxProcesses;

	if ((options.maxRequestQueueTimeMs == 0) && (other.maxRequestQueueTimeMs > 0)) {
		options.maxRequestQueueTimeMs = other.maxRequestQueueTimeMs;
		// Configuration changed: wakeup timeoutRequestsThread
		queueTimeoutCheckerCond.notify_one();
	}
}

/* Given a hook name like "queue_full_error", we return HookScriptOptions filled in with this name and a spec
 * (user settings that can be queried from agentsOptions using the external hook name that is prefixed with "hook_")
 *
 * @return false if the user parameters (agentsOptions) are not available (e.g. during ApplicationPool2_PoolTest)
 */
bool
Group::prepareHookScriptOptions(HookScriptOptions &hsOptions, const char *name) {
	Context *context = getPool()->getContext();
	LockGuard l(context->agentConfigSyncher);
	if (context->agentConfig.isNull()) {
		return false;
	}

	hsOptions.name = name;
	string hookName = string("hook_") + name;
	hsOptions.spec = context->agentConfig.get(hookName, Json::Value()).asString();

	return true;
}

// 'process' is not a reference so that bind(runAttachHooks, ...) causes the shared
// pointer reference to increment.
void
Group::runAttachHooks(const ProcessPtr process) const {
	getPool()->runHookScripts("attached_process",
		boost::bind(&Group::setupAttachOrDetachHook, this, process, _1));
}

void
Group::runDetachHooks(const ProcessPtr process) const {
	getPool()->runHookScripts("detached_process",
		boost::bind(&Group::setupAttachOrDetachHook, this, process, _1));
}

void
Group::setupAttachOrDetachHook(const ProcessPtr process, HookScriptOptions &options) const {
	options.environment.push_back(make_pair("PASSENGER_PROCESS_PID", toString(process->getPid())));
	options.environment.push_back(make_pair("PASSENGER_APP_ROOT", this->options.appRoot));
}

unsigned int
Group::generateStickySessionId() {
	unsigned int result;

	while (true) {
		result = (unsigned int) rand();
		if (result != 0 && findProcessWithStickySessionId(result) == NULL) {
			return result;
		}
	}
	// Never reached; shut up compiler warning.
	return 0;
}

ProcessPtr
Group::createNullProcessObject() {
	struct Guard {
		Context *context;
		Process *process;

		Guard(Context *c, Process *s)
			: context(c),
			  process(s)
			{ }

		~Guard() {
			if (process != NULL) {
				context->processObjectPool.free(process);
			}
		}

		void clear() {
			process = NULL;
		}
	};

	Json::Value args;
	args["pid"] = 0;
	args["gupid"] = "0";
	args["spawner_creation_time"] = 0;
	args["spawn_start_time"] = 0;
	args["dummy"] = true;
	args["sockets"] = Json::Value(Json::arrayValue);

	Context *context = getContext();
	LockGuard l(context->memoryManagementSyncher);
	Process *process = context->processObjectPool.malloc();
	Guard guard(context, process);
	process = new (process) Process(&info, args);
	process->shutdownNotRequired();
	guard.clear();
	return ProcessPtr(process, false);
}

ProcessPtr
Group::createProcessObject(const SpawningKit::Spawner &spawner,
	const SpawningKit::Result &spawnResult)
{
	struct Guard {
		Context *context;
		Process *process;

		Guard(Context *c, Process *s)
			: context(c),
			  process(s)
			{ }

		~Guard() {
			if (process != NULL) {
				context->processObjectPool.free(process);
			}
		}

		void clear() {
			process = NULL;
		}
	};

	Json::Value args;
	args["spawner_creation_time"] = (Json::UInt64) spawner.creationTime;

	Context *context = getContext();
	LockGuard l(context->memoryManagementSyncher);
	Process *process = context->processObjectPool.malloc();
	Guard guard(context, process);
	process = new (process) Process(&info, spawnResult, args);
	guard.clear();
	return ProcessPtr(process, false);
}

bool
Group::poolAtFullCapacity() const {
	return getPool()->atFullCapacityUnlocked();
}

ProcessPtr
Group::poolForceFreeCapacity(const Group *exclude,
	boost::container::vector<Callback> &postLockActions)
{
	return getPool()->forceFreeCapacity(exclude, postLockActions);
}

void
Group::wakeUpGarbageCollector() {
	getPool()->garbageCollectionCond.notify_all();
}

bool
Group::anotherGroupIsWaitingForCapacity() const {
	return findOtherGroupWaitingForCapacity() != NULL;
}

Group *
Group::findOtherGroupWaitingForCapacity() const {
	Pool *pool = getPool();
	if (pool->groups.size() == 1) {
		return NULL;
	}

	GroupMap::ConstIterator g_it(pool->groups);
	while (*g_it != NULL) {
		const GroupPtr &group = g_it.getValue();
		if (group.get() != this && group->isWaitingForCapacity()) {
			return group.get();
		}
		g_it.next();
	}
	return NULL;
}

bool
Group::pushGetWaiter(const Options &newOptions, const GetCallback &callback,
	boost::container::vector<Callback> &postLockActions)
{
	if (OXT_LIKELY(!testOverflowRequestQueue()
		&& (newOptions.maxRequestQueueSize == 0
		    || getWaitlist.size() < newOptions.maxRequestQueueSize)))
	{
		getWaitlist.push_back(GetWaiter(
			newOptions.copyAndPersist(),
			callback));
		return true;
	} else {
		postLockActions.push_back(boost::bind(GetCallback::call,
			callback, SessionPtr(), boost::make_shared<RequestQueueFullException>(newOptions.maxRequestQueueSize)));

		HookScriptOptions hsOptions;
		if (prepareHookScriptOptions(hsOptions, "queue_full_error")) {
			// TODO <Feb 17, 2015] DK> should probably rate limit this, since we are already at heavy load
			postLockActions.push_back(boost::bind(runHookScripts, hsOptions));
		}

		return false;
	}
}

template<typename Lock>
void
Group::assignSessionsToGetWaitersQuickly(Lock &lock) {
	if (getWaitlist.empty()) {
		verifyInvariants();
		lock.unlock();
		return;
	}

	boost::container::vector<Callback> postLockActions;
	SmallVector<GetAction, 8> actions;
	unsigned int i = 0;
	bool done = false;

	actions.reserve(getWaitlist.size());

	while (!done && i < getWaitlist.size()) {
		const GetWaiter &waiter = getWaitlist[i];
		if (requestTimedOut(waiter)) {
			postLockActions.push_back(
					boost::bind(GetCallback::call, waiter.callback, SessionPtr(),
							boost::make_shared<RequestQueueTimeoutException>(options.maxRequestQueueTimeMs)));
			getWaitlist.erase(getWaitlist.begin() + i);
			continue;
		}
		RouteResult result = route(waiter.options);
		if (result.process != NULL) {
			GetAction action;
			action.callback = waiter.callback;
			action.session  = newSession(result.process);
			getWaitlist.erase(getWaitlist.begin() + i);
			actions.push_back(action);
		} else {
			done = result.finished;
			if (!result.finished) {
				i++;
			}
		}
	}

	verifyInvariants();
	lock.unlock();
	SmallVector<GetAction, 50>::const_iterator it, end = actions.end();
	for (it = actions.begin(); it != end; it++) {
		it->callback(it->session, ExceptionPtr());
	}
	runAllActions(postLockActions);
}

void
Group::assignSessionsToGetWaiters(boost::container::vector<Callback> &postLockActions) {
	unsigned int i = 0;
	bool done = false;

	while (!done && i < getWaitlist.size()) {
		const GetWaiter &waiter = getWaitlist[i];
		if (requestTimedOut(waiter)) {
			postLockActions.push_back(
					boost::bind(GetCallback::call, waiter.callback, SessionPtr(),
							boost::make_shared<RequestQueueTimeoutException>(options.maxRequestQueueTimeMs)));
			getWaitlist.erase(getWaitlist.begin() + i);
			continue;
		}
		RouteResult result = route(waiter.options);
		if (result.process != NULL) {
			postLockActions.push_back(boost::bind(
				GetCallback::call,
				waiter.callback,
				newSession(result.process),
				ExceptionPtr()));
			getWaitlist.erase(getWaitlist.begin() + i);
		} else {
			done = result.finished;
			if (!result.finished) {
				i++;
			}
		}
	}
}

bool
Group::testOverflowRequestQueue() const {
	// This has a performance penalty, although I'm not sure whether the penalty is
	// any greater than a hash table lookup if I were to implement it in Options.
	Pool::DebugSupportPtr debug = getPool()->debugSupport;
	if (debug) {
		return debug->testOverflowRequestQueue;
	} else {
		return false;
	}
}

bool
Group::testTimeoutRequestQueue() const {
	Pool::DebugSupportPtr debug = getPool()->debugSupport;
	if (debug) {
		return debug->testTimeoutRequestQueue;
	} else {
		return false;
	}
}

void
Group::callAbortLongRunningConnectionsCallback(const ProcessPtr &process) {
	Pool::AbortLongRunningConnectionsCallback callback =
		getPool()->abortLongRunningConnectionsCallback;
	if (callback) {
		callback(process);
	}
}

inline bool
Group::requestTimedOut(const GetWaiter &waiter) {
	if (OXT_LIKELY(!testTimeoutRequestQueue())) {
		if (OXT_LIKELY(options.maxRequestQueueTimeMs == 0)) {
			return false;
		} else {
			return (OXT_UNLIKELY(
					((SystemTime::getMonotonicUsecWithGranularity<SystemTime::GRAN_1SEC>() - waiter.startTimeUsec)
							/ 1000) > options.maxRequestQueueTimeMs));
		}
	} else {
		return true;
	}
}

/* This thread patrols the request queue to evict any requests that have been waiting longer than
 * maxRequestQueueTimeMs (if set). Eviction is normally done whenever requests are taken from the
 * queue, but if that process is too slow (e.g. because app processes are jammed), this
 * thread provides timely rejects to clients.
 *
 * 'self' param keeps the Group object alive while this thread is running.
 */
void
Group::timeoutRequestsThread(GroupPtr self) {
	TRACE_POINT();
	boost::container::vector<Callback> postLockActions;
	while (true) {
		UPDATE_TRACE_POINT();
		ScopedLock l = ScopedLock(getPool()->syncher);
		if (options.maxRequestQueueTimeMs == 0) {
			// no queue timeout, wait until config changes or interrupted for exit
			queueTimeoutCheckerCond.wait(l);
		} else {
			// timely reject: within about 1.5x maxRequestQueueTimeMs
			queueTimeoutCheckerCond.timed_wait(l, posix_time::milliseconds(options.maxRequestQueueTimeMs/2));
		}
		UPDATE_TRACE_POINT();
		boost::this_thread::disable_interruption di;
		boost::this_thread::disable_syscall_interruption dsi;
		while (0 < getWaitlist.size()) {
			const GetWaiter &waiter = *getWaitlist.begin();
			if (!requestTimedOut(waiter)) {
				// stop processing because requests after this one are newer and thus cannot be timed out
				break;
			} else {
				postLockActions.push_back(
						boost::bind(GetCallback::call, waiter.callback, SessionPtr(),
								boost::make_shared<RequestQueueTimeoutException>(options.maxRequestQueueTimeMs)));
				getWaitlist.erase(getWaitlist.begin());
			}
		}

		l.unlock();
		UPDATE_TRACE_POINT();
		if (postLockActions.size() > 0) {
			runAllActions(postLockActions);
			postLockActions.clear();
		}
	}
}

} // namespace ApplicationPool2
} // namespace Passenger
