# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2016-2017, 2019-2020 Laboratoire de Recherche et Développement
# de l'Epita (LRDE).
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

# make sure that we are not allowed to build the product of two automata with
# different dictionaries.

aut1 = spot.translate('Xa')
aut2 = spot.translate('Xb', dict=spot.make_bdd_dict())

try:
    spot.product(aut1, aut2)
    exit(2)
except RuntimeError:
    pass

try:
    spot.otf_product(aut1, aut2)
    exit(2)
except RuntimeError:
    pass

# Test output_aborter on product().  The two automata come from an actual
# execution of ltlcross that uncovered a bug in product()'s handling of
# output_aborter.
left, right = spot.automata('''
HOA: v1 States: 40 Start: 0 AP: 2 "p1" "p0" acc-name: Buchi Acceptance:
1 Inf(0) properties: trans-labels explicit-labels state-acc terminal
--BODY-- State: 0 [0] 1 [!0&1] 2 [!0] 3 [0&1] 4 [t] 5 [t] 6 State: 1
[!0&1] 2 [!0] 3 [!0] 5 [!0] 6 State: 2 [1] 2 [!1] 13 State: 3 [0] 1 [0&1]
4 [0] 5 [0] 6 State: 4 [!0&1] 2 [0&1] 4 [!0&!1] 13 [0&!1] 39 State: 5
[0] 1 [t] 5 [t] 14 [!0] 15 [0] 19 [0&1] 21 [!0&1] 31 [0&1] 34 [0] 38
State: 6 [!0] 3 [t] 6 [t] 14 [!0] 15 [0] 19 [0&1] 25 [!0&1] 26 [!0&1]
27 [!0] 36 State: 7 [t] 7 [!0] 8 [0] 9 State: 8 [0] 29 [0] 30 State:
9 [!0] 23 [!0] 24 State: 10 [!1] 7 [!0&!1] 8 [0&!1] 9 [1] 10 [0&1] 11
[!0&1] 12 State: 11 [!0&!1] 23 [!0&!1] 24 [!0&1] 26 [!0&1] 27 State:
12 [0&!1] 29 [0&!1] 30 [0&1] 31 [0&1] 32 State: 13 {0} [t] 13 State: 14
[!0&1] 10 [!0&1] 12 [t] 14 [!0] 15 [0] 16 [0&1] 17 [!0] 18 [0] 19 [0&1]
20 State: 15 [0] 1 [0] 5 [0] 14 [0] 19 [0&1] 21 [0&1] 34 [0] 38 State:
16 [!0&1] 10 [!0&1] 12 [!0] 14 [!0] 15 [!0] 18 State: 17 [!0&!1] 23
[!0&!1] 24 [!0&1] 26 [!0&1] 27 State: 18 [0] 14 [0] 16 [0&1] 17 [0]
19 [0&1] 20 State: 19 [!0] 3 [!0] 6 [!0] 14 [!0] 15 [!0&1] 26 [!0&1]
27 [!0] 36 State: 20 [!0&!1] 7 [!0&!1] 8 [!0&1] 10 [!0&1] 12 [0&1] 17
[0&1] 20 [0&!1] 22 [0&!1] 37 State: 21 [!0&1] 2 [!0&!1] 13 State: 22 [!0]
23 [!0] 24 State: 23 [0] 13 State: 24 [!0] 23 [t] 24 State: 25 [!0&!1]
23 [!0&!1] 24 [0&1] 25 [!0&1] 26 [!0&1] 27 [0&!1] 28 State: 26 [0&1] 2
[0&!1] 13 State: 27 [!0&!1] 23 [!1] 24 [!0&1] 26 [1] 27 State: 28 [!0]
23 [!0] 24 [0] 28 State: 29 [!0] 13 State: 30 [0] 29 [t] 30 State: 31
[0&!1] 29 [!1] 30 [1] 31 [0&1] 32 State: 32 [!0&1] 2 [!0&!1] 13 State:
33 [!0] 13 State: 34 [0&1] 21 [!0&!1] 30 [!0&1] 31 [0&!1] 33 [0&1] 34
[0&!1] 35 State: 35 [!0] 30 [0] 33 [0] 35 State: 36 [0] 6 [0] 14 [0] 19
[0&1] 25 State: 37 [!0] 7 [!0] 8 [0] 22 [0] 37 State: 38 [!0] 5 [!0] 14
[!0] 15 [!0&1] 31 State: 39 [!0] 13 [0] 39 --END--
HOA: v1 States: 73 Start: 0 AP: 2 "p1" "p0" acc-name: all Acceptance: 0
t properties: implicit-labels state-acc complete deterministic --BODY--
State: 0 1 3 2 4 State: 1 5 7 6 8 State: 2 9 11 10 12 State: 3 5 7 10
13 State: 4 9 14 10 8 State: 5 15 17 16 18 State: 6 19 20 16 21 State:
7 15 17 22 23 State: 8 24 25 22 18 State: 9 26 28 27 29 State: 10 24 20
22 21 State: 11 26 28 30 31 State: 12 24 32 22 33 State: 13 24 34 22 23
State: 14 26 35 30 36 State: 15 15 17 16 18 State: 16 37 38 16 39 State:
17 15 17 22 23 State: 18 40 41 22 18 State: 19 19 20 42 43 State: 20
24 20 44 45 State: 21 40 46 22 21 State: 22 40 38 22 39 State: 23 40 47
22 23 State: 24 24 20 48 43 State: 25 24 25 44 49 State: 26 26 28 27 29
State: 27 40 38 27 50 State: 28 26 28 30 31 State: 29 40 51 30 29 State:
30 40 38 30 50 State: 31 40 52 30 31 State: 32 24 32 44 53 State: 33
40 51 22 33 State: 34 24 34 44 54 State: 35 26 35 30 36 State: 36 40 41
30 36 State: 37 37 38 55 56 State: 38 40 38 57 58 State: 39 40 59 22 39
State: 40 40 38 60 56 State: 41 40 41 57 61 State: 42 37 38 42 62 State:
43 40 46 44 43 State: 44 40 38 44 62 State: 45 40 63 44 45 State: 46 40 46
57 64 State: 47 40 47 57 65 State: 48 40 38 48 62 State: 49 40 41 44 49
State: 50 40 59 30 50 State: 51 40 51 57 66 State: 52 40 52 57 67 State:
53 40 51 44 53 State: 54 40 47 44 54 State: 55 37 38 55 68 State: 56 40 59
57 56 State: 57 40 38 57 68 State: 58 40 69 57 58 State: 59 40 59 57 70
State: 60 40 38 60 68 State: 61 40 41 57 61 State: 62 40 59 44 62 State:
63 40 63 57 71 State: 64 40 46 57 64 State: 65 40 47 57 65 State: 66 40
51 57 66 State: 67 40 52 57 67 State: 68 40 59 57 68 State: 69 40 69 57 72
State: 70 40 59 57 70 State: 71 40 63 57 71 State: 72 40 69 57 72 --END--
''')
res = spot.product(left, right)
assert res.num_states() == 977
assert res.num_edges() == 8554
res = spot.product(left, right, spot.output_aborter(1000, 6000))
assert res is None
res = spot.product(left, right, spot.output_aborter(900, 9000))
assert res is None
res = spot.product(left, right, spot.output_aborter(1000, 9000))
assert res is not None

a, b = spot.automata("""HOA: v1 States: 1 Start: 0 AP: 0 acc-name: all
Acceptance: 0 t properties: trans-labels explicit-labels state-acc complete
properties: deterministic stutter-invariant weak --BODY-- State: 0 [t] 0
--END-- HOA: v1 States: 1 Start: 0 AP: 0 acc-name: none Acceptance: 0 f
properties: trans-labels explicit-labels state-acc complete properties:
deterministic stutter-invariant weak --BODY-- State: 0 [t] 0 --END--""")
out = spot.product(a, b).to_str()
assert out == """HOA: v1
States: 1
Start: 0
AP: 0
acc-name: none
Acceptance: 0 f
properties: trans-labels explicit-labels state-acc deterministic
properties: stutter-invariant terminal
--BODY--
State: 0
--END--"""
out = spot.product_susp(a, b).to_str()
assert out == """HOA: v1
States: 1
Start: 0
AP: 0
acc-name: all
Acceptance: 0 t
properties: trans-labels explicit-labels state-acc deterministic
properties: stutter-invariant terminal
--BODY--
State: 0
--END--"""
