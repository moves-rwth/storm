#!/usr/bin/python3
# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2016 Laboratoire de Recherche et Développement de
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

# Test case reduced from a report by Reuben Rowe <r.rowe@ucl.ac.uk>
# sent to the Spot mailing list on 2016-10-31.

import spot

a = spot.automaton("""
HOA: v1
States: 5
Start: 4
AP: 3 "p_0" "p_1" "p_2"
Acceptance: 1 Inf(0)
--BODY--
State: 0
[0&!1&2] 1
[!0&!1&2] 2
State: 1 {0}
[0&!1&2] 1
State: 2 {0}
[!0&!1&2] 2
State: 3 {0}
[0&1&!2] 0
[!0&1&!2] 3
State: 4
[!0&1&!2] 3
--END--
""")

b = spot.automaton("""
HOA: v1
States: 8
Start: 0
AP: 3 "p_0" "p_1" "p_2"
Acceptance: 1 Inf(0)
--BODY--
State: 0
[t] 0
[!0&!1&2] 1
[!0&1&!2] 4
[!0&1&!2] 5
[!0&1&!2] 6
State: 1 {0}
[!0&!1&2] 1
State: 2
[0&!1&2] 7
State: 3
[!0&!1&2] 1
State: 4
[0&1&!2] 2
State: 5
[0&1&!2] 3
State: 6 {0}
[!0&1&!2] 6
State: 7 {0}
[0&!1&2] 7
--END--
""")

# In Reuben's report this first block built an incorrect deterministic
# automaton, which ultimately led to an non-empty product.  The second
# was fine.
print("use_simulation=True")
b1 = spot.tgba_determinize(b, False, True, True, True)
assert b1.num_states() == 5
b1 = spot.remove_fin(spot.dualize(b1))
assert not a.intersects(b1)

print("\nuse_simulation=False")
b2 = spot.tgba_determinize(b, False, True, False, True)
assert b2.num_states() == 5
b2 = spot.remove_fin(spot.dualize(b2))
assert not a.intersects(b2)
