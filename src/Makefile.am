bin_PROGRAMS = page-compositor 

AM_CXXFLAGS =  \
	-DCLUTTER_ENABLE_COMPOSITOR_API \
	-DCLUTTER_ENABLE_EXPERIMENTAL_API \
	-DCOGL_ENABLE_EXPERIMENTAL_API \
	-DCOGL_ENABLE_EXPERIMENTAL_2_0_API \
	-DCLUTTER_DISABLE_DEPRECATION_WARNINGS \
	-DCOGL_DISABLE_DEPRECATION_WARNINGS \
	-DDATADIR='"$(datadir)"' \
	-std=c++11 \
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
	-std=c11 \
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
	page-core.cxx \
	page-core.hxx \
	page-seat.cxx \
	page-seat.hxx \
	page-keyboard.cxx \
	page-keyboard.hxx \
	page-pointer.cxx \
	page-pointer.hxx \
	page-touch.cxx \
	page-touch.hxx \
	page-output.cxx \
	page-output.hxx \
	page-default-pointer-grab.cxx \
	page-default-pointer-grab.hxx \
	page-pointer-grab.cxx \
	page-pointer-grab.hxx \
	page-default-keyboard-grab.cxx \
	page-default-keyboard-grab.hxx \
	page-keyboard-grab.cxx \
	page-keyboard-grab.hxx \
	wayland-interface.cxx \
	wayland-interface.hxx \
	xdg-shell-unstable-v5-interface.cxx \
	xdg-shell-unstable-v5-interface.hxx \
	xdg-shell-unstable-v5-protocol.c \
	xdg-shell-unstable-v5-server-protocol.h \
	utils.cxx \
	utils.hxx \
	config_handler.cxx \
	congig_handler.hxx \
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
	rm -f wayland-interface.cxx
	rm -f wayland-interface.hxx

