## -*- coding: utf-8 -*-
## Copyright (C) 2013, 2014, 2018 Laboratoire de Recherche et
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
              -DYY_NO_INPUT -I$(top_builddir)/lib -I$(top_srcdir)/lib
# Disable -Werror because too many versions of flex yield warnings.
AM_CXXFLAGS = $(WARNING_CXXFLAGS:-Werror=)

parseautdir = $(pkgincludedir)/parseaut

parseaut_HEADERS = public.hh

noinst_LTLIBRARIES = libparseaut.la

HOAPARSE_YY = parseaut.yy
FROM_HOAPARSE_YY_MAIN = parseaut.cc
FROM_HOAPARSE_YY_OTHERS = \
  stack.hh \
  parseaut.hh

FROM_HOAPARSE_YY = $(FROM_HOAPARSE_YY_MAIN) $(FROM_HOAPARSE_YY_OTHERS)

BUILT_SOURCES = $(FROM_HOAPARSE_YY)
MAINTAINERCLEANFILES = $(FROM_HOAPARSE_YY)

$(FROM_HOAPARSE_YY_MAIN): $(srcdir)/$(HOAPARSE_YY)
## We must cd into $(srcdir) first because if we tell bison to read
## $(srcdir)/$(HOAPARSE_YY), it will also use the value of $(srcdir)/
## in the generated include statements.
	cd $(srcdir) && \
	  $(BISON) -Wall -Werror  -Wno-yacc --report=all $(BISON_EXTRA_FLAGS) \
	  $(HOAPARSE_YY) -o $(FROM_HOAPARSE_YY_MAIN)
$(FROM_HOAPARSE_YY_OTHERS): $(HOAPARSE_YY)
	@test -f $@ || $(MAKE) $(AM_MAKEFLAGS) $(FROM_HOAPARSE_YY_MAIN)

EXTRA_DIST = $(HOAPARSE_YY)

libparseaut_la_SOURCES = \
  fmterror.cc \
  $(FROM_HOAPARSE_YY) \
  scanaut.ll \
  parsedecl.hh
