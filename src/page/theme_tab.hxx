/*
 * theme_tab.hxx
 *
 *  Created on: 1 oct. 2014
 *      Author: gschwind
 */

#ifndef THEME_TAB_HXX_
#define THEME_TAB_HXX_

#include "utils/color.hxx"

namespace page {

struct theme_tab_t {
	rect position;
	std::string title;
	color_t tab_color;
	bool is_iconic;

	theme_tab_t() :
		position{},
		title{},
		is_iconic{},
		tab_color{}
	{ }

	theme_tab_t(theme_tab_t const & x) :
		position{x.position},
		title{x.title},
		is_iconic{x.is_iconic},
		tab_color{x.tab_color}
	{ }

};

}



#endif /* THEME_TAB_HXX_ */
