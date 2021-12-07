# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2017, 2018 Laboratoire de Recherche et Développement de l'Epita
# (LRDE).
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


# Tests different ways to explore a BDD label, as discussed in
# http://lists.lrde.epita.fr/pipermail/spot/2017q1/000117.html

import spot
import buddy
import sys

run = spot.translate('a & !b').accepting_run()
b = run.prefix[0].label
c = buddy.bdd_satone(b)
assert c != buddy.bddfalse
res = []
while c != buddy.bddtrue:
    var = buddy.bdd_var(c)
    h = buddy.bdd_high(c)
    if h == buddy.bddfalse:
        res.append(-var)
        c = buddy.bdd_low(c)
    else:
        res.append(var)
        c = h

assert res == [0, -1]

res2 = []
for i in run.aut.ap():
    res2.append((str(i), run.aut.register_ap(i)))
assert str(res2) == "[('a', 0), ('b', 1)]"


f = spot.bdd_to_formula(b)
assert f._is(spot.op_And)
assert f[0]._is(spot.op_ap)
assert f[1]._is(spot.op_Not)
assert f[1][0]._is(spot.op_ap)
assert str(f) == 'a & !b'

try:
    f = spot.bdd_to_formula(b, spot.make_bdd_dict())
    sys.exit(2)
except RuntimeError as e:
    assert "not in the dictionary" in str(e)
