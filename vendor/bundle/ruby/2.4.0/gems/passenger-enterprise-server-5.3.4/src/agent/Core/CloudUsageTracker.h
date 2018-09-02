/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2014-2018 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#ifndef _PASSENGER_CLOUD_USAGE_TRACKER_H_
#define _PASSENGER_CLOUD_USAGE_TRACKER_H_

#include <string>
#include <map>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <vector>
#include <utility>

#include <cstdio>
#include <ctime>
#include <cerrno>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <limits.h>

#include <curl/curl.h>

#include <uv.h>
#include <boost/config.hpp>
#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <boost/scoped_ptr.hpp>
#include <oxt/thread.hpp>
#include <oxt/system_calls.hpp>
#include <oxt/backtrace.hpp>
#include <jsoncpp/json.h>
#include <LoggingKit/LoggingKit.h>
#include <ConfigKit/ConfigKit.h>
#include <Exceptions.h>
#include <Constants.h>
#include <FileDescriptor.h>
#include <FileTools/FileManip.h>
#include <ProcessManagement/Spawn.h>
#include <SystemTools/UserDatabase.h>
#include <Utils/IOUtils.h>
#include <Utils/StrIntUtils.h>
#include <Utils/SystemTime.h>
#include <Utils/License.h>
#include <Utils/Curl.h>

namespace Passenger {

using namespace std;
using namespace oxt;


class CloudUsageTracker {
public:
	/**
	 * BEGIN ConfigKit schema: Passenger::CloudUsageTracker::Schema
	 * (do not edit: following text is automatically generated
	 * by 'rake configkit_schemas_inline_comments')
	 *
	 *   auto_send           boolean            -   default(true)
	 *   check_certificate   boolean            -   default(true)
	 *   data_dir            string             -   default
	 *   proxy_url           string             -   -
	 *   threshold           unsigned integer   -   default(288)
	 *   url                 string             -   default("https://www.phusionpassenger.com/cloud_service/track_usage")
	 *
	 * END
	 */
	class Schema: public ConfigKit::Schema {
	private:
		static void validateProxyUrl(const ConfigKit::Store &config, vector<ConfigKit::Error> &errors) {
			if (config["proxy_url"].isNull()) {
				return;
			}
			if (config["proxy_url"].asString().empty()) {
				errors.push_back(ConfigKit::Error("'{{proxy_url}}', if specified, may not be empty"));
				return;
			}

			try {
				prepareCurlProxy(config["proxy_url"].asString());
			} catch (const ArgumentException &e) {
				errors.push_back(ConfigKit::Error(
					P_STATIC_STRING("'{{proxy_url}}': ")
					+ e.what()));
			}
		}

		static Json::Value getDefaultDataDir(const ConfigKit::Store &config) {
			return getHomeDir() + ("/" USER_NAMESPACE_DIRNAME "/usage_data");
		}

	public:
		Schema() {
			using namespace ConfigKit;

			addWithDynamicDefault("data_dir", STRING_TYPE, OPTIONAL, getDefaultDataDir);
			add("url", STRING_TYPE, OPTIONAL, "https://www.phusionpassenger.com/cloud_service/track_usage");
			add("proxy_url", STRING_TYPE, OPTIONAL);
			add("check_certificate", BOOL_TYPE, OPTIONAL, true);
			add("auto_send", BOOL_TYPE, OPTIONAL, true);
			add("threshold", UINT_TYPE, OPTIONAL, 3 * 24 * 4);

			addValidator(validateProxyUrl);

			finalize();
		}
	};

	struct ConfigRealization {
		string url;
		string dataDir;
		CurlProxyInfo proxyInfo;

		ConfigRealization(const ConfigKit::Store &config)
			: url(config["url"].asString()),
			  dataDir(config["data_dir"].asString()),
			  proxyInfo(prepareCurlProxy(config["proxy_url"].asString()))
			{ }

		void swap(ConfigRealization &other) BOOST_NOEXCEPT_OR_NOTHROW {
			url.swap(other.url);
			dataDir.swap(other.dataDir);
			proxyInfo.swap(other.proxyInfo);
		}
	};

	struct ConfigChangeRequest {
		boost::scoped_ptr<ConfigKit::Store> config;
		boost::scoped_ptr<ConfigRealization> configRlz;
	};

protected:
	/*
	 * Since the cloud usage tracker runs in a separate thread,
	 * and the configuration can change while the tracker is active,
	 * we make a copy of the current configuration at the beginning
	 * of each check.
	 */
	struct SessionState {
		ConfigKit::Store config;
		ConfigRealization configRlz;

		SessionState(const ConfigKit::Store &currentConfig,
			const ConfigRealization &currentConfigRlz)
			: config(currentConfig),
			  configRlz(currentConfigRlz)
			{ }
	};

	struct UsagePointComparator {
		bool operator()(const string &a, const string &b) const {
			return atoll(a.c_str()) < atoll(b.c_str());
		}
	};

	typedef vector< pair<const char *, string> > MachineProperties;

	mutable boost::mutex configSyncher;
	ConfigKit::Store config;
	ConfigRealization configRlz;

	string hostname;
	bool machinePropertiesDetected;
	MachineProperties savedMachineProperties;
	oxt::thread *thr;
	char lastErrorMessage[CURL_ERROR_SIZE];

	void handleLicenseError(const string &message) {
		TRACE_POINT();
		if (licenseErrorHandler) {
			licenseErrorHandler(message);
		} else {
			P_ERROR(message);
			sendEmailToRoot(message);
		}
	}

	void sendEmailToRoot(const string &message) {
		string recipients = "root";
		string currentUser = lookupSystemUsernameByUid(geteuid(),
			P_STATIC_STRING("%d"));
		if (!currentUser.empty() && currentUser != "root") {
			recipients.append(",");
			recipients.append(currentUser);
		}

		P_INFO("Sending email about licensing problem to the following user(s): " + recipients);
		try {
			bool sent = sendEmailToRootUsingMail(recipients, message);
			if (!sent) {
				P_WARN("Could not send email using 'mail'. Trying again using 'sendmail'...");
				sent = sendEmailToRootUsingSendmail(recipients, message);
				if (!sent) {
					P_WARN("Could not send email using 'sendmail'. Giving up");
				}
			}
			if (sent) {
				P_INFO("Email successfully sent");
			}
		} catch (const RuntimeException &e) {
			P_WARN(e.what());
		}
	}

	bool sendEmailToRootUsingMail(const string &recipients, const string &message) {
		TRACE_POINT();
		char path[PATH_MAX];
		snprintf(path, PATH_MAX, "%s/passenger-email.XXXXXXXX", getSystemTempDir());
		int fd = mkstemp(path);
		if (fd != -1) {
			try {
				writeExact(fd, P_STATIC_STRING("\n"));
				writeExact(fd, message);
				writeExact(fd, P_STATIC_STRING("\n.\n"));
			} catch (const SystemException &e) {
				P_ERROR("Cannot write to " << path << ": " << e.what());
				close(fd);
				return false;
			}
			close(fd);

			bool result = runShellCommand("env PATH=/usr/sbin:$PATH mail "
				"-s '" PROGRAM_NAME " Enterprise cloud licensing problem' "
				"'" + recipients + "' < '" +
				string(path) + "'") == 0;
			unlink(path);
			return result;
		} else {
			throw RuntimeException("Could not send email: unable to create a temp file");
		}
	}

	bool sendEmailToRootUsingSendmail(const string &recipients, const string &message) {
		TRACE_POINT();
		char path[PATH_MAX];
		snprintf(path, PATH_MAX, "%s/passenger-email.XXXXXXXX", getSystemTempDir());
		int fd = mkstemp(path);
		if (fd != -1) {
			try {
				writeExact(fd, P_STATIC_STRING("To: "));
				writeExact(fd, recipients);
				writeExact(fd, P_STATIC_STRING("\n"
					"Subject: " PROGRAM_NAME " Enterprise cloud licensing problem\n"
					"\n"));
				writeExact(fd, message);
				writeExact(fd, P_STATIC_STRING("\n.\n"));
			} catch (const SystemException &e) {
				P_ERROR("Cannot write to " << path << ": " << e.what());
				close(fd);
				return false;
			}
			close(fd);

			bool result = runShellCommand("env PATH=/usr/sbin:$PATH sendmail -t < '" +
				string(path) + "'") == 0;
			unlink(path);
			return result;
		} else {
			throw RuntimeException("Could not send email: unable to create a temp file");
		}
	}

	void recordUsagePoint(SessionState &sessionState) {
		TRACE_POINT();
		time_t now = SystemTime::get();
		const MachineProperties &properties = autodetectMachineProperties();
		const uint64_t ram = autoDetectAvailableRAM();
		string filename = sessionState.configRlz.dataDir + "/" + toString(now);
		P_DEBUG("Recording usage point: " << filename);
		FILE *f = fopen(filename.c_str(), "w");
		if (f == NULL) {
			stringstream message;
			message << "Cannot write to the Phusion Passenger Enterprise " <<
				"licensing data directory (" << sessionState.configRlz.dataDir << "). Please " <<
				"ensure that that directory exists and is writable by user '" <<
				lookupSystemUsernameByUid(geteuid()) << "'.";
			throw RuntimeException(message.str());
		}
		setvbuf(f, NULL, _IOFBF, 0);
		fprintf(f, "%lld\n%llu\n", (long long) now, (unsigned long long) ram);
		MachineProperties::const_iterator it;
		for (it = properties.begin(); it != properties.end(); it++) {
			const pair<const char *, string> &p = *it;
			fprintf(f, "%s|%s\n", p.first, p.second.data());
		}
		if (flushFile(f) != 0) {
			int e = errno;
			fclose(f);
			unlink(filename.c_str());
			stringstream message;
			message << "An I/O error occurred while recording a usage point: " <<
				strerror(errno) << " (errno=" << e << ")";
			throw RuntimeException(message.str());
		} else {
			fsync(fileno(f));
			fclose(f);
		}
	}

	vector<string> listUsagePoints(SessionState &sessionState) {
		TRACE_POINT();
		DIR *dir = opendir(sessionState.configRlz.dataDir.c_str());
		if (dir == NULL) {
			stringstream message;
			message << "Cannot read the " PROGRAM_NAME " Enterprise " <<
				"licensing data directory (" << sessionState.configRlz.dataDir <<
				"). Please ensure that that directory exists and is readable by user '" <<
				lookupSystemUsernameByUid(geteuid()) << "'.";
			throw RuntimeException(message.str());
		}

		UPDATE_TRACE_POINT();
		vector<string> result;
		struct dirent *ent;
		bool noerror = true;
		while ((ent = readdir(dir)) != NULL && noerror) {
			if (ent->d_name[0] == '.') {
				continue;
			}

			UPDATE_TRACE_POINT();
			result.push_back(ent->d_name);
		}

		closedir(dir);
		std::sort(result.begin(), result.end(), UsagePointComparator());
		return result;
	}

	unsigned int sendUsagePoints(SessionState &sessionState, const vector<string> &usagePoints) {
		TRACE_POINT();
		unsigned int result = 0;
		unsigned int skipped = 0;
		unsigned int errors = 0;

		foreach (const string &name, usagePoints) {
			UPDATE_TRACE_POINT();
			string filename = sessionState.configRlz.dataDir + "/" + name;
			string content;
			P_DEBUG("Sending usage point " << filename << " to " << sessionState.configRlz.url);
			try {
				content = unsafeReadFile(filename);
			} catch (const SystemException &e) {
				if (e.code() == ENOENT) {
					// Ignore, file has been removed while iterating.
				} else {
					throw;
				}
			}
			if (content.empty()) {
				P_DEBUG("Skipping empty file");
				skipped++;
				unlink(filename.c_str());
			} else {
				UPDATE_TRACE_POINT();
				if (sendUsagePoint(sessionState, content)) {
					result++;
					unlink(filename.c_str());
				} else {
					P_DEBUG("Could not send usage point: " << content);
					errors++;
				}
			}
		}
		P_DEBUG("Sent " << result << " usage points, skipped " << skipped
			<< " empty points, and errored on " << errors << " usage points in this cycle");
		return result;
	}

	static size_t curlDataReceived(void *buffer, size_t size, size_t nmemb, void *userData) {
		string *str = (string *) userData;
		str->append((const char *) buffer, size * nmemb);
		return size * nmemb;
	}

	static bool validateResponse(const Json::Value &response) {
		if (response.isObject() && response["status"].isString()) {
			string status = response["status"].asString();
			if (status == "ok") {
				return true;
			} else if (status == "error") {
				return response["message"].isString();
			} else {
				return false;
			}
		} else {
			return false;
		}
	}

	bool sendUsagePoint(SessionState &sessionState, const string &content) {
		TRACE_POINT();
		string responseData;
		CURLcode code = performCurlAction(sessionState, content, responseData);

		if (code == 0) {
			Json::Reader reader;
			Json::Value response;

			if (!reader.parse(responseData, response, false) || !validateResponse(response)) {
				P_WARN("Could not contact the " PROGRAM_NAME " Enterprise licensing server "
					"(parse error: " << reader.getFormattedErrorMessages().c_str() << "; data: \"" <<
					cEscapeString(responseData) << "\"). To ensure proper access to the licensing " <<
					"server, please try these:\n"
					"- Ensure that your network connection to https://www.phusionpassenger.com works.\n"
					"- If you can only access https://www.phusionpassenger.com via a proxy, "
					"please set the config option 'PassengerCtl licensing_proxy_url PROXY_URL' (Apache) "
					"or 'passenger_ctl licensing_proxy_url PROXY_URL' (Nginx). 'PROXY_URL' takes the format of "
					"protocol://username:password@hostname:port, where 'protocol' is either 'http' or 'socks5'.\n"
					"Please contact support@phusionpassenger.com if you require any assistance.");
				return false;
			} else if (response["status"].asString() == "ok") {
				return true;
			} else {
				// response == error
				stringstream message;
				message << "There is a problem with your " PROGRAM_NAME " Enterprise "
					"license. Please contact support@phusionpassenger.com if you "
					"require assistance. The problem is as follows: " <<
					response["message"].asString();
				handleLicenseError(message.str());
				return false;
			}
		} else {
			// Failed, but stay quiet if it is because we are terminating (this happens
			// all the time when starting in the Apache integration).
			if (!boost::this_thread::interruption_requested()) {
				P_WARN("Could not contact the " PROGRAM_NAME " Enterprise licensing server "
					"(HTTP error: " << lastErrorMessage << "). "
					"To ensure proper access to the licensing server, please try these:\n"
					"- Ensure that your network connection to https://www.phusionpassenger.com works.\n"
					"- If you can only access https://www.phusionpassenger.com via a proxy, "
					"please set the config option 'PassengerCtl licensing_proxy_url PROXY_URL' (Apache) "
					"or 'passenger_ctl licensing_proxy_url PROXY_URL' (Nginx). 'PROXY_URL' takes the format of "
					"protocol://username:password@hostname:port, where 'protocol' is either 'http' or 'socks5'.\n"
					"Please contact support@phusionpassenger.com if you require any assistance.");
			}
			return false;
		}
	}

	const MachineProperties &autodetectMachineProperties() {
		TRACE_POINT();
		if (!machinePropertiesDetected) {
			P_DEBUG("Autodetecting machine properties");
			if ((machinePropertiesDetected = autodetectAmazonInstanceType(savedMachineProperties))) {
				P_DEBUG("Machine properties detected (Amazon instance type)");
			} else if ((machinePropertiesDetected = autodetectHerokuProperties(savedMachineProperties))) {
				P_DEBUG("Machine properties detected (Heroku dyno)");
			} else {
				P_DEBUG("Machine properties autodetection unsuccessful");
			}
		}
		return savedMachineProperties;
	}

	virtual uint64_t autoDetectAvailableRAM() {
		TRACE_POINT();
		P_DEBUG("Autodetecting available RAM");
		return uv_get_total_memory();
	}

	virtual bool autoDetectInContainer() {
#if BOOST_OS_LINUX
		ifstream input("/proc/self/cgroup");
		string line;
		while (getline(input, line)) {
			if (line.find(":cpu:") != std::string::npos) {
				return line.substr(line.rfind(':')+1) != "/";
			}
		}
#endif
		return false;
	}

	virtual string checkContainerHostIdentifier() {
		TRACE_POINT();
		P_DEBUG("Checking container host identifier");
		const char *host = getenv("CONTAINER_HOST_IDENTIFIER");
		if (NULL == host) {
			P_ERROR("Checking container host identifier failed (CONTAINER_HOST_IDENTIFIER envvar not set), defaulting to hostname. Deduplication cannot be handled automatically.");
			return getHostName();
		} else {
			return string(host);
		}
	}

	// Virtual so that unit tests can stub it.
	// We need to special case Heroku for our RAM-based usage calculation because Heroku doesn't
	// hide the unavailable memory between dynos (e.g. a 512 MB dyno might see 60 GB free RAM).
	virtual bool autodetectHerokuProperties(MachineProperties &properties) const {
		struct stat statBuf;

		TRACE_POINT();
		P_DEBUG("Autodetecting Heroku dyno properties");

		// if /etc/heroku exists..
		if (stat("/etc/heroku", &statBuf) != 0) {
			P_DEBUG("Marker file not found, assuming not on Heroku.");
			return false;
		}

		// ..try to detect the CGROUP memory limit (Heroku contacted us with this official recommendation)
		try {
			string cgroupMemLimit = strip(unsafeReadFile("/sys/fs/cgroup/memory/memory.limit_in_bytes"));
			P_DEBUG("Autodetected Heroku dyno cgroup memory limit: " << cgroupMemLimit);
			properties.push_back(make_pair("heroku_cgroup_memory_limit", cgroupMemLimit));
		} catch (const SystemException &e) {
			P_WARN("Heroku dyno cgroup memory detection failed: " << e.what());
		}

		// ..as a fallback, our old method was the process limit, which is highly correlated with dyno RAM limit
		errno = 0;
		string processLimit = boost::to_string(sysconf(_SC_CHILD_MAX));
		if (errno != 0) {
			P_WARN("Heroku dyno process limit detection failed (errno = " << errno << ")");
			// sending processLimit -1 to backend to inform of failure
		} else {
			P_DEBUG("Autodetected Heroku dyno process limit: " << processLimit);
		}
		properties.push_back(make_pair("heroku_process_limit", processLimit));

		const char *dyno = getenv("DYNO");
		if (dyno != NULL) {
			P_DEBUG("Autodetected Heroku $DYNO (experimental): " << dyno);
			properties.push_back(make_pair("heroku_env_dyno", string(dyno)));
		}

		return true;
	}

	// Virtual so that unit tests can stub it.
	virtual bool autodetectAmazonInstanceType(MachineProperties &properties) const {
		TRACE_POINT();
		P_DEBUG("Autodetecting Amazon instance type");
		string responseData;
		char lastErrorMessage[CURL_ERROR_SIZE] = "unknown error";
		CURL *curl = curl_easy_init();

		curl_easy_setopt(curl, CURLOPT_URL, "http://169.254.169.254/2014-02-25/meta-data/instance-type");
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15);
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, lastErrorMessage);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlDataReceived);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

		CURLcode code;
		if (CURLE_OK == (code = setCurlDefaultCaInfo(curl))) {
			code = curl_easy_perform(curl);
		}

		if (code == 0) {
			long responseCode;

			if (curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode) != CURLE_OK) {
				P_ERROR("Cannot not autodetect Amazon instance type (internal error: could "
					"not query libcurl response code). Assuming this is not an Amazon instance");
				curl_easy_cleanup(curl);
				return false;
			}

			curl_easy_cleanup(curl);

			// On some machines, we get a responseCode of 0 even though the response body
			// is correct. The reason is unknown.
			if (responseCode != 200 && responseCode != 0) {
				P_ERROR("Cannot not autodetect Amazon instance type (HTTP error: response "
					"code " << responseCode << "; body \"" << cEscapeString(responseData) <<
					"\"). Assuming this is not an Amazon instance");
				return false;
			}

			if (responseData.empty()) {
				P_ERROR("Cannot not autodetect Amazon instance type (HTTP error: the server "
					"returned an empty response). Assuming this is not an Amazon instance");
				return false;
			}

			P_DEBUG("Autodetected Amazon instance type: " << responseData);
			properties.push_back(make_pair("aws_instance_type", responseData));
			return true;
		} else {
			P_DEBUG("Cannot contact Amazon metadata server (HTTP error: " << lastErrorMessage <<
				"). Assuming this is not an Amazon instance");
			curl_easy_cleanup(curl);
			return false;
		}
	}

	// Virtual so that unit tests can stub it.
	virtual CURLcode performCurlAction(SessionState &sessionState, const string &content, string &responseData)
	{
		TRACE_POINT();
		CURL *curl = curl_easy_init();

		strncpy(lastErrorMessage, "unknown error", CURL_ERROR_SIZE);
		curl_easy_setopt(curl, CURLOPT_URL, sessionState.configRlz.url.c_str());
		curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 180);
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, lastErrorMessage);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlDataReceived);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);
		if (!sessionState.config["check_certificate"].asBool()) {
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
		}
		setCurlProxy(curl, sessionState.configRlz.proxyInfo);

		struct curl_httppost *post = NULL;
		struct curl_httppost *last = NULL;
		string now = toString(SystemTime::get());
		istringstream iss(content);
		string timestamp;
		getline(iss, timestamp);
		string ram;
		getline(iss, ram);

		curl_formadd(&post, &last,
			CURLFORM_PTRNAME, "content",
			CURLFORM_PTRCONTENTS, timestamp.data(),
			CURLFORM_CONTENTSLENGTH, (long) timestamp.size(),
			CURLFORM_END);
		curl_formadd(&post, &last,
			CURLFORM_PTRNAME, "license_key",
			CURLFORM_PTRCONTENTS, licenseKey,
			CURLFORM_CONTENTSLENGTH, (long) strlen(licenseKey),
			CURLFORM_END);
		curl_formadd(&post, &last,
			CURLFORM_PTRNAME, "hostname",
			CURLFORM_PTRCONTENTS, hostname.data(),
			CURLFORM_CONTENTSLENGTH, (long) hostname.size(),
			CURLFORM_END);
		curl_formadd(&post, &last,
			CURLFORM_PTRNAME, "time",
			CURLFORM_PTRCONTENTS, now.data(),
			CURLFORM_CONTENTSLENGTH, (long) now.size(),
			CURLFORM_END);
		curl_formadd(&post, &last,
			CURLFORM_PTRNAME, "total_memory",
			CURLFORM_PTRCONTENTS, ram.data(),
			CURLFORM_CONTENTSLENGTH, (long) ram.size(),
			CURLFORM_END);
		string containerHostId;
		if (autoDetectInContainer()) {
			containerHostId = checkContainerHostIdentifier();
			curl_formadd(&post, &last,
				CURLFORM_PTRNAME, "container_host_id",
				CURLFORM_PTRCONTENTS, containerHostId.data(),
				CURLFORM_CONTENTSLENGTH, (long) containerHostId.size(),
				CURLFORM_END);
		}

		string line;
		typedef pair<string,string> keyvalue;
		vector<keyvalue> list;
		while (getline(iss, line)) {
			size_t delim = line.find("|");
			string value = line.substr(delim+1);
			string key = line.substr(0, delim);
			list.push_back(keyvalue(key,value));
		}
		foreach (const keyvalue &keyValue, list) {
			curl_formadd(&post, &last,
				CURLFORM_PTRNAME, keyValue.first.data(),
				CURLFORM_NAMELENGTH, (long) keyValue.first.size(),
				CURLFORM_PTRCONTENTS, keyValue.second.data(),
				CURLFORM_CONTENTSLENGTH, (long) keyValue.second.size(),
				CURLFORM_END);
		}

		curl_easy_setopt(curl, CURLOPT_HTTPPOST, post);

		CURLcode code;
		if (CURLE_OK == (code = setCurlDefaultCaInfo(curl))) {
			code = curl_easy_perform(curl);
		}
		curl_formfree(post);
		curl_easy_cleanup(curl);

		return code;
	}

	// Stubable by unit tests.
	virtual int flushFile(FILE *f) const {
		return fflush(f);
	}

	void threadMain() {
		TRACE_POINT();
		while (!boost::this_thread::interruption_requested()) {
			UPDATE_TRACE_POINT();
			try {
				runOneCycle();
			} catch (const tracable_exception &e) {
				P_ERROR(e.what() << "\n" << e.backtrace());
			}
			UPDATE_TRACE_POINT();
			syscalls::usleep(15 * 60 * 1000000);
		}
	}

public:
	boost::function<void (const string &message)> licenseErrorHandler;

	CloudUsageTracker(const Schema &schema, const Json::Value &initialConfig,
		const ConfigKit::Translator &translator = ConfigKit::DummyTranslator())
		: config(schema, initialConfig, translator),
		  configRlz(config),
		  machinePropertiesDetected(false),
		  thr(NULL)
	{
		long size = sysconf(_SC_HOST_NAME_MAX) + 1;
		char buf[size];
		memset(buf, 0, size);
		if (gethostname(buf, size) == 0) {
			hostname = buf;
		} else {
			int e = errno;
			throw SystemException("Cannot query the host name", e);
		}
	}

	virtual ~CloudUsageTracker() {
		if (thr != NULL) {
			thr->interrupt_and_join();
			delete thr;
		}
	}

	/*
	 * Assumes curl_global_init() was already performed.
	 */
	void start() {
		thr = new oxt::thread(
			boost::bind(&CloudUsageTracker::threadMain, this),
			"Cloud usage tracker",
			1024 * 512
		);
	}

	bool runOneCycle() {
		TRACE_POINT();
		bool result;
		P_DEBUG("Begin tracking usage cycle");

		boost::unique_lock<boost::mutex> l(configSyncher);
		SessionState sessionState(config, configRlz);
		l.unlock();

		recordUsagePoint(sessionState);
		if (sessionState.config["auto_send"].asBool()) {
			vector<string> usagePoints = listUsagePoints(sessionState);
			unsigned int sent = sendUsagePoints(sessionState, usagePoints);
			if (usagePoints.size() - sent > sessionState.config["threshold"].asUInt()) {
				handleLicenseError(PROGRAM_NAME " Enterprise hasn't been able to contact "
					"the licensing server (https://www.phusionpassenger.com) for "
					"more than 3 days.\n"
					"- Please ensure that your network connection to https://www.phusionpassenger.com works.\n"
					"- If you can only access https://www.phusionpassenger.com via a proxy, "
					"please set the config option 'PassengerCtl licensing_proxy_url PROXY_URL' (Apache) "
					"or 'passenger_ctl licensing_proxy_url PROXY_URL' (Nginx). 'PROXY_URL' takes the format of "
					"protocol://username:password@hostname:port, where 'protocol' is either 'http' or 'socks5'.\n"
					"If the problem persists, please contact support@phusionpassenger.com.");
			}
			result = usagePoints.size() - sent == 0;
		} else {
			P_DEBUG("Not auto-sending usage cycle");
			result = false;
		}
		P_DEBUG("Done tracking usage cycle");
		return result;
	}

	Json::Value dumpMachineProperties() {
		const MachineProperties &properties = autodetectMachineProperties();
		Json::Value doc(Json::objectValue);
		MachineProperties::const_iterator it;

		for (it = properties.begin(); it != properties.end(); it++) {
			const pair<const char *, string> &p = *it;
			doc[string(p.first)] = p.second;
		}
		doc["total_memory"] = autoDetectAvailableRAM();
		if (autoDetectInContainer()) {
			doc["container_host_id"] = checkContainerHostIdentifier();
		}
		return doc;
	}

	bool prepareConfigChange(const Json::Value &updates,
		vector<ConfigKit::Error> &errors, ConfigChangeRequest &req)
	{
		{
			boost::lock_guard<boost::mutex> l(configSyncher);
			req.config.reset(new ConfigKit::Store(config, updates, errors));
		}
		if (errors.empty()) {
			req.configRlz.reset(new ConfigRealization(*req.config));
		}
		return errors.empty();
	}

	void commitConfigChange(ConfigChangeRequest &req) BOOST_NOEXCEPT_OR_NOTHROW {
		boost::lock_guard<boost::mutex> l(configSyncher);
		config.swap(*req.config);
		configRlz.swap(*req.configRlz);
	}
};


} // namespace Passenger

#endif /* _PASSENGER_CLOUD_USAGE_TRACKER_H_ */
