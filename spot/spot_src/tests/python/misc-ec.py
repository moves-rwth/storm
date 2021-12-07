# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2017 Laboratoire de Recherche et Développement de l'Epita
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

import spot
aut = spot.translate("G(p0 | (p0 R Xp0) | XF(!p0 & p1))", 'BA')
ec = spot.make_emptiness_check_instantiator('SE05')[0].instantiate(aut)
n = 0
while True:
    res = ec.check()
    if not res:
        break
    print(res.accepting_run())
    n += 1
assert n == 2

for name in ['SE05', 'CVWY90', 'GV04']:
    aut = spot.translate("GFa && GFb")
    try:
        ec = spot.make_emptiness_check_instantiator(name)[0].instantiate(aut)
        print(ec.check().accepting_run())
    except RuntimeError as e:
        assert "Büchi or weak" in str(e)

aut = spot.translate("a", 'monitor')
try:
    ec = spot.make_emptiness_check_instantiator('Tau03')[0].instantiate(aut)
except RuntimeError as e:
    assert "at least one" in str(e)

aut = spot.translate("a", 'ba')
ec = spot.make_emptiness_check_instantiator('Tau03')[0].instantiate(aut)
