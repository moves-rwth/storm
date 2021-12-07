# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2018 Laboratoire de Recherche et
# Développement de l'Epita
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


def incl(a, b):
    return not b.intersects(spot.dualize(spot.tgba_determinize(a)))


def equiv(a, b):
    return incl(a, b) and incl(b, a)


def do_split(f, in_list):
    aut = spot.translate(f)
    inputs = spot.buddy.bddtrue
    for a in in_list:
        inputs &= spot.buddy.bdd_ithvar(aut.get_dict().varnum(spot.formula(a)))
    s = spot.split_2step(aut, inputs)
    return aut, s


aut, s = do_split('(FG !a) <-> (GF b)', ['a'])
assert equiv(aut, spot.unsplit_2step(s))

aut, s = do_split('GFa && GFb', ['a'])
assert equiv(aut, spot.unsplit_2step(s))
assert s.to_str() == """HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
acc-name: generalized-Buchi 2
Acceptance: 2 Inf(0)&Inf(1)
properties: trans-labels explicit-labels trans-acc complete
properties: deterministic
--BODY--
State: 0
[0] 1
[!0] 2
State: 1
[!1] 0 {0}
[1] 0 {0 1}
State: 2
[!1] 0
[1] 0 {1}
--END--"""

aut, s = do_split('! ((G (req -> (F ack))) && (G (go -> (F grant))))', ['go',
                                                                        'req'])
assert equiv(aut, spot.unsplit_2step(s))
# FIXME s.to_str() is NOT the same on Debian stable and on Debian unstable
#       we should investigate this
# assert s.to_str() == """HOA: v1
# States: 9
# Start: 0
# AP: 4 "ack" "req" "go" "grant"
# acc-name: Buchi
# Acceptance: 1 Inf(0)
# properties: trans-labels explicit-labels state-acc
# --BODY--
# State: 0
# [1&!2] 3
# [!1&!2] 4
# [1&2] 5
# [!1&2] 6
# State: 1
# [t] 7
# State: 2
# [t] 8
# State: 3
# [t] 0
# [!0] 1
# State: 4
# [t] 0
# State: 5
# [t] 0
# [!0] 1
# [!3] 2
# State: 6
# [t] 0
# [!3] 2
# State: 7 {0}
# [!0] 1
# State: 8 {0}
# [!3] 2
# --END--"""

aut, s = do_split('((G (((! g_0) || (! g_1)) && ((r_0 && (X r_1)) -> (F (g_0 \
    && g_1))))) && (G (r_0 -> F g_0))) && (G (r_1 -> F g_1))', ['r_0', 'r_1'])
assert equiv(aut, spot.unsplit_2step(s))
