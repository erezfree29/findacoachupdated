/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2010-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */

#include <boost/bind.hpp>
#include <Shared/Fundamentals/Utils.h>
#include <Utils/AsyncSignalSafeUtils.h>

/**
 * Touch all files in the server instance dir every 6 hours in order to prevent /tmp
 * cleaners from weaking havoc:
 * http://code.google.com/p/phusion-passenger/issues/detail?id=365
 */
class InstanceDirToucher {
private:
	WorkingObjectsPtr wo;
	oxt::thread *thr;

	static void afterFork(const StaticString &originalOomScore, const char *workingDir) {
		namespace ASSU = AsyncSignalSafeUtils;
		int prio, ret, e;

		// Make process nicer.
		do {
			prio = getpriority(PRIO_PROCESS, getpid());
		} while (prio == -1 && errno == EINTR);
		if (prio != -1) {
			prio++;
			if (prio > 20) {
				prio = 20;
			}
			do {
				ret = setpriority(PRIO_PROCESS, getpid(), prio);
			} while (ret == -1 && errno == EINTR);
		} else {
			char buf[1024];
			char *pos = buf;
			const char *end = buf + sizeof(buf);

			e = errno;
			pos = ASSU::appendData(pos, end, "getpriority() failed: ");
			pos = ASSU::appendData(pos, end, ASSU::limitedStrerror(e));
			pos = ASSU::appendData(pos, end, " (errno=");
			pos = ASSU::appendInteger<int, 10>(pos, end, e);
			pos = ASSU::appendData(pos, end, ")");
			ASSU::printError(buf, pos - buf);
		}

		do {
			ret = chdir(workingDir);
		} while (ret == -1 && errno == EINTR);
		if (ret == -1) {
			char buf[1024];
			char *pos = buf;
			const char *end = buf + sizeof(buf);

			e = errno;
			pos = ASSU::appendData(pos, end, "chdir(\"");
			pos = ASSU::appendData(pos, end, workingDir);
			pos = ASSU::appendData(pos, end, "\") failed: ");
			pos = ASSU::appendData(pos, end, ASSU::limitedStrerror(e));
			pos = ASSU::appendData(pos, end, " (errno=");
			pos = ASSU::appendInteger<int, 10>(pos, end, e);
			pos = ASSU::appendData(pos, end, ")");
			ASSU::printError(buf, pos - buf);
			_exit(1);
		}

#if !BOOST_OS_MACOS
		bool isLegacy;
		ret = tryRestoreOomScore(originalOomScore, isLegacy);
		if (ret != 0) {
			char buf[1024];
			char *pos = buf;
			const char *end = buf + sizeof(buf);

			pos = ASSU::appendData(pos, end, "Unable to set OOM score to ");
			pos = ASSU::appendData(pos, end, originalOomScore.data(), originalOomScore.size());
			pos = ASSU::appendData(pos, end, " (legacy: ");
			pos = ASSU::appendData(pos, end, isLegacy ? "true" : "false");
			pos = ASSU::appendData(pos, end, ") due to error: ");
			pos = ASSU::appendData(pos, end, ASSU::limitedStrerror(ret));
			pos = ASSU::appendData(pos, end, " (errno=");
			pos = ASSU::appendInteger<int, 10>(pos, end, ret);
			pos = ASSU::appendData(pos, end, "). Process will remain at inherited OOM score.");
			ASSU::printError(buf, pos - buf);
		}
#endif
	}

	void
	threadMain() {
		string originalOomScore = wo->extraConfigToPassToSubAgents["oom_score"].asString();
		string workingDir = wo->instanceDir->getPath().c_str();

		while (!boost::this_thread::interruption_requested()) {
			syscalls::sleep(60 * 60);

			begin_touch:

			try {
				const char *command[] = {
					"/bin/sh",
					"-c",
					"find . | xargs touch",
					NULL
				};
				SubprocessInfo info;
				runCommand(command, info, true, true,
					boost::bind(afterFork, originalOomScore, workingDir.c_str()));
			} catch (const SystemException &e) {
				P_WARN("Could not touch the " PROGRAM_NAME " instance directory ("
					<< e.what() << "). Retrying in 2 minutes...");
				syscalls::sleep(60 * 2);
				goto begin_touch;
			}
		}
	}

public:
	InstanceDirToucher(const WorkingObjectsPtr &wo) {
		this->wo = wo;
		thr = new oxt::thread(boost::bind(&InstanceDirToucher::threadMain, this),
			"Server instance dir toucher", 256 * 1024);
	}

	~InstanceDirToucher() {
		thr->interrupt_and_join();
		delete thr;
	}
};

typedef boost::shared_ptr<InstanceDirToucher> InstanceDirToucherPtr;
