/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2018 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */

#include <sys/stat.h>
#include <unistd.h>

#include <cerrno>

#include <FileTools/PathSecurityCheck.h>
#include <FileTools/PathManip.h>
#include <SystemTools/UserDatabase.h>
#include <Utils.h>
#include <Utils/StrIntUtils.h>

namespace Passenger {


static bool
isSinglePathProbablySecureForRootUse(const string &path,
	vector<string> &errors, vector<string> &checkErrors)
{
	struct stat s;
	int ret;

	do {
		ret = stat(path.c_str(), &s);
	} while (ret == -1 && errno == EAGAIN);
	if (ret == -1) {
		int e = errno;
		checkErrors.push_back("Security check skipped on " + path
			+ ": stat() failed: " + strerror(e) + " (errno="
			+ toString(e) + ")");
		return true;
	}

	if (s.st_uid != 0) {
		errors.push_back(path + " is not secure: it can be modified by user "
			+ lookupSystemUsernameByUid(s.st_uid));
		return false;
	}

	if (s.st_mode & S_ISVTX) {
		return true;
	}

	if (s.st_mode & S_IWGRP) {
		errors.push_back(path + " is not secure: it can be modified by group "
			+ lookupSystemGroupnameByGid(s.st_gid));
		return false;
	}

	if (s.st_mode & S_IWOTH) {
		errors.push_back(path + " is not secure: it can be modified by anybody");
		return false;
	}

	return true;
}

bool
isPathProbablySecureForRootUse(const StaticString &path, vector<string> &errors,
	vector<string> &checkErrors)
{
	string fullPath = absolutizePath(path);
	bool result = true;

	while (!fullPath.empty() && fullPath != "/") {
		result = isSinglePathProbablySecureForRootUse(fullPath, errors, checkErrors)
			&& result;
		fullPath = extractDirName(fullPath);
	}

	return result;
}


} // namespace Passenger