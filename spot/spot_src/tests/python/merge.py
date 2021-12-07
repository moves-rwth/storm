#!/usr/bin/python3
# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2017, 2020 Laboratoire de Recherche et Développement de
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

aut = spot.automaton("""
HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
Acceptance: 4 Inf(0) & Inf(1) | Fin(3)
properties: trans-labels explicit-labels trans-acc
--BODY--
State: 0
[0] 1 {0 1}
State: 1
[0] 1 {2 3}
[1] 2 {0 1 2 3}
State: 2
[1] 0 {2 3}
--END--""")
spot.simplify_acceptance_here(aut)
hoa = aut.to_str('hoa')

assert hoa == """HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
acc-name: parity min even 2
Acceptance: 2 Inf(0) | Fin(1)
properties: trans-labels explicit-labels trans-acc
--BODY--
State: 0
[0] 1 {0}
State: 1
[0] 1 {1}
[1] 2 {0 1}
State: 2
[1] 0 {1}
--END--"""

aut = spot.automaton("""
HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
Acceptance: 4 Inf(0) & Inf(1) & Inf(2) | Fin(3)
properties: trans-labels explicit-labels trans-acc
--BODY--
State: 0
[0] 1 {0 1}
State: 1
[0] 1 {2 3}
[1] 2 {0 1 2 3}
State: 2
[1] 0 {2 3}
--END--""")
spot.simplify_acceptance_here(aut)
hoa = aut.to_str('hoa')

assert hoa == """HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
acc-name: parity min even 2
Acceptance: 2 Inf(0) | Fin(1)
properties: trans-labels explicit-labels trans-acc
--BODY--
State: 0
[0] 1 {0}
State: 1
[0] 1 {1}
[1] 2 {0 1}
State: 2
[1] 0 {1}
--END--"""

aut = spot.automaton("""
HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
Acceptance: 4 (Inf(0) & Fin(1)) | (Inf(2) & Fin(3))
properties: trans-labels explicit-labels trans-acc
--BODY--
State: 0
[0] 1 {0 1}
State: 1
[0] 1 {2 3}
[1] 2 {0 1 2 3}
State: 2
[1] 0 {2 3}
--END--""")
spot.simplify_acceptance_here(aut)
hoa = aut.to_str('hoa')

assert hoa == """HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
acc-name: none
Acceptance: 0 f
properties: trans-labels explicit-labels state-acc
--BODY--
State: 0
[0] 1
State: 1
[0] 1
[1] 2
State: 2
[1] 0
--END--"""

aut = spot.automaton("""
HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
Acceptance: 4 (Inf(0) | Fin(1)) & (Inf(2) | Fin(3))
properties: trans-labels explicit-labels trans-acc
--BODY--
State: 0
[0] 1 {0 1}
State: 1
[0] 1 {2 3}
[1] 2 {0 1 3}
State: 2
[1] 0 {2 3}
--END--""")
spot.simplify_acceptance_here(aut)
hoa = aut.to_str('hoa')

assert hoa == """HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
acc-name: parity min even 2
Acceptance: 2 Inf(0) | Fin(1)
properties: trans-labels explicit-labels trans-acc
--BODY--
State: 0
[0] 1
State: 1
[0] 1 {0 1}
[1] 2 {1}
State: 2
[1] 0 {0 1}
--END--"""

aut = spot.automaton("""
HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
Acceptance: 4 (Inf(0) | Fin(1)) & (Inf(2) | Fin(3))
properties: trans-labels explicit-labels trans-acc
--BODY--
State: 0
[0] 1 {0 1}
State: 1
[0] 1 {2 3}
[1] 2 {0 1 2 3}
State: 2
[1] 0 {2 3}
--END--""")
spot.simplify_acceptance_here(aut)
hoa = aut.to_str('hoa')

assert hoa == """HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
acc-name: all
Acceptance: 0 t
properties: trans-labels explicit-labels state-acc
--BODY--
State: 0
[0] 1
State: 1
[0] 1
[1] 2
State: 2
[1] 0
--END--"""

aut = spot.automaton("""HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
Acceptance: 4 (Inf(0) & Fin(2)) | (Inf(1) & Fin(3))
properties: trans-labels explicit-labels trans-acc
--BODY--
State: 0
[0] 1 {0 1}
State: 1
[0] 1 {2 3}
[1] 2 {2 3}
State: 2
[1] 0 {2 3}
--END--""")
spot.simplify_acceptance_here(aut)
hoa = aut.to_str('hoa')

assert hoa == """HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
acc-name: co-Buchi
Acceptance: 1 Fin(0)
properties: trans-labels explicit-labels state-acc
--BODY--
State: 0
[0] 1
State: 1 {0}
[0] 1
[1] 2
State: 2 {0}
[1] 0
--END--"""

aut = spot.automaton("""HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
Acceptance: 4 (Inf(0) & Fin(2)) | (Inf(1) & (Fin(3) | Fin(2)))
properties: trans-labels explicit-labels trans-acc
--BODY--
State: 0
[0] 1 {0 1}
State: 1
[0] 1 {2 3}
[1] 2 {2 3}
State: 2
[1] 0 {2 3}
--END--""")
spot.simplify_acceptance_here(aut)
hoa = aut.to_str('hoa')

assert hoa == """HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
acc-name: co-Buchi
Acceptance: 1 Fin(0)
properties: trans-labels explicit-labels state-acc
--BODY--
State: 0
[0] 1
State: 1 {0}
[0] 1
[1] 2
State: 2 {0}
[1] 0
--END--"""

aut = spot.automaton("""HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
Acceptance: 4 (Inf(0) & Fin(2)) | (Inf(1) & (Fin(3) | Fin(2)))
properties: trans-labels explicit-labels trans-acc
--BODY--
State: 0
[0] 1 {0 1}
State: 1
[0] 1 {2 3}
[1] 2 {2 3}
State: 2
[1] 0
--END--""")
spot.simplify_acceptance_here(aut)
hoa = aut.to_str('hoa')

assert hoa == """HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
Acceptance: 2 (Inf(0) & Fin(1)) | (Inf(0) & Fin(1))
properties: trans-labels explicit-labels state-acc
--BODY--
State: 0 {0}
[0] 1
State: 1 {1}
[0] 1
[1] 2
State: 2
[1] 0
--END--"""

aut = spot.automaton("""HOA: v1
States: 4
Start: 0
AP: 2 "a" "b"
acc-name: Rabin 2
Acceptance: 4 (Fin(0) & Inf(1)) | (Fin(2) & Inf(3))
--BODY--
State: 0 {0}
[!0&!1] 1
[0&!1] 0
[!0&1] 3
[0&1] 2
State: 1 {1}
[!0&!1] 1
[0&!1] 0
[!0&1] 3
[0&1] 2
State: 2 {0 3}
[!0&!1] 1
[0&!1] 0
[!0&1] 3
[0&1] 2
State: 3 {1 3}
[!0&!1] 1
[0&!1] 0
[!0&1] 3
[0&1] 2
--END--""")

spot.simplify_acceptance_here(aut)
hoa = aut.to_str('hoa')

assert hoa == """HOA: v1
States: 4
Start: 0
AP: 2 "a" "b"
acc-name: Streett 1
Acceptance: 2 Fin(0) | Inf(1)
properties: trans-labels explicit-labels state-acc complete
properties: deterministic
--BODY--
State: 0 {0}
[!0&!1] 1
[0&!1] 0
[!0&1] 3
[0&1] 2
State: 1
[!0&!1] 1
[0&!1] 0
[!0&1] 3
[0&1] 2
State: 2 {0 1}
[!0&!1] 1
[0&!1] 0
[!0&1] 3
[0&1] 2
State: 3 {1}
[!0&!1] 1
[0&!1] 0
[!0&1] 3
[0&1] 2
--END--"""

aut = spot.automaton("""HOA: v1
States: 3
Start: 0
AP: 2 "p0" "p1"
acc-name: Streett 2
Acceptance: 4 (Fin(0) | Inf(1)) & (Fin(2) | Inf(3))
properties: trans-labels explicit-labels trans-acc complete
properties: deterministic
--BODY--
State: 0
[0] 1
[!0] 0 {2}
State: 1
[0] 1 {1 2}
[!0] 2
State: 2
[0] 2 {0 1 2}
[!0] 1 {0}
--END--""")
spot.simplify_acceptance_here(aut)
hoa = aut.to_str('hoa')

assert hoa == """HOA: v1
States: 3
Start: 0
AP: 2 "p0" "p1"
Acceptance: 3 (Fin(0) | Inf(1)) & Fin(2)
properties: trans-labels explicit-labels trans-acc complete
properties: deterministic
--BODY--
State: 0
[0] 1
[!0] 0 {2}
State: 1
[0] 1 {1 2}
[!0] 2
State: 2
[0] 2 {0 1 2}
[!0] 1 {0}
--END--"""

aut = spot.automaton("""HOA: v1
States: 4
Start: 0
AP: 2 "p0" "p1"
acc-name: Streett 2
Acceptance: 4 (Fin(0) | Inf(1)) & (Fin(2) | Inf(3))
properties: trans-labels explicit-labels trans-acc complete
properties: deterministic
--BODY--
State: 0
[!0&1] 2
[!0&!1] 0 {0}
[0] 3
State: 1
[0] 0 {1 2 3}
[!0] 3 {0 2}
State: 2
[t] 1 {1 2}
State: 3
[0&1] 0 {1}
[0&!1] 3 {1 2}
[!0] 1 {2 3}
--END--""")
spot.simplify_acceptance_here(aut)
hoa = aut.to_str('hoa')

assert hoa == """HOA: v1
States: 4
Start: 0
AP: 2 "p0" "p1"
acc-name: Streett 2
Acceptance: 4 (Fin(0) | Inf(1)) & (Fin(2) | Inf(3))
properties: trans-labels explicit-labels trans-acc complete
properties: deterministic
--BODY--
State: 0
[!0&1] 2
[!0&!1] 0 {0}
[0] 3
State: 1
[0] 0 {1 2 3}
[!0] 3 {0 2}
State: 2
[t] 1 {1 2}
State: 3
[0&1] 0 {1}
[0&!1] 3 {1 2}
[!0] 1 {2 3}
--END--"""

aut = spot.automaton("""HOA: v1
States: 1
Start: 0
AP: 2 "p0" "p1"
acc-name: Streett 2
Acceptance: 4 (Fin(0) | Inf(1)) & (Fin(2) | Inf(3))
properties: trans-labels explicit-labels state-acc complete
properties: deterministic
--BODY--
State: 0 {1 2}
[t] 0
--END--
""")
spot.simplify_acceptance_here(aut)
hoa = aut.to_str('hoa')

assert hoa == """HOA: v1
States: 1
Start: 0
AP: 2 "p0" "p1"
acc-name: none
Acceptance: 0 f
properties: trans-labels explicit-labels state-acc complete
properties: deterministic
--BODY--
State: 0
[t] 0
--END--"""

aut = spot.automaton("""HOA: v1
States: 2
Start: 0
AP: 2 "p0" "p1"
acc-name: Streett 2
Acceptance: 4 (Fin(0) | Inf(1)) & (Fin(2) | Inf(3))
properties: trans-labels explicit-labels trans-acc complete
properties: deterministic
--BODY--
State: 0
[0] 0 {0 2}
[!0] 1 {3}
State: 1
[t] 1 {1 3}
--END--""")
spot.simplify_acceptance_here(aut)
hoa = aut.to_str('hoa')

assert hoa == """HOA: v1
States: 2
Start: 0
AP: 2 "p0" "p1"
Acceptance: 3 (Fin(0) | Inf(1)) & Inf(2)
properties: trans-labels explicit-labels trans-acc complete
properties: deterministic
--BODY--
State: 0
[0] 0 {0}
[!0] 1 {2}
State: 1
[t] 1 {1 2}
--END--"""

aut = spot.automaton("""HOA: v1
States: 1
Start: 0
AP: 2 "p0" "p1"
acc-name: Streett 2
Acceptance: 4 (Fin(0) | Inf(1)) & (Fin(2) | Inf(3))
properties: trans-labels explicit-labels state-acc complete
properties: deterministic
--BODY--
State: 0 {0 1 3}
[t] 0
--END--""")
spot.simplify_acceptance_here(aut)
hoa = aut.to_str('hoa')

assert hoa == """HOA: v1
States: 1
Start: 0
AP: 2 "p0" "p1"
acc-name: all
Acceptance: 0 t
properties: trans-labels explicit-labels state-acc complete
properties: deterministic
--BODY--
State: 0
[t] 0
--END--"""

aut = spot.automaton("""HOA: v1
States: 2
Start: 0
AP: 2 "p0" "p1"
acc-name: Streett 2
Acceptance: 4 (Fin(0) | Inf(1)) & (Fin(2) | Inf(3))
properties: trans-labels explicit-labels trans-acc complete
properties: deterministic
--BODY--
State: 0
[0] 0 {0}
[!0] 1
State: 1
[0] 1 {3}
[!0] 0 {1 3}
--END--""")
spot.simplify_acceptance_here(aut)
hoa = aut.to_str('hoa')

assert hoa == """HOA: v1
States: 2
Start: 0
AP: 2 "p0" "p1"
acc-name: Streett 1
Acceptance: 2 Fin(0) | Inf(1)
properties: trans-labels explicit-labels trans-acc complete
properties: deterministic
--BODY--
State: 0
[0] 0 {0}
[!0] 1
State: 1
[0] 1
[!0] 0 {1}
--END--"""

aut = spot.automaton("""HOA: v1
States: 2
Start: 0
AP: 2 "p0" "p1"
acc-name: Streett 1
Acceptance: 2 Fin(0) | Inf(1)
properties: trans-labels explicit-labels state-acc colored complete
properties: deterministic
--BODY--
State: 0 {1}
[t] 1
State: 1 {1}
[t] 0
--END--""")
spot.simplify_acceptance_here(aut)
hoa = aut.to_str('hoa')

assert hoa == """HOA: v1
States: 2
Start: 0
AP: 2 "p0" "p1"
acc-name: all
Acceptance: 0 t
properties: trans-labels explicit-labels state-acc complete
properties: deterministic
--BODY--
State: 0
[t] 1
State: 1
[t] 0
--END--"""

aut = spot.automaton("""HOA: v1
States: 3
Start: 0
AP: 2 "p0" "p1"
acc-name: Streett 2
Acceptance: 4 (Fin(0) | Inf(1)) & (Fin(2) | Inf(3))
properties: trans-labels explicit-labels state-acc complete
properties: deterministic
--BODY--
State: 0 {1}
[t] 2
State: 1 {0 3}
[t] 1
State: 2 {2}
[t] 1
--END--""")
spot.simplify_acceptance_here(aut)
hoa = aut.to_str('hoa')

assert hoa == """HOA: v1
States: 3
Start: 0
AP: 2 "p0" "p1"
Acceptance: 3 (Fin(0) | Inf(1)) & (Fin(2) | Inf(0))
properties: trans-labels explicit-labels state-acc colored complete
properties: deterministic
--BODY--
State: 0 {1}
[t] 2
State: 1 {0}
[t] 1
State: 2 {2}
[t] 1
--END--"""

aut = spot.automaton("""HOA: v1
States: 3
Start: 0
AP: 2 "p0" "p1"
acc-name: Streett 2
Acceptance: 4 (Fin(0) | Inf(1)) & (Fin(2) | Inf(3))
properties: trans-labels explicit-labels state-acc complete
properties: deterministic
--BODY--
State: 0 {0 1 2}
[t] 2
State: 1 {0 3}
[t] 2
State: 2 {1 2 3}
[t] 1
--END--
""")
spot.simplify_acceptance_here(aut)
hoa = aut.to_str('hoa')

assert hoa == """HOA: v1
States: 3
Start: 0
AP: 2 "p0" "p1"
Acceptance: 3 (Fin(0) | Inf(1)) & (Fin(1) | Inf(2))
properties: trans-labels explicit-labels state-acc complete
properties: deterministic
--BODY--
State: 0 {0 1}
[t] 2
State: 1 {0 2}
[t] 2
State: 2 {1 2}
[t] 1
--END--"""

aut = spot.automaton("""HOA: v1
States: 2
Start: 0
AP: 2 "p0" "p1"
acc-name: Streett 1
Acceptance: 2 Fin(0) | Inf(1)
properties: trans-labels explicit-labels trans-acc complete
properties: deterministic
--BODY--
State: 0
[t] 1 {1}
State: 1
[0] 1 {1}
[!0] 0
--END--""")
spot.simplify_acceptance_here(aut)
hoa = aut.to_str('hoa')

assert hoa == """HOA: v1
States: 2
Start: 0
AP: 2 "p0" "p1"
acc-name: all
Acceptance: 0 t
properties: trans-labels explicit-labels state-acc complete
properties: deterministic
--BODY--
State: 0
[t] 1
State: 1
[0] 1
[!0] 0
--END--"""

aut = spot.automaton("""HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
Acceptance: 4 Fin(0) & Fin(1) & Inf(2)
--BODY--
State: 0
[0] 1 {0}
State: 1
[0] 1 {1 2}
[1] 2 {0 2}
State: 2
[1] 0 {1}
--END--""")
spot.simplify_acceptance_here(aut)
hoa = aut.to_str('hoa')
assert hoa == """HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
acc-name: none
Acceptance: 0 f
properties: trans-labels explicit-labels state-acc
--BODY--
State: 0
[0] 1
State: 1
[0] 1
[1] 2
State: 2
[1] 0
--END--"""

aut = spot.automaton("""HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
Acceptance: 4 Inf(0) | Inf(1) | Inf(2)
--BODY--
State: 0
[0] 1 {0}
State: 1
[0] 1 {1 2}
[1] 2 {0 2}
State: 2
[1] 0 {1}
--END--""")
spot.simplify_acceptance_here(aut)
hoa = aut.to_str('hoa')
assert hoa == """HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
acc-name: all
Acceptance: 0 t
properties: trans-labels explicit-labels state-acc
--BODY--
State: 0
[0] 1
State: 1
[0] 1
[1] 2
State: 2
[1] 0
--END--"""
