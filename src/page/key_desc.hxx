/*
 * key_desc.hxx
 *
 *  Created on: 12 juil. 2014
 *      Author: gschwind
 */

#ifndef KEY_DESC_HXX_
#define KEY_DESC_HXX_

#include <string>
#include <functional>
#include <xkbcommon/xkbcommon.h>

#include "utils/exception.hxx"
#include "core/page-types.hxx"

namespace page {

using namespace std;

struct key_desc_t {
	string keysym_name;
	uint32_t keycode;
	uint32_t modmask;
	function<void(void)> handler;

	bool match(uint32_t modmask, uint32_t keycode) {
		return (keycode == this->keycode) and ((modmask&this->modmask) == this->modmask);
	}

	key_desc_t(page_keyboard * kbd, string const & desc, function<void(void)> handler) {
		_find_key_from_string(kbd, desc);
	}

	/**
	 * Parse std::string like "mod4 f" to modifier mask (mod) and keysym (ks)
	 **/
	void _find_key_from_string(page_keyboard * kbd, string const & desc);

};


}


#endif /* KEY_DESC_HXX_ */
