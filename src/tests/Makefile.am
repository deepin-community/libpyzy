# vim:set noet ts=4:
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


# @MAINTAINER_MODE_FALSE@skip_gentable=test -f $@ ||

# AM_CFLAGS = \
# 	@IBUS_CFLAGS@ \
# 	@SQLITE_CFLAGS@ \
# 	-DPKGDATADIR=\"$(pkgdatadir)\" \
# 	$(NULL)
# AM_CXXFLAGS = $(AM_CFLAGS)
# AM_LDADD = \
# 	@IBUS_LIBS@ \
# 	@SQLITE_LIBS@ \
# 	$(NULL)

DEPS = $(top_builddir)/src/libpyzy-@PYZY_API_VERSION@.la

INCLUDES =                  \
        -g                  \
        @GLIB2_CFLAGS@      \
        @SQLITE_CFLAGS@     \
        @OPENCC_CFLAGS@     \
        -I$(top_srcdir)/src \
        $(NULL)

prog_ldadd =                \
        @GLIB2_LIBS@        \
        @SQLITE_LIBS@       \
        @OPENCC_LIBS@       \
        $(top_builddir)/src/libpyzy-@PYZY_API_VERSION@.la       \
        $(NULL)

noinst_PROGRAMS = $(TESTS)
TESTS =                   \
        basic             \
        $(NULL)

basic_SOURCES = basic.cc
basic_LDADD = $(prog_ldadd)
