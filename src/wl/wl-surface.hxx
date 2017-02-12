/*
 * Copyright (2016) Benoit Gschwind
 *
 * surface.hxx is part of page-compositor.
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

#ifndef SRC_WL_WL_SURFACE_HXX_
#define SRC_WL_WL_SURFACE_HXX_

#include <cairo/cairo.h>
#include <list>
#include <clutter/wayland/clutter-wayland-surface.h>

#include "meta/meta-surface-actor-wayland.hxx"

#include "utils/signals.hxx"
#include "wayland-interface.hxx"
#include "wl-types.hxx"
#include "page/ui-types.hxx"
#include "wl/wl-surface-state.hxx"

namespace page {
namespace wl {

using namespace std;
using namespace wayland_cxx_wrapper;

struct wl_surface : public wl_surface_vtable {
	wl_compositor * compositor;

	MetaSurfaceActor * actor;

	// Set when a role is given, can only have one role at a time.
	surface_t * role;

	/* current state of the surface */
	wl_buffer * buffer;
	cairo_region_t * input_region;
	cairo_region_t * opaque_region;
	list<wl_callback*> frame_callback_list;

	wl_subsurface * subsurface;

	int32_t width;
	int32_t height;

	/* Parent's list of its sub-surfaces, weston_subsurface:parent_link.
	 * Contains also the parent itself as a dummy weston_subsurface,
	 * if the list is not empty. */
	list<wl_subsurface *> subsurface_list;
	list<wl_subsurface *> subsurface_pending_list;

	signal<wl_surface *> on_commit;

	/* All the pending state, that wl_surface.commit will apply. */
	wl_surface_state pending;

	wl_surface(wl_compositor * compositor, struct wl_client *client, uint32_t version, uint32_t id);
	virtual ~wl_surface();

	static wl_surface * get(struct wl_resource *r);
	void process_damage(cairo_region_t *region);
	void state_set_buffer(wl_surface_state * state, wl_buffer * buffer);
	void commit_state(wl_surface_state & state);
	void synchronize_subsurface_stack();
	void commit_synchronized_state_recursively();
	void get_relative_coordinates(float abs_x, float abs_y, float & sx, float & sy) const;

	/* wl_surface_vtable */
	virtual void recv_destroy(struct wl_client * client, struct wl_resource * resource) override;
	virtual void recv_attach(struct wl_client * client, struct wl_resource * resource, struct wl_resource * buffer, int32_t x, int32_t y) override;
	virtual void recv_damage(struct wl_client * client, struct wl_resource * resource, int32_t x, int32_t y, int32_t width, int32_t height) override;
	virtual void recv_frame(struct wl_client * client, struct wl_resource * resource, uint32_t callback) override;
	virtual void recv_set_opaque_region(struct wl_client * client, struct wl_resource * resource, struct wl_resource * region) override;
	virtual void recv_set_input_region(struct wl_client * client, struct wl_resource * resource, struct wl_resource * region) override;
	virtual void recv_commit(struct wl_client * client, struct wl_resource * resource) override;
	virtual void recv_set_buffer_transform(struct wl_client * client, struct wl_resource * resource, int32_t transform) override;
	virtual void recv_set_buffer_scale(struct wl_client * client, struct wl_resource * resource, int32_t scale) override;
	virtual void recv_damage_buffer(struct wl_client * client, struct wl_resource * resource, int32_t x, int32_t y, int32_t width, int32_t height) override;
	virtual void delete_resource(struct wl_resource * resource) override;
};

}
} /* namespace page */

#endif /* SRC_WL_WL_SURFACE_HXX_ */
