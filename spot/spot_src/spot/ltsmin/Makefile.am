## -*- coding: utf-8 -*-
## Copyright (C) 2011, 2013, 2014, 2015, 2016, 2017, 2018 Laboratoire de
## Recherche et Developpement de l'Epita (LRDE).
##
## This file is part of Spot, a model checking library.
##
## Spot is free software; you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 3 of the License, or
## (at your option) any later version.
##
## Spot is distributed in the hope that it will be useful, but WITHOUT
## ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
## or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
## License for more details.
##
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.

AM_CPPFLAGS = -I$(top_builddir) -I$(top_srcdir) \
              $(BUDDY_CPPFLAGS) $(LTDLINCL) \
              -I$(top_builddir)/lib -I$(top_srcdir)/lib
AM_CXXFLAGS = $(WARNING_CXXFLAGS)

ltsmindir = $(pkgincludedir)/ltsmin

ltsmin_HEADERS = ltsmin.hh

lib_LTLIBRARIES = libspotltsmin.la
libspotltsmin_la_DEPENDENCIES = \
  $(top_builddir)/spot/libspot.la \
  $(top_builddir)/buddy/src/libbddx.la \
  $(top_builddir)/lib/libgnu.la \
  $(LTDLDEPS)
libspotltsmin_la_LIBADD = \
  $(top_builddir)/spot/libspot.la \
  $(top_builddir)/buddy/src/libbddx.la \
  $(top_builddir)/lib/libgnu.la \
  $(LIBLTDL) -lpthread
libspotltsmin_la_LDFLAGS = -no-undefined $(SYMBOLIC_LDFLAGS)
libspotltsmin_la_SOURCES = ltsmin.cc


EXTRA_DIST = $(srcdir)/libspotltsmin.pc.in
pkgconfigdir = $(libdir)/pkgconfig
nodist_pkgconfig_DATA = libspotltsmin.pc

libspotltsmin.pc: $(srcdir)/libspotltsmin.pc.in Makefile
	sed -e 's![@]prefix[@]!$(prefix)!g' \
	    -e 's![@]exec_prefix[@]!$(exec_prefix)!g' \
	    -e 's![@]includedir[@]!$(includedir)!g' \
	    -e 's![@]libdir[@]!$(libdir)!g' \
	    -e 's![@]PACKAGE_VERSION[@]!$(PACKAGE_VERSION)!g' \
	$(srcdir)/libspotltsmin.pc.in > $@.tmp && mv $@.tmp $@

CLEANFILES = libspotltsmin.pc
