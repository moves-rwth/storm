#!/usr/bin/python3
# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2017-2019 Laboratoire de Recherche et Développement de
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
import buddy

match_strings = [('is_buchi', 'is_co_buchi'),
                 ('is_generalized_buchi', 'is_generalized_co_buchi'),
                 ('is_all', 'is_none'),
                 ('is_all', 'is_all'),
                 ('is_buchi', 'is_all')]

# existential and universal are dual
# deterministic is self-dual


def dualtype(aut, dual):
    if dual.acc().is_none():
        return True
    return (not spot.is_deterministic(aut) or spot.is_deterministic(dual))\
        and (spot.is_universal(dual) or not aut.is_existential())\
        and (dual.is_existential() or not spot.is_universal(aut))


def produce_phi(rg, n):
    phi = []
    while len(phi) < n:
        phi.append(rg.next())
    return phi


def produce_automaton(phi):
    aut = []
    for f in phi:
        aut.append(spot.translate(f))
    return aut


def test_aut(aut, d=None):
    if d is None:
        d = spot.dualize(aut)
    aa = aut.acc()
    da = d.acc()

    complete = spot.is_complete(aut)
    univ = aut.is_univ_dest(aut.get_init_state_number())
    an = aut.num_states()
    dn = d.num_states()

    if not dualtype(aut, d):
        return (False, 'Incorrect transition mode resulting of dual')
    for p in match_strings:
        if ((getattr(aa, p[0])() and getattr(da, p[1])())
                or (getattr(aa, p[1])() and getattr(da, p[0])())):
            return (True, '')
    return (False, 'Incorrect acceptance type dual')

# Tests that a (deterministic) automaton and its complement have complementary
# languages.
# FIXME This test could be extended to non-deterministic automata with a
# dealternization procedure.


def test_complement(aut):
    assert aut.is_deterministic()
    d = spot.dualize(aut)
    s = spot.product_or(aut, d)
    assert spot.dualize(s).is_empty()


def test_assert(a, d=None):
    t = test_aut(a, d)
    if not t[0]:
        print (t[1])
        print (a.to_str('hoa'))
        print (spot.dualize(a).to_str('hoa'))
        assert False


aut = spot.translate('a')

test_assert(aut)

dual = spot.dualize(aut)
h = dual.to_str('hoa')

assert h == """HOA: v1
States: 3
Start: 1
AP: 1 "a"
acc-name: co-Buchi
Acceptance: 1 Fin(0)
properties: trans-labels explicit-labels state-acc complete
properties: deterministic stutter-invariant very-weak
--BODY--
State: 0 {0}
[t] 0
State: 1
[0] 0
[!0] 2
State: 2
[t] 2
--END--"""

aut = spot.automaton("""
HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels state-acc
--BODY--
State: 0
[0] 1
[0] 2
State: 1 {0}
[0] 1
State: 2 {0}
[1] 2
--END--""")

test_assert(aut)
dual = spot.dualize(aut)
h = dual.to_str('hoa')

assert h == """HOA: v1
States: 4
Start: 0
AP: 2 "a" "b"
acc-name: co-Buchi
Acceptance: 1 Fin(0)
properties: trans-labels explicit-labels state-acc complete
properties: deterministic univ-branch
--BODY--
State: 0
[!0] 3
[0] 1&2
State: 1 {0}
[0] 1
[!0] 3
State: 2 {0}
[1] 2
[!1] 3
State: 3
[t] 3
--END--"""

aut = spot.automaton("""
HOA: v1
States: 4
Start: 0&2
AP: 2 "a" "b"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels state-acc univ-branch
--BODY--
State: 0
[0] 1
State: 1 {0}
[t] 1
State: 2
[1] 3
State: 3 {0}
[t] 3
--END--""")

test_assert(aut)
dual = spot.dualize(aut)
h = dual.to_str('hoa')

assert h == """HOA: v1
States: 2
Start: 1
AP: 2 "a" "b"
acc-name: all
Acceptance: 0 t
properties: trans-labels explicit-labels state-acc deterministic
--BODY--
State: 0
[t] 0
State: 1
[!0 | !1] 0
--END--"""

aut = spot.automaton("""
HOA: v1
States: 4
Start: 0&2
AP: 2 "a" "b"
Acceptance: 2 Inf(0) | Inf(1)
properties: trans-labels explicit-labels state-acc univ-branch
--BODY--
State: 0
[0] 1
State: 1 {0}
[t] 1
State: 2
[1] 3
State: 3 {1}
[t] 3
--END--""")

dual = spot.dualize(aut)
assert dualtype(aut, dual)
h = dual.to_str('hoa')

assert h == """HOA: v1
States: 2
Start: 1
AP: 2 "a" "b"
acc-name: all
Acceptance: 0 t
properties: trans-labels explicit-labels state-acc deterministic
--BODY--
State: 0
[t] 0
State: 1
[!0 | !1] 0
--END--"""

aut = spot.automaton("""
HOA: v1
States: 4
Start: 0
AP: 2 "a" "b"
Acceptance: 1 Inf(0) | Fin(0)
properties: trans-labels explicit-labels state-acc
--BODY--
State: 0
[0] 1
State: 1 {0}
[0] 3
State: 2
[0] 3
State: 3 {0}
[t] 3
--END--""")

dual = spot.dualize(aut)
assert dualtype(aut, dual)
h = dual.to_str('hoa')

assert h == """HOA: v1
States: 5
Start: 0
AP: 2 "a" "b"
acc-name: co-Buchi
Acceptance: 1 Fin(0)
properties: trans-labels explicit-labels state-acc complete
properties: deterministic
--BODY--
State: 0 {0}
[0] 1
[!0] 4
State: 1 {0}
[0] 3
[!0] 4
State: 2 {0}
[0] 3
[!0] 4
State: 3 {0}
[t] 3
State: 4
[t] 4
--END--"""

aut = spot.automaton("""
HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
Acceptance: 2 Inf(0) & Fin(1)
properties: trans-labels explicit-labels state-acc
--BODY--
State: 0 {0}
[0&!1] 1
[0&1] 2
State: 1 {1}
[1] 1
[0&!1] 2
State: 2
[!0&!1] 0
[t] 2
--END--""")

dual = spot.dualize(aut)
assert dualtype(aut, dual)
h = dual.to_str('hoa')

assert h == """HOA: v1
States: 4
Start: 0
AP: 2 "a" "b"
acc-name: Streett 1
Acceptance: 2 Fin(0) | Inf(1)
properties: trans-labels explicit-labels state-acc complete
properties: deterministic univ-branch
--BODY--
State: 0 {0}
[0&!1] 1
[0&1] 2
[!0] 3
State: 1 {1}
[1] 1
[0&!1] 2
[!0&!1] 3
State: 2
[0 | 1] 2
[!0&!1] 0&2
State: 3
[t] 3
--END--"""

aut = spot.automaton("""
HOA: v1
States: 3
Start: 0
AP: 1 "a"
Acceptance: 1 Inf(0) | Fin(0)
properties: trans-labels explicit-labels state-acc complete
--BODY--
State: 0
[0] 1
[!0] 2
State: 1 {0}
[t] 1
State: 2
[t] 2
[0] 0
--END--""")

dual = spot.dualize(aut)
assert dualtype(aut, dual)
h = dual.to_str('hoa')

assert h == """HOA: v1
States: 1
Start: 0
AP: 1 "a"
acc-name: none
Acceptance: 0 f
properties: trans-labels explicit-labels state-acc complete
properties: deterministic terminal
--BODY--
State: 0
[t] 0
--END--"""

aut = spot.automaton("""
HOA: v1
States: 3
Start: 0
AP: 1 "a"
Acceptance: 1 Inf(0) | Fin(0)
properties: trans-labels explicit-labels state-acc complete
--BODY--
State: 0
[0] 1
[!0] 2
State: 1 {0}
[t] 1
State: 2
[t] 2
--END--""")

dual = spot.dualize(aut)
assert dualtype(aut, dual)
h = dual.to_str('hoa')

assert h == """HOA: v1
States: 1
Start: 0
AP: 1 "a"
acc-name: none
Acceptance: 0 f
properties: trans-labels explicit-labels state-acc complete
properties: deterministic terminal
--BODY--
State: 0
[t] 0
--END--"""

aut = spot.automaton("""
HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels trans-acc
--BODY--
State: 0
[0&1] 1
[0&!1] 2
State: 1
[t] 1 {0}
[0] 0
State: 2
[0] 2
--END--""")

dual = spot.dualize(aut)
h = dual.to_str('hoa')

assert h == """HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
acc-name: co-Buchi
Acceptance: 1 Fin(0)
properties: trans-labels explicit-labels state-acc complete
properties: deterministic
--BODY--
State: 0
[0&1] 1
[!0 | !1] 2
State: 1 {0}
[t] 1
State: 2
[t] 2
--END--"""

aut = spot.automaton("""
HOA: v1
States: 3
Start: 0
AP: 1 "a"
Acceptance: 1 Fin(0)
properties: trans-labels explicit-labels state-acc
--BODY--
State: 0
[0] 1
[0] 2
State: 1 {0}
[0] 1
State: 2
[t] 2
--END--""")


dual = spot.dualize(aut)
assert dualtype(aut, dual)
h = dual.to_str('hoa')

assert h == """HOA: v1
States: 2
Start: 0
AP: 1 "a"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels state-acc deterministic
--BODY--
State: 0
[!0] 1
State: 1 {0}
[t] 1
--END--"""

aut = spot.automaton("""
HOA: v1
States: 4
Start: 0
AP: 1 "a"
Acceptance: 1 Fin(0)
properties: trans-labels explicit-labels state-acc
--BODY--
State: 0
[0] 1
[!0] 2
[0] 0
State: 1 {0}
[t] 1
State: 2
[0] 3
[!0] 0
State: 3 {0}
[t] 3
--END--""")

dual = spot.dualize(aut)
assert dualtype(aut, dual)
h = dual.to_str('hoa')

assert h == """HOA: v1
States: 3
Start: 0
AP: 1 "a"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels state-acc complete
properties: deterministic
--BODY--
State: 0
[0] 0
[!0] 1
State: 1
[!0] 0
[0] 2
State: 2 {0}
[t] 2
--END--"""

aut = spot.automaton("""
HOA: v1
States: 3
Start: 0
AP: 1 "a"
Acceptance: 2 Fin(0) & Inf(1)
properties: trans-labels explicit-labels
--BODY--
State: 0
[!0] 0
[0] 1 {0}
[0] 2 {1}
State: 1
[t] 0
State: 2
[t] 0
--END--""")

dual = spot.dualize(aut)
assert dualtype(aut, dual)
h = dual.to_str('hoa')

assert h == """HOA: v1
States: 3
Start: 0
AP: 1 "a"
acc-name: parity min even 2
Acceptance: 2 Inf(0) | Fin(1)
properties: trans-labels explicit-labels state-acc complete
properties: deterministic univ-branch
--BODY--
State: 0
[!0] 0
[0] 1&2
State: 1 {0}
[t] 0
State: 2 {1}
[t] 0
--END--"""

aut = spot.translate('G!a R XFb')
test_assert(aut)
dual = spot.dualize(aut)
h = dual.to_str('hoa')

assert h == """HOA: v1
States: 5
Start: 0
AP: 2 "a" "b"
acc-name: co-Buchi
Acceptance: 1 Fin(0)
properties: trans-labels explicit-labels state-acc complete
properties: deterministic univ-branch
--BODY--
State: 0 {0}
[0&1] 0
[0&!1] 1
[!0&!1] 1&2
[!0&1] 0&2
State: 1
[0&1] 0
[0&!1] 1
[!0&!1] 1&2
[!0&1] 0&2
State: 2
[!0&1] 3
[0 | !1] 4
State: 3 {0}
[!0] 3
[0] 4
State: 4
[t] 4
--END--"""

opts = spot.option_map()
opts.set('output', spot.randltlgenerator.LTL)
opts.set('tree_size_min', 15)
opts.set('tree_size_max', 15)
opts.set('seed', 0)
opts.set('simplification_level', 0)
spot.srand(0)
rg = spot.randltlgenerator(2, opts)

for a in produce_automaton(produce_phi(rg, 1000)):
    test_assert(a)
    test_assert(spot.dualize(a), spot.dualize(spot.dualize(a)))

aut = spot.automaton("""
HOA: v1
States: 1
Start: 0
AP: 1 "a"
Acceptance: 3 Fin(2) & (Inf(1) | Fin(0))
--BODY--
State: 0
--END--""")
test_complement(aut)

for a in spot.automata('randaut -A \"random 0..6\" -H -D -n50 4|'):
    test_complement(a)
