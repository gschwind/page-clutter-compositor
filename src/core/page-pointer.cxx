/*
 * Copyright (2016) Benoit Gschwind
 *
 * page-pointer.cxx is part of page-compositor.
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

#include "page-pointer.hxx"

#include <cassert>
#include <algorithm>
#include <linux/input.h>

#include "utils/utils.hxx"
#include "page-default-pointer-grab.hxx"
#include "page-seat.hxx"
#include "wl/wl-surface.hxx"
#include "wl/wl-pointer.hxx"

namespace page {

using namespace std;

page_pointer::page_pointer(page_seat * seat) :
		focus_sx{0},
		focus_sy{0},
		focus_serial{0},
		focus_surface{nullptr},
		seat{seat},
		button_count{0},
		x{0.0},
		y{0.0}
{
	// TODO Auto-generated constructor stub

	default_grab = nullptr;
	grab = nullptr;

	auto manager = clutter_device_manager_get_default();
	device = clutter_device_manager_get_core_device(manager, CLUTTER_POINTER_DEVICE);

}

page_pointer::~page_pointer() {

}

/**
 * Find an actor under the cursor
 * @input event: may be NULL
 * @return: the actor where the cursor is above.
 **/
auto page_pointer::pick_actor(ClutterEvent const * event) -> ClutterActor *
{
	ClutterActor *actor;

	if (event)
		actor = clutter_event_get_source(event);
	else
		actor = clutter_input_device_get_pointer_actor(device);

	printf("actor = %p\n", actor);
	printf("actor type = %s\n", g_type_name(G_TYPE_FROM_INSTANCE(actor)));

	return actor;

}

/**
 * Find a wl_surface actor under the cursor
 * @input event: may be NULL
 * @output sx: relative position of pointer for the surface returned
 * @output sy: relative position of pointer for the surface returned
 * @return: the surface where the cursor is above.
 **/
wl::wl_surface * page_pointer::pick_surface_actor(ClutterEvent const * event, wl_fixed_t & sx, wl_fixed_t & sy)
{

	auto actor = pick_actor(event);

	if(not META_IS_SURFACE_ACTOR_WAYLAND(actor)) {
		return nullptr;
	}

	auto surface = meta_surface_actor_wayland_get_surface(META_SURFACE_ACTOR_WAYLAND(actor));
	get_relative_coordinates(*surface, sx, sx);
	return surface;

}


int page_pointer::count_buttons(ClutterEvent const & event)
{
	static gint maskmap[] = {
			CLUTTER_BUTTON1_MASK,
			CLUTTER_BUTTON2_MASK,
			CLUTTER_BUTTON3_MASK,
			CLUTTER_BUTTON4_MASK,
			CLUTTER_BUTTON5_MASK
	};
	ClutterModifierType mod_mask;
	int i, count;

	mod_mask = clutter_event_get_state(&event);
	count = 0;
	for (i = 0; i < countof(maskmap); i++) {
		if (mod_mask & maskmap[i])
			count++;
	}

	return count;
}

void page_pointer::handle_pointer_event(ClutterEvent const & event) {
	update_pointer_focus_for_event(event);

	switch(event.type) {
	case CLUTTER_BUTTON_PRESS:
	case CLUTTER_BUTTON_RELEASE:
		grab->button(event);
		break;
	case CLUTTER_MOTION:
		grab->motion(event);
		break;
	case CLUTTER_SCROLL:
		grab->axis(event);
		break;
	default:
		break;
	}
}

/**
 * This function must be call on every clutter events related to pointer :
 * CLUTTER_MOTION, CLUTTER_BUTTON_PRESS, CLUTTER_BUTTON_RELEASE, CLUTTER_SCROLL
 **/
void page_pointer::update_pointer_focus_for_event(ClutterEvent const & event)
{
	grab->focus(event);
	button_count = count_buttons(event);

	clutter_event_get_coords(&event, &x, &y);

	//sync_focus_surface();
	//meta_wayland_pointer_update_cursor_surface (pointer);
}

void page_pointer::set_focus(wl::wl_surface * surface, wl_fixed_t sx, wl_fixed_t sy)
{
//	if (display == NULL)
//		return;

	if (focus_surface == surface)
		return;

	if (focus_surface != nullptr) {
		auto client = wl_resource_get_client(focus_surface->_self_resource);
		auto display = wl_client_get_display(client);
		uint32_t serial;

		serial = wl_display_next_serial(display);

		if (focus_surface) {
			broadcast_leave(serial, focus_surface);
			focus_surface = NULL;
		}

		//wl_list_remove(&pointer->focus_surface_listener.link);
		focus_surface = NULL;
	}

	if (surface != NULL) {
		struct wl_client *client = wl_resource_get_client(surface->_self_resource);
		struct wl_display *display = wl_client_get_display(client);
		ClutterPoint pos;

		focus_surface = surface;
		//wl_resource_add_destroy_listener(pointer->focus_surface->resource,
		//		&pointer->focus_surface_listener);

		clutter_input_device_get_coords(device, NULL, &pos);

//		if (focus_surface->window)
//			meta_window_handle_enter(focus_surface,
//			/* XXX -- can we reliably get a timestamp for setting focus? */
//			clutter_get_current_event_time(), pos.x, pos.y);

		auto range = client_pointers.equal_range(client);
		if (range.first != range.second) {
			focus_serial = wl_display_next_serial(display);
			broadcast_enter(focus_serial, focus_surface);
		}
	}

	//meta_wayland_pointer_update_cursor_surface (pointer);
}

void page_pointer::register_pointer(wl::wl_pointer * p)
{
	auto client = wl_resource_get_client(p->_self_resource);
	client_pointers.insert(make_pair(client, p));
}

void page_pointer::unregister_pointer(wl::wl_pointer * p)
{
	auto client = wl_resource_get_client(p->_self_resource);
	auto range = client_pointers.equal_range(client);
	auto x = find_if(range.first, range.second,
			[p](pair<struct wl_client *, wl::wl_pointer *> x) -> bool { return p == x.second; });
	if(x != client_pointers.end())
		client_pointers.erase(x);
}

void page_pointer::get_relative_coordinates(wl::wl_surface & surface, wl_fixed_t & sx, wl_fixed_t & sy)
{
	float xf = 0.0f, yf = 0.0f;
	ClutterPoint pos;

	clutter_input_device_get_coords(device, NULL, &pos);
	surface.get_relative_coordinates(pos.x, pos.y, xf, yf);

	sx = wl_fixed_from_double(xf);
	sy = wl_fixed_from_double(yf);
}

void page_pointer::broadcast_button(ClutterEvent const & event)
{
	auto event_type = clutter_event_type(&event);

	if (focus_surface) {
		auto client = wl_resource_get_client(focus_surface->_self_resource);
		auto range = client_pointers.equal_range(client);
		if(range.first != range.second) {

		auto display = wl_client_get_display(client);
		uint32_t time;
		uint32_t button;
		uint32_t serial;

#ifdef HAVE_NATIVE_BACKEND
		MetaBackend *backend = meta_get_backend ();
		if (META_IS_BACKEND_NATIVE (backend))
		button = clutter_evdev_event_get_event_code (event);
		else
#endif
		{
			button = clutter_event_get_button(&event);
			switch (button) {
			case 1:
				button = BTN_LEFT;
				break;

				/* The evdev input right and middle button numbers are swapped
				 relative to how Clutter numbers them */
			case 2:
				button = BTN_MIDDLE;
				break;

			case 3:
				button = BTN_RIGHT;
				break;

			default:
				button = button + (BTN_LEFT - 1) + 4;
				break;
			}
		}

		time = clutter_event_get_time(&event);
		serial = wl_display_next_serial(display);

		for(auto i = range.first; i != range.second; ++i) {
			i->second->send_button(serial, time, button,
					event_type == CLUTTER_BUTTON_PRESS ? WL_POINTER_BUTTON_STATE_PRESSED : WL_POINTER_BUTTON_STATE_RELEASED);
		}

		broadcast_frame();
	}

	}

	if (button_count == 0 && event_type == CLUTTER_BUTTON_RELEASE)
		grab->focus(event);

}


void page_pointer::broadcast_motion(ClutterEvent const & event)
{
	struct wl_resource *resource;
	uint32_t time;
	float sx, sy;

	if (!focus_surface)
		return;

	time = clutter_event_get_time(&event);
	focus_surface->get_relative_coordinates(event.motion.x, event.motion.y, sx, sy);

	auto client = wl_resource_get_client(focus_surface->_self_resource);
	auto range = client_pointers.equal_range(client);
	for(auto i = range.first; i != range.second; ++i) {
		i->second->send_motion(time, wl_fixed_from_double(sx), wl_fixed_from_double(sy));
	}

	//meta_wayland_pointer_send_relative_motion(pointer, event);

	broadcast_frame();

}

void page_pointer::broadcast_enter(uint32_t serial, wl::wl_surface * surface)
{
	wl_fixed_t sx, sy;
	get_relative_coordinates(*surface, sx, sy);
	auto client = wl_resource_get_client(surface->_self_resource);
	auto range = client_pointers.equal_range(client);
	for(auto i = range.first; i != range.second; ++i) {
		i->second->send_enter(serial, surface->_self_resource, sx, sy);
	}
	broadcast_frame();
}

void page_pointer::broadcast_leave(uint32_t serial, wl::wl_surface * surface)
{

	auto client = wl_resource_get_client(surface->_self_resource);
	auto range = client_pointers.equal_range(client);
	for(auto i = range.first; i != range.second; ++i) {
		i->second->send_leave(serial, surface->_self_resource);
	}
	broadcast_frame();
}

void page_pointer::broadcast_frame()
{
	auto client = wl_resource_get_client(focus_surface->_self_resource);
	auto range = client_pointers.equal_range(client);
	for(auto i = range.first; i != range.second; ++i) {
		i->second->send_frame();
	}
}

void page_pointer::set_default_grab(shared_ptr<page_pointer_grab> g)
{
	grab = g;
	default_grab = g;
}

void page_pointer::start_grab(shared_ptr<page_pointer_grab> g)
{
	assert(grab == default_grab);
	grab = g;
}

void page_pointer::stop_grab()
{
	assert(grab != default_grab);
	grab = default_grab;
}

} /* namespace page */
