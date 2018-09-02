/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2010-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#include <boost/thread.hpp>
#include <oxt/system_calls.hpp>
#include <string>
#include <cerrno>

#include <ProcessManagement/Spawn.h>
#include <ProcessManagement/Utils.h>
#include <StaticString.h>
#include <Exceptions.h>
#include <Utils/IOUtils.h>

namespace Passenger {

using namespace std;


int
runShellCommand(const StaticString &command) {
	string commandNt = command;
	const char *argv[] = {
		"/bin/sh",
		"-c",
		commandNt.c_str(),
		NULL
	};
	SubprocessInfo info;
	runCommand(argv, info);
	return info.status;
}

void
runCommand(const char **command, SubprocessInfo &info, bool wait, bool killSubprocessOnInterruption,
	const boost::function<void ()> &afterFork,
	const boost::function<void (const char **, int errcode)> &onExecFail)
{
	int e, waitStatus;
	pid_t waitRet;

	info.pid = asyncFork();
	if (info.pid == 0) {
		resetSignalHandlersAndMask();
		disableMallocDebugging();
		if (afterFork) {
			afterFork();
		}
		closeAllFileDescriptors(2, true);
		execvp(command[0], (char * const *) command);
		if (onExecFail) {
			onExecFail(command, errno);
		}
		_exit(1);
	} else if (info.pid == -1) {
		e = errno;
		throw SystemException("Cannot fork() a new process", e);
	} else if (wait) {
		try {
			waitRet = syscalls::waitpid(info.pid, &waitStatus, 0);
		} catch (const boost::thread_interrupted &) {
			if (killSubprocessOnInterruption) {
				boost::this_thread::disable_syscall_interruption dsi;
				syscalls::kill(SIGKILL, info.pid);
				syscalls::waitpid(info.pid, NULL, 0);
			}
			throw;
		}

		if (waitRet != -1) {
			info.status = waitStatus;
		} else if (errno == ECHILD || errno == ESRCH) {
			info.status = -2;
		} else {
			int e = errno;
			throw SystemException(string("Error waiting for the '") +
				command[0] + "' command", e);
		}
	}
}

void
runCommandAndCaptureOutput(const char **command, SubprocessInfo &info,
	string &output, bool killSubprocessOnInterruption,
	const boost::function<void ()> &afterFork,
	const boost::function<void (const char **command, int errcode)> &onExecFail)
{
	pid_t waitRet;
	int e, waitStatus;
	Pipe p;

	p = createPipe(__FILE__, __LINE__);

	info.pid = asyncFork();
	if (info.pid == 0) {
		dup2(p[1], 1);
		close(p[0]);
		close(p[1]);
		resetSignalHandlersAndMask();
		disableMallocDebugging();
		if (afterFork) {
			afterFork();
		}
		closeAllFileDescriptors(2, true);
		execvp(command[0], (char * const *) command);
		if (onExecFail) {
			onExecFail(command, errno);
		}
		_exit(1);
	} else if (info.pid == -1) {
		e = errno;
		throw SystemException("Cannot fork() a new process", e);
	} else {
		bool done = false;

		p[1].close();
		while (!done) {
			char buf[1024 * 4];
			ssize_t ret;

			try {
				ret = syscalls::read(p[0], buf, sizeof(buf));
			} catch (const boost::thread_interrupted &) {
				if (killSubprocessOnInterruption) {
					boost::this_thread::disable_syscall_interruption dsi;
					syscalls::kill(SIGKILL, info.pid);
					syscalls::waitpid(info.pid, NULL, 0);
				}
				throw;
			}
			if (ret == -1) {
				e = errno;
				if (killSubprocessOnInterruption) {
					boost::this_thread::disable_syscall_interruption dsi;
					syscalls::kill(SIGKILL, info.pid);
					syscalls::waitpid(info.pid, NULL, 0);
				}
				throw SystemException(string("Cannot read output from the '") +
					command[0] + "' command", e);
			}
			done = ret == 0;
			output.append(buf, ret);
		}
		p[0].close();

		try {
			waitRet = syscalls::waitpid(info.pid, &waitStatus, 0);
		} catch (const boost::thread_interrupted &) {
			if (killSubprocessOnInterruption) {
				boost::this_thread::disable_syscall_interruption dsi;
				syscalls::kill(SIGKILL, info.pid);
				syscalls::waitpid(info.pid, NULL, 0);
			}
			throw;
		}

		if (waitRet != -1) {
			info.status = waitStatus;
		} else if (errno == ECHILD || errno == ESRCH) {
			info.status = -2;
		} else {
			int e = errno;
			throw SystemException(string("Error waiting for the '") +
				command[0] + "' command", e);
		}
	}
}


} // namespace Passenger
