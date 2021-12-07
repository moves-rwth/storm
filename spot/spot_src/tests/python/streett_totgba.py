#!/usr/bin/python3
# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2017, 2018 Laboratoire de Recherche et Développement de
# l'EPITA.
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

import spot
import os
import shutil
import sys


def tgba(a):
    if not a.is_existential():
        a = spot.remove_alternation(a)
    a = spot.to_generalized_buchi(a)
    return a


def test_aut(aut):
    stgba = tgba(aut)
    assert stgba.equivalent_to(aut)
    os.environ["SPOT_STREETT_CONV_MIN"] = '1'
    sftgba = tgba(aut)
    del os.environ["SPOT_STREETT_CONV_MIN"]
    assert stgba.equivalent_to(sftgba)

    slike = spot.simplify_acceptance(aut)

    sltgba = tgba(slike)
    os.environ["SPOT_STREETT_CONV_MIN"] = "1"
    slftgba = tgba(slike)
    del os.environ["SPOT_STREETT_CONV_MIN"]
    assert sltgba.equivalent_to(slftgba)

# Those automata are generated with ltl2dstar, which is NOT part of spot,
# using the following command:
# genltl --eh-patterns --dac-patterns --hkrss-patterns --sb-patterns |\
# ltldo "ltl2dstar --automata=streett --output-format=hoa\
#       --ltl2nba=spin:ltl2tgba@-s %L ->%O" -F- --name=%f -H"


if shutil.which('ltl2dstar') is None:
    sys.exit(77)
for a in spot.automata('genltl --eh-patterns --dac-patterns --hkrss-patterns\
                        --sb-patterns |\
                        ltldo "ltl2dstar --automata=streett --output-format=hoa\
                        --ltl2nba=spin:ltl2tgba@-s %L ->%O"\
                        -T5 -F- --name=%f -H|'):
    test_aut(a)
