## -*- coding: utf-8 -*-
## Copyright (C) 2012-2017 Laboratoire de Recherche et Développement
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

common_dep = $(top_srcdir)/configure.ac
x_to_1 = $(top_builddir)/tools/x-to-1
convman = ARGP_HELP_FMT=header-col=0 $(SHELL) "$(x_to_1)" \
          "$(PERL)" "$(top_srcdir)/tools/help2man -N"
convman7 = ARGP_HELP_FMT=header-col=0 $(SHELL) "$(x_to_1)" \
          "$(PERL)" "$(top_srcdir)/tools/help2man -s7 -N"

dist_man1_MANS = \
  autcross.1 \
  autfilt.1 \
  dstar2tgba.1 \
  genaut.1 \
  genltl.1 \
  ltl2tgba.1 \
  ltl2tgta.1 \
  ltlcross.1 \
  ltldo.1 \
  ltlfilt.1 \
  ltlgrind.1 \
  ltlsynt.1 \
  randaut.1 \
  randltl.1
dist_man7_MANS = \
  spot.7 \
  spot-x.7

MAINTAINERCLEANFILES = $(dist_man1_MANS) $(dist_man7_MANS)
EXTRA_DIST = $(dist_man1_MANS:.1=.x) $(dist_man7_MANS:.7=.x)

autcross.1: $(common_dep) $(srcdir)/autcross.x $(srcdir)/../autcross.cc
	$(convman) ../autcross$(EXEEXT) $(srcdir)/autcross.x $@

autfilt.1: $(common_dep) $(srcdir)/autfilt.x $(srcdir)/../autfilt.cc
	$(convman) ../autfilt$(EXEEXT) $(srcdir)/autfilt.x $@

dstar2tgba.1: $(common_dep) $(srcdir)/dstar2tgba.x $(srcdir)/../dstar2tgba.cc
	$(convman) ../dstar2tgba$(EXEEXT) $(srcdir)/dstar2tgba.x $@

ltl2tgba.1: $(common_dep) $(srcdir)/ltl2tgba.x $(srcdir)/../ltl2tgba.cc
	$(convman) ../ltl2tgba$(EXEEXT) $(srcdir)/ltl2tgba.x $@

ltl2tgta.1: $(common_dep) $(srcdir)/ltl2tgta.x $(srcdir)/../ltl2tgta.cc
	$(convman) ../ltl2tgta$(EXEEXT) $(srcdir)/ltl2tgta.x $@

ltlcross.1: $(common_dep) $(srcdir)/ltlcross.x $(srcdir)/../ltlcross.cc
	$(convman) ../ltlcross$(EXEEXT) $(srcdir)/ltlcross.x $@

ltldo.1: $(common_dep) $(srcdir)/ltlcross.x $(srcdir)/../ltldo.cc
	$(convman) ../ltldo$(EXEEXT) $(srcdir)/ltldo.x $@

ltlfilt.1: $(common_dep) $(srcdir)/ltlfilt.x $(srcdir)/../ltlfilt.cc
	$(convman) ../ltlfilt$(EXEEXT) $(srcdir)/ltlfilt.x $@

ltlgrind.1: $(common_dep) $(srcdir)/ltlgrind.x $(srcdir)/../ltlgrind.cc
	$(convman) ../ltlgrind$(EXEEXT) $(srcdir)/ltlgrind.x $@

ltlsynt.1: $(common_dep) $(srcdir)/ltlsynt.x $(srcdir)/../ltlsynt.cc
	$(convman) ../ltlsynt$(EXEEXT) $(srcdir)/ltlsynt.x $@

genaut.1: $(common_dep) $(srcdir)/genaut.x $(srcdir)/../genaut.cc
	$(convman) ../genaut$(EXEEXT) $(srcdir)/genaut.x $@

genltl.1: $(common_dep) $(srcdir)/genltl.x $(srcdir)/../genltl.cc
	$(convman) ../genltl$(EXEEXT) $(srcdir)/genltl.x $@

randltl.1: $(common_dep) $(srcdir)/randltl.x $(srcdir)/../randltl.cc
	$(convman) ../randltl$(EXEEXT) $(srcdir)/randltl.x $@

randaut.1: $(common_dep) $(srcdir)/randaut.x $(srcdir)/../randaut.cc
	$(convman) ../randaut$(EXEEXT) $(srcdir)/randaut.x $@

spot-x.7: $(common_dep) $(srcdir)/spot-x.x $(srcdir)/../spot-x.cc
	$(convman7) ../spot-x$(EXEEXT) $(srcdir)/spot-x.x $@

spot.7: $(common_dep) $(srcdir)/spot.x $(srcdir)/../spot.cc
	$(convman7) ../spot$(EXEEXT) $(srcdir)/spot.x $@
