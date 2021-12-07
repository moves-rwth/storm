# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2009, 2010, 2012, 2014, 2015, 2016 Laboratoire de Recherche et
# Développement de l'Epita (LRDE).
# Copyright (C) 2003, 2004 Laboratoire d'Informatique de Paris 6 (LIP6),
# département Systèmes Répartis Coopératifs (SRC), Université Pierre
# et Marie Curie.
#
# This file is part of Spot, a model checking library.
#
# Spot is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.
#
# Spot is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
# or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
# License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# This is a python implementation of a small ltl2tgba translator that calls the
# translation and simplifications functions itself.  It's better to use the
# translate() function today.

import sys
import getopt
import spot


def usage(prog):
    sys.stderr.write("""Usage: %s [OPTIONS...] formula

Options:
  -d   turn on traces during parsing
  -D   degeneralize the automaton
  -f   use Couvreur's FM algorithm for translation
  -t   display reachable states in LBTT's format
  -T   use ltl2taa for translation
  -v   display the BDD variables used by the automaton
  -W   minimize obligation properties
""" % prog)
    sys.exit(2)


prog = sys.argv[0]
try:
    opts, args = getopt.getopt(sys.argv[1:], 'dDftTvW')
except getopt.GetoptError:
    usage(prog)

exit_code = 0
debug_opt = False
degeneralize_opt = None
output = 0
fm_opt = 0
taa_opt = 0
wdba = 0

for o, a in opts:
    if o == '-d':
        debug_opt = True
    elif o == '-D':
        degeneralize_opt = 1
    elif o == '-f':
        fm_opt = 1
    elif o == '-t':
        output = 6
    elif o == '-T':
        taa_opt = 1
    elif o == '-v':
        output = 5
    elif o == '-W':
        wdba = 1
    else:
        usage(prog)

if len(args) != 1:
    usage(prog)


cout = spot.get_cout()
cerr = spot.get_cerr()

e = spot.default_environment.instance()

pf = spot.parse_infix_psl(args[0], e, debug_opt)
if pf.format_errors(cerr):
    exit_code = 1
f = pf.f

dict = spot.make_bdd_dict()

if f:
    if fm_opt:
        a = spot.ltl_to_tgba_fm(f, dict)
        concrete = 0
    elif taa_opt:
        a = concrete = spot.ltl_to_taa(f, dict)
    else:
        assert "unspecified translator"

    if wdba:
        a = spot.ensure_digraph(a)
        a = spot.minimize_obligation(a, f)

    del f

    degeneralized = None
    if degeneralize_opt:
        a = degeneralized = spot.degeneralize(a)

    if output == 0:
        spot.print_dot(cout, a)
    elif output == 5:
        a.get_dict().dump(cout)
    elif output == 6:
        spot.print_lbtt(cout, a)
    else:
        assert "unknown output option"

    if degeneralize_opt:
        del degeneralized
    # Must delete absolutely all references to an automaton
    # so that the C++ destructor gets called.
    del a, concrete

else:
    exit_code = 1

del pf
del dict
assert spot.fnode_instances_check()
