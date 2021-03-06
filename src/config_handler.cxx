/*
 * config_handler.cxx
 *
 * copyright (2010-2016) Benoit Gschwind
 *
 * This code is licensed under the GPLv3. see COPYING file for more details.
 *
 */

#include "config_handler.hxx"

#include <stdexcept>
#include <sstream>
#include <string>
#include <memory>
#include <cstring>

#include <unistd.h>
#include <glib.h>

namespace page {

using namespace std;

config_handler_t::config_handler_t() {

}

config_handler_t::~config_handler_t() {
	_data.clear();
}

/* this function merge config file, the last one override previous loaded files */
void config_handler_t::merge_from_file_if_exist(string const & f) {

	/* check if file exist and is readable */
	if (access(f.c_str(), R_OK) != 0)
		return;

	GKeyFile * kf = g_key_file_new();
	if (kf == 0)
		throw runtime_error("could not allocate memory");

	if (!g_key_file_load_from_file(kf, f.c_str(), G_KEY_FILE_NONE, 0)) {
		throw runtime_error("could not load configuration file");
	}

	gchar ** groups = 0;
	gsize groups_length;

	groups = g_key_file_get_groups(kf, &groups_length);

	if (groups != 0) {
		for (unsigned g = 0; g < groups_length; ++g) {

			gchar ** keys = 0;
			gsize keys_length;

			keys = g_key_file_get_keys(kf, groups[g], &keys_length, 0);

			if (keys != 0) {
				for (unsigned k = 0; k < keys_length; ++k) {
					gchar * value = g_key_file_get_value(kf, groups[g], keys[k],
							0);
					if (value != 0) {
						_data[_key_t(groups[g], keys[k])] = value;
						g_free(value);
					}
				}
				g_strfreev(keys);
			}
		}
		g_strfreev(groups);
	}
	g_key_file_free(kf);
}

auto config_handler_t::get_string(char const * group, char const * key) const -> string {
	string const & tmp = find(group, key);
	shared_ptr<gchar> p_ret{g_strcompress(tmp.c_str()), &g_free};
	return string(p_ret.get());
}

auto config_handler_t::get_double(char const * group, char const * key) const -> double{
	std::string const & tmp = find(group, key);
	return g_strtod(tmp.c_str(), NULL);
}

auto config_handler_t::get_long(char const * group, char const * key) const -> long {
	std::string const & tmp = find(group, key);
	return g_ascii_strtoll(tmp.c_str(), NULL, 10);
}

bool config_handler_t::has_key(char const * group, char const * key) const {
	auto x = _data.find(_key_t(group, key));
	if(x == _data.end())
		return false;
	return true;
}

auto config_handler_t::find(char const * group, char const * key) const -> string const & {
	auto x = _data.find(_key_t(group, key));
	if(x == _data.end())
		throw except("following group/key %s:%s not found", group, key);
	return x->second;
}

}


