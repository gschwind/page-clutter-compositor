AC_INIT([page-compositor], [0.0.1])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([-Wall])
AC_CONFIG_SRCDIR([src/main.cxx])
AC_CONFIG_HEADERS([src/config.hxx])

CHECK_GNU_MAKE

dnl Check for required programs
AC_GNU_SOURCE
AC_PROG_CXX
AC_PROG_LIBTOOL

# check for C++11
AX_CXX_COMPILE_STDCXX_11

PKG_CHECK_MODULES(CLUTTER, [
	clutter-1.0 >= 1.26.1
	clutter-wayland-compositor-1.0 >= 1.26.0
	clutter-wayland-1.0 >= 1.26.1
	clutter-x11-1.0 >= 1.26.1
	clutter-egl-1.0 >= 1.26.1
	clutter-cogl-1.0 >= 1.26.1
])
	
AC_SUBST(CLUTTER_CFLAGS)
AC_SUBST(CLUTTER_LIBS)

PKG_CHECK_MODULES(CAIRO, cairo >= 1.0.0)
AC_SUBST(CAIRO_CFLAGS)
AC_SUBST(CAIRO_LIBS)

PKG_CHECK_MODULES(PANGO, pangocairo >= 0.28)
AC_SUBST(PANGO_CFLAGS)
AC_SUBST(PANGO_LIBS)

PKG_CHECK_MODULES(WAYLAND, [
	wayland-server >= 1.9.0
	wayland-client >= 1.9.0
	wayland-cursor >= 1.9.0
])

AC_SUBST(WAYLAND_CFLAGS)
AC_SUBST(WAYLAND_LIBS)

PKG_CHECK_MODULES(PIXMAN, pixman-1 >= 0.32.8)
AC_SUBST(PIXMAN_CFLAGS)
AC_SUBST(PIXMAN_LIBS)

PKG_CHECK_MODULES(GLIB, glib-2.0 >= 2.0.0)
AC_SUBST(GLIB_CFLAGS)
AC_SUBST(GLIB_LIBS)

AC_CHECK_LIB(rt, clock_gettime,
    [RT_LIBS="-lrt"
     AC_DEFINE(HAVE_CLOCK_GETTIME, 1,
	       [Define to 1 if you have the `clock_gettime` function.])])
AC_SUBST(RT_LIBS)

AC_PATH_PROG([has_wayland_scanner], [wayland-scanner], [yes], [no])
if test $has_wayland_scanner = yes; then
	wayland_scanner="wayland-scanner"
else
	dnl If wayland scanner is not found, try another way.
	PKG_CHECK_MODULES(WAYLAND_SCANNER, [wayland-scanner])
	wayland_scanner=`$PKG_CONFIG --variable=wayland_scanner wayland-scanner`
fi

AC_SUBST(wayland_scanner)

dnl Try LTO flags.
OLD_LDFLAGS="${LDFLAGS}"
LDFLAGS="${LDFLAGS} -flto"
AC_LINK_IFELSE([AC_LANG_SOURCE([int main () { return 0; }])],[
 AC_SUBST(LTO, [-flto])
])
LDFLAGS="${OLD_LDFLAGS}"

AC_CONFIG_FILES([
  Makefile
  src/Makefile
  ])
AC_OUTPUT
