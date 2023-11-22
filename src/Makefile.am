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
# 

if ENABLE_TESTS
SUBDIRS = \
	. \
	tests \
	$(NULL)
endif

libpyzy=libpyzy-1.0.la

libpyzyincludedir = $(includedir)/pyzy-@PYZY_API_VERSION@/PyZy
libpyzyinclude_HEADERS = \
	Const.h \
	InputContext.h \
	Variant.h \
	$(NULL)

lib_LTLIBRARIES = $(libpyzy)

libpyzy_built_c_sources = \
	$(NULL)
libpyzy_built_h_sources = \
	BopomofoKeyboard.h \
	PinyinParserTable.h \
	SimpTradConverterTable.h \
	$(NULL)
libpyzy_c_sources = \
	BopomofoContext.cc \
	Database.cc \
	DoublePinyinContext.cc \
	DynamicSpecialPhrase.cc \
	FullPinyinContext.cc \
	InputContext.cc \
	PhoneticContext.cc \
	PhraseEditor.cc \
	PinyinContext.cc \
	PinyinParser.cc \
	SimpTradConverter.cc \
	SpecialPhraseTable.cc \
	Variant.cc \
	$(NULL)
libpyzy_h_sources = \
	Bopomofo.h \
	BopomofoContext.h \
	Config.h \
	Const.h \
	Database.h \
	DoublePinyinContext.h \
	DoublePinyinTable.h \
	DynamicSpecialPhrase.h \
	FullPinyinContext.h \
	InputContext.h \
	PhoneticContext.h \
	Phrase.h \
	PhraseArray.h \
	PhraseEditor.h \
	PinyinArray.h \
	PinyinContext.h \
	PinyinParser.h \
	SimpTradConverter.h \
	SpecialPhrase.h \
	SpecialPhraseTable.h \
	String.h \
	Types.h \
	Util.h \
	Variant.h \
	$(NULL)

libpyzy_1_0_la_SOURCES = \
	$(libpyzy_c_sources) \
	$(libpyzy_h_sources) \
	$(libpyzy_built_c_sources) \
	$(libpyzy_built_h_sources) \
	$(NULL)

libpyzy_1_0_la_LDFLAGS = \
	-no-undefined \
	-version-info @LT_VERSION_INFO@ \
	$(NULL)

libpyzy_1_0_la_CXXFLAGS = \
	@GLIB2_CFLAGS@ \
	@SQLITE_CFLAGS@ \
	@OPENCC_CFLAGS@ \
	-DPKGDATADIR=\"$(pkgdatadir)\" \
	$(NULL)

if HAVE_BOOST
libpyzy_1_0_la_CXXFLAGS += \
	@BOOST_CPPFLAGS@ \
	$(NULL)
else
libpyzy_1_0_la_CXXFLAGS += \
	-std=c++0x \
	$(NULL)
endif

libpyzy_1_0_la_LIBADD = \
	@GLIB2_LIBS@ \
	@SQLITE_LIBS@ \
	@OPENCC_LIBS@ \
	$(NULL)
#	-lgtest

if HAVE_LIBUUID
libpyzy_1_0_la_CXXFLAGS += $(LIBUUID_CFLAGS)
libpyzy_1_0_la_LIBADD += $(LIBUUID_LIBS)
endif

BUILT_SOURCES = \
	$(libpyzy_built_c_sources) \
	$(libpyzy_built_h_sources) \
	$(NULL)

pkgdata_DATA = \
	phrases.txt \
	$(NULL)

EXTRA_DIST = \
	phrases.txt \
	$(NULL)

CLEANFILES = \
	ZhConversion.* \
	$(NULL)

BopomofoKeyboard.h:
	$(AM_V_GEN) \
	$(PYTHON) $(top_srcdir)/scripts/genbopomofokeyboard.py > $@ || \
		( $(RM) $@; exit 1 )

PinyinParserTable.h:
	$(AM_V_GEN) \
	$(PYTHON) $(top_srcdir)/scripts/genpytable.py > $@ || \
		( $(RM) $@; exit 1 )

SimpTradConverterTable.h:
	$(AM_V_GEN) \
	$(PYTHON) $(top_srcdir)/scripts/update-simptrad-table.py > $@ || \
		( $(RM) $@; exit 1 )

update-simptrad-table:
	$(RM) ZhConversion.php ZhConversion.py SimpTradConverterTable.h
	$(MAKE) ZhConversion.php
	$(MAKE) ZhConversion.py
	$(MAKE) SimpTradConverterTable.h

ZhConversion.php:
	$(AM_V_GEN) \
	svn cat http://svn.wikimedia.org/svnroot/mediawiki/trunk/phase3/includes/ZhConversion.php > $@ || \
		( $(RM) $@; exit 1 )

ZhConversion.py: ZhConversion.php
	$(AM_V_GEN) \
	php $(top_srcdir)/scripts/php_2_py.php > $@ || \
		( $(RM) $@; exit 1 )

