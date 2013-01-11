AC_PREREQ([2.65])

AC_INIT([TimeWatcher], [0.1],
	[[julien.chaintron@gmail.com][laurent.senta@gmail.com]])

AC_MSG_CHECKING([whether to build with debug information])
AC_ARG_ENABLE([debug], [], [debugit="yes"])

if test x"$debugit" = x"yes"; then
	AM_CPPFLAGS="-DDEBUG -DALLOW_CHECK_ASSERT -g -ggdb"
else
	AM_CPPFLAGS="-DNDEBUG"
fi

AM_CXXFLAGS="$AM_CXXFLAGS -O3 -W -Wall -Wextra -std=c++0x"

AC_PROG_CXX(g++)

# OpenCV
PKG_CHECK_MODULES(OPENCV, opencv >= 2.2)
AC_SUBST(OPENCV_CFLAGS)
AC_SUBST(OPENCV_LIBS)

AC_SUBST([AM_CXXFLAGS])
AC_SUBST([AM_CPPFLAGS])

AM_INIT_AUTOMAKE([foreign -Wall -Werror])
AC_CONFIG_HEADER(config.h)

AC_CONFIG_FILES([Makefile]
		[src/Makefile])

AC_OUTPUT
