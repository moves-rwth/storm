# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2015-2018, 2020 Laboratoire de Recherche et Développement de
# l'Epita
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

# This test used to trigger an assertion (or a segfault)
# in scc_filter_states().
aut = spot.automaton("""
HOA: v1
States: 3
Start: 1
AP: 1 "a"
Acceptance: 1 Inf(0)
--BODY--
State: 0 {0}
[t] 0
State: 1
[!0] 0
[0] 2
State: 2
[t] 2
--END--
""")
aut.prop_inherently_weak(True)
aut = spot.dualize(aut)
aut1 = spot.scc_filter_states(aut)
assert(aut1.to_str('hoa') == """HOA: v1
States: 2
Start: 0
AP: 1 "a"
acc-name: co-Buchi
Acceptance: 1 Fin(0)
properties: trans-labels explicit-labels state-acc deterministic
properties: very-weak
--BODY--
State: 0 {0}
[0] 1
State: 1
[t] 1
--END--""")

assert(aut.scc_filter_states().to_str() == aut1.to_str())
assert(aut1.get_name() == None)
aut1.set_name("test me")
assert(aut1.get_name() == "test me")
# The method is the same as the function

a = spot.translate('true', 'low', 'any')
assert(a.prop_universal().is_maybe())
assert(a.prop_unambiguous().is_maybe())
assert(a.is_deterministic() == True)
assert(a.is_unambiguous() == True)

a = spot.automaton("""
HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
acc-name: Buchi
Acceptance: 5 Inf(0)&Fin(4) | Inf(2)&Inf(3) | Inf(1)
--BODY--
State: 0 {3}
[t] 0
[0] 1 {1}
[!0] 2 {0 4}
State: 1 {3}
[1] 0
[0&1] 1 {0}
[!0&1] 2 {2 4}
State: 2
[!1] 0
[0&!1] 1 {0}
[!0&!1] 2 {0 4}
--END--
""")
b = spot.remove_fin(a)
size = (b.num_states(), b.num_edges())
assert size == (5, 17);
