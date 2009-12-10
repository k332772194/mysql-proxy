/* $%BEGINLICENSE%$
 Copyright (C) 2009 MySQL AB, 2009 Sun Microsystems, Inc

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; version 2 of the License.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 $%ENDLICENSE%$ */

/** @addtogroup unittests Unit tests */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif
#include <fcntl.h>
#include <errno.h>

#include <glib.h>

#include "chassis-filemode.h"

#ifndef _WIN32
/* only run theses tests on non windows platforms */

#define TOO_OPEN	0666
#define GOOD_PERMS	0660
/**
 * @test 
 */
void test_file_permissions(void)
{
	char filename[MAXPATHLEN] = "/tmp/permsXXXXXX";
	int	 fd;
	int ret;
	
	g_log_set_always_fatal(G_LOG_FATAL_MASK);

	/* 1st test: non-existent file */
	g_assert_cmpint(chassis_filemode_check("/tmp/a_non_existent_file"), ==, -1);

	fd = mkstemp(filename);
	if (fd < 0) {
		g_critical("%s: mkstemp(%s) failed: %s (%d)",
				G_STRLOC,
				filename,
				g_strerror(errno), errno);
	}
	g_assert_cmpint(fd, >=, 0);

	/* 2nd test: too permissive */
	ret = chmod(filename, TOO_OPEN);
	if (ret < 0) {
		g_critical("%s: chmod(%s) failed: %s (%d)",
				G_STRLOC,
				filename,
				g_strerror(errno), errno);
	}
	g_assert_cmpint(ret, ==, 0);
	g_assert_cmpint(chassis_filemode_check(filename), ==, 1);

	/* 3rd test: OK */
	ret = chmod(filename, GOOD_PERMS);
	if (ret < 0) {
		g_critical("%s: chmod(%s) failed: %s (%d)",
				G_STRLOC,
				filename,
				g_strerror(errno), errno);
	}
	g_assert_cmpint(ret, ==, 0);
	g_assert_cmpint(chassis_filemode_check(filename), ==, 0);

	/* 4th test: non-regular file */
	close(fd);
	remove(filename);
	ret = mkdir(filename, GOOD_PERMS);
	if (ret < 0) {
		g_critical("%s: mkdir(%s) failed: %s (%d)",
				G_STRLOC,
				filename,
				g_strerror(errno), errno);
	}
	g_assert_cmpint(ret, ==, 0);
	g_assert_cmpint(chassis_filemode_check(filename), ==, -1);

	/* clean up */
	ret = rmdir(filename);
	if (ret < 0) {
		g_critical("%s: rmdir(%s) failed: %s (%d)",
				G_STRLOC,
				filename,
				g_strerror(errno), errno);
	}
	g_assert_cmpint(ret, ==, 0);

}
/*@}*/

int main(int argc, char **argv) {
	g_thread_init(NULL);

	
	g_test_init(&argc, &argv, NULL);
	g_test_bug_base("http://bugs.mysql.com/");
	
	g_test_add_func("/core/basedir/fileperm", test_file_permissions);
	
	return g_test_run();
}
#else
int main() {
	return 77;
}
#endif
