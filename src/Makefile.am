bin_PROGRAMS = page-compositor 

AM_CXXFLAGS =  \
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
	wl/wl-data-device-manager.cxx \
	wl/wl-data-device-manager.hxx \
	wl/wl-touch.cxx \
	wl/wl-touch.hxx \
	wl/wl-surface.cxx \
	wl/wl-surface.hxx \
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
	wayland-interface.cxx \
	wayland-interface.hxx \
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

