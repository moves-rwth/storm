## -*- coding: utf-8 -*-
## Copyright (C) 2011, 2013, 2014, 2016, 2018 Laboratoire de Recherche
## et Developpement de l'Epita (LRDE).
## Copyright (C) 2004  Laboratoire d'Informatique de Paris 6 (LIP6),
## département Systèmes Répartis Coopératifs (SRC), Université Pierre
## et Marie Curie.
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

gtecdir = $(pkgincludedir)/twaalgos/gtec

gtec_HEADERS = \
  ce.hh \
  gtec.hh \
  sccstack.hh \
  status.hh

noinst_LTLIBRARIES = libgtec.la
libgtec_la_SOURCES = \
  ce.cc \
  gtec.cc \
  sccstack.cc \
  status.cc
