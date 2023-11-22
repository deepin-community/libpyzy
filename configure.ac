# vim:set et ts=4:
#
# libpyzy - The Chinese PinYin and Bopomofo conversion library.
#
# Copyright (c) 2008-2010 Peng Huang <shawn.p.huang@gmail.com>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
# USA

# If pyzy_released is 0, append datestamp to the version number.
m4_define([pyzy_released], [1])

m4_define([pyzy_major_version], [0])
m4_define([pyzy_minor_version], [1])
m4_define([pyzy_micro_version], [0])
m4_define([pyzy_interface_age], [0])
m4_define([pyzy_binary_age],
          [m4_eval(100 * pyzy_minor_version + pyzy_micro_version)])
m4_define([pyzy_maybe_datestamp],
    m4_esyscmd([test x]pyzy_released[ != x1 && date +.%Y%m%d | tr -d '\n\r']))
m4_define([pyzy_version],
    pyzy_major_version.pyzy_minor_version.pyzy_micro_version[]pyzy_maybe_datestamp)
m4_define([pyzy_api_version], [1.0])

AC_PREFEQ([2.62])
AC_INIT([pyzy], [pyzy_version],
        [http://code.google.com/p/pyzy/issues/entry],
        [pyzy])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

m4_define([pyzy_binary_version], [1.0.0])

# Init automake.
AM_INIT_AUTOMAKE([1.10])
AM_MAINTAINER_MODE([enable])
AC_GNU_SOURCE

# Support silent build rules. Disable
# by either passing --disable-silent-rules to configure or passing V=1
# to make
AM_SILENT_RULES([yes])

# Define sustituted variables:
PYZY_MAJOR_VERSION=pyzy_major_version
PYZY_MINOR_VERSION=pyzy_minor_version
PYZY_MICRO_VERSION=pyzy_micro_version
PYZY_INTERFACE_AGE=pyzy_interface_age
PYZY_BINARY_AGE=pyzy_binary_age
PYZY_VERSION=pyzy_version
PYZY_API_VERSION=pyzy_api_version
PYZY_BINARY_VERSION=pyzy_binary_version
AC_SUBST(PYZY_MAJOR_VERSION)
AC_SUBST(PYZY_MINOR_VERSION)
AC_SUBST(PYZY_MICRO_VERSION)
AC_SUBST(PYZY_INTERFACE_AGE)
AC_SUBST(PYZY_BINARY_AGE)
AC_SUBST(PYZY_API_VERSION)
AC_SUBST(PYZY_VERSION)
AC_SUBST(PYZY_BINARY_VERSION)

# libtool versioning
m4_define([lt_current],
          [m4_eval(100 * pyzy_minor_version + pyzy_micro_version - pyzy_interface_age)])
m4_define([lt_revision], [pyzy_interface_age])
m4_define([lt_age], [m4_eval(pyzy_binary_age - pyzy_interface_age)])
LT_VERSION_INFO="lt_current:lt_revision:lt_age"
LT_CURRENT_MINUS_AGE=m4_eval(lt_current - lt_age)
AC_SUBST(LT_VERSION_INFO)
AC_SUBST(LT_CURRENT_MINUS_AGE)

# define PACKAGE_VERSION_* variables
AM_SANITY_CHECK
AM_DISABLE_STATIC
AC_PROG_CC
AC_PROG_CXX
AM_PROG_CC_C_O
AC_ISC_POSIX
AC_HEADER_STDC
AM_PROG_LIBTOOL

# check glib2
AM_PATH_GLIB_2_0
PKG_CHECK_MODULES(GLIB2, [
    glib-2.0 >= 2.24.0
])

# check sqlite
PKG_CHECK_MODULES(SQLITE, [
    sqlite3
])
AC_PATH_PROG(SQLITE3, sqlite3)

# check uuid
AC_CHECK_FUNCS([uuid_create], [], [
    PKG_CHECK_MODULES(LIBUUID, uuid, [
        # uuid.pc exist
        AC_DEFINE(HAVE_LIBUUID, 1, [Define if found libuuid])
        HAVE_LIBUUID=yes
    ], [
        # uuid.pc does not exist
        uuid_old_LIBS=$LIBS
        AC_CHECK_HEADERS(uuid/uuid.h, [
            AC_SEARCH_LIBS(uuid_generate, uuid, [
                test x"$ac_cv_search_uuid_generate" != x"none required" && LIBUUID_LIBS=$ac_cv_search_uuid_generate
                AC_DEFINE(HAVE_LIBUUID, 1, [Define if found libuuid])
                HAVE_LIBUUID=yes
            ], [
                AC_MSG_ERROR([Can not find uuid_create and uuid_generate in libc or libuuid])
            ])
        ], [
            AC_MSG_ERROR([Can not find uuid_create and uuid_generate in libc or libuuid])
        ])
        LIBS=$uuid_old_LIBS
    ])
])

AM_CONDITIONAL(HAVE_LIBUUID, test x"$HAVE_LIBUUID" = x"yes")

# check env
AC_PATH_PROG(ENV, env)
AC_SUBST(ENV)

# --enable-boost
AC_ARG_ENABLE(boost,
    AC_HELP_STRING([--enable-boost],
        [Do not use boost and use stdc++0x to replace boost.]),
        [enable_boost=$enableval],
        [enable_boost=no]
)
if test x"$enable_boost" = x"yes"; then
    # check boost
    BOOST_REQUIRE([1.39])
    BOOST_FIND_HEADER([boost/bind.hpp])
    BOOST_FIND_HEADER([boost/signals2.hpp])
fi
AM_CONDITIONAL(HAVE_BOOST, test x"$enable_boost" = x"yes")

# --enable-opencc
AC_ARG_ENABLE(opencc,
    AC_HELP_STRING([--enable-opencc],
        [Use opencc for simplified and traditional Chinese conversion]),
        [enable_opencc=$enableval],
        [enable_opencc=no]
)
if test x"$enable_opencc" = x"yes"; then
    # check opencc
    PKG_CHECK_MODULES(OPENCC, [opencc], [
        AC_DEFINE(HAVE_OPENCC, 1, [Define if found opencc])
    ])
fi

# --disable-db-android
AC_ARG_ENABLE(db-android,
    AS_HELP_STRING([--disable-db-android],
        [do not build database from pinyin in android]),
    [enable_db_android=$enableval],
    [enable_db_android=yes]
)
AM_CONDITIONAL(PYZY_BUILD_DB_ANDROID, [test x"$enable_db_android" = x"yes" ])

# --enable-db-open-phrase
AC_ARG_ENABLE(db-open-phrase,
    AS_HELP_STRING([--enable-db-open-phrase],
        [build open-phrase database]),
    [enable_db_open_phrase=$enableval],
    [enable_db_open_phrase=no]
)
AM_CONDITIONAL(PYZY_BUILD_DB_OPEN_PHRASE, [test x"$enable_db_open_phrase" = x"yes" ])

# --enable-tests option.
AC_ARG_ENABLE(tests,
    AS_HELP_STRING([--enable-tests],
                   [Run tests]),
    [enable_tests=$enableval],
    [enable_tests=no]
)
AM_CONDITIONAL([ENABLE_TESTS], [test x"$enable_tests" = x"yes"])

# doxygen
AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN"; then
    AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
fi
AM_CONDITIONAL([HAVE_DOXYGEN], [test -n "$DOXYGEN"])
AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([docs/Doxyfile])])   

# OUTPUT files
AC_CONFIG_FILES([
Makefile
data/Makefile
data/db/Makefile
data/db/android/Makefile
data/db/open-phrase/Makefile
docs/Makefile
m4/Makefile
pyzy-1.0.pc
pyzy.spec
src/Makefile
src/tests/Makefile
])

AC_OUTPUT

AC_MSG_RESULT([
Build options:
    Version                     $VERSION
    Install prefix              $prefix
    Use boost                   $enable_boost
    Use opencc                  $enable_opencc
    Build database android      $enable_db_android
    Build database open-phrase  $enable_db_open_phrase
    Run test cases              $enable_tests
])

