/*
 * config_handler.hxx
 *
 * copyright (2010-2016) Benoit Gschwind
 *
 * This code is licensed under the GPLv3. see COPYING file for more details.
 *
 */

#ifndef CONFIG_HANDLER_HXX_
#define CONFIG_HANDLER_HXX_

#include <utility>
#include <map>
#include <string>

namespace page {

using namespace std;

class config_handler_t {

	// key are compound by (group,identifier)
	typedef pair<string, string> _key_t;
	map<_key_t, string> _data;

	auto find(char const * group, char const * key) const -> string const &;

	config_handler_t(config_handler_t const &) = delete;
	config_handler_t & operator=(config_handler_t const &) = delete;
public:
	config_handler_t();
	~config_handler_t();

	void merge_from_file_if_exist(string const & filename);

	bool has_key(char const * groups, char const * key) const;

	auto get_value(char const * group, char const * key) const -> string;
	auto get_string(char const * groups, char const * key) const -> string;
	auto get_double(char const * groups, char const * key) const -> double;
	auto get_long(char const * group, char const * key) const -> long;

};


}



#endif /* CONFIG_HANDLER_HXX_ */
