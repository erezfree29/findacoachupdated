/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2010-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#ifndef _PASSENGER_BLOCKING_QUEUE_H_
#define _PASSENGER_BLOCKING_QUEUE_H_

#include <boost/thread.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <queue>

namespace Passenger {

using namespace std;
using namespace boost;

template<typename T>
class BlockingQueue {
private:
	mutable boost::timed_mutex lock;
	boost::condition_variable_any added;
	boost::condition_variable_any removed;
	unsigned int max;
	std::queue<T> queue;

	bool atMaxCapacity() const {
		return max > 0 && queue.size() >= max;
	}

public:
	BlockingQueue(unsigned int max = 0) {
		this->max = max;
	}

	unsigned int size() const {
		boost::lock_guard<boost::timed_mutex> l(lock);
		return queue.size();
	}

	void add(const T &item) {
		boost::unique_lock<boost::timed_mutex> l(lock);
		while (atMaxCapacity()) {
			removed.wait(l);
		}
		queue.push(item);
		added.notify_one();
		if (!atMaxCapacity()) {
			removed.notify_one();
		}
	}

	bool tryAdd(const T &item) {
		boost::lock_guard<boost::timed_mutex> l(lock);
		if (!atMaxCapacity()) {
			queue.push(item);
			added.notify_one();
			if (!atMaxCapacity()) {
				removed.notify_one();
			}
			return true;
		} else {
			return false;
		}
	}

	T get() {
		boost::unique_lock<boost::timed_mutex> l(lock);
		while (queue.empty()) {
			added.wait(l);
		}
		T item = queue.front();
		queue.pop();
		removed.notify_one();
		if (!queue.empty()) {
			added.notify_one();
		}
		return item;
	}

	bool timedGet(T &output, unsigned int timeout) {
		boost::unique_lock<boost::timed_mutex> l(lock);
		posix_time::ptime deadline = posix_time::microsec_clock::local_time() +
			posix_time::milliseconds(timeout);
		bool timedOut = false;

		while (queue.empty() && !timedOut) {
			posix_time::time_duration diff = deadline -
				posix_time::microsec_clock::local_time();
			if (diff.is_negative()) {
				timedOut = true;
			} else {
				timedOut = !added.timed_wait(l,
					posix_time::milliseconds(diff.total_milliseconds()));
			}
		}
		if (!queue.empty()) {
			output = queue.front();
			queue.pop();
			removed.notify_one();
			if (!queue.empty()) {
				added.notify_one();
			}
			return true;
		} else {
			return false;
		}
	}

	bool tryGet(T &output) {
		boost::unique_lock<boost::timed_mutex> l(lock);
		if (queue.empty()) {
			return false;
		} else {
			output = queue.front();
			queue.pop();
			removed.notify_one();
			if (!queue.empty()) {
				added.notify_one();
			}
			return true;
		}
	}

	T peek() {
		boost::unique_lock<boost::timed_mutex> l(lock);
		while (queue.empty()) {
			added.wait(l);
		}
		return queue.front();
	}
};

} // namespace Passenger

#endif /* _PASSENGER_BLOCKING_QUEUE_H_ */
