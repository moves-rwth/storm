## -*- coding: utf-8 -*-
## Copyright (C) 2017, 2018 Laboratoire de Recherche et Développement
## de l'Epita (LRDE).
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

AM_CPPFLAGS = -I$(top_builddir) -I$(top_srcdir) $(BUDDY_CPPFLAGS) \
              -I$(top_builddir)/lib -I$(top_srcdir)/lib
AM_CXXFLAGS = $(WARNING_CXXFLAGS)

spotgendir = $(pkgincludedir)/gen
spotgen_HEADERS = automata.hh formulas.hh

lib_LTLIBRARIES = libspotgen.la
libspotgen_la_SOURCES = automata.cc formulas.cc
libspotgen_la_LIBADD = \
  $(top_builddir)/spot/libspot.la \
  $(top_builddir)/buddy/src/libbddx.la
libspotgen_la_DEPENDENCIES = $(libspotgen_la_LIBADD)
libspotgen_la_LDFLAGS = -no-undefined $(SYMBOLIC_LDFLAGS)

EXTRA_DIST = $(srcdir)/libspotgen.pc.in
pkgconfigdir = $(libdir)/pkgconfig
nodist_pkgconfig_DATA = libspotgen.pc

libspotgen.pc: $(srcdir)/libspotgen.pc.in Makefile
	sed -e 's![@]prefix[@]!$(prefix)!g' \
	    -e 's![@]exec_prefix[@]!$(exec_prefix)!g' \
	    -e 's![@]includedir[@]!$(includedir)!g' \
	    -e 's![@]libdir[@]!$(libdir)!g' \
	    -e 's![@]PACKAGE_VERSION[@]!$(PACKAGE_VERSION)!g' \
	$(srcdir)/libspotgen.pc.in > $@.tmp && mv $@.tmp $@

CLEANFILES = libspotgen.pc
