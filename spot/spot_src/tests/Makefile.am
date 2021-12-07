## -*- coding: utf-8 -*-
## Copyright (C) 2009-2020 Laboratoire de Recherche et Développement
## de l'Epita (LRDE).
## Copyright (C) 2003-2006 Laboratoire d'Informatique de Paris 6
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

AUTOMAKE_OPTIONS = subdir-objects
AM_CPPFLAGS = -I$(top_builddir) -I$(top_srcdir) $(BUDDY_CPPFLAGS) \
              -I$(top_builddir)/lib -I$(top_srcdir)/lib
AM_CXXFLAGS = $(WARNING_CXXFLAGS)
LDADD = $(top_builddir)/spot/libspot.la $(top_builddir)/buddy/src/libbddx.la


# Explicitely set it to avoid default value ".test"
TEST_EXTENSIONS =
LOG_COMPILER = ./run
# ensure run is rebuilt before the tests are run.
check_SCRIPTS = run

# We try to keep this somehow by strength.  Test basic things first,
# because such failures will be easier to diagnose and fix.  The only
# exception is that we keep TESTS_python at the beginning, because if
# these tests are run at the same time as some heavy tests in
# TESTS_twa they can easily fail due to some timeout in the Jupyter
# communications.
TESTS = $(TESTS_sanity) $(TESTS_python) $(TESTS_misc) $(TESTS_tl) \
  $(TESTS_graph) $(TESTS_kripke) $(TESTS_twa) $(TESTS_ltsmin)

distclean-local:
	find . -name '*.dir' -type d -print | xargs rm -rf
## This is crazy: Even if the python test suite passes without
## reporting an error, it can non-deterministically leave a coredump
## behind.  This happens with ipython 4.0.1 on ArchLinux; ipython
## somehow recovers from this so the only annoyance is that it can
## leave a coredump behind.
	rm -f python/core


############################## CORE ##############################

check_SCRIPTS += core/defs

core/defs: $(top_builddir)/config.status $(srcdir)/core/defs.in
	$(top_builddir)/config.status --file core/defs

# Keep this sorted alphabetically.
check_PROGRAMS = \
  core/acc \
  core/bdddict \
  core/bitvect \
  core/checkpsl \
  core/checkta \
  core/consterm \
  core/emptchk \
  core/equals \
  core/graph \
  core/kind \
  core/length \
  core/ikwiad \
  core/intvcomp \
  core/intvcmp2 \
  core/kripkecat \
  core/ltl2dot \
  core/ltl2text \
  core/ltlrel \
  core/lunabbrev \
  core/mempool \
  core/nequals \
  core/nenoform \
  core/ngraph \
  core/parity \
  core/randtgba \
  core/reduc \
  core/reduccmp \
  core/reduceu \
  core/reductaustr \
  core/safra \
  core/sccif \
  core/syntimpl \
  core/taatgba \
  core/trival \
  core/tgbagraph \
  core/tostring \
  core/tunabbrev \
  core/tunenoform

# Keep this sorted alphabetically.
core_acc_SOURCES = core/acc.cc
core_bdddict_SOURCES  = core/bdddict.cc
core_bitvect_SOURCES  = core/bitvect.cc
core_checkpsl_SOURCES = core/checkpsl.cc
core_checkta_SOURCES = core/checkta.cc
core_emptchk_SOURCES  = core/emptchk.cc
core_graph_SOURCES = core/graph.cc
core_ikwiad_SOURCES = core/ikwiad.cc
core_intvcomp_SOURCES = core/intvcomp.cc
core_intvcmp2_SOURCES = core/intvcmp2.cc
core_kripkecat_SOURCES = core/kripkecat.cc
core_mempool_SOURCES = core/mempool.cc
core_ngraph_SOURCES = core/ngraph.cc
core_randtgba_SOURCES = core/randtgba.cc
core_taatgba_SOURCES = core/taatgba.cc
core_tgbagraph_SOURCES = core/twagraph.cc
core_consterm_SOURCES = core/consterm.cc
core_equals_SOURCES = core/equalsf.cc
core_kind_SOURCES = core/kind.cc
core_length_SOURCES = core/length.cc
core_ltl2dot_SOURCES = core/readltl.cc
core_ltl2dot_CPPFLAGS = $(AM_CPPFLAGS) -DDOTTY
core_ltl2text_SOURCES = core/readltl.cc
core_ltlrel_SOURCES = core/ltlrel.cc
core_lunabbrev_SOURCES = core/equalsf.cc
core_lunabbrev_CPPFLAGS = $(AM_CPPFLAGS) -DUNABBREV='"^ie"'
core_nenoform_SOURCES = core/equalsf.cc
core_nenoform_CPPFLAGS = $(AM_CPPFLAGS) -DNENOFORM
core_nequals_SOURCES = core/equalsf.cc
core_nequals_CPPFLAGS = $(AM_CPPFLAGS) -DNEGATE
core_parity_SOURCES = core/parity.cc
core_reduc_SOURCES = core/reduc.cc
core_reduccmp_SOURCES = core/equalsf.cc
core_reduccmp_CPPFLAGS = $(AM_CPPFLAGS) -DREDUC
core_reduceu_SOURCES = core/equalsf.cc
core_reduceu_CPPFLAGS = $(AM_CPPFLAGS) -DREDUC -DEVENT_UNIV
core_reductaustr_SOURCES = core/equalsf.cc
core_reductaustr_CPPFLAGS = $(AM_CPPFLAGS) -DREDUC_TAUSTR
core_safra_SOURCES = core/safra.cc
core_sccif_SOURCES = core/sccif.cc
core_syntimpl_SOURCES = core/syntimpl.cc
core_tostring_SOURCES = core/tostring.cc
core_trival_SOURCES = core/trival.cc
core_tunabbrev_SOURCES = core/equalsf.cc
core_tunabbrev_CPPFLAGS = $(AM_CPPFLAGS) -DUNABBREV='"^ieFG"'
core_tunenoform_SOURCES = core/equalsf.cc
core_tunenoform_CPPFLAGS = $(AM_CPPFLAGS) -DNENOFORM -DUNABBREV='"^ieFG"'


TESTS_tl = \
  core/bare.test \
  core/parse.test \
  core/parseerr.test \
  core/utf8.test \
  core/length.test \
  core/equals.test \
  core/tostring.test \
  core/genaut.test \
  core/genltl.test \
  core/lunabbrev.test \
  core/tunabbrev.test \
  core/nenoform.test \
  core/tunenoform.test \
  core/unabbrevwm.test \
  core/consterm.test \
  core/kind.test \
  core/remove_x.test \
  core/ltlrel.test \
  core/ltlgrind.test \
  core/ltlcrossgrind.test \
  core/ltlfilt.test \
  core/exclusive-ltl.test \
  core/latex.test \
  core/lbt.test \
  core/lenient.test \
  core/rand.test \
  core/isop.test \
  core/syntimpl.test \
  core/reduc.test \
  core/reduc0.test \
  core/reducpsl.test \
  core/reduccmp.test \
  core/uwrm.test \
  core/eventuniv.test \
  core/stutter-ltl.test \
  core/hierarchy.test \
  core/mempool.test \
  core/format.test

TESTS_graph = \
  core/graph.test \
  core/ngraph.test \
  core/tgbagraph.test

TESTS_kripke = \
  core/kripke.test

TESTS_misc = \
  core/bdd.test \
  core/bitvect.test \
  core/intvcomp.test \
  core/minusx.test \
  core/full.test \
  core/trival.test

TESTS_twa = \
  core/385.test \
  core/acc.test \
  core/acc2.test \
  core/bdddict.test \
  core/alternating.test \
  core/ltlcross3.test \
  core/ltlcross5.test \
  core/taatgba.test \
  core/renault.test \
  core/nondet.test \
  core/det.test \
  core/semidet.test \
  core/neverclaimread.test \
  core/parseaut.test \
  core/optba.test \
  core/complete.test \
  core/complement.test \
  core/remfin.test \
  core/gragsa.test \
  core/dstar.test \
  core/readsave.test \
  core/dot2tex.test \
  core/ltldo.test \
  core/ltldo2.test \
  core/maskacc.test \
  core/maskkeep.test \
  core/prodchain.test \
  core/prodor.test \
  core/simdet.test \
  core/sim2.test \
  core/sim3.test \
  core/ltl2tgba.test \
  core/ltl2tgba2.test \
  core/ltl2neverclaim.test \
  core/ltl2neverclaim-lbtt.test \
  core/explprod.test \
  core/explpro2.test \
  core/explpro3.test \
  core/explpro4.test \
  core/explsum.test \
  core/dualize.test \
  core/accsimpl.test \
  core/tripprod.test \
  core/dupexp.test \
  core/exclusive-tgba.test \
  core/remprop.test \
  core/degendet.test \
  core/degenid.test \
  core/degenlskip.test \
  core/degenscc.test \
  core/randomize.test \
  core/lbttparse.test \
  core/scc.test \
  core/sccdot.test \
  core/sccif.test \
  core/sccsimpl.test \
  core/sepsets.test \
  core/split.test \
  core/sugar.test \
  core/dbacomp.test \
  core/obligation.test \
  core/wdba.test \
  core/wdba2.test \
  core/babiak.test \
  core/monitor.test \
  core/dra2dba.test \
  core/unambig.test \
  core/unambig2.test \
  core/ltlcross4.test \
  core/ltl3ba.test \
  core/streett.test \
  core/ltl3dra.test \
  core/ltl2dstar.test \
  core/ltl2dstar2.test \
  core/ltl2dstar3.test \
  core/ltl2dstar4.test \
  core/ltl2ta.test \
  core/ltl2ta2.test \
  core/pdegen.test \
  core/randaut.test \
  core/randtgba.test \
  core/isomorph.test \
  core/included.test \
  core/uniq.test \
  core/safra.test \
  core/sbacc.test \
  core/stutter-tgba.test \
  core/strength.test \
  core/emptchk.test \
  core/emptchke.test \
  core/dfs.test \
  core/ltlcrossce.test \
  core/ltlcrossce2.test \
  core/emptchkr.test \
  core/ltlcounter.test \
  core/basimul.test \
  core/satmin.test \
  core/satmin2.test \
  core/satmin3.test \
  core/spotlbtt.test \
  core/ltlcross.test \
  core/spotlbtt2.test \
  core/ltlcross2.test \
  core/ltlcross6.test \
  core/autcross.test \
  core/autcross2.test \
  core/autcross3.test \
  core/autcross4.test \
  core/autcross5.test \
  core/complementation.test \
  core/randpsl.test \
  core/cycles.test \
  core/acc_word.test \
  core/dca.test \
  core/dca2.test \
  core/dnfstreett.test \
  core/parity.test \
  core/parity2.test \
  core/ltlsynt.test \
  core/rabin2parity.test

############################## PYTHON ##############################

if USE_PYTHON

# TESTS_ipython contains notebooks that we also want to publish as
# part of the documentation.
TESTS_ipython = \
  python/acc_cond.ipynb \
  python/accparse.ipynb \
  python/alternation.ipynb \
  python/atva16-fig2a.ipynb \
  python/atva16-fig2b.ipynb \
  python/automata-io.ipynb \
  python/automata.ipynb \
  python/contains.ipynb \
  python/decompose.ipynb \
  python/formulas.ipynb \
  python/gen.ipynb \
  python/highlighting.ipynb \
  python/ltsmin-dve.ipynb \
  python/ltsmin-pml.ipynb \
  python/parity.ipynb \
  python/product.ipynb \
  python/randaut.ipynb \
  python/randltl.ipynb \
  python/satmin.ipynb \
  python/stutter-inv.ipynb \
  python/testingaut.ipynb \
  python/twagraph-internals.ipynb \
  python/word.ipynb

# TESTS_python contains all tests.  It may includes notebooks we
# do not consider part of the documentation: those have to start
# with a _.
TESTS_python = \
  python/341.py \
  python/_altscc.ipynb \
  python/_autparserr.ipynb \
  python/_aux.ipynb \
  python/accparse2.py \
  python/alarm.py \
  python/alternating.py \
  python/bdddict.py \
  python/bdditer.py \
  python/bddnqueen.py \
  python/bugdet.py \
  python/complement_semidet.py \
  python/declenv.py \
  python/decompose_scc.py \
  python/dualize.py \
  python/ecfalse.py \
  python/except.py \
  python/gen.py \
  python/genem.py \
  python/implies.py \
  python/interdep.py \
  python/intrun.py \
  python/kripke.py \
  python/ltl2tgba.test \
  python/ltlf.py \
  python/ltlparse.py \
  python/ltlsimple.py \
  python/merge.py \
  python/mergedge.py \
  python/minato.py \
  python/misc-ec.py \
  python/optionmap.py \
  python/origstate.py \
  python/otfcrash.py \
  python/parsetgba.py \
  python/parity.py \
  python/pdegen.py \
  python/prodexpt.py \
  python/_product_weak.ipynb \
  python/_product_susp.ipynb \
  python/randgen.py \
  python/relabel.py \
  python/remfin.py \
  python/removeap.py \
  python/satmin.py \
  python/sbacc.py \
  python/sccfilter.py \
  python/sccinfo.py \
  python/sccsplit.py \
  python/semidet.py \
  python/setacc.py \
  python/setxor.py \
  python/simplacc.py \
  python/simstate.py \
  python/split.py \
  python/streett_totgba.py \
  python/streett_totgba2.py \
  python/stutter.py \
  python/rs_like.py \
  python/sum.py \
  python/toparity.py \
  python/trival.py \
  python/tra2tba.py \
  python/twagraph.py \
  python/toweak.py \
  python/_word.ipynb \
  $(TESTS_ipython)
endif

CLEANFILES = python/test1.dve python/test1.dve2C python/test1.dve.cpp

SUFFIXES = .ipynb .html
.ipynb.html:
	$(IPYTHON) nbconvert $< --to html --stdout >$@

.PHONY: nb-html
nb-html: $(TESTS_ipython:.ipynb=.html)

EXTRA_DIST = \
  $(TESTS) \
  python/ltl2tgba.py \
  python/ipnbdoctest.py


############################## LTSMIN ##############################

if USE_LTSMIN
check_PROGRAMS += ltsmin/modelcheck

ltsmin_modelcheck_SOURCES = ltsmin/modelcheck.cc
ltsmin_modelcheck_LDADD = $(top_builddir)/spot/ltsmin/libspotltsmin.la \
	$(LDADD) -lpthread

check_SCRIPTS += ltsmin/defs

ltsmin/defs: $(top_builddir)/config.status $(srcdir)/core/defs.in
	$(top_builddir)/config.status --file ltsmin/defs:core/defs.in

TESTS_ltsmin = \
  ltsmin/check.test \
  ltsmin/check2.test \
  ltsmin/check3.test \
  ltsmin/finite.test \
  ltsmin/finite2.test \
  ltsmin/finite3.test \
  ltsmin/kripke.test

EXTRA_DIST += ltsmin/beem-peterson.4.dve ltsmin/beem-peterson.4.gal \
  ltsmin/elevator2.1.pm ltsmin/finite.dve ltsmin/finite.pm ltsmin/finite.gal

ltlsmin/kripke.log: core/kripkecat$(EXEEXT)
endif

############################## SANITY ##############################

TESTS_sanity = \
  sanity/80columns.test \
  sanity/bin.test \
  sanity/getenv.test \
  sanity/includes.test \
  sanity/ipynb.pl \
  sanity/namedprop.test \
  sanity/private.test \
  sanity/readme.pl \
  sanity/style.test
