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

if PYZY_BUILD_DB_OPEN_PHRASE
db_file = open-phrase.db
main_db_DATA = \
	db/$(db_file) \
	$(NULL)
main_dbdir = $(pkgdatadir)/db

DBVER = 1.0.0
DBTAR = pyzy-database-$(DBVER).tar.bz2

$(DBTAR):
	$(AM_V_GEN) \
	wget http://pyzy.googlecode.com/files/$(DBTAR) || \
	( $(RM) $@; exit 1)

stamp-db: $(DBTAR)
	$(AM_V_GEN) \
	tar jxfm $(DBTAR); \
	mv db/main.db db/$(db_file); \
	touch $@

$(main_db_DATA): stamp-db
	$(AM_V_GEN)

CLEANFILES = \
	stamp-db \
	db/* \
	$(NULL)

DISTCLEANFILES = \
	$(DBTAR) \
	$(NULL)
endif

