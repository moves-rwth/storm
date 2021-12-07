# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2015, 2017  Laboratoire de Recherche et Développement
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
from sys import exit

aut = spot.automaton("""
HOA: v1
States: 2
Start: 0
AP: 2 "a" "b"
acc-name: Buchi
Acceptance: 1 Inf(0)
--BODY--
State: 0
[0] 0 {0}
[!0] 0
[1] 1
State: 1
[1] 1 {0}
--END--
""")

aut2 = spot.degeneralize(aut)
assert aut2.to_str() == """HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels state-acc
--BODY--
State: 0 {0}
[0] 0
[!0] 1
[1] 2
State: 1
[0] 0
[!0] 1
[1] 2
State: 2 {0}
[1] 2
--END--"""

aut2.copy_state_names_from(aut)
assert aut2.to_str() == """HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels state-acc
--BODY--
State: 0 "0#1" {0}
[0] 0
[!0] 1
[1] 2
State: 1 "0#0"
[0] 0
[!0] 1
[1] 2
State: 2 "1#1" {0}
[1] 2
--END--"""

aut2.set_init_state(2)
aut2.purge_unreachable_states()
ref = """HOA: v1
States: 1
Start: 0
AP: 2 "a" "b"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels state-acc colored
properties: deterministic
--BODY--
State: 0 "1#1" {0}
[1] 0
--END--"""
assert aut2.to_str() == ref
# This makes sure that the original-states vector has also been renamed.
aut2.copy_state_names_from(aut)
assert aut2.to_str() == ref

aut2 = spot.degeneralize(aut)
aut2.release_named_properties()
try:
    aut2.copy_state_names_from(aut)
except RuntimeError as e:
    assert "state does not exist in source automaton" in str(e)
else:
    exit(1)
