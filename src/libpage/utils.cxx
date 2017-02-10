/*
 * Copyright (2016) Benoit Gschwind
 *
 * utils.cxx is part of page-compositor.
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

#include "utils.hxx"

#include <unistd.h>
#include <cstring>
#include <glib.h>
#include <sys/fcntl.h>

namespace page {

cairo_region_t * cairo_region_create_infini()
{
	cairo_rectangle_int_t rect = {
			numeric_limits<int>::min(), numeric_limits<int>::min(),
			numeric_limits<int>::max(), numeric_limits<int>::max()
	};
	return cairo_region_create_rectangle(&rect);
}

int create_anonymous_file(off_t size, GError **error)
{
	static const char tpl[] = "page-shared-XXXXXX";
	char *path;
	int fd, flags;

	fd = g_file_open_tmp(tpl, &path, error);

	if (fd == -1)
		return -1;

	unlink(path);
	g_free(path);

	flags = fcntl(fd, F_GETFD);
	if (flags == -1)
		goto err;

	if (fcntl(fd, F_SETFD, flags | FD_CLOEXEC) == -1)
		goto err;

	if (ftruncate(fd, size) < 0)
		goto err;

	return fd;

	err:

	g_set_error_literal(error, G_FILE_ERROR,
			g_file_error_from_errno(errno), strerror(errno));
	close(fd);

	return -1;
}


/**
 * Draw rectangle with all corner rounded
 **/
void cairo_rectangle_arc_corner(cairo_t * cr, double x, double y, double w, double h, double radius, uint8_t corner_mask) {

	if(radius * 2 > w) {
		radius = w / 2.0;
	}

	if(radius * 2 > h) {
		radius = h / 2.0;
	}

	cairo_new_path(cr);
	cairo_move_to(cr, x, y + h - radius);
	cairo_line_to(cr, x, y + radius);
	/* top-left */
	if(corner_mask&CAIRO_CORNER_TOPLEFT) {
		cairo_arc(cr, x + radius, y + radius, radius, 2.0 * M_PI_2, 3.0 * M_PI_2);
	} else {
		cairo_line_to(cr, x, y);
	}
	cairo_line_to(cr, x + w - radius, y);
	/* top-right */
	if(corner_mask&CAIRO_CORNER_TOPRIGHT) {
		cairo_arc(cr, x + w - radius, y + radius, radius, 3.0 * M_PI_2, 4.0 * M_PI_2);
	} else {
		cairo_line_to(cr, x + w, y);
	}
	cairo_line_to(cr, x + w, y + h - radius);
	/* bot-right */
	if(corner_mask&CAIRO_CORNER_BOTRIGHT) {
		cairo_arc(cr, x + w - radius, y + h - radius, radius, 0.0 * M_PI_2, 1.0 * M_PI_2);
	} else {
		cairo_line_to(cr, x + w, y + h);
	}
	cairo_line_to(cr, x + radius, y + h);
	/* bot-left */
	if(corner_mask&CAIRO_CORNER_BOTLEFT) {
		cairo_arc(cr, x + radius, y + h - radius, radius, 1.0 * M_PI_2, 2.0 * M_PI_2);
	} else {
		cairo_line_to(cr, x, y + h);
	}
	cairo_close_path(cr);

}

void cairo_rectangle_arc_corner(cairo_t * cr, rect const & position, double radius, uint8_t corner_mask) {
	cairo_rectangle_arc_corner(cr, position.x, position.y, position.w, position.h, radius, corner_mask);
}


}


