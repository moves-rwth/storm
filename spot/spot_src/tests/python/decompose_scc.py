# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2017 Laboratoire de Recherche et
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

aut = spot.translate('(Ga -> Gb) W c')
si = spot.scc_info(aut)

assert (spot.decompose_scc(si, 2).to_str('hoa', '1.1') == """HOA: v1.1
States: 3
Start: 0
AP: 3 "b" "a" "c"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels state-acc !complete
properties: deterministic terminal
--BODY--
State: 0
[!1&!2] 0
[1&!2] 1
State: 1
[!1&!2] 0
[1&!2] 1
[1&2] 2
State: 2
[1] 2
--END--""")

try:
    spot.decompose_scc(si, 4)
except RuntimeError:
    pass
else:
    raise AssertionError

assert (spot.decompose_scc(si, 0, True).to_str('hoa', '1.1') == """HOA: v1.1
States: 4
Start: 0
AP: 3 "b" "a" "c"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels state-acc complete
properties: deterministic terminal
--BODY--
State: 0
[!1&!2] 0
[2] 1
[1&!2] 2
State: 1 {0}
[t] 1
State: 2
[!1&!2] 0
[!1&2] 1
[1&!2] 2
[1&2] 3
State: 3
[!1] 1
[1] 3
--END--""")

assert (spot.decompose_scc(si, 2, True).to_str('hoa', '1.1') == """HOA: v1.1
States: 2
Start: 0
AP: 3 "b" "a" "c"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels trans-acc !complete
properties: deterministic !weak
--BODY--
State: 0
[!1&!2] 0 {0}
[1&!2] 1
State: 1
[!1&!2] 0 {0}
[1&!2] 1
--END--""")

try:
    spot.decompose_scc(si, 3, True)
except RuntimeError:
    pass
else:
    raise AssertionError
