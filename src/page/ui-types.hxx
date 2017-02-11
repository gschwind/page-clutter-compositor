/*
 * Copyright (2017) Benoit Gschwind
 *
 * xpage-types.hxx is part of page-compositor.
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

#ifndef SRC_PAGE_UI_TYPES_HXX_
#define SRC_PAGE_UI_TYPES_HXX_

#include "tree-types.hxx"
#include "key_desc.hxx"

namespace page {

struct page_t;
struct surface_t;
struct theme_t;

enum managed_window_type_e : uint32_t {
	MANAGED_UNCONFIGURED,
	MANAGED_FLOATING,
	MANAGED_NOTEBOOK,
	MANAGED_FULLSCREEN,
	MANAGED_DOCK,
	MANAGED_POPUP
};

enum edge_e : uint32_t {
	EDGE_NONE = 0,
	EDGE_TOP = 1,
	EDGE_BOTTOM = 2,
	EDGE_LEFT = 4,
	EDGE_TOP_LEFT = 5,
	EDGE_BOTTOM_LEFT = 6,
	EDGE_RIGHT = 8,
	EDGE_TOP_RIGHT = 9,
	EDGE_BOTTOM_RIGHT = 10
};

}



#endif /* SRC_PAGE_UI_TYPES_HXX_ */
