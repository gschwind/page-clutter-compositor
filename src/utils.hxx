/*
 * Copyright (2016) Benoit Gschwind
 *
 * utils.hxx is part of page-compositor.
 *
 * page-compositor is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * page-compositor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with page-compositor.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef SRC_UTILS_HXX_
#define SRC_UTILS_HXX_

#include "wayland-server-core.h"

#include <memory>
#include <map>
#include <list>
#include <functional>
#include <vector>

namespace page {

using namespace std;

template<typename T>
T * resource_get(struct wl_resource * r) {
	return reinterpret_cast<T *>(wl_resource_get_user_data(r));
}


template <typename T>
bool is_expired(weak_ptr<T> & x) {
	return x.expired();
}

/**
 * /!\ also remove expired !
 **/
template<typename T0>
vector<shared_ptr<T0>> lock(list<weak_ptr<T0>> & x) {
	x.remove_if(is_expired<T0>);
	vector<shared_ptr<T0>> ret;
	for(auto i: x) ret.push_back(i.lock());
	return ret;
}

using slot = shared_ptr<void>;

template<typename ... F>
class signal {
	using _func_t = std::function<void(F ...)>;
	list<weak_ptr<_func_t>> _callback_list;

public:

	signal() : _callback_list{} { }
	~signal() { }

	// default connect
	slot connect(void(*func)(F ...)) {
		auto ret = make_shared<_func_t>(func);
		_callback_list.push_front(weak_ptr<_func_t>{ret});
		return std::static_pointer_cast<void>(ret);
	}

	slot connect(_func_t func) {
		auto ret = make_shared<_func_t>(func);
		_callback_list.push_front(weak_ptr<_func_t>{ret});
		return std::static_pointer_cast<void>(ret);
	}

	/**
	 * Connect a function to this signal and return a reference to this function. This function
	 * is not owned by the signal (i.e. signal only keep a weak ptr to the function). The caller
	 * must keep the returned shared ptr until he want to remove the signal. The caller can
	 * disconnect signal or turn the shared_ptr to nullptr to remove the handler from the signal queue.
	 **/
	template<typename T0>
	slot connect(T0 * ths, void(T0::*func)(F ...)) {
		auto ret = make_shared<_func_t>([ths, func](F ... args) -> void {
			(ths->*func)(args...);
		});
		_callback_list.push_front(weak_ptr<_func_t>{ret});
		return std::static_pointer_cast<void>(ret);
	}

	void remove(slot s) {
		auto _s = std::static_pointer_cast<_func_t>(s);
		_callback_list.remove_if([_s] (weak_ptr<_func_t> & x) -> bool {
			if(x.expired())
				return true;
			if(x.lock() == _s)
				return true;
			return false;
		});
	}

	void emit(F ... args) {
		/**
		 * Copy the list of callback to avoid issue
		 * if 'remove' is called during the signal.
		 **/
		auto callbacks = lock(_callback_list);

		for(auto func: callbacks) {
			(*func)(args...);
		}
	}

};

class connectable {
	map<void *, slot> _signal_handlers;

public:

	connectable() { }
	virtual ~connectable() { }

	template<typename T, typename ... Args>
	void connect(signal<Args...> &sig, T * x, void (T::* func)(Args...)) {
		_signal_handlers[&sig] = sig.connect(x, func);
	}

	template<typename ... Args>
	void disconnect(signal<Args...> &sig) {
		_signal_handlers.erase(&sig);
	}

};


}

#endif /* SRC_UTILS_HXX_ */
