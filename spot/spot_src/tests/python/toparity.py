#!/usr/bin/python3
# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2018-2020 Laboratoire de Recherche et Développement de
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
from itertools import zip_longest
from buddy import bddfalse

# Tests for the new version of to_parity

no_option = spot.to_parity_options()
no_option.search_ex = False
no_option.use_last = False
no_option.force_order = False
no_option.partial_degen = False
no_option.acc_clean = False
no_option.parity_equiv = False
no_option.parity_prefix = False
no_option.rabin_to_buchi = False
no_option.propagate_col = False

acc_clean_search_opt = spot.to_parity_options()
acc_clean_search_opt.force_order = False
acc_clean_search_opt.partial_degen = False
acc_clean_search_opt.parity_equiv = False
acc_clean_search_opt.parity_prefix = False
acc_clean_search_opt.rabin_to_buchi = False
acc_clean_search_opt.propagate_col = False

partial_degen_opt = spot.to_parity_options()
partial_degen_opt.search_ex = False
partial_degen_opt.force_order = False
partial_degen_opt.parity_equiv = False
partial_degen_opt.parity_prefix = False
partial_degen_opt.rabin_to_buchi = False
partial_degen_opt.propagate_col = False

parity_equiv_opt = spot.to_parity_options()
parity_equiv_opt.search_ex = False
parity_equiv_opt.use_last = False
parity_equiv_opt.force_order = False
parity_equiv_opt.partial_degen = False
parity_equiv_opt.parity_prefix = False
parity_equiv_opt.rabin_to_buchi = False
parity_equiv_opt.propagate_col = False

rab_to_buchi_opt = spot.to_parity_options()
rab_to_buchi_opt.use_last = False
rab_to_buchi_opt.force_order = False
rab_to_buchi_opt.partial_degen = False
rab_to_buchi_opt.parity_equiv = False
rab_to_buchi_opt.parity_prefix = False
rab_to_buchi_opt.propagate_col = False

# Force to use CAR or IAR for each SCC
use_car_opt = spot.to_parity_options()
use_car_opt.partial_degen = False
use_car_opt.parity_equiv = False
use_car_opt.parity_prefix = False
use_car_opt.rabin_to_buchi = False
use_car_opt.propagate_col = False

all_opt = spot.to_parity_options()
all_opt.pretty_print = True


options = [
    no_option,
    acc_clean_search_opt,
    partial_degen_opt,
    parity_equiv_opt,
    rab_to_buchi_opt,
    use_car_opt,
    all_opt,
]


def test(aut, expected_num_states=[], full=True):
    for (opt, expected_num) in zip_longest(options, expected_num_states):
        p1 = spot.to_parity(aut,
                            search_ex = opt.search_ex,
                            use_last = opt.use_last,
                            force_order = opt.force_order,
                            partial_degen = opt.partial_degen,
                            acc_clean = opt.acc_clean,
                            parity_equiv = opt.parity_equiv,
                            parity_prefix = opt.parity_prefix,
                            rabin_to_buchi = opt.rabin_to_buchi,
                            reduce_col_deg = opt.reduce_col_deg,
                            propagate_col = opt.propagate_col,
                            pretty_print = opt.pretty_print,
                            )
        p1st, p1ed, p1se = p1.num_states(), p1.num_edges(), p1.num_sets()
        if opt.parity_prefix is False:
            # Reduce the number of colors to help are_equivalent
            spot.reduce_parity_here(p1)
        assert spot.are_equivalent(aut, p1)
        if expected_num is not None:
            # print(p1.num_states(), expected_num)
            assert p1.num_states() == expected_num
        if full:
            # Make sure passing opt is the same as setting
            # each argument individually
            p2 = spot.to_parity(aut, opt)
            assert p2.num_states() == p1st
            assert p2.num_edges() == p1ed
            assert p2.num_sets() == p1se

test(spot.automaton("""HOA: v1
name: "(FGp0 & ((XFp0 & F!p1) | F(Gp1 & XG!p0))) | G(F!p0 & (XFp0 | F!p1) &
F(Gp1 | G!p0))"
States: 14
Start: 0
AP: 2 "p1" "p0"
Acceptance: 6 (Fin(0) & Fin(1)) | ((Fin(4)|Fin(5)) & (Inf(2)&Inf(3)))
properties: trans-labels explicit-labels trans-acc complete
properties: deterministic
--BODY--
State: 0
[!0] 1
[0] 2
State: 1
[!0&!1] 1 {0 1 2 3 5}
[0&!1] 3
[!0&1] 4
[0&1] 5
State: 2
[0&!1] 2 {1}
[!0&1] 4
[!0&!1] 6
[0&1] 7
State: 3
[0&!1] 3 {1 3}
[!0&1] 4
[!0&!1] 6 {0 1 2 3 5}
[0&1] 8
State: 4
[!0&!1] 4 {1 2 3 5}
[!0&1] 4 {2 4 5}
[0&!1] 5 {1 3}
[0&1] 5 {4}
State: 5
[!0&1] 4 {2 4 5}
[0&!1] 5 {1 3}
[0&1] 8 {2 4}
[!0&!1] 9 {1 2 3 5}
State: 6
[0&!1] 3 {1 3}
[!0&1] 4
[0&1] 5
[!0&!1] 10
State: 7
[!0&1] 4
[0&!1] 7 {1 3}
[!0&!1] 11
[0&1] 12 {0 4}
State: 8
[!0&1] 4 {2 4 5}
[0&1] 5 {4}
[0&!1] 8 {1 3}
[!0&!1] 11 {1 3 5}
State: 9
[!0&1] 4 {2 4 5}
[0&!1] 5 {1 3}
[0&1] 5 {4}
[!0&!1] 11 {1 3 5}
State: 10
[!0&1] 4
[0&1] 8
[!0&!1] 10 {0 1 2 3 5}
[0&!1] 13 {1 2 3}
State: 11
[!0&1] 4 {2 4 5}
[0&!1] 8 {1 2 3}
[0&1] 8 {2 4}
[!0&!1] 11 {1 2 3 5}
State: 12
[!0&1] 4
[0&1] 7 {0 2 4}
[!0&!1] 9
[0&!1] 12 {1 3}
State: 13
[!0&1] 4
[0&1] 5
[!0&!1] 10 {0 1 3 5}
[0&!1] 13 {1 3}
--END--"""), [35, 28, 23, 30, 29, 25, 22])

test(spot.automaton("""
HOA: v1
States: 2
Start: 0
AP: 2 "p0" "p1"
Acceptance: 5 (Inf(0)&Inf(1)) | ((Fin(2)|Fin(3)) & Fin(4))
--BODY--
State: 0
[!0 & 1] 0 {2 3}
[!0 & !1] 0 {3}
[0] 1
State: 1
[0&1] 1 {1 2 4}
[0&!1] 1 {4}
[!0&1] 1 {0 1 2 3}
[!0&!1] 1 {0 3}
--END--"""), [7, 5, 3, 6, 5, 5, 3])


for i,f in enumerate(spot.randltl(10, 400)):
    test(spot.translate(f, "det", "G"), full=(i<100))

for f in spot.randltl(5, 2500):
    test(spot.translate(f), full=False)


test(spot.automaton("""
HOA: v1
States: 4
Start: 0
AP: 2 "p0" "p1"
Acceptance: 6
((Fin(1) | Inf(2)) & Inf(5)) | (Fin(0) & (Fin(1) | (Fin(3) & Inf(4))))
properties: trans-labels explicit-labels trans-acc complete
properties: deterministic
--BODY--
State: 0
[!0&!1] 0 {2}
[0&1] 1 {0 5}
[0&!1] 1 {0 2 5}
[!0&1] 2 {1}
State: 1
[0&1] 1 {0}
[!0&!1] 1 {2}
[0&!1] 1 {0 2}
[!0&1] 2 {1}
State: 2
[!0&!1] 0 {2 3}
[0&!1] 0 {0 2 3 5}
[!0&1] 2 {1 4}
[0&1] 3 {0 5}
State: 3
[!0&!1] 0 {2 3}
[0&!1] 0 {0 2 3 5}
[!0&1] 2 {1 4}
[0&1] 3 {0}
--END--
"""), [80, 22, 80, 80, 80, 17, 10])

test(spot.automaton("""
HOA: v1
States: 5
Start: 0
AP: 2 "p1" "p0"
Acceptance: 5 (Fin(0) & Fin(1)) | (Fin(3) & (Inf(2)&Inf(4)))
properties: trans-labels explicit-labels trans-acc complete
properties: deterministic
--BODY--
State: 0
[!0] 1
[0] 2
State: 1
[!0&!1] 1 {0 1}
[!0&1] 3
[0] 4
State: 2
[!0&1] 1
[0&!1] 2
[0&1] 2 {0 1 2 4}
[!0&!1] 3
State: 3
[!0&1] 3 {1 2 3}
[!0&!1] 3 {4}
[0&!1] 4 {3}
[0&1] 4 {1 2 3}
State: 4
[!0&!1] 3 {3}
[!0&1] 3 {1 2 3}
[0&!1] 4
[0&1] 4 {1 2 4}
--END--
"""), [9, 6, 7, 7, 6, 6, 6])

test(spot.automaton("""
HOA: v1
States: 2
Start: 0
AP: 2 "p1" "p0"
Acceptance: 5 (Fin(0) & (Fin(3)|Fin(4)) & (Inf(1)&Inf(2))) | Inf(3)
properties: trans-labels explicit-labels trans-acc complete
properties: deterministic stutter-invariant
--BODY--
State: 0
[0&!1] 0 {2 3}
[!0&!1] 0 {2 3 4}
[!0&1] 1
[0&1] 1 {2 4}
State: 1
[!0&!1] 0 {0 2 3 4}
[!0&1] 1 {1}
[0&!1] 1 {2 3}
[0&1] 1 {1 2 4}
--END--
"""), [11, 6, 3, 7, 7, 4, 3])


# Tests both the old and new version of to_parity
a = spot.automaton("""HOA: v1
States: 1
Start: 0
AP: 2 "a" "b"
Acceptance: 2 Inf(0)|Inf(1)
--BODY--
State: 0
[0&1] 0 {0 1}
[0&!1] 0 {0}
[!0&1] 0 {1}
[!0&!1] 0
--END--""")
p = spot.to_parity_old(a, True)
assert spot.are_equivalent(a, p)
test(a)

a = spot.automaton("""
HOA: v1 States: 6 Start: 0 AP: 2 "p0" "p1" Acceptance: 6 Inf(5) |
Fin(2) | Inf(1) | (Inf(0) & Fin(3)) | Inf(4) properties: trans-labels
explicit-labels trans-acc --BODY-- State: 0 [0&1] 2 {4 5} [0&1] 4 {0 4}
[!0&!1] 3 {3 5} State: 1 [0&!1] 3 {1 5} [!0&!1] 5 {0 1} State: 2 [!0&!1]
0 {0 3} [0&!1] 1 {0} State: 3 [!0&1] 4 {1 2 3} [0&1] 3 {3 4 5} State:
4 [!0&!1] 1 {2 4} State: 5 [!0&1] 4 --END--
""")
p = spot.to_parity_old(a, True)
assert p.num_states() == 22
assert spot.are_equivalent(a, p)
test(a, [8, 7, 8, 8, 6, 7, 6])

# Force a few edges to false, to make sure to_parity() is OK with that.
for e in a.out(2):
    if e.dst == 1:
        e.cond = bddfalse
        break
for e in a.out(3):
    if e.dst == 3:
        e.cond = bddfalse
        break
p = spot.to_parity_old(a, True)
assert p.num_states() == 22
assert spot.are_equivalent(a, p)
test(a, [7, 6, 7, 7, 6, 6, 6])

for f in spot.randltl(4, 400):
    d = spot.translate(f, "det", "G")
    p = spot.to_parity_old(d, True)
    assert spot.are_equivalent(p, d)

for f in spot.randltl(5, 2000):
    n = spot.translate(f)
    p = spot.to_parity_old(n, True)
    assert spot.are_equivalent(n, p)

# Issue #390.
a = spot.translate('!(GFa -> (GFb & GF(!b & !Xb)))', 'gen', 'det')
b = spot.to_parity_old(a, True)
assert a.equivalent_to(b)
test(a, [7, 7, 3, 7, 8, 7, 3])
