## -*- coding: utf-8 -*-
## Copyright (C) 2015-2018 Laboratoire de Recherche et
## Développement de l'Epita (LRDE).
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

tldir = $(pkgincludedir)/tl

tl_HEADERS =					\
  apcollect.hh					\
  contain.hh					\
  declenv.hh					\
  defaultenv.hh					\
  dot.hh					\
  environment.hh				\
  exclusive.hh					\
  formula.hh					\
  hierarchy.hh					\
  length.hh					\
  ltlf.hh					\
  mutation.hh					\
  nenoform.hh					\
  parse.hh					\
  print.hh					\
  randomltl.hh					\
  relabel.hh					\
  remove_x.hh					\
  simplify.hh					\
  snf.hh					\
  unabbrev.hh

noinst_LTLIBRARIES = libtl.la
libtl_la_SOURCES =				\
  apcollect.cc					\
  contain.cc					\
  declenv.cc					\
  defaultenv.cc					\
  dot.cc					\
  exclusive.cc					\
  formula.cc					\
  hierarchy.cc					\
  length.cc					\
  ltlf.cc					\
  mark.cc					\
  mark.hh					\
  mutation.cc					\
  nenoform.cc					\
  print.cc					\
  randomltl.cc					\
  relabel.cc					\
  remove_x.cc					\
  simplify.cc					\
  snf.cc					\
  unabbrev.cc
