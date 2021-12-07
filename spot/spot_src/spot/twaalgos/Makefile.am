## -*- coding: utf-8 -*-
## Copyright (C) 2008-2018 Laboratoire de Recherche et Développement
## de l'Epita (LRDE).
## Copyright (C) 2003-2005 Laboratoire d'Informatique de Paris 6
## (LIP6), département Systèmes Répartis Coopératifs (SRC), Université
## Pierre et Marie Curie.
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

SUBDIRS = gtec

AM_CPPFLAGS = -I$(top_builddir) -I$(top_srcdir) $(BUDDY_CPPFLAGS) \
              -I$(top_builddir)/lib -I$(top_srcdir)/lib
AM_CXXFLAGS = $(WARNING_CXXFLAGS)

twaalgosdir = $(pkgincludedir)/twaalgos

twaalgos_HEADERS = \
  aiger.hh \
  alternation.hh \
  are_isomorphic.hh \
  bfssteps.hh \
  canonicalize.hh \
  cleanacc.hh \
  cobuchi.hh \
  complete.hh \
  complement.hh \
  compsusp.hh \
  contains.hh \
  copy.hh \
  cycles.hh \
  degen.hh \
  determinize.hh \
  dot.hh \
  dtbasat.hh \
  dtwasat.hh \
  dualize.hh \
  emptiness.hh \
  emptiness_stats.hh \
  genem.hh \
  gfguarantee.hh \
  gv04.hh \
  hoa.hh \
  iscolored.hh \
  isdet.hh \
  isunamb.hh \
  isweakscc.hh \
  langmap.hh \
  lbtt.hh \
  ltl2taa.hh \
  ltl2tgba_fm.hh \
  magic.hh \
  mask.hh \
  minimize.hh \
  couvreurnew.hh \
  neverclaim.hh \
  parity.hh \
  postproc.hh \
  powerset.hh \
  product.hh \
  randomgraph.hh \
  randomize.hh \
  reachiter.hh \
  relabel.hh \
  remfin.hh \
  remprop.hh \
  split.hh \
  strength.hh \
  sbacc.hh \
  sccfilter.hh \
  sccinfo.hh \
  se05.hh \
  sepsets.hh \
  simulation.hh \
  stats.hh \
  stripacc.hh \
  stutter.hh \
  sum.hh \
  tau03.hh \
  tau03opt.hh \
  toparity.hh \
  totgba.hh \
  toweak.hh \
  translate.hh \
  word.hh

noinst_LTLIBRARIES = libtwaalgos.la
libtwaalgos_la_SOURCES = \
  aiger.cc \
  alternation.cc \
  are_isomorphic.cc \
  bfssteps.cc \
  canonicalize.cc \
  cleanacc.cc \
  cobuchi.cc \
  complete.cc \
  complement.cc \
  compsusp.cc \
  contains.cc \
  cycles.cc \
  degen.cc \
  determinize.cc \
  dot.cc \
  dtbasat.cc \
  dtwasat.cc \
  dualize.cc \
  emptiness.cc \
  genem.cc \
  gfguarantee.cc \
  gv04.cc \
  hoa.cc \
  iscolored.cc \
  isdet.cc \
  isunamb.cc \
  isweakscc.cc \
  langmap.cc \
  lbtt.cc \
  ltl2taa.cc \
  ltl2tgba_fm.cc \
  magic.cc \
  mask.cc \
  minimize.cc \
  couvreurnew.cc \
  ndfs_result.hxx \
  neverclaim.cc \
  parity.cc \
  postproc.cc \
  powerset.cc \
  product.cc \
  randomgraph.cc \
  randomize.cc \
  reachiter.cc \
  remfin.cc \
  remprop.cc \
  relabel.cc \
  split.cc \
  strength.cc \
  sbacc.cc \
  sccinfo.cc \
  sccfilter.cc \
  se05.cc \
  sepsets.cc \
  simulation.cc \
  stats.cc \
  stripacc.cc \
  stutter.cc \
  sum.cc \
  tau03.cc \
  tau03opt.cc \
  toparity.cc \
  totgba.cc \
  toweak.cc \
  translate.cc \
  word.cc

libtwaalgos_la_LIBADD = gtec/libgtec.la
