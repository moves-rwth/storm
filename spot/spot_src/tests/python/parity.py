#!/usr/bin/python3
# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2018, 2019 Laboratoire de Recherche et Développement de
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

max_even_5 = spot.acc_code.parity(True, False, 5)
assert max_even_5 == spot.acc_code.parity_max_even(5)
assert max_even_5 == spot.acc_code.parity_max(False, 5)
min_even_5 = spot.acc_code.parity(False, False, 5)
assert min_even_5 == spot.acc_code.parity_min_even(5)
assert min_even_5 == spot.acc_code.parity_min(False, 5)

max_odd_5 = spot.acc_code.parity(True, True, 5)
assert max_odd_5 == spot.acc_code.parity_max_odd(5)
assert max_odd_5 == spot.acc_code.parity_max(True, 5)
min_odd_5 = spot.acc_code.parity(False, True, 5)
assert min_odd_5 == spot.acc_code.parity_min_odd(5)
assert min_odd_5 == spot.acc_code.parity_min(True, 5)


for f in ('FGa', 'GFa & GFb & FGc', 'XXX(a U b)'):
    a1 = spot.translate(f, 'parity')
    assert a1.acc().is_parity()
    a2 = spot.translate(f).postprocess('parity')
    assert a2.acc().is_parity()
    a3 = spot.translate(f, 'det').postprocess('parity', 'colored')
    assert a3.acc().is_parity()
    assert spot.is_colored(a3)

a = spot.translate('GFa & GFb')
try:
    spot.change_parity_here(a, spot.parity_kind_same, spot.parity_style_even)
except RuntimeError as e:
    assert 'input should have parity acceptance' in str(e)
else:
    exit(2)

a = spot.automaton("""
HOA: v1
States: 1
Start: 0
AP: 1 "a"
Acceptance: 2 Fin(0) & Inf(1)
--BODY--
State: 0
[t] 0 {0}
--END--
""")
spot.cleanup_parity_here(a)
assert a.to_str() == """HOA: v1
States: 1
Start: 0
AP: 1 "a"
acc-name: none
Acceptance: 0 f
properties: trans-labels explicit-labels state-acc complete
properties: deterministic
--BODY--
State: 0
[t] 0
--END--"""

a = spot.automaton("""
HOA: v1
States: 1
Start: 0
AP: 1 "a"
Acceptance: 2 Fin(0) | Inf(1)
--BODY--
State: 0
[t] 0 {1}
--END--
""")
spot.cleanup_parity_here(a)
assert a.to_str() == """HOA: v1
States: 1
Start: 0
AP: 1 "a"
acc-name: all
Acceptance: 0 t
properties: trans-labels explicit-labels state-acc complete
properties: deterministic
--BODY--
State: 0
[t] 0
--END--"""
