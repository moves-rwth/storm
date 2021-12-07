# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2010, 2012, 2018 Laboratoire de Recherche et Développement
# de l'EPITA.
# Copyright (C) 2005  Laboratoire d'Informatique de Paris 6 (LIP6),
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

import spot

o = spot.option_map()
res = o.parse_options("optA, opta=2M, optb =4 ; optB =       7\
                       ,   optC=  10")
assert not res

assert o.get('optA') == 1
assert o.get('opta') == 2*1024*1024
assert o.get('optb') == 4
assert o.get('optB') == 7
assert o.get('optC') == 10
assert o.get('none') == 0
assert o.get('none', 16) == 16

o.set('optb', 40)
assert o.get('optb') == 40

res = o.parse_options("!optA !optb optC, !optB")
assert not res
assert o.get('optA') == 0
assert o.get('opta') == 2*1024*1024
assert o.get('optb') == 0
assert o.get('optB') == 0
assert o.get('optC') == 1

res = o.parse_options("!")
assert res == "!"

res = o.parse_options("foo, !opt = 1")
assert res == "!opt = 1"

res = o.parse_options("foo=3, opt == 1")
assert res == "opt == 1"

res = o.parse_options("foo=3opt == 1")
assert res == "3opt == 1"

aut1 = spot.translate('GF(a <-> XXa)', 'det')
assert aut1.num_states() == 4
aut2 = spot.translate('GF(a <-> XXa)', 'det', xargs='gf-guarantee=0')
assert aut2.num_states() == 9

try:
    spot.translate('GF(a <-> XXa)', 'det', xargs='foobar=1')
except RuntimeError as e:
    assert "option 'foobar' was not used" in str(e)
else:
    raise RuntimeError("missing exception")

try:
    spot.translate('GF(a <-> XXa)').postprocess('det', xargs='gf-guarantee=0')
except RuntimeError as e:
    assert "option 'gf-guarantee' was not used" in str(e)
else:
    raise RuntimeError("missing exception")

try:
    spot.translate('GF(a <-> XXa)').postprocess('det', xargs='gf-guarantee=x')
except RuntimeError as e:
    assert "failed to parse option at: 'gf-guarantee=x'" in str(e)
else:
    raise RuntimeError("missing exception")
