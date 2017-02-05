bin_PROGRAMS = page-compositor 

AM_CXXFLAGS =  \
	-DCLUTTER_ENABLE_COMPOSITOR_API \
	-DCLUTTER_ENABLE_EXPERIMENTAL_API \
	-DCOGL_ENABLE_EXPERIMENTAL_API \
	-DCOGL_ENABLE_EXPERIMENTAL_2_0_API \
	-DCLUTTER_DISABLE_DEPRECATION_WARNINGS \
	-DCOGL_DISABLE_DEPRECATION_WARNINGS \
	-DDATADIR='"$(datadir)"' \
	-rdynamic \
	@LTO@ \
	@CLUTTER_CFLAGS@ \
	@WAYLAND_CFLAGS@ \
	@PIXMAN_CFLAGS@ \
	@CAIRO_CFLAGS@ \
	@PANGO_CFLAGS@ \
	@GLIB_CFLAGS@ \
	-fno-strict-aliasing
	
AM_CFLAGS =  \
	-DCLUTTER_ENABLE_COMPOSITOR_API \
	-DCLUTTER_ENABLE_EXPERIMENTAL_API \
	-DCOGL_ENABLE_EXPERIMENTAL_API \
	-DCOGL_ENABLE_EXPERIMENTAL_2_0_API \
	-DCLUTTER_DISABLE_DEPRECATION_WARNINGS \
	-DCOGL_DISABLE_DEPRECATION_WARNINGS \
	-DDATADIR='"$(datadir)"' \
	-rdynamic \
	@LTO@ \
	@CLUTTER_CFLAGS@ \
	@WAYLAND_CFLAGS@ \
	@PIXMAN_CFLAGS@ \
	@CAIRO_CFLAGS@ \
	@PANGO_CFLAGS@ \
	@GLIB_CFLAGS@ \
	-fno-strict-aliasing

BUILT_SOURCES = \
	xdg-shell-unstable-v5-protocol.c \
	xdg-shell-unstable-v5-server-protocol.h \
	xdg-shell-unstable-v5-interface.cxx \
	xdg-shell-unstable-v5-interface.hxx \
	xdg-shell-unstable-v6-protocol.c \
	xdg-shell-unstable-v6-server-protocol.h \
	xdg-shell-unstable-v6-interface.cxx \
	xdg-shell-unstable-v6-interface.hxx \
	wayland-interface.cxx \
	wayland-interface.hxx

page_compositor_SOURCES = \
	page/blur_image_surface.cxx \
	page/dropdown_menu.cxx \
	page/grab_handlers.cxx \
	page/leak_checker.cxx \
	page/notebook.cxx \
	page/page.cxx \
	page/page_region_test.cxx \
	page/page_root.cxx \
	page/popup_alt_tab.cxx \
	page/simple2_theme.cxx \
	page/split.cxx \
	page/surface.cxx \
	page/tiny_theme.cxx \
	page/tree.cxx \
	page/view.cxx \
	page/viewport.cxx \
	page/wl-shell-shell.cxx \
	page/xwl-shell-surface.cxx \
	page/workspace.cxx \
	page/xdg-shell-v5-shell.cxx \
	page/xdg-shell-v5-surface-base.cxx \
	page/xdg-shell-v5-surface-popup.cxx \
	page/xdg-shell-v5-surface-toplevel.cxx \
	page/xdg-shell-v6-popup.cxx \
	page/xdg-shell-v6-positioner.cxx \
	page/xdg-shell-v6-shell.cxx \
	page/xdg-shell-v6-surface.cxx \
	page/xdg-shell-v6-toplevel.cxx \
	page/blur_image_surface.hxx \
	page/box.hxx \
	page/cairo_surface_type_name.hxx \
	page/color.hxx \
	page/compositor_overlay.hxx \
	page/config.hxx \
	page/dropdown_menu.hxx \
	page/exception.hxx \
	page/floating_event.hxx \
	page/grab_handlers.hxx \
	page/key_desc.hxx \
	page/keymap.hxx \
	page/leak_checker.hxx \
	page/listener.hxx \
	page/mainloop.hxx \
	page/motif_hints.hxx \
	page/notebook.hxx \
	page/page_component.hxx \
	page/page_context.hxx \
	page/page_event.hxx \
	page/page_exception.hxx \
	page/page.hxx \
	page/page_root.hxx \
	page/popup_alt_tab.hxx \
	page/popup_notebook0.hxx \
	page/popup_split.hxx \
	page/region.hxx \
	page/renderable_empty.hxx \
	page/renderable_floating_outer_gradien.hxx \
	page/renderable.hxx \
	page/renderable_notebook_fading.hxx \
	page/renderable_pixmap.hxx \
	page/renderable_solid.hxx \
	page/renderable_thumbnail.hxx \
	page/renderable_unmanaged_gaussian_shadow.hxx \
	page/renderable_unmanaged_outer_gradien.hxx \
	page/simple2_theme.hxx \
	page/smart_pointer.hxx \
	page/split.hxx \
	page/surface.hxx \
	page/theme.hxx \
	page/theme_managed_window.hxx \
	page/theme_notebook.hxx \
	page/theme_split.hxx \
	page/theme_tab.hxx \
	page/time.hxx \
	page/tiny_theme.hxx \
	page/transition.hxx \
	page/tree.hxx \
	page/tree-types.hxx \
	page/view.hxx \
	page/viewport.hxx \
	page/window_overlay.hxx \
	page/wl-shell-shell.hxx \
	page/xwl-shell-surface.hxx \
	page/workspace.hxx \
	page/x11_func_name.hxx \
	page/xdg-popup-interface.hxx \
	page/xdg-shell-v5-shell.hxx \
	page/xdg-shell-v5-surface-base.hxx \
	page/xdg-shell-v5-surface-popup.hxx \
	page/xdg-shell-v5-surface-toplevel.hxx \
	page/xdg-shell-v6-popup.hxx \
	page/xdg-shell-v6-positioner.hxx \
	page/xdg-shell-v6-shell.hxx \
	page/xdg-shell-v6-surface.hxx \
	page/xdg-shell-v6-toplevel.hxx \
	meta/meta-surface-actor-wayland.cxx \
	meta/meta-surface-actor-wayland.hxx \
	meta/clutter-utils.c \
	meta/clutter-utils.h \
	meta/region-utils.c \
	meta/region-utils.h \
	meta/meta-cullable.c \
	meta/meta-cullable.h \
	meta/meta-shaped-texture.c \
	meta/meta-shaped-texture.h \
	meta/meta-shaped-texture-private.h \
	meta/meta-surface-actor.c \
	meta/meta-surface-actor.h \
	meta/meta-texture-rectangle.c \
	meta/meta-texture-rectangle.h \
	meta/meta-texture-tower.c \
	meta/meta-texture-tower.h \
	meta/window.h \
	wl/wl-data-device-manager.cxx \
	wl/wl-data-device-manager.hxx \
	wl/wl-data-device.cxx \
	wl/wl-data-device.hxx \
	wl/wl-touch.cxx \
	wl/wl-touch.hxx \
	wl/wl-surface.cxx \
	wl/wl-surface.hxx \
	wl/wl-surface-state.cxx \
	wl/wl-surface-state.hxx \
	wl/wl-shell.cxx \
	wl/wl-shell.hxx \
	wl/wl-shell-surface.cxx \
	wl/wl-shell-surface.hxx \
	wl/wl-seat.cxx \
	wl/wl-seat.hxx \
	wl/wl-pointer.cxx \
	wl/wl-pointer.hxx \
	wl/wl-keyboard.cxx \
	wl/wl-keyboard.hxx \
	wl/wl-compositor.cxx \
	wl/wl-compositor.hxx \
	wl/wl-buffer.cxx \
	wl/wl-buffer.hxx \
	wl/wl-region.cxx \
	wl/wl-region.hxx \
	wl/wl-callback.cxx \
	wl/wl-callback.hxx \
	wl/wl-output.cxx \
	wl/wl-output.hxx \
	wl/wl-subcompositor.cxx \
	wl/wl-subcompositor.hxx \
	wl/wl-subsurface.cxx \
	wl/wl-subsurface.hxx \
	sh/xdg-v5-shell.cxx \
	sh/xdg-v5-shell.hxx \
	sh/xdg-v5-surface.cxx \
	sh/xdg-v5-surface.hxx \
	sh/xdg-v5-popup.cxx \
	sh/xdg-v5-popup.hxx \
	libpage/page-core.cxx \
	libpage/page-core.hxx \
	libpage/page-seat.cxx \
	libpage/page-seat.hxx \
	libpage/page-keyboard.cxx \
	libpage/page-keyboard.hxx \
	libpage/page-pointer.cxx \
	libpage/page-pointer.hxx \
	libpage/page-touch.cxx \
	libpage/page-touch.hxx \
	libpage/page-output.cxx \
	libpage/page-output.hxx \
	libpage/page-default-pointer-grab.cxx \
	libpage/page-default-pointer-grab.hxx \
	libpage/page-pointer-grab.cxx \
	libpage/page-pointer-grab.hxx \
	libpage/page-default-keyboard-grab.cxx \
	libpage/page-default-keyboard-grab.hxx \
	libpage/page-keyboard-grab.cxx \
	libpage/page-keyboard-grab.hxx \
	libpage/utils.cxx \
	libpage/utils.hxx \
	libpage/config_handler.cxx \
	libpage/congig_handler.hxx \
	wayland-interface.cxx \
	wayland-interface.hxx \
	xdg-shell-unstable-v5-interface.cxx \
	xdg-shell-unstable-v5-interface.hxx \
	xdg-shell-unstable-v5-protocol.c \
	xdg-shell-unstable-v5-server-protocol.h \
	xdg-shell-unstable-v6-interface.cxx \
	xdg-shell-unstable-v6-interface.hxx \
	xdg-shell-unstable-v6-protocol.c \
	xdg-shell-unstable-v6-server-protocol.h \
	main.cxx 

page_compositor_LDADD = \
	@LTO@ \
	@CLUTTER_LIBS@ \
	@PIXMAN_LIBS@ \
	@WAYLAND_LIBS@ \
	@CAIRO_LIBS@ \
	@PANGO_LIBS@ \
	@GLIB_LIBS@ \
	@RT_LIBS@ 

%-protocol.c : $(top_srcdir)/protocol/%.xml
	@wayland_scanner@ code < $< > $@

%-server-protocol.h : $(top_srcdir)/protocol/%.xml
	@wayland_scanner@ server-header < $< > $@

%-client-protocol.h : $(top_srcdir)/protocol/%.xml
	@wayland_scanner@ client-header < $< > $@

%-interface.hxx : $(top_srcdir)/protocol/%.xml
	$(top_srcdir)/tools/wayland-cxx-scanner.py header $< > $@

%-interface.cxx : $(top_srcdir)/protocol/%.xml %-interface.hxx
	$(top_srcdir)/tools/wayland-cxx-scanner.py code $< > $@

clean-local:
	rm -f xdg-shell-protocol.c
	rm -f xdg-shell-server-protocol.h
	rm -f xdg-shell-unstable-v5-protocol.c
	rm -f xdg-shell-unstable-v5-server-protocol.h
	rm -f xdg-shell-unstable-v5-interface.cxx
	rm -f xdg-shell-unstable-v5-interface.hxx
	rm -f xdg-shell-unstable-v6-protocol.c
	rm -f xdg-shell-unstable-v6-server-protocol.h
	rm -f xdg-shell-unstable-v6-interface.cxx
	rm -f xdg-shell-unstable-v6-interface.hxx
	rm -f wayland-interface.cxx
	rm -f wayland-interface.hxx

