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

# Make sure scc_filter preserves state-names (suggested by Juraj
# Major)

import spot

a = spot.automaton("""
HOA: v1.1
States: 3
Start: 1
AP: 1 "a"
acc-name: Buchi
Acceptance: 1 Inf(0)
spot.highlight.states: 0 0 2 3
--BODY--
State: 0 "baz"
[t] 0
State: 2 "foo" {0}
[0] 0
[0] 2
[!0] 2
State: 1 "bar"
[0] 2
--END--
""")

assert (spot.scc_filter(a, True).to_str('hoa', '1.1') == """HOA: v1.1
States: 2
Start: 0
AP: 1 "a"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels state-acc !complete
properties: deterministic
spot.highlight.states: 1 3
--BODY--
State: 0 "bar"
[0] 1
State: 1 "foo" {0}
[t] 1
--END--""")
