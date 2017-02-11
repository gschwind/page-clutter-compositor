/*
 * page.cxx
 *
 * copyright (2010-2015) Benoit Gschwind
 *
 * This code is licensed under the GPLv3. see COPYING file for more details.
 *
 */

#include "config.hxx"

/* According to POSIX.1-2001 */
#include <sys/select.h>
#include <poll.h>

#include <cairo.h>

#include <cstdlib>
#include <cstring>
#include <cassert>

#include <string>
#include <sstream>
#include <limits>
#include <stdint.h>
#include <stdexcept>
#include <set>
#include <stack>
#include <vector>
#include <typeinfo>
#include <memory>
#include <utility>
#include <list>

#include <sys/types.h>
#include <sys/socket.h>

#include <wayland-util.h>
#include <linux/input.h>

#include "xdg-shell-unstable-v5-server-protocol.h"
#include "xdg-shell-unstable-v6-server-protocol.h"

#include "utils/utils.hxx"

#include "renderable.hxx"
#include "key_desc.hxx"
#include "time.hxx"
#include "grab_handlers.hxx"

#include "simple2_theme.hxx"
#include "tiny_theme.hxx"

#include "notebook.hxx"
#include "workspace.hxx"
#include "split.hxx"
#include "page.hxx"

#include "popup_alt_tab.hxx"
#include "view.hxx"

#include "wl/wl-shell.hxx"
#include "sh/xdg-v5-shell.hxx"
#include "sh/xdg-v6-shell.hxx"

#include "core/page-core.hxx"
#include "core/page-keyboard.hxx"
#include "core/page-pointer.hxx"
#include "core/page-seat.hxx"

#include "page/page_root.hxx"

/* ICCCM definition */
#define _NET_WM_STATE_REMOVE 0
#define _NET_WM_STATE_ADD 1
#define _NET_WM_STATE_TOGGLE 2

namespace page {

using namespace wayland_cxx_wrapper;

static void wrapper_bind_xdg_v5_shell(struct wl_client *client, void *data, uint32_t version, uint32_t id) {
	reinterpret_cast<page_t*>(data)->bind_xdg_v5_shell(client, version, id);
}

void page_t::bind_xdg_v5_shell(struct wl_client *client, uint32_t version, uint32_t id)
{
	new sh::xdg_v5_shell{client, version, id, this};
}

void page_t::destroy_surface(surface_t * s) {
	if(s->_master_view.expired())
		return;

//	if(_grab_handler and s->_has_popup_grab) {
//		seat->pointer->grab_stop(_grab_handler->base.grab.pointer);
//	}

	detach(s->_master_view.lock());
	assert(s->_master_view.expired());
	//weston_compositor_damage_all(ec);
	sync_tree_view();
}

void page_t::start_move(surface_t * s, page_seat * seat, uint32_t serial) {
	//printf("call %s\n", __PRETTY_FUNCTION__);
	if(s->_master_view.expired())
		return;

	auto pointer = seat->pointer;

	gfloat x = pointer->x;
	gfloat y = pointer->y;

	auto master_view = s->_master_view.lock();
	if(master_view->is(MANAGED_NOTEBOOK)) {
		seat->pointer->start_grab(make_shared<grab_bind_client_t>(this, master_view, BTN_LEFT, rect(x, y, 1, 1)));
	} else if(master_view->is(MANAGED_FLOATING)) {
		seat->pointer->start_grab(make_shared<grab_floating_move_t>(this, master_view, BTN_LEFT, x, y));
	}
}

void page_t::start_resize(surface_t * s, page_seat * seat, uint32_t serial, edge_e edges) {
	if(s->_master_view.expired())
		return;

	auto pointer = seat->pointer;
	double x = pointer->x;
	double y = pointer->y;

	auto master_view = s->_master_view.lock();
	if(master_view->is(MANAGED_FLOATING)) {
		pointer->start_grab(make_shared<grab_floating_resize_t>(this, master_view, BTN_LEFT, x, y, edges));
	}
}

//time64_t const page_t::default_wait{1000000000L / 120L};

void page_t::bind_xdg_v5_shell(struct wl_client * client, void * data,
				      uint32_t version, uint32_t id)
{
	page_t * ths = reinterpret_cast<page_t *>(data);
	auto c = new sh::xdg_v5_shell(client, version, id, ths);
}

void page_t::bind_xdg_v6_shell(struct wl_client * client, void * data,
				      uint32_t version, uint32_t id)
{
	page_t * ths = reinterpret_cast<page_t *>(data);
	auto c = new sh::xdg_v6_shell(client, version, id, ths);
}

void page_t::bind_wl_shell(struct wl_client * client, void * data,
				      uint32_t version, uint32_t id)
{
	page_t * ths = reinterpret_cast<page_t *>(data);
	auto c = new wl::wl_shell(client, version, id, ths);
}

void page_t::print_tree_binding(struct weston_keyboard *keyboard, uint32_t time,
		  uint32_t key, void *data) {
	page_t * ths = reinterpret_cast<page_t *>(data);
	printf("call %s\n", __PRETTY_FUNCTION__);
	ths->_root->print_tree(0);
}

page_t::page_t() :
		page_core(),
		repaint_scheduled{false}
{
	use_x11_backend = false;
	use_pixman = false;
	_global_wl_shell = nullptr;
	_global_xdg_shell_v5 = nullptr;
	_global_xdg_shell_v6 = nullptr;
	configuration._replace_wm = false;
	configuration._menu_drop_down_shadow = false;
	_theme = nullptr;
}

void page_t::init(int * argc, char *** argv)
{
	page_core::init(argc, argv);

	char const * conf_file_name = 0;

	/** parse command line **/

	int k = 1;
	while(k < *argc) {
		string x = *argv[k];
		if(x == "--replace") {
			configuration._replace_wm = true;
		} else {
			conf_file_name = *argv[k];
		}

		if(strcmp("--use-pixman", *argv[k]) == 0) {
			use_pixman = true;
		}

		++k;
	}

	/* load configurations, from lower priority to high one */

	/* load default configuration */
	_conf.merge_from_file_if_exist(string{DATADIR "/page/page.conf"});

	/* load homedir configuration */
	{
		char const * chome = getenv("HOME");
		if(chome != nullptr) {
			string xhome = chome;
			string file = xhome + "/.page.conf";
			_conf.merge_from_file_if_exist(file);
		}
	}

	/* load file in arguments if provided */
	if (conf_file_name != nullptr) {
		string s(conf_file_name);
		_conf.merge_from_file_if_exist(s);
	}

	page_base_dir = _conf.get_string("default", "theme_dir");
	_theme_engine = _conf.get_string("default", "theme_engine");

//	_left_most_border = std::numeric_limits<int>::max();
//	_top_most_border = std::numeric_limits<int>::max();


	bind_page_quit           = _conf.get_string("default", "bind_page_quit");
	bind_close               = _conf.get_string("default", "bind_close");
	bind_toggle_fullscreen   = _conf.get_string("default", "bind_toggle_fullscreen");
	bind_toggle_compositor   = _conf.get_string("default", "bind_toggle_compositor");
	bind_right_desktop       = _conf.get_string("default", "bind_right_desktop");
	bind_left_desktop        = _conf.get_string("default", "bind_left_desktop");

	bind_bind_window         = _conf.get_string("default", "bind_bind_window");
	bind_fullscreen_window   = _conf.get_string("default", "bind_fullscreen_window");
	bind_float_window        = _conf.get_string("default", "bind_float_window");

	bind_cmd[0].key = _conf.get_string("default", "bind_cmd_0");
	bind_cmd[1].key = _conf.get_string("default", "bind_cmd_1");
	bind_cmd[2].key = _conf.get_string("default", "bind_cmd_2");
	bind_cmd[3].key = _conf.get_string("default", "bind_cmd_3");
	bind_cmd[4].key = _conf.get_string("default", "bind_cmd_4");
	bind_cmd[5].key = _conf.get_string("default", "bind_cmd_5");
	bind_cmd[6].key = _conf.get_string("default", "bind_cmd_6");
	bind_cmd[7].key = _conf.get_string("default", "bind_cmd_7");
	bind_cmd[8].key = _conf.get_string("default", "bind_cmd_8");
	bind_cmd[9].key = _conf.get_string("default", "bind_cmd_9");

	bind_cmd[0].cmd = _conf.get_string("default", "exec_cmd_0");
	bind_cmd[1].cmd = _conf.get_string("default", "exec_cmd_1");
	bind_cmd[2].cmd = _conf.get_string("default", "exec_cmd_2");
	bind_cmd[3].cmd = _conf.get_string("default", "exec_cmd_3");
	bind_cmd[4].cmd = _conf.get_string("default", "exec_cmd_4");
	bind_cmd[5].cmd = _conf.get_string("default", "exec_cmd_5");
	bind_cmd[6].cmd = _conf.get_string("default", "exec_cmd_6");
	bind_cmd[7].cmd = _conf.get_string("default", "exec_cmd_7");
	bind_cmd[8].cmd = _conf.get_string("default", "exec_cmd_8");
	bind_cmd[9].cmd = _conf.get_string("default", "exec_cmd_9");

	if(_conf.get_string("default", "auto_refocus") == "true") {
		configuration._auto_refocus = true;
	} else {
		configuration._auto_refocus = false;
	}

	if(_conf.get_string("default", "enable_shade_windows") == "true") {
		configuration._enable_shade_windows = true;
	} else {
		configuration._enable_shade_windows = false;
	}

	if(_conf.get_string("default", "mouse_focus") == "true") {
		configuration._mouse_focus = true;
	} else {
		configuration._mouse_focus = false;
	}

	if(_conf.get_string("default", "menu_drop_down_shadow") == "true") {
		configuration._menu_drop_down_shadow = true;
	} else {
		configuration._menu_drop_down_shadow = false;
	}

	configuration._fade_in_time = _conf.get_long("compositor", "fade_in_time");

	wl_global_create(dpy, &xdg_shell_interface, xdg_shell_vtable::INTERFACE_VERSION, this, &wrapper_bind_xdg_v5_shell);

	seat->pointer->set_default_grab(make_shared<default_pointer_grab>(this));

}

page_t::~page_t() {
	// cleanup cairo, for valgrind happiness.
	//cairo_debug_reset_static_data();
}

static FILE * g_logfile = nullptr;
static int page_log_print(const char * fmt, va_list args) {
	return vfprintf(g_logfile, fmt, args);
}

void page_t::run() {

	if(g_logfile) {
		fclose(g_logfile);
	}

	g_logfile = fopen(_conf.get_string("default", "log_file").c_str() ,"w");

	/** Initialize theme **/
	if(_theme_engine == "tiny") {
		cout << "using tiny theme engine" << endl;
		_theme = new tiny_theme_t{_conf};
	} else {
		/* The default theme engine */
		cout << "using simple theme engine" << endl;
		_theme = new simple2_theme_t{_conf};
	}

	/** initialize the empty desktop **/
	_root = make_shared<page_root_t>(this);

	{
		/** create the first desktop **/
		auto d = make_shared<workspace_t>(this, 0);
		_root->_desktop_list.push_back(d);
		_root->_desktop_stack->push_front(d);
		d->hide();

		vector<shared_ptr<viewport_t>> new_layout;
		new_layout.push_back(make_shared<viewport_t>(this, rect{0, 0, 1600, 1600}));
		d->set_layout(new_layout);
		d->update_default_pop();
	}

	_global_wl_shell = wl_global_create(dpy, &wl_shell_interface, 1, this,
			&page_t::bind_wl_shell);
	_global_xdg_shell_v5 = wl_global_create(dpy, &xdg_shell_interface, 1, this,
			&page_t::bind_xdg_v5_shell);
	_global_xdg_shell_v6 = wl_global_create(dpy, &zxdg_shell_v6_interface, 1, this,
			&page_t::bind_xdg_v6_shell);

	page_core::run();

	cout << "Page END" << endl;

	//_dpy->on_visibility_change.remove(on_visibility_change_func);
	//_mainloop.remove_poll(_dpy->fd());

	/** destroy the tree **/
	_root = nullptr;

	//delete _keymap; _keymap = nullptr;
	delete _theme; _theme = nullptr;

}

//void page_t::unmanage(shared_ptr<xdg_surface_toplevel_t> mw) {
//	if(mw == nullptr)
//		return;
//
//	/* if window is in move/resize/notebook move, do cleanup */
//	cleanup_grab();
//
//	detach(mw);
//
//	printf("unmanaging : '%s'\n", mw->title().c_str());
//
//	if (has_key(_fullscreen_client_to_viewport, mw.get())) {
//		fullscreen_data_t & data = _fullscreen_client_to_viewport[mw.get()];
//		if(not data.workspace.expired() and not data.viewport.expired()) {
//			if(data.workspace.lock()->is_visible()) {
//				data.viewport.lock()->show();
//			}
//		}
//		_fullscreen_client_to_viewport.erase(mw.get());
//	}
//
//	/* if managed window have active clients */
//	for(auto i: mw->children()) {
//		auto c = dynamic_pointer_cast<xdg_surface_base_t>(i);
//		if(c != nullptr) {
//			insert_in_tree_using_transient_for(c);
//		}
//	}
//
//	if(not mw->skip_task_bar()) {
//		_need_update_client_list = true;
//	}
//
//	update_workarea();
//
//	/** if the window is destroyed, this not work, see fix on destroy **/
//	for(auto x: _root->_desktop_list) {
//		x->client_focus_history_remove(mw);
//	}
//
//	global_focus_history_remove(mw);
//
//	shared_ptr<xdg_surface_toplevel_t> new_focus;
//	set_focus(nullptr, XCB_CURRENT_TIME);
//
//}
//
//void page_t::scan() {
//		/* TODO: remove */
//}
//
//void page_t::update_net_supported() {
//	/* TODO: remove */
//}
//
//void page_t::update_client_list() {
//	/* TODO: remove */
//}
//
//void page_t::update_client_list_stacking() {
//	/* TODO: remove */
//}
//

void page_t::handle_quit_page(page_keyboard * wk, ClutterEvent const & event) {
	//wl_display_terminate(_dpy);
}

void page_t::handle_toggle_fullscreen(page_keyboard * wk, ClutterEvent const & event) {
	printf("call %s\n", __PRETTY_FUNCTION__);
	if(_current_focus.expired())
		return;
	auto v = _current_focus.lock();
	if(v->is(MANAGED_FULLSCREEN)) {
		unfullscreen(v);
	} else if (v->is(MANAGED_FLOATING) or v->is(MANAGED_NOTEBOOK)) {
		fullscreen(v);
	}
}

void page_t::handle_close_window(page_keyboard * wk, ClutterEvent const & event) {
	printf("call %s\n", __PRETTY_FUNCTION__);
	if(_current_focus.expired())
		return;
	auto v = _current_focus.lock();
	v->send_close();
}

void page_t::handle_goto_desktop_at_right(page_keyboard * wk, ClutterEvent const & event) {

}

void page_t::handle_goto_desktop_at_left(page_keyboard * wk, ClutterEvent const & event) {

}

void page_t::handle_bind_window(page_keyboard * wk, ClutterEvent const & event) {
	printf("call %s\n", __PRETTY_FUNCTION__);
	if(_current_focus.expired())
		return;
	auto v = _current_focus.lock();
	if(v->is(MANAGED_FULLSCREEN)) {
		unfullscreen(v);
	}

	if (v->is(MANAGED_FLOATING)) {
		bind_window(v);
		set_keyboard_focus(wk->seat, v);
	}

}

void page_t::handle_set_fullscreen_window(page_keyboard * wk, ClutterEvent const & event) {
	printf("call %s\n", __PRETTY_FUNCTION__);
	if(_current_focus.expired())
		return;
	auto v = _current_focus.lock();
	if(v->is(MANAGED_FLOATING) or v->is(MANAGED_NOTEBOOK)) {
		fullscreen(v);
	}
}

void page_t::handle_set_floating_window(page_keyboard * wk, ClutterEvent const & event) {
	printf("call %s\n", __PRETTY_FUNCTION__);
	if(_current_focus.expired())
		return;
	auto v = _current_focus.lock();
	if(v->is(MANAGED_FULLSCREEN)) {
		unfullscreen(v);
	}

	if(v->is(MANAGED_NOTEBOOK)) {
		unbind_window(v);
	}

}

void page_t::handle_alt_left_button(page_pointer *pointer, ClutterEvent const & event) {
//	wl_fixed_t sx, sy;
//
//	auto v = weston_compositor_pick_view(ec, pointer->x, pointer->y, &sx, &sy);
//	if(not v)
//		return;
//	auto view = lookup_for_view(v);
//	if(not view)
//		return;
//
//	double x = wl_fixed_to_double(pointer->x);
//	double y = wl_fixed_to_double(pointer->y);
//
//	if(view->is(MANAGED_NOTEBOOK)) {
//		grab_start(pointer, new grab_bind_client_t{this, view,
//			BTN_LEFT, rect(x, y, 1, 1)});
//	} else if(view->is(MANAGED_FLOATING)) {
//		grab_start(pointer, new grab_floating_move_t(this, view,
//			BTN_LEFT, x, y));
//	}

}

void page_t::handle_alt_right_button(page_pointer *pointer, ClutterEvent const & event) {
	wl_fixed_t sx, sy;

	auto v = pointer->pick_actor(&event);
	if(not v)
		return;
	auto view = lookup_for_view(v);
	if(not view)
		return;

	double x = event.button.x;
	double y = event.button.y;

//	if(view->is(MANAGED_FLOATING)) {
//		grab_start(pointer, new grab_floating_resize_t(this, view,
//			BTN_LEFT, x, y, EDGE_BOTTOM_RIGHT));
//	}
}

void page_t::handle_bind_cmd_0(page_keyboard * wk, ClutterEvent const & event) {
	printf("call %s\n", __PRETTY_FUNCTION__);
	run_cmd(bind_cmd[0].cmd);
}

void page_t::handle_bind_cmd_1(page_keyboard * wk, ClutterEvent const & event) {
	printf("call %s\n", __PRETTY_FUNCTION__);
	run_cmd(bind_cmd[1].cmd);
}

void page_t::handle_bind_cmd_2(page_keyboard * wk, ClutterEvent const & event) {
	printf("call %s\n", __PRETTY_FUNCTION__);
	run_cmd(bind_cmd[2].cmd);
}

void page_t::handle_bind_cmd_3(page_keyboard * wk, ClutterEvent const & event) {
	printf("call %s\n", __PRETTY_FUNCTION__);
	run_cmd(bind_cmd[3].cmd);
}

void page_t::handle_bind_cmd_4(page_keyboard * wk, ClutterEvent const & event) {
	printf("call %s\n", __PRETTY_FUNCTION__);
	run_cmd(bind_cmd[4].cmd);
}

void page_t::handle_bind_cmd_5(page_keyboard * wk, ClutterEvent const & event) {
	printf("call %s\n", __PRETTY_FUNCTION__);
	run_cmd(bind_cmd[5].cmd);
}

void page_t::handle_bind_cmd_6(page_keyboard * wk, ClutterEvent const & event) {
	printf("call %s\n", __PRETTY_FUNCTION__);
	run_cmd(bind_cmd[6].cmd);
}

void page_t::handle_bind_cmd_7(page_keyboard * wk, ClutterEvent const & event) {
	printf("call %s\n", __PRETTY_FUNCTION__);
	run_cmd(bind_cmd[7].cmd);
}

void page_t::handle_bind_cmd_8(page_keyboard * wk, ClutterEvent const & event) {
	printf("call %s\n", __PRETTY_FUNCTION__);
	run_cmd(bind_cmd[8].cmd);
}

void page_t::handle_bind_cmd_9(page_keyboard * wk, ClutterEvent const & event) {
	printf("call %s\n", __PRETTY_FUNCTION__);
	run_cmd(bind_cmd[9].cmd);
}


view_p page_t::lookup_for_view(ClutterActor * v) {
	auto children = filter_class<view_t>(_root->get_all_children());
	for(auto x: children) {
		if(v == x->get_default_view()) {
			return x;
		}
	}

	return nullptr;
}

void page_t::fullscreen(view_p mw) {

	if(mw->is(MANAGED_FULLSCREEN))
		return;

	shared_ptr<viewport_t> v;
	if(mw->is(MANAGED_NOTEBOOK)) {
		v = find_viewport_of(mw);
	} else if (mw->is(MANAGED_FLOATING)) {
		v = get_current_workspace()->get_any_viewport();
	} else {
		cout << "WARNING: a dock trying to become fullscreen" << endl;
		return;
	}

	fullscreen(mw, v);
}

void page_t::fullscreen(view_p mw, shared_ptr<viewport_t> v) {
	assert(v != nullptr);

	if(mw->is(MANAGED_FULLSCREEN))
		return;

	/* WARNING: Call order is important, change it with caution */

	fullscreen_data_t data;

	if(mw->is(MANAGED_NOTEBOOK)) {
		/**
		 * if the current window is managed in notebook:
		 *
		 * 1. search for the current notebook,
		 * 2. search the viewport for this notebook, and use it as default
		 *    fullscreen host or use the first available viewport.
		 **/
		data.revert_type = MANAGED_NOTEBOOK;
		data.revert_notebook = find_parent_notebook_for(mw);
	} else if (mw->is(MANAGED_FLOATING)) {
		data.revert_type = MANAGED_FLOATING;
		data.revert_notebook.reset();
	} else {
		cout << "WARNING: a dock trying to become fullscreen" << endl;
		return;
	}

	auto workspace = find_desktop_of(v);

	detach(mw);

	// unfullscreen client that already use this screen
	for (auto &x : _fullscreen_client_to_viewport) {
		if (x.second.viewport.lock() == v) {
			unfullscreen(x.second.client.lock());
			break;
		}
	}

	data.client = mw;
	data.workspace = workspace;
	data.viewport = v;

	_fullscreen_client_to_viewport[mw.get()] = data;

	mw->set_managed_type(MANAGED_FULLSCREEN);
	workspace->attach(mw);

	/* it's a trick */
	mw->set_notebook_wished_position(v->raw_area());
	mw->reconfigure();

	sync_tree_view();

}

void page_t::unfullscreen(view_p mw) {
	/* WARNING: Call order is important, change it with caution */

	/** just in case **/
	//mw->net_wm_state_remove(_NET_WM_STATE_FULLSCREEN);

	if(!has_key(_fullscreen_client_to_viewport, mw.get()))
		return;

	detach(mw);

	fullscreen_data_t data = _fullscreen_client_to_viewport[mw.get()];
	_fullscreen_client_to_viewport.erase(mw.get());

	shared_ptr<workspace_t> d;

	if(data.workspace.expired()) {
		d = get_current_workspace();
	} else {
		d = data.workspace.lock();
	}

	shared_ptr<viewport_t> v;

	if(data.viewport.expired()) {
		v = d->get_any_viewport();
	} else {
		v = data.viewport.lock();
	}

	if (data.revert_type == MANAGED_NOTEBOOK) {
		shared_ptr<notebook_t> n;
		if(data.revert_notebook.expired()) {
			n = d->default_pop();
		} else {
			n = data.revert_notebook.lock();
		}
		mw->set_managed_type(MANAGED_NOTEBOOK);
		n->add_client(mw);
		mw->reconfigure();
	} else {
		mw->set_managed_type(MANAGED_FLOATING);
		insert_in_tree_using_transient_for(mw);
		mw->reconfigure();
	}

	if(d->is_visible() and not v->is_visible()) {
		v->show();
	}

	sync_tree_view();

}

void page_t::toggle_fullscreen(view_p c) {
	if(c->is(MANAGED_FULLSCREEN))
		unfullscreen(c);
	else
		fullscreen(c);
}


//void page_t::process_event(xcb_generic_event_t const * e) {
//	auto x = _event_handlers.find(e->response_type);
//	if(x != _event_handlers.end()) {
//		if(x->second != nullptr) {
//			(this->*(x->second))(e);
//		}
//	} else {
//		//std::cout << "not handled event: " << cnx->event_type_name[(e->response_type&(~0x80))] << (e->response_type&(0x80)?" (fake)":"") << std::endl;
//	}
//}

void page_t::insert_window_in_notebook(view_p x, notebook_p n) {
	assert(x != nullptr);
	if (n == nullptr)
		n = get_current_workspace()->default_pop();
	assert(n != nullptr);
	n->add_client(x);
}

/* update viewport and childs allocation */
//void page_t::update_workarea() {
////	for (auto d : _root->_desktop_list) {
////		for (auto v : d->get_viewports()) {
////			compute_viewport_allocation(d, v);
////		}
////		d->set_workarea(d->primary_viewport()->allocation());
////	}
////
////	std::vector<uint32_t> workarea_data(_root->_desktop_list.size()*4);
////	for(unsigned k = 0; k < _root->_desktop_list.size(); ++k) {
////		workarea_data[k*4+0] = _root->_desktop_list[k]->workarea().x;
////		workarea_data[k*4+1] = _root->_desktop_list[k]->workarea().y;
////		workarea_data[k*4+2] = _root->_desktop_list[k]->workarea().w;
////		workarea_data[k*4+3] = _root->_desktop_list[k]->workarea().h;
////	}
////
////	_dpy->change_property(_dpy->root(), _NET_WORKAREA, CARDINAL, 32,
////			&workarea_data[0], workarea_data.size());
//
//}

void page_t::set_keyboard_focus(page_seat * seat,
		shared_ptr<view_t> new_focus) {
	printf("call %s\n", __PRETTY_FUNCTION__);
	assert(new_focus != nullptr);
	assert(new_focus->get_default_view() != nullptr);

	if(new_focus->is(MANAGED_POPUP))
		return;

	if(!_current_focus.expired()) {
		if(_current_focus.lock() == new_focus)
			return;
		_current_focus.lock()->set_focus_state(false);
	}

	seat->keyboard->set_focus(new_focus->surface());

	_current_focus = new_focus;

	get_current_workspace()->client_focus_history_move_front(new_focus);
	global_focus_history_move_front(new_focus);
	new_focus->activate();
	new_focus->set_focus_state(true);

	sync_tree_view();

}

void page_t::split_left(notebook_p nbk, view_p c) {
	auto parent = dynamic_pointer_cast<page_component_t>(nbk->parent()->shared_from_this());
	auto n = make_shared<notebook_t>(this);
	auto split = make_shared<split_t>(this, VERTICAL_SPLIT);
	parent->replace(nbk, split);
	split->set_pack0(n);
	split->set_pack1(nbk);
	if (c != nullptr) {
		detach(c);
		insert_window_in_notebook(c, n);
	}
	split->show();
}

void page_t::split_right(notebook_p nbk, view_p c) {
	auto parent = dynamic_pointer_cast<page_component_t>(nbk->parent()->shared_from_this());
	auto n = make_shared<notebook_t>(this);
	auto split = make_shared<split_t>(this, VERTICAL_SPLIT);
	parent->replace(nbk, split);
	split->set_pack0(nbk);
	split->set_pack1(n);
	if (c != nullptr) {
		detach(c);
		insert_window_in_notebook(c, n);
	}
	split->show();
}

void page_t::split_top(notebook_p nbk, view_p c) {
	auto parent = dynamic_pointer_cast<page_component_t>(nbk->parent()->shared_from_this());
	auto n = make_shared<notebook_t>(this);
	auto split = make_shared<split_t>(this, HORIZONTAL_SPLIT);
	parent->replace(nbk, split);
	split->set_pack0(n);
	split->set_pack1(nbk);
	if (c != nullptr) {
		detach(c);
		insert_window_in_notebook(c, n);
	}
	split->show();
}

void page_t::split_bottom(notebook_p nbk, view_p c) {
	auto parent = dynamic_pointer_cast<page_component_t>(nbk->parent()->shared_from_this());
	auto n = make_shared<notebook_t>(this);
	auto split = make_shared<split_t>(this, HORIZONTAL_SPLIT);
	parent->replace(nbk, split);
	split->set_pack0(nbk);
	split->set_pack1(n);
	if (c != nullptr) {
		detach(c);
		insert_window_in_notebook(c, n);
	}
	split->show();
}

/*
 * This function will close the given notebook, if possible
 */
void page_t::notebook_close(notebook_p nbk) {
	/**
	 * Closing notebook mean destroying the split base of this
	 * notebook, plus this notebook.
	 **/

	assert(nbk->parent() != nullptr);

	auto splt = dynamic_pointer_cast<split_t>(nbk->parent()->shared_from_this());

	/* if parent is viewport then we cannot close current notebook */
	if(splt == nullptr)
		return;

	assert(nbk == splt->get_pack0() or nbk == splt->get_pack1());

	/* find the sibling branch of note that we want close */
	auto dst = dynamic_pointer_cast<page_component_t>((nbk == splt->get_pack0()) ? splt->get_pack1() : splt->get_pack0());

	assert(dst != nullptr);

	/* remove this split from tree  and replace it by sibling branch */
	detach(dst);
	dynamic_pointer_cast<page_component_t>(splt->parent()->shared_from_this())->replace(splt, dst);

	/**
	 * if notebook that we want destroy was the default_pop, select
	 * a new one.
	 **/
	if (get_current_workspace()->default_pop() == nbk) {
		get_current_workspace()->update_default_pop();
		/* damage the new default pop to show the notebook mark properly */
	}

	/* move all client from destroyed notebook to new default pop */
	auto clients = filter_class<view_t>(nbk->children());
//	bool notebook_has_focus = false;
	for(auto i : clients) {
//		if(i->has_focus())
//			notebook_has_focus = true;
		nbk->remove(i);
		insert_window_in_notebook(i, nullptr);
	}

	/**
	 * if a fullscreen client want revert to this notebook,
	 * change it to default_window_pop
	 **/
	for (auto & i : _fullscreen_client_to_viewport) {
		if (i.second.revert_notebook.lock() == nbk) {
			i.second.revert_notebook = _root->_desktop_list[_root->_current_desktop]->default_pop();
		}
	}

//	if(notebook_has_focus) {
//		set_focus(nullptr, XCB_CURRENT_TIME);
//	}

}

void page_t::insert_in_tree_using_transient_for(view_p c) {

	c->set_managed_type(MANAGED_FLOATING);
	if(c->_page_surface->_transient_for
			and not c->_page_surface->_transient_for->_master_view.expired()) {
		auto parent = c->_page_surface->_transient_for->_master_view.lock();
		parent->add_transient_child(c);
	} else {
		get_current_workspace()->attach(c);
	}
	c->update_view();
	sync_tree_view();

}

void page_t::detach(shared_ptr<tree_t> t) {
	assert(t != nullptr);

	/** detach a tree_t will cause it to be restacked, at less **/
	//add_global_damage(t->get_visible_region());
	if(t->parent() != nullptr) {

		/**
		 * Keeping a client in the focus history of wrong workspace
		 * trouble the workspace switching, because when a switch occur,
		 * page look within the workspace focus history to choose the
		 * proper client to re-focus and may re-focus that is not belong the
		 * new workspace.
		 **/
		if(typeid(*t.get()) == typeid(view_t)) {
			auto x = dynamic_pointer_cast<view_t>(t);
			for(auto w: _root->_desktop_list)
				w->client_focus_history_remove(x);
		}

		t->parent()->remove(t);
		if(t->parent() != nullptr) {
			printf("XXX %s\n", typeid(*t->parent()).name());
			assert(false);
		}
	}
}

void page_t::fullscreen_client_to_viewport(view_p c, viewport_p v) {
	detach(c);
	if (has_key(_fullscreen_client_to_viewport, c.get())) {
		fullscreen_data_t & data = _fullscreen_client_to_viewport[c.get()];
		if (v != data.viewport.lock()) {
			if(not data.viewport.expired()) {
				//data.viewport.lock()->show();
				//data.viewport.lock()->queue_redraw();
				//add_global_damage(data.viewport.lock()->raw_area());
			}
			//v->hide();
			//add_global_damage(v->raw_area());
			data.viewport = v;
			data.workspace = find_desktop_of(v);
			c->set_notebook_wished_position(v->raw_area());
			c->reconfigure();
			//update_desktop_visibility();
		}
	}
}

void page_t::bind_window(view_p mw) {
	detach(mw);
	insert_window_in_notebook(mw, nullptr);
}

void page_t::unbind_window(view_p mw) {
	detach(mw);
	mw->set_managed_type(MANAGED_FLOATING);
	insert_in_tree_using_transient_for(mw);
	mw->queue_redraw();
	mw->show();
	mw->activate();
}

/* look for a notebook in tree base, that is deferent from nbk */
shared_ptr<notebook_t> page_t::get_another_notebook(shared_ptr<tree_t> base, shared_ptr<tree_t> nbk) {
	vector<shared_ptr<notebook_t>> l;

	if (base == nullptr) {
		l = filter_class<notebook_t>(_root->get_all_children());
	} else {
		l = filter_class<notebook_t>(base->get_all_children());;
	}

	if (!l.empty()) {
		if (l.front() != nbk)
			return l.front();
		if (l.back() != nbk)
			return l.back();
	}

	return nullptr;

}

shared_ptr<notebook_t> page_t::find_parent_notebook_for(shared_ptr<view_t> mw) {
	return dynamic_pointer_cast<notebook_t>(mw->parent()->shared_from_this());
}

shared_ptr<workspace_t> page_t::find_desktop_of(shared_ptr<tree_t> n) {
	shared_ptr<tree_t> x = n;
	while (x != nullptr) {
		auto ret = dynamic_pointer_cast<workspace_t>(x);
		if (ret != nullptr)
			return ret;

		if (x->parent() != nullptr)
			x = (x->parent()->shared_from_this());
		else
			return nullptr;
	}
	return nullptr;
}

void page_t::update_windows_stack() {
	sync_tree_view();
}

/**
 * This function will update viewport layout on xrandr events.
 *
 * It cut the visible outputs area in rectangle, where viewport will cover. The
 * rule is that the first output get the area first, the last one is cut in
 * sub-rectangle that do not overlap previous allocated area.
 **/
void page_t::update_viewport_layout() {

//	/* compute the extends of all outputs */
//	rect outputs_extends{numeric_limits<int>::max(), numeric_limits<int>::max(),
//		numeric_limits<int>::min(), numeric_limits<int>::min()};
//
//	for(auto o: _outputs) {
//		outputs_extends.x = std::min(outputs_extends.x, o->x);
//		outputs_extends.y = std::min(outputs_extends.y, o->y);
//		outputs_extends.w = std::max(outputs_extends.w, o->x+o->width);
//		outputs_extends.h = std::max(outputs_extends.h, o->y+o->height);
//	}
//
//	outputs_extends.w -= outputs_extends.x;
//	outputs_extends.h -= outputs_extends.y;
//
//	_root->_root_position = outputs_extends;
//
//	/* compute all viewport  that does not overlap and cover the full area of
//	 * outputs */
//
//	/* list of future viewport locations */
//	vector<pair<weston_output *, rect>> viewport_allocation;
//
//	/* start with not allocated area */
//	region already_allocated;
//	for(auto o: _outputs) {
//		/* the location of outputs */
//		region location{o->x, o->y, o->width, o->height};
//		/* remove overlapped areas */
//		location -= already_allocated;
//		/* for remaining rectangles, allocate a viewport */
//		for(auto & b: location.rects()) {
//			viewport_allocation.push_back(make_pair(o, b));
//		}
//		already_allocated += location;
//	}
//
//	/* for each desktop we update the list of viewports, without destroying
//	 * existing is not nessesary */
//	for(auto d: _root->_desktop_list) {
//		//d->set_allocation(_root->_root_position);
//		/** get old layout to recycle old viewport, and keep unchanged outputs **/
//		vector<shared_ptr<viewport_t>> old_layout = d->get_viewport_map();
//		/** store the newer layout, to be able to cleanup obsolete viewports **/
//		vector<shared_ptr<viewport_t>> new_layout;
//		/** for each not overlaped rectangle **/
//		for(unsigned i = 0; i < viewport_allocation.size(); ++i) {
//			printf("%d: found viewport (%d,%d,%d,%d)\n", d->id(),
//					viewport_allocation[i].second.x, viewport_allocation[i].second.y,
//					viewport_allocation[i].second.w, viewport_allocation[i].second.h);
//			shared_ptr<viewport_t> vp;
//			if(i < old_layout.size()) {
//				vp = old_layout[i];
//				vp->set_raw_area(viewport_allocation[i].second);
//			} else {
//				vp = make_shared<viewport_t>(this, viewport_allocation[i].second, viewport_allocation[i].first);
//			}
//			new_layout.push_back(vp);
//		}
//
//		d->set_layout(new_layout);
//		d->update_default_pop();
//
//		/** clean up obsolete layout **/
//		for (unsigned i = new_layout.size(); i < old_layout.size(); ++i) {
//			/** destroy this viewport **/
//			remove_viewport(d, old_layout[i]);
//			old_layout[i] = nullptr;
//		}
//
//
//		if(new_layout.size() > 0) {
//			// update position of floating managed clients to avoid offscreen
//			// floating window
//			for(auto x: filter_class<view_t>(_root->get_all_children())) {
//				if(x->is(MANAGED_FLOATING)) {
//					auto r = x->get_window_position();
//					r.x = new_layout[0]->allocation().x;
//					r.y = new_layout[0]->allocation().y;
//					x->set_floating_wished_position(r);
//					x->reconfigure();
//				}
//			}
//		}
//	}
//
//	_root->broadcast_update_layout(time64_t::now());
//	sync_tree_view();

}

void page_t::remove_viewport(workspace_p d, viewport_p v) {

	/* TODO: on output */

	/* remove fullscreened clients if needed */
//	for (auto &x : _fullscreen_client_to_viewport) {
//		if (x.second.viewport.lock() == v) {
//			unfullscreen(x.second.client.lock());
//			break;
//		}
//	}

	/* Transfer clients to a valid notebook */
	for (auto nbk : filter_class<notebook_t>(v->get_all_children())) {
		for (auto c : filter_class<view_t>(nbk->children())) {
			d->default_pop()->add_client(c);
		}
	}

}

void page_t::manage_client(surface_t * s) {
	printf("call %s %p\n", __PRETTY_FUNCTION__, this);

	auto view = make_shared<view_t>(this, s);
	s->_master_view = view;


//	if (mw->_pending.fullscreen) {
//		/**
//		 * Here client that default to fullscreen
//		 **/
//
//		fullscreen(mw);
//		update_desktop_visibility();
//		mw->show();
//		mw->activate();
//		set_focus(mw, XCB_CURRENT_TIME);
//
//	} else if (mw->_pending.transient_for == nullptr) {
//		/**
//		 * Here client that is NOTEBOOK
//		 **/
//
//		bind_window(mw, true);
//		mw->reconfigure();
//
//	} else {
//		/**
//		 * Here client that default to floating
//		 **/
//
//		mw->normalize();
//		mw->show();
//		mw->activate();
//		//set_focus(mw, XCB_CURRENT_TIME);
//		//if(mw->is(MANAGED_DOCK)) {
//		//	update_workarea();
//		//}
//
//		mw->reconfigure();
//		//_need_restack = true;
//	}

//	mw->show();

//	auto view = xdg_surface->create_view();
//	weston_view_set_position(view, 0, 0);
//	surface->timeline.force_refresh = 1;
//
//	wl_array array;
//	wl_array_init(&array);
//	wl_array_add(&array, sizeof(uint32_t)*2);
//	((uint32_t*)array.data)[0] = XDG_SURFACE_STATE_MAXIMIZED;
//	((uint32_t*)array.data)[1] = XDG_SURFACE_STATE_ACTIVATED;
//	xdg_surface_send_configure(resource, 800, 800, &array, 10);
//	wl_array_release(&array);
//
//	weston_view_geometry_dirty(view);

	/** case is notebook window **/

	if(s->_transient_for == nullptr) {
		bind_window(view);
	} else {
		insert_in_tree_using_transient_for(view);
	}

//	weston_seat * seat;
//	wl_list_for_each(seat, &ec->seat_list, link) {
//		set_keyboard_focus(seat, view);
//	}

	/** case is floating window **/
	//insert_in_tree_using_transient_for(mw);

}

void page_t::manage_popup(surface_t * s) {
//	printf("call %s %p\n", __PRETTY_FUNCTION__, this);
//	assert(s->_parent != nullptr);
//
//	auto grab = dynamic_cast<grab_popup_t *>(_grab_handler);
//
//	/* starting popup while another grab is ongoing is wrong */
//	if(s->_seat and _grab_handler and grab == nullptr)
//		return;
//
//	auto parent_view = s->_parent->_master_view.lock();
//
//	if(parent_view != nullptr) {
//		auto view = make_shared<view_t>(this, s);
//		s->_master_view = view;
//		printf("%s x=%d, y=%d\n", __PRETTY_FUNCTION__, s->_x_offset, s->_y_offset);
//		parent_view->add_popup_child(view, s->_x_offset, s->_y_offset);
//		sync_tree_view();
//		if(s->_seat) {
//			set_keyboard_focus(s->_seat, parent_view);
//
//			if(grab) {
//				grab->_surface->_has_popup_grab = false;
//				grab_stop(grab->base.grab.pointer);
//			}
//
//			s->_has_popup_grab = true;
//			grab_start(weston_seat_get_pointer(s->_seat), new grab_popup_t{this, s});
//		}
//	}
}

void page_t::configure_popup(surface_t * s) {
	printf("call %s %p\n", __PRETTY_FUNCTION__, this);
	s->send_configure_popup(s->_x_offset, s->_y_offset, s->width(), s->height());
}

//void page_t::create_unmanaged_window(xcb_window_t w, xcb_atom_t type) {
//	auto uw = make_shared<client_not_managed_t>(this, w, type);
//	_dpy->map(uw->orig());
//	uw->show();
//	safe_update_transient_for(uw);
//}

shared_ptr<viewport_t> page_t::find_mouse_viewport(int x, int y) const {
	auto viewports = get_current_workspace()->get_viewports();
	for (auto v: viewports) {
		if (v->raw_area().is_inside(x, y))
			return v;
	}
	return shared_ptr<viewport_t>{};
}

void page_t::remove_client(view_p c) {
	auto parent = c->parent()->shared_from_this();
	detach(c);
	for(auto i: c->children()) {
		auto c = dynamic_pointer_cast<view_t>(i);
		if(c != nullptr) {
			insert_in_tree_using_transient_for(c);
		}
	}
}


static bool is_keycode_for_keysym(struct xkb_keymap *keymap,
		xkb_keycode_t keycode, xkb_keysym_t keysym) {
	xkb_layout_index_t num_layouts = xkb_keymap_num_layouts_for_key(keymap,
			keycode);
	for (xkb_layout_index_t i = 0; i < num_layouts; i++) {
		xkb_level_index_t num_levels = xkb_keymap_num_levels_for_key(keymap,
				keycode, i);
		for (xkb_level_index_t j = 0; j < num_levels; j++) {
			const xkb_keysym_t *syms;
			int num_syms = xkb_keymap_key_get_syms_by_level(keymap, keycode, i,
					j, &syms);
			for (int k = 0; k < num_syms; k++) {
				if (syms[k] == keysym)
					return true;
			}
		}
	}
	return false;
}

static void xkb_keymap_key_iter(struct xkb_keymap *keymap, xkb_keycode_t key, void *data) {
	auto input = reinterpret_cast<pair<xkb_keysym_t, xkb_keycode_t>*>(data);
	if(is_keycode_for_keysym(keymap, key, input->first)) {
		input->second = key;
	}
}

static xkb_keycode_t find_keycode_for_keysim(xkb_keymap * keymap, xkb_keysym_t ks) {
	xkb_keymap_key_iter_t iter;
	pair<xkb_keysym_t, xkb_keycode_t> data = {ks, XKB_KEYCODE_INVALID};
	xkb_keymap_key_for_each(keymap, &xkb_keymap_key_iter, &data);
	return data.second;
}

/* Inspired from openbox */
void page_t::run_cmd(std::string const & cmd_with_args)
{
	printf("executing %s\n", cmd_with_args.c_str());

    GError *e;
    gchar **argv = NULL;
    gchar *cmd;

    if (cmd_with_args == "null")
    	return;

    cmd = g_filename_from_utf8(cmd_with_args.c_str(), -1, NULL, NULL, NULL);
    if (!cmd) {
        printf("Failed to convert the path \"%s\" from utf8\n", cmd_with_args.c_str());
        return;
    }

    e = NULL;
    if (!g_shell_parse_argv(cmd, NULL, &argv, &e)) {
        printf("%s\n", e->message);
        g_error_free(e);
    } else {
        gchar *program = NULL;
        gboolean ok;

        e = NULL;
        ok = g_spawn_async(NULL, argv, NULL,
                           (GSpawnFlags)(G_SPAWN_SEARCH_PATH |
                           G_SPAWN_DO_NOT_REAP_CHILD),
                           NULL, NULL, NULL, &e);
        if (!ok) {
            printf("%s\n", e->message);
            g_error_free(e);
        }

        g_free(program);
        g_strfreev(argv);
    }

    g_free(cmd);

    return;
}

shared_ptr<viewport_t> page_t::find_viewport_of(shared_ptr<tree_t> t) {
	while(t != nullptr) {
		auto ret = dynamic_pointer_cast<viewport_t>(t);
		if(ret != nullptr)
			return ret;
		t = t->parent()->shared_from_this();
	}

	return nullptr;
}

theme_t const * page_t::theme() const {
	return _theme;
}

shared_ptr<workspace_t> const & page_t::get_current_workspace() const {
	return _root->_desktop_list[_root->_current_desktop];
}

shared_ptr<workspace_t> const & page_t::get_workspace(int id) const {
	return _root->_desktop_list[id];
}

int page_t::get_workspace_count() const {
	return _root->_desktop_list.size();
}

int page_t::create_workspace() {
	auto d = make_shared<workspace_t>(this, _root->_desktop_list.size());
	_root->_desktop_list.push_back(d);
	_root->_desktop_stack->push_front(d);
	d->hide();

//	update_viewport_layout();
//	update_current_desktop();
//	update_desktop_visibility();
	return d->id();
}

list<view_w> page_t::global_client_focus_history() {
	return _global_focus_history;
}

bool page_t::global_focus_history_front(shared_ptr<view_t> & out) {
	if(not global_focus_history_is_empty()) {
		out = _global_focus_history.front().lock();
		return true;
	}
	return false;
}

void page_t::global_focus_history_remove(shared_ptr<view_t> in) {
	_global_focus_history.remove_if([in](weak_ptr<tree_t> const & w) { return w.expired() or w.lock() == in; });
}

void page_t::global_focus_history_move_front(shared_ptr<view_t> in) {
	move_front(_global_focus_history, in);
}

bool page_t::global_focus_history_is_empty() {
	_global_focus_history.remove_if([](weak_ptr<tree_t> const & w) { return w.expired(); });
	return _global_focus_history.empty();
}

auto page_t::conf() const -> page_configuration_t const & {
	return configuration;
}

/**
 * This function synchronize the page tree with the weston scene graph. The side
 * effects are damage all outputs and schedule repaint for all outputs.
 **/
void page_t::sync_tree_view() {

	/* create the list of weston views */
	list<ClutterActor *> views;
	auto children = _root->get_all_children();
	printf("found %lu children\n", children.size());
	for(auto x: children) {
		auto v = x->get_default_view();
		if(v)
			views.push_back(v);
	}

	clutter_actor_remove_all_children(_main_stage);

	//_root->print_tree(0);

	printf("found %lu views\n", views.size());

	for(auto actor: views) {
		clutter_actor_add_child(_main_stage, actor);
	}

	//schedule_repaint();

}

void page_t::switch_focused_to_fullscreen() {
	if(_current_focus.expired())
		return;
	auto current = _current_focus.lock();
	toggle_fullscreen(current);

}

void page_t::switch_focused_to_floating() {
	if(_current_focus.expired())
		return;
	auto current = _current_focus.lock();
	unbind_window(current);
}

void page_t::switch_focused_to_notebook() {
	if(_current_focus.expired())
		return;
	auto current = _current_focus.lock();
	bind_window(current);
}


}

