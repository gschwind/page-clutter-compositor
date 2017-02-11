/*
 * page.hxx
 *
 * copyright (2010-2015) Benoit Gschwind
 *
 * This code is licensed under the GPLv3. see COPYING file for more details.
 *
 */

#ifndef PAGE_HXX_
#define PAGE_HXX_

#include <memory>
#include <list>
#include <vector>
#include <string>
#include <map>
#include <array>
#include <thread>


#include "config.hxx"
#include "utils/utils.hxx"
#include "core/page-types.hxx"
#include "page/ui-types.hxx"
#include "sh/sh-types.hxx"
#include "wl/wl-types.hxx"

#include "core/page-core.hxx"

namespace page {

using namespace std;

struct fullscreen_data_t {
	view_w client;
	workspace_w workspace;
	viewport_w viewport;
	managed_window_type_e revert_type;
	notebook_w revert_notebook;
};

struct page_configuration_t {
	bool _replace_wm;
	bool _menu_drop_down_shadow;
	bool _auto_refocus;
	bool _mouse_focus;
	bool _enable_shade_windows;
	int64_t _fade_in_time;
};


struct key_bind_cmd_t {
	key_desc_t key;
	string cmd;
};

struct page_t : public page_core, public connectable {
	shared_ptr<page_root_t> _root;
	theme_t * _theme;
	page_configuration_t configuration;
	config_handler_t _conf;

	/**
	 * Store data to allow proper revert fullscreen window to
	 * their original positions
	 **/
	map<view_t *, fullscreen_data_t> _fullscreen_client_to_viewport;

	string page_base_dir;
	string _theme_engine;

	bool use_x11_backend;
	bool use_pixman;
	bool repaint_scheduled;

	view_w _current_focus;

	struct wl_global * _global_wl_shell;
	struct wl_global * _global_xdg_shell_v5;
	struct wl_global * _global_xdg_shell_v6;

	key_desc_t bind_page_quit;
	key_desc_t bind_toggle_fullscreen;
	key_desc_t bind_toggle_compositor;
	key_desc_t bind_close;

	key_desc_t bind_right_desktop;
	key_desc_t bind_left_desktop;

	key_desc_t bind_bind_window;
	key_desc_t bind_fullscreen_window;
	key_desc_t bind_float_window;

	array<key_bind_cmd_t, 10> bind_cmd;

	//xcb_timestamp_t _last_focus_time;
	//xcb_timestamp_t _last_button_press;

	/** store all client in mapping order, older first **/
	list<view_w> _net_client_list;
	list<view_w> _global_focus_history;

//	int _left_most_border;
//	int _top_most_border;

	/* do no allow copy */
	page_t(page_t const &) = delete;
	page_t &operator=(page_t const &) = delete;

	page_t();

	virtual ~page_t();

	void init(int * argc, char *** argv);
	/* run page main loop */
	void run();

	void bind_xdg_v5_shell(struct wl_client *client, uint32_t version, uint32_t id);

	void xdg_shell_v5_client_destroy(sh::xdg_v5_shell *);
	void xdg_shell_v6_client_destroy(sh::xdg_v5_shell *);
	void wl_shell_client_destroy(wl::wl_shell *);

	void switch_focused_to_fullscreen();
	void switch_focused_to_floating();
	void switch_focused_to_notebook();

	void handle_quit_page(page_keyboard * wk, ClutterEvent const & event);
	void handle_toggle_fullscreen(page_keyboard * wk, ClutterEvent const & event);
	void handle_close_window(page_keyboard * wk, ClutterEvent const & event);
	void handle_goto_desktop_at_right(page_keyboard * wk, ClutterEvent const & event);
	void handle_goto_desktop_at_left(page_keyboard * wk, ClutterEvent const & event);
	void handle_bind_window(page_keyboard * wk, ClutterEvent const & event);
	void handle_set_fullscreen_window(page_keyboard * wk, ClutterEvent const & event);
	void handle_set_floating_window(page_keyboard * wk, ClutterEvent const & event);

	void handle_alt_left_button(page_pointer *pointer, ClutterEvent const & event);
	void handle_alt_right_button(page_pointer *pointer, ClutterEvent const & event);

	void handle_bind_cmd_0(page_keyboard * wk, ClutterEvent const & event);
	void handle_bind_cmd_1(page_keyboard * wk, ClutterEvent const & event);
	void handle_bind_cmd_2(page_keyboard * wk, ClutterEvent const & event);
	void handle_bind_cmd_3(page_keyboard * wk, ClutterEvent const & event);
	void handle_bind_cmd_4(page_keyboard * wk, ClutterEvent const & event);
	void handle_bind_cmd_5(page_keyboard * wk, ClutterEvent const & event);
	void handle_bind_cmd_6(page_keyboard * wk, ClutterEvent const & event);
	void handle_bind_cmd_7(page_keyboard * wk, ClutterEvent const & event);
	void handle_bind_cmd_8(page_keyboard * wk, ClutterEvent const & event);
	void handle_bind_cmd_9(page_keyboard * wk, ClutterEvent const & event);


	view_p lookup_for_view(ClutterActor * v);

	//	void set_default_pop(shared_ptr<notebook_t> x);
//	display_compositor_t * get_render_context();
//
//	/** user inputs **/
//	void process_key_press_event(xcb_generic_event_t const * e);
//	void process_key_release_event(xcb_generic_event_t const * e);
//	void process_button_press_event(xcb_generic_event_t const * e);
//	void process_motion_notify(xcb_generic_event_t const * _e);
//	void process_button_release(xcb_generic_event_t const * _e);
//	/* SubstructureNotifyMask */
//	//void process_event(xcb_circulate_notify_event_t const * e);
//	void process_configure_notify_event(xcb_generic_event_t const * e);
//	void process_create_notify_event(xcb_generic_event_t const * e);
//	void process_destroy_notify_event(xcb_generic_event_t const * e);
//	void process_gravity_notify_event(xcb_generic_event_t const * e);
//	void process_map_notify_event(xcb_generic_event_t const * e);
//	void process_reparent_notify_event(xcb_generic_event_t const * e);
//	void process_unmap_notify_event(xcb_generic_event_t const * e);
//	void process_fake_unmap_notify_event(xcb_generic_event_t const * e);
//	void process_mapping_notify_event(xcb_generic_event_t const * e);
//	void process_selection_clear_event(xcb_generic_event_t const * e);
//	void process_focus_in_event(xcb_generic_event_t const * e);
//	void process_focus_out_event(xcb_generic_event_t const * e);
//	void process_enter_window_event(xcb_generic_event_t const * e);
//	void process_leave_window_event(xcb_generic_event_t const * e);
//
//	void process_expose_event(xcb_generic_event_t const * e);
//
//	void process_randr_notify_event(xcb_generic_event_t const * e);
//	void process_shape_notify_event(xcb_generic_event_t const * e);
//
//	/* SubstructureRedirectMask */
//	void process_circulate_request_event(xcb_generic_event_t const * e);
//	void process_configure_request_event(xcb_generic_event_t const * e);
//	void process_map_request_event(xcb_generic_event_t const * e);
//
//	/* PropertyChangeMask */
//	void process_property_notify_event(xcb_generic_event_t const * e);
//
//	/* Unmaskable Events */
//	void process_fake_client_message_event(xcb_generic_event_t const * e);
//
//	/* extension events */
//	void process_damage_notify_event(xcb_generic_event_t const * ev);
//
//	void process_event(xcb_generic_event_t const * e);
//
//	/* update _NET_CLIENT_LIST_STACKING and _NET_CLIENT_LIST */
//	void update_client_list();
//	void update_client_list_stacking();
//
//	/* update _NET_SUPPORTED */
//	void update_net_supported();
//
//	/* setup and create managed window */
//	xdg_surface_toplevel_t * manage(xcb_atom_t net_wm_type, std::shared_ptr<client_proxy_t> wa);
//
//	/* unmanage a managed window */
//	void unmanage(shared_ptr<xdg_surface_toplevel_t> mw);
//
//	/* update viewport and childs allocation */
//	void update_workarea();
//
	/* turn a managed window into fullscreen */
	void fullscreen(view_p c);
	void fullscreen(view_p c, shared_ptr<viewport_t> v);

	/* switch a fullscreened and managed window into floating or notebook window */
	void unfullscreen(view_p c);

	/* toggle fullscreen */
	void toggle_fullscreen(view_p c);
//
//	/* split a notebook into two notebook */
//	void split(shared_ptr<notebook_t> nbk, split_type_e type);
//
//	/* compute the allocation of viewport taking in account DOCKs */
//	void compute_viewport_allocation(shared_ptr<workspace_t> d, shared_ptr<viewport_t> v);
//
//	//void cleanup_not_managed_client(shared_ptr<xdg_surface_popup_t> c);
//
//	void process_net_vm_state_client_message(xcb_window_t c, long type, xcb_atom_t state_properties);
//
	void insert_in_tree_using_transient_for(view_p c);
//
//	void safe_update_transient_for(shared_ptr<xdg_surface_base_t> c);
//
//	shared_ptr<xdg_surface_base_t> get_transient_for(shared_ptr<xdg_surface_base_t> c);
//	void logical_raise(shared_ptr<xdg_surface_base_t> c);
//
//	/* attach floating window in a notebook */
	void bind_window(shared_ptr<view_t> mw);
//	void grab_pointer();
//	/* if grab is linked to a given window remove this grab */
//	void cleanup_grab();
//	/* find a valid notebook, that is in subtree base and that is no nbk */
	shared_ptr<notebook_t> get_another_notebook(shared_ptr<tree_t> base, shared_ptr<tree_t> nbk);
//	/* find where the managed window is */
	static shared_ptr<notebook_t> find_parent_notebook_for(shared_ptr<view_t> mw);
//	shared_ptr<xdg_surface_toplevel_t> find_managed_window_with(xcb_window_t w);
	static shared_ptr<viewport_t> find_viewport_of(shared_ptr<tree_t> n);
	static shared_ptr<workspace_t> find_desktop_of(shared_ptr<tree_t> n);
//	void set_window_cursor(xcb_window_t w, xcb_cursor_t c);
	void update_windows_stack();
	void update_viewport_layout();
	void remove_viewport(shared_ptr<workspace_t> d, shared_ptr<viewport_t> v);
//	void onmap(xcb_window_t w);
//	void create_managed_window(xcb_window_t w, xcb_atom_t type);
//	void ackwoledge_configure_request(xcb_configure_request_event_t const * e);
//	void create_unmanaged_window(xcb_window_t w, xcb_atom_t type);
//	bool get_safe_net_wm_user_time(shared_ptr<xdg_surface_base_t> c, xcb_timestamp_t & time);
//	void update_page_areas();
//	void set_desktop_geometry(long width, long height);
//	shared_ptr<xdg_surface_base_t> find_client_with(xcb_window_t w);
//	shared_ptr<xdg_surface_base_t> find_client(xcb_window_t w);
	void remove_client(view_p c);
//
//	void raise_child(shared_ptr<tree_t> t);
//	void process_notebook_client_menu(shared_ptr<xdg_surface_toplevel_t> c, int selected);
//
//	void check_x11_extension();
//
//	void create_identity_window();
//	void register_wm();
//	void register_cm();
//
//	void render(cairo_t * cr, time64_t time);
//	bool need_render(time64_t time);
//
//	bool check_for_managed_window(xcb_window_t w);
//	bool check_for_destroyed_window(xcb_window_t w);
//
//	void update_keymap();
//
//	template<void (page_t::*func)(weston_keyboard *, uint32_t, uint32_t)>
//	void bind_key(xkb_keymap * keymap, key_desc_t & key);
//	void bind_all_keys(weston_seat * seat);
//
//	void on_seat_created(weston_seat * seat);

//	shared_ptr<xdg_surface_toplevel_t> find_hidden_client_with(xcb_window_t w);
//
//	vector<page_event_t> compute_page_areas(viewport_t * v) const;
//
//	void render();
//
//	/** debug function that try to print the state of page in stdout **/
//	void print_state() const;
//	void update_current_desktop() const;
//	void switch_to_desktop(unsigned int desktop);
//	void update_fullscreen_clients_position();
//	void update_desktop_visibility();
//	//void process_error(xcb_generic_event_t const * e);
//	void start_compositor();
//	void stop_compositor();
	void run_cmd(string const & cmd_with_args);
//
//	void start_alt_tab(xcb_timestamp_t time);
//
//	vector<shared_ptr<xdg_surface_toplevel_t>> get_sticky_client_managed(shared_ptr<tree_t> base);
//	void reconfigure_docks(shared_ptr<workspace_t> const & d);
//
//	void mark_durty(shared_ptr<tree_t> t);
//
//
//	void process_pending_events();
//
	bool global_focus_history_front(shared_ptr<view_t> & out);
	void global_focus_history_remove(shared_ptr<view_t> in);
	void global_focus_history_move_front(shared_ptr<view_t> in);
	bool global_focus_history_is_empty();
//
//	void on_visibility_change_handler(xcb_window_t xid, bool visible);
//	void on_block_mainloop_handler();
//
//	auto find_client_managed_with(xcb_window_t w) -> shared_ptr<xdg_surface_toplevel_t>;


	static void bind_wl_shell(wl_client * client, void * data,
					      uint32_t version, uint32_t id);
	static void bind_xdg_v5_shell(wl_client * client, void * data,
					      uint32_t version, uint32_t id);
	static void bind_xdg_v6_shell(wl_client * client, void * data,
					      uint32_t version, uint32_t id);
	static void print_tree_binding(struct weston_keyboard *keyboard, uint32_t time,
			  uint32_t key, void *data);

	void configure_surface(view_p,
			int32_t sx, int32_t sy);

//	/**
//	 * page_t virtual API
//	 **/
//
	auto conf() const -> page_configuration_t const &;
	auto theme() const -> theme_t const *;
	auto find_mouse_viewport(int x, int y) const -> viewport_p;
	auto get_current_workspace() const -> workspace_p const &;
	auto get_workspace(int id) const -> workspace_p const &;
	int  get_workspace_count() const;
	int  create_workspace();
	void detach(tree_p t);
	void insert_window_in_notebook(view_p x, notebook_p n = nullptr);
	void fullscreen_client_to_viewport(view_p c, viewport_p v);
	void unbind_window(view_p mw);
	void split_left(notebook_p nbk, view_p c);
	void split_right(notebook_p nbk, view_p c);
	void split_top(notebook_p nbk, view_p c);
	void split_bottom(notebook_p nbk, view_p c);
	void set_keyboard_focus(page_seat * seat, view_p w);
	void notebook_close(notebook_p nbk);
	auto global_client_focus_history() -> list<view_w>;
	void sync_tree_view();
	void manage_client(surface_t * s);
	void manage_popup(surface_t * s);
	void configure_popup(surface_t * s);
	void schedule_repaint();
	void destroy_surface(surface_t * s);
	void start_move(surface_t * s, page_seat * seat, uint32_t serial);
	void start_resize(surface_t * s, page_seat * seat, uint32_t serial, edge_e edges);

};


}



#endif /* PAGE_HXX_ */
