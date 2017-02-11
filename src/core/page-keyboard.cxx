/*
 * Copyright (2016) Benoit Gschwind
 *
 * page-keyboard.cxx is part of page-compositor.
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

#include "page-keyboard.hxx"

#include "page-seat.hxx"

#include <cstring>
#include <algorithm>
#include <sys/mman.h>
#include <clutter/x11/clutter-x11.h>
#include <X11/Xlib-xcb.h>
#include <xkbcommon/xkbcommon-x11.h>

#include "utils/utils.hxx"
#include "page-default-keyboard-grab.hxx"
#include "wl/wl-keyboard.hxx"
#include "wl/wl-surface.hxx"

namespace page {

page_keyboard::page_keyboard(page_seat * seat) :
		focus_surface{nullptr},
		focus_serial{0},
		key_serial{0},
		seat{seat}
{
	keyboard_info.keymap_fd = -1;
	default_grab = new page_default_keyboard_grab{this};
	grab = default_grab;

	take_keymap(x11_get_keymap());

}

page_keyboard::~page_keyboard() {
	// TODO Auto-generated destructor stub
}

void page_keyboard::inform_clients_of_new_keymap()
{

	for (auto const & i : client_keyboards) {
		i.second->send_keymap(WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1, keyboard_info.keymap_fd,
				keyboard_info.keymap_size);
	}

	if (not focus_surface)
		return;

	auto client = wl_resource_get_client(focus_surface->_self_resource);
	auto range = client_keyboards.equal_range(client);
	for (auto & i = range.first; i != range.second; ++i) {
		i->second->send_keymap(WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1, keyboard_info.keymap_fd,
				keyboard_info.keymap_size);
	}

}

/* replace old keyboard state by a new one */
void page_keyboard::update_xkb_state()
{
	xkb_mod_mask_t latched, locked, group;
	/* Preserve latched/locked modifiers state */
	if (keyboard_info.state) {
		latched = xkb_state_serialize_mods(keyboard_info.state,
				XKB_STATE_MODS_LATCHED);
		locked = xkb_state_serialize_mods(keyboard_info.state,
				XKB_STATE_MODS_LOCKED);
		group = xkb_state_serialize_layout(keyboard_info.state,
				XKB_STATE_LAYOUT_EFFECTIVE);
		xkb_state_unref(keyboard_info.state);
	} else
		latched = locked = group = 0;
	keyboard_info.state = xkb_state_new(keyboard_info.keymap);
	if (latched || locked || group)
		xkb_state_update_mask(keyboard_info.state, 0, latched, locked, 0, 0, group);
}

/* Thi is only valid for X11 backend. TODO; update */
struct xkb_keymap * page_keyboard::x11_get_keymap()
{
	printf("XXX %s\n", clutter_check_windowing_backend (CLUTTER_WINDOWING_X11)?"x11": "not x11");
	printf("XXX %s\n", clutter_check_windowing_backend (CLUTTER_WINDOWING_GDK)?"gdk": "not gdk");

	auto xdisplay = clutter_x11_get_default_display();
	auto xcb = XGetXCBConnection(xdisplay);
	struct xkb_keymap * keymap = NULL;

	if (keymap == NULL) {
		struct xkb_context *context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);
		keymap = xkb_x11_keymap_new_from_device(context, xcb,
				xkb_x11_get_core_keyboard_device_id(xcb),
				XKB_KEYMAP_COMPILE_NO_FLAGS);
		if (keymap == NULL)
			keymap = xkb_keymap_new_from_names(context, NULL,
					XKB_KEYMAP_COMPILE_NO_FLAGS);

		xkb_context_unref(context);
	}

	return keymap;
}

void page_keyboard::take_keymap(struct xkb_keymap *keymap)
{
	GError *error = NULL;
	char *keymap_str;
	size_t previous_size;

	if (keymap == NULL) {
		printf("Attempting to set null keymap (compilation probably failed)");
		return;
	}

	xkb_keymap_unref(keyboard_info.keymap);
	keyboard_info.keymap = xkb_keymap_ref(keymap);

	update_xkb_state();

	/* retrieve modified index for binding */
	keyboard_info.shift_mod = xkb_keymap_mod_get_index(keyboard_info.keymap, XKB_MOD_NAME_SHIFT);
	keyboard_info.caps_mod = xkb_keymap_mod_get_index(keyboard_info.keymap, XKB_MOD_NAME_CAPS);
	keyboard_info.ctrl_mod = xkb_keymap_mod_get_index(keyboard_info.keymap, XKB_MOD_NAME_CTRL);
	keyboard_info.alt_mod = xkb_keymap_mod_get_index(keyboard_info.keymap, XKB_MOD_NAME_ALT);
	keyboard_info.mod2_mod = xkb_keymap_mod_get_index(keyboard_info.keymap, "Mod2");
	keyboard_info.mod3_mod = xkb_keymap_mod_get_index(keyboard_info.keymap, "Mod3");
	keyboard_info.super_mod = xkb_keymap_mod_get_index(keyboard_info.keymap, XKB_MOD_NAME_LOGO);
	keyboard_info.mod5_mod = xkb_keymap_mod_get_index(keyboard_info.keymap, "Mod5");

	keymap_str = xkb_map_get_as_string(keyboard_info.keymap);
	if (keymap_str == NULL) {
		printf("failed to get string version of keymap");
		return;
	}
	previous_size = keyboard_info.keymap_size;
	keyboard_info.keymap_size = strlen(keymap_str) + 1;

	if (keyboard_info.keymap_fd >= 0)
		close(keyboard_info.keymap_fd);

	keyboard_info.keymap_fd = create_anonymous_file(keyboard_info.keymap_size, &error);
	if (keyboard_info.keymap_fd < 0) {
		printf("creating a keymap file for %lu bytes failed: %s",
				(unsigned long ) keyboard_info.keymap_size, error->message);
		g_clear_error(&error);
		goto err_keymap_str;
	}

	if (keyboard_info.keymap_area)
		munmap(keyboard_info.keymap_area, previous_size);

	keyboard_info.keymap_area = reinterpret_cast<char*>(mmap(nullptr, keyboard_info.keymap_size,
			PROT_READ | PROT_WRITE, MAP_SHARED, keyboard_info.keymap_fd, 0));
	if (keyboard_info.keymap_area == MAP_FAILED) {
		g_warning("failed to mmap() %lu bytes\n",
				(unsigned long ) keyboard_info.keymap_size);
		goto err_dev_zero;
	}
	strcpy(keyboard_info.keymap_area, keymap_str);
	free(keymap_str);

	inform_clients_of_new_keymap();

	if(grab)
		grab->modifiers();

	return;

err_dev_zero:
	close(keyboard_info.keymap_fd);
	keyboard_info.keymap_fd = -1;
err_keymap_str:
	free(keymap_str);
	return;
}

bool page_keyboard::handle_keyboard_event(ClutterEvent const & event)
{
	bool is_press = (event.type == CLUTTER_KEY_PRESS);
	bool handled;

	/* Synthetic key events are for autorepeat. Ignore those, as
	 * autorepeat in Wayland is done on the client side. */
	if (event.key.flags & CLUTTER_EVENT_FLAG_SYNTHETIC)
		return false;

	{ // update key state
		gboolean is_press = (event.type == CLUTTER_KEY_PRESS);

		/* If we get a key event but still have pending modifier state
		 * changes from a previous event that didn't get cleared, we need to
		 * send that state right away so that the new key event can be
		 * interpreted by clients correctly modified. */
		if (mods_changed)
			grab->modifiers();

		mods_changed = xkb_state_update_key(keyboard_info.state,
				event.key.hardware_keycode, is_press ? XKB_KEY_DOWN : XKB_KEY_UP);
	}

	//meta_verbose("Handling key %s event code %d\n",
	//		is_press ? "press" : "release", event->hardware_keycode);

	handled = grab->key(event);

//	if (handled)
//		meta_verbose("Sent event to wayland client\n");
//	else
//		meta_verbose(
//				"No wayland surface is focused, continuing normal operation\n");

	if (mods_changed != 0) {
		grab->modifiers();
		mods_changed = static_cast<decltype(mods_changed)>(0);
	}

	return handled;
}

/* This fonction eat the key events if we have a focused surface. */
bool page_keyboard::broadcast_key(uint32_t time, uint32_t key, uint32_t state)
{
	if(not focus_surface)
		return true;

	auto client = wl_resource_get_client(focus_surface->_self_resource);
	auto range = client_keyboards.equal_range(client);

	if (range.first == range.second)
		return focus_surface != nullptr;

	auto display = wl_client_get_display(client);

	key_serial = wl_display_next_serial(display);
	for (auto i = range.first; i != range.second; ++i) {
		i->second->send_key(key_serial, time, key, state);
	}

	return focus_surface != nullptr;
}

//static xkb_mod_mask_t add_vmod(xkb_mod_mask_t mask, xkb_mod_mask_t mod,
//		xkb_mod_mask_t vmod, xkb_mod_mask_t *added)
//{
//	if ((mask & mod) && !(mod & *added)) {
//		mask |= vmod;
//		*added |= mod;
//	}
//	return mask;
//}
//
//static xkb_mod_mask_t
//add_virtual_mods (xkb_mod_mask_t mask)
//{
//  MetaKeyBindingManager *keys = &(meta_get_display ()->key_binding_manager);
//  xkb_mod_mask_t added;
//  guint i;
//  /* Order is important here: if multiple vmods share the same real
//     modifier we only want to add the first. */
//  struct {
//    xkb_mod_mask_t mod;
//    xkb_mod_mask_t vmod;
//  } mods[] = {
//    { keys->super_mask, keys->virtual_super_mask },
//    { keys->hyper_mask, keys->virtual_hyper_mask },
//    { keys->meta_mask,  keys->virtual_meta_mask },
//  };
//
//  added = 0;
//  for (i = 0; i < countof(mods); ++i)
//    mask = add_vmod (mask, mods[i].mod, mods[i].vmod, &added);
//
//  return mask;
//}

void page_keyboard::broadcast_modifiers()
{
	if(not focus_surface)
		return;

	auto client = wl_resource_get_client(focus_surface->_self_resource);
	auto range = client_keyboards.equal_range(client);

	if (range.first == range.second)
		return;

	auto display = wl_client_get_display(client);
	uint32_t serial = wl_display_next_serial(display);

	struct xkb_state *state = keyboard_info.state;
	xkb_mod_mask_t depressed, latched, locked, group;

	depressed = xkb_state_serialize_mods(state, XKB_STATE_MODS_DEPRESSED);
	latched = xkb_state_serialize_mods(state, XKB_STATE_MODS_LATCHED);
	locked = xkb_state_serialize_mods(state, XKB_STATE_MODS_LOCKED);
	group = xkb_state_serialize_layout(state, XKB_STATE_LAYOUT_EFFECTIVE);

	for (auto i = range.first; i != range.second; ++i) {
		i->second->send_modifiers(serial, depressed, latched, locked, group);
	}

}

void page_keyboard::register_keyboard(wl::wl_keyboard * k)
{
	auto client = wl_resource_get_client(k->_self_resource);
	client_keyboards.insert(make_pair(client, k));
}

void page_keyboard::unregister_keyboard(wl::wl_keyboard * k)
{
	auto client = wl_resource_get_client(k->_self_resource);
	auto range = client_keyboards.equal_range(client);
	auto x = find_if(range.first, range.second,
			[k](pair<struct wl_client *, wl::wl_keyboard *> x) -> bool { return k == x.second; });
	if(x != client_keyboards.end())
		client_keyboards.erase(x);
}

void page_keyboard::broadcast_focus()
{

	if(not focus_surface)
		return;

	struct wl_array fake_keys;

	/* We never want to send pressed keys to wayland clients on
	 * enter. The protocol says that we should send them, presumably so
	 * that clients can trigger their own key repeat routine in case
	 * they are given focus and a key is physically pressed.
	 *
	 * Unfortunately this causes some clients, in particular Xwayland,
	 * to register key events that they really shouldn't handle,
	 * e.g. on an Alt+Tab keybinding, where Alt is released before Tab,
	 * clients would see Tab being pressed on enter followed by a key
	 * release event for Tab, meaning that Tab would be processed by
	 * the client when it really shouldn't.
	 *
	 * Since the use case for the pressed keys array on enter seems weak
	 * to us, we'll just fake that there are no pressed keys instead
	 * which should be spec compliant even if it might not be true.
	 */
	wl_array_init(&fake_keys);

	struct wl_client *client = wl_resource_get_client(
			focus_surface->_self_resource);

	auto range = client_keyboards.equal_range(client);
	if (range.first != range.second) {
		struct xkb_state *state = keyboard_info.state;
		xkb_mod_mask_t depressed, latched, locked, group;

		depressed = xkb_state_serialize_mods(state, XKB_STATE_MODS_DEPRESSED);
		latched = xkb_state_serialize_mods(state, XKB_STATE_MODS_LATCHED);
		locked = xkb_state_serialize_mods(state, XKB_STATE_MODS_LOCKED);
		group = xkb_state_serialize_layout(state, XKB_STATE_LAYOUT_EFFECTIVE);

		struct wl_display *display = wl_client_get_display(client);

		focus_serial = wl_display_next_serial(display);

		for (auto i = range.first; i != range.second; ++i) {
			i->second->send_modifiers(focus_serial, depressed, latched, locked, group);
			i->second->send_enter(focus_serial, focus_surface->_self_resource, &fake_keys);
		}

	}
}

void page_keyboard::set_focus (wl::wl_surface * surface)
{

	if (focus_surface == surface)
		return;

	if (focus_surface != nullptr) {
		struct wl_client *client = wl_resource_get_client(focus_surface->_self_resource);
		auto range = client_keyboards.equal_range(client);
		if (range.first != range.second) {
			struct wl_display *display = wl_client_get_display(client);
			uint32_t serial = wl_display_next_serial(display);
			for (auto i = range.first; i != range.second; ++i) {
				i->second->send_leave(serial, focus_surface->_self_resource);
			}
		}
	}

	focus_surface = surface;
	broadcast_focus();

}

} /* namespace page */
