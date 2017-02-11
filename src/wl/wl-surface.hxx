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
#include "wl/wl-surface-state.hxx"

namespace page {
namespace wl {

using namespace std;
using namespace wayland_cxx_wrapper;

struct wl_surface : public wl_surface_vtable {
	wl_compositor * compositor;
	MetaSurfaceActor * actor;

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

//	struct wl_resource *resource;
//	struct wl_signal destroy_signal; /* callback argument: this surface */
//	struct weston_compositor *compositor;
//	struct wl_signal commit_signal;
//
//	/** Damage in local coordinates from the client, for tex upload. */
//	pixman_region32_t damage;
//
//	pixman_region32_t opaque;        /* part of geometry, see below */
//	pixman_region32_t input;
//	int32_t width, height;
//	int32_t ref_count;
//
//	/* Not for long-term storage.  This exists for book-keeping while
//	 * iterating over surfaces and views
//	 */
//	bool touched;
//
//	void *renderer_state;
//
//	struct wl_list views;
//
//	/*
//	 * Which output to vsync this surface to.
//	 * Used to determine whether to send or queue frame events, and for
//	 * other client-visible syncing/throttling tied to the output
//	 * repaint cycle.
//	 */
//	struct weston_output *output;
//
//	/*
//	 * A more complete representation of all outputs this surface is
//	 * displayed on.
//	 */
//	uint32_t output_mask;
//
//	struct wl_list frame_callback_list;
//	struct wl_list feedback_list;
//
//	struct weston_buffer_reference buffer_ref;
//	struct weston_buffer_viewport buffer_viewport;
//	int32_t width_from_buffer; /* before applying viewport */
//	int32_t height_from_buffer;
//	bool keep_buffer; /* for backends to prevent early release */
//
//	/* wp_viewport resource for this surface */
//	struct wl_resource *viewport_resource;
//
	/* All the pending state, that wl_surface.commit will apply. */
	wl_surface_state pending;

//	/* Matrices representating of the full transformation between
//	 * buffer and surface coordinates.  These matrices are updated
//	 * using the weston_surface_build_buffer_matrix function. */
//	struct weston_matrix buffer_to_surface_matrix;
//	struct weston_matrix surface_to_buffer_matrix;
//
//	/*
//	 * If non-NULL, this function will be called on
//	 * wl_surface::commit after a new buffer has been set up for
//	 * this surface. The integer params are the sx and sy
//	 * parameters supplied to wl_surface::attach.
//	 */
//	void (*committed)(struct weston_surface *es, int32_t sx, int32_t sy);
//	void *committed_private;
//	int (*get_label)(struct weston_surface *surface, char *buf, size_t len);
//
//	/* Parent's list of its sub-surfaces, weston_subsurface:parent_link.
//	 * Contains also the parent itself as a dummy weston_subsurface,
//	 * if the list is not empty.
//	 */
//	struct wl_list subsurface_list; /* weston_subsurface::parent_link */
//	struct wl_list subsurface_list_pending; /* ...::parent_link_pending */
//
//	/*
//	 * For tracking protocol role assignments. Different roles may
//	 * have the same configure hook, e.g. in shell.c. Configure hook
//	 * may get reset, this will not.
//	 * XXX: map configure functions 1:1 to roles, and never reset it,
//	 * and replace role_name with configure.
//	 */
//	const char *role_name;
//
//	struct weston_timeline_object timeline;
//
//	bool is_mapped;
//
//	/* An list of per seat pointer constraints. */
//	struct wl_list pointer_constraints;

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
