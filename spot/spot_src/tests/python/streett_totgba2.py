#!/usr/bin/python3
# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2018 Laboratoire de Recherche et Développement de
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

# Issue 316
a = spot.automaton("""
HOA: v1
States: 8
Start: 0
AP: 3 "b" "a" "c"
Acceptance: 5 (Fin(0) | Inf(1)) & (Fin(2) | Inf(3)) & Inf(4)
properties: trans-labels explicit-labels state-acc
--BODY--
State: 0
[!0&!2] 1
[0&!2] 2
[2] 3
State: 1
[0&2] 4
[0&1&2] 5
State: 2
[!0&!2] 1
[!0&2] 3
State: 3
[!0&!2] 1
[!0&2] 3
[0&2] 4
[0&1&2] 5
State: 4
[2] 4
[0&1&2] 5
[!0&!2] 6
State: 5 {4}
[1&2] 5
[!0&1&!2] 7
State: 6
[0&2] 4
[0&1&2] 5
State: 7 {1 3 4}
[0&1&2] 5
--END--
""")

tgba = spot.streett_to_generalized_buchi(a)
assert tgba.acc().is_generalized_buchi()
ba = spot.simplify_acceptance(a)
assert ba.acc().is_buchi()

nba = spot.dualize(ba.postprocess('generic', 'deterministic'))
ntgba = spot.dualize(tgba.postprocess('generic', 'deterministic'))
assert not ba.intersects(ntgba)
assert not tgba.intersects(nba)
