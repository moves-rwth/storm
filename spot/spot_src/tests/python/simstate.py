# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2015, 2017-2018, 2020  Laboratoire de Recherche et Développement
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
[t] 1 {0}
State: 1
[t] 1 {0}
--END--
""")

aut2 = spot.simulation(aut)
assert aut2.to_str() == """HOA: v1
States: 1
Start: 0
AP: 2 "a" "b"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels state-acc colored complete
properties: deterministic
--BODY--
State: 0 {0}
[t] 0
--END--"""

aut2.copy_state_names_from(aut)
assert aut2.to_str() == """HOA: v1
States: 1
Start: 0
AP: 2 "a" "b"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels state-acc colored complete
properties: deterministic
--BODY--
State: 0 "[0,1]" {0}
[t] 0
--END--"""

aut = spot.translate('GF((p0 -> Gp0) R p1)')

daut = spot.tgba_determinize(aut, True)
assert daut.to_str() == """HOA: v1
States: 3
Start: 0
AP: 2 "p1" "p0"
acc-name: parity min even 3
Acceptance: 3 Inf(0) | (Fin(1) & Inf(2))
properties: trans-labels explicit-labels trans-acc complete
properties: deterministic stutter-invariant
--BODY--
State: 0 "{₀[0]₀}"
[!0&!1] 0
[!0&1] 0
[0&!1] 0 {0}
[0&1] 1 {2}
State: 1 "{₀[0]{₂[2]₂}₀}{₁[1]₁}"
[!0&!1] 0 {1}
[!0&1] 2
[0&!1] 0 {0}
[0&1] 1 {2}
State: 2 "{₀[0]₀}{₁[1]₁}"
[!0&!1] 0 {1}
[!0&1] 2
[0&!1] 0 {0}
[0&1] 1 {2}
--END--"""

aut = spot.automaton("""
HOA: v1
States: 2
Start: 0
AP: 2 "a" "b"
Acceptance: 2 Inf(0)&Inf(1)
--BODY--
State: 0
[0] 0
[1] 1 {1}
State: 1
[0] 1 {0}
[1] 0
--END--
""")

daut = spot.tgba_determinize(aut, True)
assert daut.to_str() == """HOA: v1
States: 12
Start: 0
AP: 2 "a" "b"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels trans-acc deterministic
--BODY--
State: 0 "{₀[0#0,0#1]₀}"
[!0&1] 1
[0&!1] 0
[0&1] 2
State: 1 "{₀[1#1]₀}"
[!0&1] 0
[0&!1] 3 {0}
[0&1] 4
State: 2 "{₀[0#0,0#1] [1#1]₀}"
[!0&1] 2
[0&!1] 4
[0&1] 5
State: 3 "{₀[1#0]₀}"
[!0&1] 0
[0&!1] 3
[0&1] 6
State: 4 "{₀[0#0,0#1]{₁[1#0]₁}₀}"
[!0&1] 7
[0&!1] 4
[0&1] 8
State: 5 "{₀[0#0,0#1] [1#1]{₁[1#0]₁}₀}"
[!0&1] 7
[0&!1] 4
[0&1] 8
State: 6 "{₀[0#0,0#1] [1#0]₀}"
[!0&1] 2
[0&!1] 6
[0&1] 9
State: 7 "{₀[1#1]{₁[0#0,0#1]₁}₀}"
[!0&1] 10
[0&!1] 6 {0}
[0&1] 9 {0}
State: 8 "{₀[1#1]{₁[0#0,0#1] [1#0]₁}₀}"
[!0&1] 2 {0}
[0&!1] 6 {0}
[0&1] 9 {0}
State: 9 "{₀[0#0,0#1] [1#1] [1#0]₀}"
[!0&1] 2
[0&!1] 4
[0&1] 5
State: 10 "{₀[0#0,0#1]{₁[1#1]₁}₀}"
[!0&1] 7
[0&!1] 4 {0}
[0&1] 11
State: 11 "{₀[1#1]{₁[0#0,0#1]{₂[1#0]₂}₁}₀}"
[!0&1] 2 {0}
[0&!1] 6 {0}
[0&1] 9 {0}
--END--"""

a = spot.translate('!Gp0 xor FG((p0 W Gp1) M p1)')
a = spot.degeneralize_tba(a)
assert a.num_states() == 8
b = spot.simulation(a)
assert b.num_states() == 3
b.set_init_state(1)
b.purge_unreachable_states()
b.copy_state_names_from(a)
assert b.to_str() == """HOA: v1
States: 1
Start: 0
AP: 2 "p0" "p1"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels trans-acc complete
properties: deterministic stutter-invariant
--BODY--
State: 0 "[1,7]"
[!1] 0 {0}
[1] 0
--END--"""
