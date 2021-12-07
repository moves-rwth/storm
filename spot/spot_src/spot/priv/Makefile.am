## -*- coding: utf-8 -*-
## Copyright (C) 2013-2019 Laboratoire de Recherche et Développement
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

noinst_LTLIBRARIES = libpriv.la
libpriv_la_SOURCES = \
  accmap.hh \
  allocator.hh \
  bddalloc.cc \
  bddalloc.hh \
  freelist.cc \
  freelist.hh \
  robin_hood.hh \
  satcommon.hh\
  satcommon.cc\
  trim.cc \
  trim.hh \
  weight.cc \
  weight.hh

GH = https://raw.githubusercontent.com/martinus
RH = $(GH)/robin-hood-hashing/master/src/include/robin_hood.h
.PHONY: update
update:
	wget $(RH) -O $(srcdir)/robin_hood.hh || \
	  curl $(RH) -o $(srcdir)/robin_hood.hh
