/*
 *  Phusion Passenger - https://www.phusionpassenger.com/
 *  Copyright (c) 2014-2017 Phusion Holding B.V.
 *
 *  "Passenger", "Phusion Passenger" and "Union Station" are registered
 *  trademarks of Phusion Holding B.V.
 *
 *  See LICENSE file for license information.
 */
#ifndef _PASSENGER_SERVER_KIT_HOOKS_H_
#define _PASSENGER_SERVER_KIT_HOOKS_H_

namespace Passenger {
namespace ServerKit {


class HooksImpl;

struct Hooks {
	HooksImpl *impl;
	void *userData;
};

class HooksImpl {
public:
	virtual ~HooksImpl() { }

	virtual bool hook_isConnected(Hooks *hooks, void *source) {
		return true;
	}

	virtual void hook_ref(Hooks *hooks, void *source, const char *file, unsigned int line) { }
	virtual void hook_unref(Hooks *hooks, void *source, const char *file, unsigned int line) { }
};

struct RefGuard {
	Hooks *hooks;
	void *source;
	const char *file;
	unsigned int line;

	RefGuard(Hooks *_hooks, void *_source, const char *_file, unsigned int _line)
		: hooks(_hooks),
		  source(_source),
		  file(_file),
		  line(_line)
	{
		if (_hooks != NULL && _hooks->impl != NULL) {
			_hooks->impl->hook_ref(_hooks, _source, _file, _line);
		}
	}

	~RefGuard() {
		if (hooks != NULL && hooks->impl != NULL) {
			hooks->impl->hook_unref(hooks, source, file, line);
		}
	}
};


} // namespace ServerKit
} // namespace Passenger

#endif /* _PASSENGER_SERVER_KIT_HOOKS_H_ */
