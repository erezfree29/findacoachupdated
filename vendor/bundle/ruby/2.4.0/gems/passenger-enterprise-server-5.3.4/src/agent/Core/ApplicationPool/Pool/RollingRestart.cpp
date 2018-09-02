/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2011-2015 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#include <Core/ApplicationPool/Pool.h>

/*************************************************************************
 *
 * Rolling restarting functions for ApplicationPool2::Pool
 *
 *************************************************************************/

namespace Passenger {
namespace ApplicationPool2 {

using namespace std;
using namespace boost;


/*
 * The restarter thread takes care of rolling restarting processes.
 * It works by going over all processes in the pool, and replacing
 * processes for which holds 'spawnerCreationTime < group.spawner.creationTime'.
 */
void
Pool::startRestarterThread() {
	if (!restarterThreadActive) {
		P_DEBUG("Starting rolling restarter thread");
		restarterThreadActive = true;
		restarterThreadStatus = "Looking for process for rolling restart...";
		restarterThreadGupid.clear();
		interruptableThreads.create_thread(
			boost::bind(&Pool::restarterThreadMain, this),
			"Restarter thread",
			POOL_HELPER_THREAD_STACK_SIZE);
	}
}

/**
 * Returns a random process within the entire pool that should be rolling restarted,
 * or null if there is no such process.
 */
ProcessPtr
Pool::findProcessNeedingRollingRestart(const set<string> &ignoreList) const {
	GroupMap::ConstIterator g_it(groups);
	while (*g_it != NULL) {
		GroupPtr group = g_it.getValue();
		ProcessPtr process = findProcessNeedingRollingRestart(group, ignoreList);
		if (process != NULL) {
			return process;
		}
		g_it.next();
	}
	return ProcessPtr();
}

/**
 * Returns a random process within a specific group that should be rolling
 * restarted, or null if there is no such process.
 */
ProcessPtr
Pool::findProcessNeedingRollingRestart(const GroupPtr &group,
	const set<string> &ignoreList) const
{
	if (!group->isAlive() || group->hasSpawnError) {
		return ProcessPtr();
	}

	ProcessPtr process;
	process = findProcessNeedingRollingRestart(group, group->enabledProcesses, ignoreList);
	if (process == NULL) {
		process = findProcessNeedingRollingRestart(group, group->disablingProcesses, ignoreList);
	}
	if (process == NULL) {
		process = findProcessNeedingRollingRestart(group, group->disabledProcesses, ignoreList);
	}
	return process;
}

ProcessPtr
Pool::findProcessNeedingRollingRestart(const GroupPtr &group,
	const ProcessList &list, const set<string> &ignoreList) const
{
	foreach (ProcessPtr process, list) {
		if (ignoreList.find(process->getGupid()) == ignoreList.end()
		 && process->getSpawnerCreationTime() < group->spawner->creationTime)
		{
			return process;
		}
	}
	return ProcessPtr();
}

void
Pool::setRestarterThreadInactive(this_thread::disable_interruption *di,
	this_thread::disable_syscall_interruption *dsi)
{
	ScopedLock l(syncher);
	restarterThreadActive = false;
	restarterThreadStatus.clear();
	restarterThreadGupid.clear();
	P_DEBUG("Rolling restarter thread done");
	if (debugSupport != NULL && debugSupport->rollingRestarting) {
		this_thread::restore_interruption ri(*di);
		this_thread::restore_syscall_interruption rsi(*dsi);
		debugSupport->debugger->send("Done rolling restarting");
	}
}

void
Pool::restarterThreadMain() {
	try {
		restarterThreadRealMain();
	} catch (const thread_interrupted &) {
		// Return.
	}
}

void
Pool::waitUntilOldProcessIsGone(const ProcessPtr &process, const string &name,
	boost::unique_lock<boost::mutex> &l)
{
	P_DEBUG("Waiting until old process " << name << " is gone...");
	restarterThreadStatus = "Waiting until old process " + name + " is gone...";
	while (!process->isDead()) {
		l.unlock();
		syscalls::usleep(100000);
		l.lock();
	}
	P_DEBUG("Old process " << name << " is now gone");
}

void
Pool::restarterThreadRealMain() {
	TRACE_POINT();
	this_thread::disable_interruption di;
	this_thread::disable_syscall_interruption dsi;
	ScopeGuard guard(boost::bind(&Pool::setRestarterThreadInactive, this, &di, &dsi));
	ScopedLock l(syncher);
	set<string> ignoreList;

	// Restaring the spawner is already taken care of by Group::finalizeRestart().
	while (true) {
		UPDATE_TRACE_POINT();
		ProcessPtr oldProcess = findProcessNeedingRollingRestart(ignoreList);
		if (oldProcess == NULL) {
			break;
		}

		UPDATE_TRACE_POINT();
		string oldProcessId = oldProcess->inspect();
		GroupPtr group = oldProcess->getGroup()->shared_from_this();
		SpawningKit::SpawnerPtr spawner = group->spawner;
		Options options = group->options.copyAndPersist();
		ProcessPtr newProcess;
		ExceptionPtr exception;

		restarterThreadStatus = "Restarting process " + oldProcessId +
			" in group " + group->getName() + "...";
		restarterThreadGupid = oldProcess->getGupid();
		P_DEBUG("Rolling restarting process " << oldProcessId <<
			" in group " << group->getName());

		l.unlock();
		UPDATE_TRACE_POINT();
		try {
			UPDATE_TRACE_POINT();
			this_thread::restore_interruption ri(di);
			this_thread::restore_syscall_interruption rsi(dsi);
			newProcess = group->createProcessObject(*spawner, spawner->spawn(options));
		} catch (const thread_interrupted &) {
			// Returning so that we don't verify invariants.
			return;
		} catch (tracable_exception &e) {
			UPDATE_TRACE_POINT();

			if (debugSupport != NULL) {
				LockGuard g(debugSupport->syncher);
				debugSupport->spawnErrors++;
				debugSupport->debugger->send("Spawn error " +
					toString(debugSupport->spawnErrors));
			}

			l.lock();
			if (group->isAlive() && oldProcess->isAlive()) {
				// Don't try to rolling restart this process next time.
				ignoreList.insert(oldProcess->getGupid());
				if (group->options.ignoreSpawnErrors) {
					group->hasSpawnError = true;
				}
			}

			SpawningKit::SpawnException *se =
				dynamic_cast<SpawningKit::SpawnException *>(&e);
			if (se != NULL) {
				processAndLogNewSpawnException(*se, options, getContext());
			} else {
				P_ERROR("Could not spawn process for group " << group->getName() <<
				": " << exception->what());
			}

			exception = copyException(e);

			continue;

			// Let other (unexpected) exceptions crash the program so
			// gdb can generate a backtrace.
		}

		ScopeGuard newProcessGuard(boost::bind(Process::forceTriggerShutdownAndCleanup,
			newProcess));

		if (debugSupport != NULL && debugSupport->rollingRestarting) {
			this_thread::restore_interruption ri(di);
			this_thread::restore_syscall_interruption rsi(dsi);
			debugSupport->debugger->send("About to attach rolling restarted process");
			debugSupport->messages->recv("Proceed with attaching rolling restarted process");
		}

		l.lock();

		UPDATE_TRACE_POINT();
		if (!group->isAlive()) {
			P_DEBUG("Group " << group->getName() << " was detached after process " <<
				oldProcessId << " has been rolling restarted; " <<
				"discarding newly spawned process " <<
				newProcess->inspect());
			continue;
		}

		UPDATE_TRACE_POINT();
		if (!oldProcess->isAlive() || oldProcess->enabled == Process::DETACHED) {
			/* Apparently the old process has gone away during the
			 * time we spent on spawning a new one. But we can
			 * replace another old process, if any.
			 */
			oldProcess = findProcessNeedingRollingRestart(group, ignoreList);
		}

		boost::container::vector<Callback> actions;
		if (oldProcess == NULL) {
			UPDATE_TRACE_POINT();
			/* The old process has gone away, and there's no process that
			 * needs to be rolling restarted. To avoid wasting the resources,
			 * add it to the group if we can.
			 */
			switch (group->attach(newProcess, actions)) {
			case AR_OK:
				newProcessGuard.clear();
				break;
			case AR_GROUP_UPPER_LIMITS_REACHED:
			case AR_POOL_AT_FULL_CAPACITY:
				P_DEBUG("Process " << oldProcessId << " was detached and the resource " <<
					"limits do not allow spawning a new process, so discarding new " <<
					"rolling restarted process " << newProcess->inspect());
				break;
			case AR_ANOTHER_GROUP_IS_WAITING_FOR_CAPACITY:
				P_DEBUG("Process " << oldProcessId << " was detached and another app " <<
					"group is waiting for capacity, so discarding new " <<
					"rolling restarted process " << newProcess->inspect());
				break;
			default:
				P_BUG("Unknown result for Group::attach()");
			}
		} else {
			UPDATE_TRACE_POINT();
			assert(oldProcess->isAlive());
			group->detach(oldProcess, actions);
			switch (group->attach(newProcess, actions)) {
			case AR_OK:
				newProcessGuard.clear();
				break;
			case AR_GROUP_UPPER_LIMITS_REACHED:
			case AR_POOL_AT_FULL_CAPACITY:
				P_DEBUG("Canceling rolling restarting process " << oldProcessId <<
					": the resource limits have changed. Discarding newly spawned " <<
					"process " << newProcess->inspect());
				break;
			case AR_ANOTHER_GROUP_IS_WAITING_FOR_CAPACITY:
				P_DEBUG("Canceling rolling restarting process " << oldProcessId <<
					": another app group is waiting for capacity. Discarding newly spawned " <<
					"process " << newProcess->inspect());
				break;
			default:
				P_BUG("Unknown result for Group::attach()");
			}
		}

		UPDATE_TRACE_POINT();
		if (!group->getWaitlist.empty()) {
			group->assignSessionsToGetWaiters(actions);
		}
		assignSessionsToGetWaiters(actions);
		// Because the rolling restarter aborted any concurrent process
		// spawning threads, we check whether we need to spawn after
		// we're done.
		possiblySpawnMoreProcessesForExistingGroups();

		UPDATE_TRACE_POINT();
		fullVerifyInvariants();

		if (oldProcess != NULL) {
			waitUntilOldProcessIsGone(oldProcess, oldProcess->inspect(), l);
		}

		if (!actions.empty()) {
			UPDATE_TRACE_POINT();
			l.unlock();
			runAllActions(actions);
			actions.clear();
			UPDATE_TRACE_POINT();
			l.lock();
			fullVerifyInvariants();
		}
	}

	UPDATE_TRACE_POINT();
	verifyInvariants();
	verifyExpensiveInvariants();
}


} // namespace ApplicationPool2
} // namespace Passenger
