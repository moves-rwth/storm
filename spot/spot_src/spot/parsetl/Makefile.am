## -*- coding: utf-8 -*-
## Copyright (C) 2008-2015, 2018 Laboratoire de Recherche et
## Développement de l'Epita (LRDE).
## Copyright (C) 2003, 2004 Laboratoire d'Informatique de Paris
## 6 (LIP6), département Systèmes Répartis Coopératifs (SRC),
## Université Pierre et Marie Curie.
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

AM_CPPFLAGS = -I$(top_builddir) -I$(top_srcdir) -DYY_NO_INPUT \
              -I$(top_builddir)/lib -I$(top_srcdir)/lib
# Disable -Werror because too many versions of flex yield warnings.
AM_CXXFLAGS = $(WARNING_CXXFLAGS:-Werror=)

noinst_LTLIBRARIES = libparsetl.la

PARSETL_YY = parsetl.yy
FROM_PARSETL_YY_MAIN = parsetl.cc
FROM_PARSETL_YY_OTHERS = \
  stack.hh \
  parsetl.hh
FROM_PARSETL_YY = $(FROM_PARSETL_YY_MAIN) $(FROM_PARSETL_YY_OTHERS)

BUILT_SOURCES = $(FROM_PARSETL_YY)
MAINTAINERCLEANFILES = $(FROM_PARSETL_YY)

$(FROM_PARSETL_YY_MAIN): $(srcdir)/$(PARSETL_YY)
## We must cd into $(srcdir) first because if we tell bison to read
## $(srcdir)/$(PARSETL_YY), it will also use the value of $(srcdir)/
## in the generated include statements.
	cd $(srcdir) && \
	  $(BISON) -Wall -Werror --report=all $(BISON_EXTRA_FLAGS) \
	  $(PARSETL_YY) -o $(FROM_PARSETL_YY_MAIN)
$(FROM_PARSETL_YY_OTHERS): $(PARSETL_YY)
	@test -f $@ || $(MAKE) $(AM_MAKEFLAGS) $(FROM_PARSETL_YY_MAIN)

EXTRA_DIST = $(PARSETL_YY)

libparsetl_la_SOURCES = \
  fmterror.cc \
  $(FROM_PARSETL_YY) \
  scantl.ll \
  parsedecl.hh
