# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2017-2019 Laboratoire de Recherche et Développement
# de l'Epita
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
import sys
import itertools

# make sure that we are not allowed to build the sum of two automata with
# different dictionaries.

aut1 = spot.translate('Xa')
aut2 = spot.translate('Xb', dict=spot.make_bdd_dict())

try:
    spot.sum(aut1, aut2)
    exit(2)
except RuntimeError:
    pass


opts = spot.option_map()
opts.set('output', spot.randltlgenerator.LTL)
opts.set('tree_size_min', 15)
opts.set('tree_size_max', 15)
opts.set('wf', False)
opts.set('seed', 0)
opts.set('simplification_level', 0)
spot.srand(0)
rg = spot.randltlgenerator(2, opts)


dict = spot.make_bdd_dict()


def produce_phi(rg, n):
    phi = []
    while len(phi) < n:
        f = rg.next()
        if f.is_syntactic_persistence():
            phi.append(f)
    return phi


phi1 = produce_phi(rg, 1000)
phi2 = produce_phi(rg, 1000)
for p in zip(phi1, phi2):
    a1 = spot.ltl_to_tgba_fm(p[0], dict)
    a2 = spot.ltl_to_tgba_fm(p[1], dict)

    p0orp1 = spot.formula.Or(p)
    a1ora2 = spot.remove_alternation(spot.sum(a1, a2), True)
    assert p0orp1.equivalent_to(a1ora2)

    p0andp1 = spot.formula.And(p)
    a1anda2 = spot.remove_alternation(spot.sum_and(a1, a2), True)
    assert p0andp1.equivalent_to(a1anda2)
