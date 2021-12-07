# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2019-2021 Laboratoire de Recherche et Développement de
# l'Epita (LRDE).
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

# Test that the spot.gen package works, in particular, we want
# to make sure that the objects created from spot.gen methods
# are usable with methods from the spot package.


import spot


def explain_stut(f):
    f = spot.formula(f)
    pos = spot.translate(f)
    neg = spot.translate(spot.formula.Not(f))
    word = spot.product(spot.closure(pos), spot.closure(neg)).accepting_word()
    if word is None:
        print(f, "is stutter invariant")
        return
    word.simplify()
    # This line used to be missing, causing issue #388.
    word.use_all_aps(pos.ap_vars())
    waut = word.as_automaton()
    aut = neg if waut.intersects(pos) else pos
    word2 = spot.sl2(waut).intersecting_word(aut)
    word2.simplify()
    return(word, word2)


# Test from issue #388
w1, w2 = explain_stut('{(a:b) | (a;b)}|->Gc')
assert str(w1) == 'a & !b & !c; cycle{!a & b & !c}'
assert str(w2) == 'a & !b & !c; a & !b & !c; cycle{!a & b & !c}'

# Test from issue #401
w1, w2 = explain_stut('G({x} |-> ({x[+]} <>-> ({Y1[+]} <>=> Y2)))')
assert str(w1) == 'cycle{!Y1 & !Y2 & x; Y1 & Y2 & x}'
assert str(w2) == 'cycle{!Y1 & !Y2 & x; Y1 & Y2 & x; Y1 & Y2 & x}'

# Related to issue #401 as well.  sl() and sl2() should upgrade
# the t acceptance condition into inf(0).
pos = spot.translate('Xa & XXb')
w = pos.accepting_word().as_automaton()
assert w.acc().is_t()
a = spot.sl2(w)
assert a.acc().is_buchi()
a = spot.sl(w)
assert a.acc().is_buchi()
