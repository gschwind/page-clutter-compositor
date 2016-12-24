/*
 * Copyright (2016) Benoit Gschwind
 *
 * wl-subsurface.hxx is part of page-compositor.
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

#ifndef WL_WL_SUBSURFACE_HXX_
#define WL_WL_SUBSURFACE_HXX_

#include <list>

#include "wayland-interface.hxx"
#include "wl-types.hxx"
#include "wl/wl-surface-state.hxx"

namespace page {
namespace wl {

using namespace std;
using namespace wcxx;

struct  wl_subsurface : public wl_subsurface_vtable {
	wl_surface * surface;

	/* can be NULL ? */
	wl_surface * parent;

	//struct wl_listener surface_destroy_listener;
	//struct wl_listener parent_destroy_listener;

	struct {
		int32_t x;
		int32_t y;
		bool set;
	} position;

	bool has_cached_data;
	//struct weston_surface_state cached;
	//struct weston_buffer_reference cached_buffer_ref;

	/* store the offset of this subsurface */
	int32_t offset_x;
	int32_t offset_y;

	bool has_pending_state;
	wl_surface_state synchronized_pending_state;
	bool synchronized;

	/* Used for constructing the view tree */
	//struct wl_list unused_views;

	wl_subsurface(struct wl_client *client, uint32_t version, uint32_t id, wl_surface * surfaces, wl_surface * parent);
	virtual ~wl_subsurface();

	auto find_sibling_in_pending_list(wl_surface * sibling) -> list<wl_subsurface*>::iterator;
	/* callled by the surface when she get commited */
	void commit();

	bool is_synchronized();
	void commit_state(wl_surface_state & state);

	/* wl_subsurface_vtable */
	virtual void recv_destroy(struct wl_client * client, struct wl_resource * resource) override;
	virtual void recv_set_position(struct wl_client * client, struct wl_resource * resource, int32_t x, int32_t y) override;
	virtual void recv_place_above(struct wl_client * client, struct wl_resource * resource, struct wl_resource * sibling) override;
	virtual void recv_place_below(struct wl_client * client, struct wl_resource * resource, struct wl_resource * sibling) override;
	virtual void recv_set_sync(struct wl_client * client, struct wl_resource * resource) override;
	virtual void recv_set_desync(struct wl_client * client, struct wl_resource * resource) override;
	virtual void delete_resource(struct wl_resource * resource) override;

};

}
} /* namespace page */

#endif /* WL_WL_SUBSURFACE_HXX_ */
