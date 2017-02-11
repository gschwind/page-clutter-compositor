/*
 * viewport.cxx
 *
 * copyright (2010-2014) Benoit Gschwind
 *
 * This code is licensed under the GPLv3. see COPYING file for more details.
 *
 */

#include <algorithm>
#include <typeinfo>
#include "notebook.hxx"
#include "viewport.hxx"

namespace page {

using namespace std;

gboolean viewport_t::wrapper_draw_callback(ClutterCanvas *canvas, cairo_t *cr, int width,
		int height, gpointer user_data)
{
	auto viewport = reinterpret_cast<viewport_t*>(user_data);
	viewport->draw(cr, width, height);
	return FALSE;
}

viewport_t::viewport_t(page_t * ctx, rect const & area) :
		_ctx{ctx},
		_raw_aera{area},
		_effective_area{area},
		_is_durty{true},
		_exposed{false},
		_subtree{nullptr}
{
	_page_area = rect{0, 0, _effective_area.w, _effective_area.h};
	_subtree = make_shared<notebook_t>(_ctx);
	_subtree->set_parent(this);

	_canvas = clutter_canvas_new();
	g_object_ref_sink(_canvas);
	clutter_canvas_set_size(CLUTTER_CANVAS(_canvas), _effective_area.w, _effective_area.h);

	_default_view = clutter_actor_new();
	g_object_ref_sink(_default_view);
	clutter_actor_set_content(_default_view, _canvas);
	clutter_actor_set_content_scaling_filters(_default_view,
			CLUTTER_SCALING_FILTER_NEAREST, CLUTTER_SCALING_FILTER_NEAREST);
	clutter_actor_set_reactive (_default_view, TRUE);
	g_signal_connect(CLUTTER_CANVAS(_canvas), "draw",
			G_CALLBACK(wrapper_draw_callback), this);
	clutter_content_invalidate(_canvas);
	clutter_actor_set_position(_default_view, _effective_area.x, _effective_area.y);
	clutter_actor_set_size(_default_view, _effective_area.w, _effective_area.h);

	_subtree->set_allocation(_page_area);
	printf("viewport size = %s\n", _page_area.to_string().c_str());

}

viewport_t::~viewport_t() {
	g_object_unref(_canvas);
	g_object_unref(_default_view);
}

void viewport_t::replace(shared_ptr<page_component_t> src, shared_ptr<page_component_t> by) {
	//printf("replace %p by %p\n", src, by);

	if (_subtree == src) {
		_subtree->clear_parent();
		_subtree = by;
		_subtree->set_parent(this);
		_subtree->set_allocation(_page_area);
	} else {
		throw std::runtime_error("viewport: bad child replacement!");
	}
}

void viewport_t::remove(shared_ptr<tree_t> src) {
	if(src == _subtree) {
		_subtree->clear_parent();
		_subtree.reset();
	}
}

void viewport_t::set_allocation(rect const & area) {
	_effective_area = area;
	_page_area = rect{0, 0, _effective_area.w, _effective_area.h};
	if(_subtree != nullptr)
		_subtree->set_allocation(_page_area);
	update_renderable();
}

void viewport_t::set_raw_area(rect const & area) {
	_raw_aera = area;
}

rect const & viewport_t::raw_area() const {
	return _raw_aera;
}

void viewport_t::activate() {
	if(_parent != nullptr) {
		_parent->activate(shared_from_this());
	}

	queue_redraw();
}


void viewport_t::activate(shared_ptr<tree_t> t) {
	assert(t != nullptr);
	assert(t == _subtree);
	activate();
}

string viewport_t::get_node_name() const {
	return _get_node_name<'V'>();
}

void viewport_t::update_layout(time64_t const time) {

}

void viewport_t::render_finished() {
	_damaged.clear();
}

rect viewport_t::allocation() const {
	return _effective_area;
}

rect const & viewport_t::page_area() const {
	return _page_area;
}

void viewport_t::append_children(vector<shared_ptr<tree_t>> & out) const {
	if(_subtree != nullptr) {
		out.push_back(_subtree);
	}
}

void viewport_t::hide() {
	if(_subtree != nullptr) {
		_subtree->hide();
	}
	_is_visible = false;
}

void viewport_t::show() {
	_is_visible = true;
	//_ctx->dpy()->map(_win);
	update_renderable();
	if(_subtree != nullptr) {
		_subtree->show();
	}
}

void viewport_t::update_renderable() {
//	if(_ctx->cmp() != nullptr) {
//		_back_surf = make_shared<pixmap_t>(PIXMAP_RGB, _page_area.w, _page_area.h);
//	}
	//_ctx->dpy()->move_resize(_win, _effective_area);
}

void viewport_t::_redraw_back_buffer() {
	clutter_content_invalidate(CLUTTER_CONTENT(_default_view));
}

void viewport_t::trigger_redraw() {
	/** redraw all child **/
	tree_t::trigger_redraw();
	_redraw_back_buffer();

//	if(_exposed and _ctx->cmp() == nullptr) {
//		_exposed = false;
//		paint_expose();
//	}

}

/* mark renderable_page for redraw */
void viewport_t::queue_redraw() {
	_is_durty = true;
	clutter_content_invalidate(_canvas);
}

region viewport_t::get_damaged() {
	return _damaged;
}

void viewport_t::paint_expose() {
	if(not _is_visible)
		return;
//
//	cairo_surface_t * surf = cairo_xcb_surface_create(_ctx->dpy()->xcb(), _win, _ctx->dpy()->root_visual(), _effective_area.w, _effective_area.h);
//	cairo_t * cr = cairo_create(surf);
//	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
//	cairo_set_source_surface(cr, _back_surf->get_cairo_surface(), 0.0, 0.0);
//	cairo_rectangle(cr, 0.0, 0.0, _effective_area.w, _effective_area.h);
//	cairo_fill(cr);
//
//	cairo_destroy(cr);
//	cairo_surface_destroy(surf);
}

rect viewport_t::get_window_position() const {
	return _effective_area;
}

//void viewport_t::expose(xcb_expose_event_t const * e) {
//	if(e->window == _win) {
//		_exposed = true;
//	}
//}

auto viewport_t::get_visible_region() -> region {
	return region{_effective_area};
}

auto viewport_t::get_opaque_region() -> region {
	return region{_effective_area};
}

void viewport_t::draw(cairo_t * cr, int width, int height) {
	printf("call %s\n", __PRETTY_FUNCTION__);

	if(not _is_durty)
		return;

	cairo_save(cr);
	cairo_identity_matrix(cr);
	cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
	cairo_paint(cr);

	auto splits = filter_class<split_t>(get_all_children());
	for (auto x : splits) {
		x->render_legacy(cr);
	}

	auto notebooks = filter_class<notebook_t>(get_all_children());
	for (auto x : notebooks) {
		x->render_legacy(cr);
	}

	cairo_restore(cr);

	_is_durty = false;
	_exposed = true;
	_damaged += _effective_area;

}

void viewport_t::get_min_allocation(int & width, int & height) const {
	width = 0;
	height = 0;

	if(_subtree != nullptr) {
		_subtree->get_min_allocation(width, height);
	}

}

auto viewport_t::get_default_view() const -> ClutterActor * {
	return _default_view;
}

}
