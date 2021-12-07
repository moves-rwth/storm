# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2020 Laboratoire de Recherche et Développement de l'Epita
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
from buddy import bddfalse, bddtrue

a = spot.automaton("""
HOA: v1
States: 2
Start: 1
AP: 2 "p0" "p1"
acc-name: Buchi
Acceptance: 1 Inf(0)
--BODY--
State: 0
[!0 | 1] 0 {0}
[0&!1] 1
State: 1
/* we want this edge to be false, but the parser
   would ignore it if we wrote it here */
[0] 0 {0}
[!1] 1
--END--
""")
# Make the false edge.
for e in a.out(1):
    if e.dst == 0:
        e.cond = bddfalse

assert a.accepting_run() is None
assert a.is_empty()

for name in ['SE05', 'CVWY90', 'GV04', 'Cou99(shy)', 'Cou99', 'Tau03']:
    print(name)
    ec = spot.make_emptiness_check_instantiator(name)[0].instantiate(a)
    res = ec.check()
    if res is not None:
        print(res.accepting_run())
    assert res is None

si = spot.scc_info(a)
assert si.scc_count() == 1 # only one accessible SCC
a.set_init_state(0)
si = spot.scc_info(a)
assert si.scc_count() == 2

a = spot.automaton("""HOA: v1 States: 11 Start: 0 AP: 2 "a" "b" Acceptance: 8
(Fin(0) | Inf(1)) & (Fin(2) | Inf(3)) & ((Fin(4) & Inf(5)) | (Fin(6) & Inf(7)))
properties: trans-labels explicit-labels trans-acc --BODY-- State: 0 [!0&!1] 1
{0 4 6 7} [!0&!1] 2 {0 5 6} [!0&!1] 3 {3 4 6 7} [!0&!1] 4 {3 5 6} State: 1
[0&1] 5 {2 5 7} [0&1] 6 {2 7} [0&1] 7 {2 3 5 7} [0&1] 8 {2 3 7} [0&1] 0 {2 5 7}
[0&1] 9 {2 7} State: 2 [0&1] 1 {2} [0&1] 3 {2 3} [0&1] 10 {2} State: 3 [0&1] 0
{3 5 7} [0&1] 9 {3 7} State: 4 [!0&!1] 5 {4 6} [!0&!1] 6 {7} [0&1] 10 {3}
State: 5 State: 6 State: 7 [!0&!1] 1 {4 6 7} [!0&!1] 2 {5 6} State: 8 [!0&!1] 2
{4} State: 9 [!0&!1] 2 {0 4} [!0&!1] 4 {3 4} State: 10 --END-- """)

r = a.accepting_run()
assert r is not None
assert r.replay(spot.get_cout())
for e in a.out(7):
    if e.dst == 2:
        e.cond = bddfalse
s = a.accepting_run()
assert s is not None
assert s.replay(spot.get_cout())
for e in a.out(2):
    if e.dst == 1:
        e.cond = bddfalse
s = a.accepting_run()
assert s is None
