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

SUBDIRS = . man

AM_CPPFLAGS = -I$(top_builddir) -I$(top_srcdir) $(BUDDY_CPPFLAGS) \
              -I$(top_builddir)/lib -I$(top_srcdir)/lib
AM_CXXFLAGS = $(WARNING_CXXFLAGS)
LDADD =						\
  libcommon.a					\
  $(top_builddir)/lib/libgnu.la			\
  $(top_builddir)/spot/libspot.la		\
  $(top_builddir)/buddy/src/libbddx.la

noinst_LIBRARIES = libcommon.a
libcommon_a_SOURCES =				\
  common_aoutput.cc				\
  common_aoutput.hh				\
  common_color.cc				\
  common_color.hh				\
  common_conv.hh				\
  common_conv.cc				\
  common_cout.hh				\
  common_cout.cc				\
  common_file.cc				\
  common_file.hh				\
  common_finput.cc				\
  common_finput.hh				\
  common_hoaread.cc				\
  common_hoaread.hh				\
  common_output.cc				\
  common_output.hh				\
  common_post.cc				\
  common_post.hh				\
  common_range.cc				\
  common_range.hh				\
  common_r.cc					\
  common_r.hh					\
  common_setup.cc				\
  common_setup.hh				\
  common_sys.hh                                 \
  common_trans.cc				\
  common_trans.hh

bin_PROGRAMS = \
  autcross \
  autfilt \
  dstar2tgba \
  genaut \
  genltl \
  ltl2tgba \
  ltl2tgta \
  ltlcross \
  ltldo \
  ltlfilt \
  ltlgrind \
  ltlsynt \
  randaut \
  randltl

# Dummy programs used just to generate man/spot-x.7 and man/spot.7 in
# a way that is consistent with the other man pages (e.g., with a
# version number that is automatically updated).
noinst_PROGRAMS = spot-x spot

autcross_SOURCES = autcross.cc
autfilt_SOURCES = autfilt.cc
ltlfilt_SOURCES = ltlfilt.cc
genaut_SOURCES = genaut.cc
genaut_LDADD = $(top_builddir)/spot/gen/libspotgen.la $(LDADD)
genltl_SOURCES = genltl.cc
genltl_LDADD = $(top_builddir)/spot/gen/libspotgen.la $(LDADD)
randaut_SOURCES = randaut.cc
randltl_SOURCES = randltl.cc
ltl2tgba_SOURCES = ltl2tgba.cc
ltl2tgta_SOURCES = ltl2tgta.cc
ltlcross_SOURCES = ltlcross.cc
ltlgrind_SOURCES = ltlgrind.cc
ltldo_SOURCES = ltldo.cc
ltlsynt_SOURCES = ltlsynt.cc
dstar2tgba_SOURCES = dstar2tgba.cc
spot_x_SOURCES = spot-x.cc
spot_SOURCES = spot.cc
ltlcross_LDADD = $(LDADD) $(LIB_GETHRXTIME)

EXTRA_DIST = options.py
