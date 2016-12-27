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

}


