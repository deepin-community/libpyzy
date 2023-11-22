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

main_db_DATA = \
	create_index.sql \
	$(NULL)
main_dbdir = $(pkgdatadir)/db

EXTRA_DIST = \
	create_index.sql \
	$(NULL)

SUBDIRS = \
	android \
	open-phrase \
	$(NULL)


install-data-hook:
	@( \
if test "$(NO_INDEX)" = ""; then \
  cd $(DESTDIR)$(main_dbdir); \
  if test -f android.db; then \
    echo "Creating index for android.db"; \
    sqlite3 android.db ".read create_index.sql"; \
  fi; \
  if test -f open-phrase.db; then \
    echo "Creating index for open-phrase.db"; \
    sqlite3 open-phrase.db ".read create_index.sql"; \
  fi; \
fi)


