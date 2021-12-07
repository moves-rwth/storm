# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2016-2018, 2020  Laboratoire de Recherche et Développement
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

# Test 1.
aut = spot.automaton("""
HOA: v1
States: 2
Start: 0
AP: 2 "a" "b"
Acceptance: 2 (Fin(0) & Inf(1))
--BODY--
State: 0
[!0] 0 {1}
[0] 1
State: 1
[0] 1 {0}
--END--
""")

exp = """HOA: v1
States: 2
Start: 0
AP: 2 "a" "b"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels trans-acc deterministic
--BODY--
State: 0
[!0] 0 {0}
[0] 1
State: 1
[0] 1
--END--"""

res = spot.remove_fin(aut)
assert(res.to_str('hoa') == exp)

# Test 2.
aut = spot.automaton("""
HOA: v1
States: 3
Start: 0
AP: 1 "a"
Acceptance: 2 (Inf(1) & Fin(0))
--BODY--
State: 0
[!0] 0 {0}
[0] 1
State: 1
[0] 2 {}
State: 2
[!0] 1 {}
[0] 2 {1}
--END--
""")

exp = """HOA: v1
States: 3
Start: 0
AP: 1 "a"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels trans-acc deterministic
--BODY--
State: 0
[!0] 0
[0] 1
State: 1
[0] 2
State: 2
[!0] 1
[0] 2 {0}
--END--"""

res = spot.remove_fin(aut)
assert(res.to_str('hoa') == exp)

# Test 3.
aut = spot.automaton("""
HOA: v1
States: 2
Start: 0
AP: 2 "a" "b"
Acceptance: 4 (Fin(0) & Inf(1)) | (Fin(2) & Inf(3))
--BODY--
State: 0
[!0] 0 {0}
[0] 1
State: 1
[0] 1 {2}
--END--
""")

exp = """HOA: v1
States: 1
Start: 0
AP: 0
acc-name: all
Acceptance: 0 t
properties: trans-labels explicit-labels state-acc deterministic
properties: stutter-invariant weak
--BODY--
State: 0
--END--"""

res = spot.remove_fin(aut)
assert(res.to_str('hoa') == exp)

# Test 4.
aut = spot.automaton("""
HOA: v1
States: 3
Start: 0
Acceptance: 4 (Fin(0) & Inf(1)) | (Fin(2) & Inf(3))
AP: 2 "a" "b"
--BODY--
State: 0
[0 & 1] 0 {0 2}
  [!0] 1
  [!1] 2
State: 1
  [t] 1 {1}
State: 2
  [t] 2 {3}
--END--
""")

exp = """HOA: v1
States: 3
Start: 0
AP: 2 "a" "b"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels state-acc complete
--BODY--
State: 0
[0&1] 0
[!0] 1
[!1] 2
State: 1 {0}
[t] 1
State: 2 {0}
[t] 2
--END--"""

res = spot.remove_fin(aut)
assert(res.to_str('hoa') == exp)

# Test 5.
aut = spot.automaton("""
HOA: v1
States: 4
Start: 0
Acceptance: 6 (Fin(0) & Inf(1)) | (Fin(2) & Inf(3)) | (Fin(4) & Inf(5))
AP: 3 "a" "b" "c"
--BODY--
State: 0
[0 & 1 &2] 0 {0 2 4}
  [!0] 1
  [!1] 2
  [!2] 3
State: 1
  [t] 1 {1}
State: 2
  [t] 2 {3}
State: 3
  [t] 3 {5}
--END--
""")

exp = """HOA: v1
States: 4
Start: 0
AP: 3 "a" "b" "c"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels state-acc complete
--BODY--
State: 0
[0&1&2] 0
[!0] 1
[!1] 2
[!2] 3
State: 1 {0}
[t] 1
State: 2 {0}
[t] 2
State: 3 {0}
[t] 3
--END--"""

res = spot.remove_fin(aut)
assert(res.to_str('hoa') == exp)

# Test 6.
aut = spot.automaton("""
HOA: v1
States: 2
Start: 0
Acceptance: 4 Fin(0)&Inf(1) | Fin(2)&Inf(3)
AP: 2 "p3" "p2"
--BODY--
State: 1 "F(Gp3|GFp2)"
[0&1] 1 {1 3 }
[0&!1] 0 {1 3 }
[!0&1] 1 {1 2 3 }
[!0&!1] 0 {1 2 3 }
State: 0 "F(Gp3|GFp2)"
[0&1] 1 {3 }
[0&!1] 0 {3 }
[!0&1] 1 {2 3 }
[!0&!1] 0 {2 3 }
--END--
""")

exp = """HOA: v1
States: 4
Start: 0
AP: 2 "p3" "p2"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels state-acc
--BODY--
State: 0
[!1] 0
[1] 1
[0&!1] 2
State: 1 {0}
[!1] 0
[1] 1
[0&!1] 2
[0&1] 3
State: 2 {0}
[0&!1] 2
[0&1] 3
State: 3 {0}
[0&!1] 2
[0&1] 3
--END--"""

res = spot.remove_fin(aut)
assert(res.to_str('hoa') == exp)

# Test 7.
aut = spot.automaton("""
HOA: v1
name: "FG(a)"
properties: deterministic
properties: complete
States: 1
Start: 0
Acceptance: 2 Fin(0)&Inf(1)
AP: 1 "a"
--BODY--
State: 0 "FG(a)"
[0] 0 {1 }
[!0] 0 {0 1 }
--END--
""")

exp = """HOA: v1
States: 2
Start: 0
AP: 1 "a"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels state-acc
--BODY--
State: 0
[t] 0
[0] 1
State: 1 {0}
[0] 1
--END--"""

res = spot.remove_fin(aut)
assert(res.to_str('hoa') == exp)

# Test 8.
aut = spot.automaton("""
HOA: v1
States: 7
Start: 0
Acceptance: 4 Fin(0)&Inf(1) | Fin(2)&Inf(3)
AP: 3 "p0" "p2" "p3"
--BODY--
State: 1 "0"
[t] 1 {1 }
State: 5 "1"
[0&2] 5 {0 1 3 }
[0&!2] 6 {0 1 3 }
[!0&2] 3 {0 1 2 3 }
[!0&!2] 4 {0 1 2 3 }
State: 0 "2"
[!0&1&2] 3 {0 1 2 }
[(0&!1&!2|!0&1&!2)] 4 {0 1 2 }
[(0&1|!0&!1)] 1 {0 1 2 }
[0&!1&2] 2 {0 1 2 }
State: 2 "3"
[0&2] 5 {0 1 }
[0&!2] 6 {0 1 }
[!0&2] 3 {0 1 2 }
[!0&!2] 4 {0 1 2 }
State: 4 "4"
[t] 4 {0 1 2 }
State: 6 "5"
[0] 6 {0 1 3 }
[!0] 4 {0 1 2 3 }
State: 3 "6"
[0&2] 2 {0 1 2 }
[!0&2] 3 {0 1 2 }
[!2] 4 {0 1 2 }
--END--
""")

exp = """HOA: v1
States: 8
Start: 0
AP: 3 "p0" "p2" "p3"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels trans-acc
--BODY--
State: 0
[!0&!1 | 0&1] 1
[0&!1&2] 2
[!0&1&2] 3
[!0&1&!2 | 0&!1&!2] 4
State: 1
[t] 1 {0}
State: 2
[!0&2] 3
[!0&!2] 4
[0&2] 5
[0&!2] 6
State: 3
[0&2] 2
[!0&2] 3
[!2] 4
State: 4
[t] 4
State: 5
[!0&2] 3
[!0&!2] 4
[0&2] 5
[0&!2] 6
[0&2] 7
State: 6
[!0] 4
[0] 6 {0}
State: 7
[0&2] 7 {0}
--END--"""

res = spot.remove_fin(aut)
assert(res.to_str('hoa') == exp)

# Test 9.
aut = spot.automaton("""
HOA: v1
States: 2
Start: 0
AP: 3 "p0" "p4" "p1"
Acceptance: 4 (Fin(0) & Inf(1)) | (Fin(2) & Inf(3))
--BODY--
State: 0
[0&!2] 1 {0 1 3}
[!0&!2] 0 {0 1 3}
State: 1
[0&!2] 1 {0 1}
[!0&!2] 0 {0 1}
--END--
""")

exp = """HOA: v1
States: 2
Start: 0
AP: 3 "p4" "p0" "p1"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels state-acc deterministic
--BODY--
State: 0 {0}
[1&!2] 1
[!1&!2] 0
State: 1
[1&!2] 1
[!1&!2] 0
--END--"""

res = spot.remove_fin(aut)
assert(res.to_str('hoa') == exp)

# Test 10.
aut = spot.automaton("""
HOA: v1
States: 2
Start: 0
acc-name: Rabin 1
Acceptance: 2 Fin(0)&Inf(1)
AP: 2 "p0" "p2"
--BODY--
State: 0 "0"
[0&1] 0 {1 }
[!0&1] 1 {1 }
[0&!1] 0 {0 1 }
State: 1 "1"
[1] 1 {1 }
--END--
""")

exp = """HOA: v1
States: 3
Start: 0
AP: 2 "p0" "p2"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels state-acc
--BODY--
State: 0
[0] 0
[!0&1] 1
[0&1] 2
State: 1 {0}
[1] 1
State: 2 {0}
[0&1] 2
--END--"""

res = spot.remove_fin(aut)
assert(res.to_str('hoa') == exp)

# Test 11.
aut = spot.automaton("""
HOA: v1
States: 2
Start: 0
Acceptance: 4 Fin(0)&Inf(1) | Fin(2)&Inf(3)
AP: 2 "p2" "p0"
--BODY--
State: 1 "1"
[1] 1 {1 2 3 }
[!1] 0 {1 3 }
State: 0 "3"
[1] 1 {2 3 }
[!1] 0 {3 }
--END--
""")

exp = """HOA: v1
States: 2
Start: 0
AP: 2 "p0" "p2"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels trans-acc complete
properties: deterministic
--BODY--
State: 0
[!0] 0 {0}
[0] 1
State: 1
[!0] 0 {0}
[0] 1 {0}
--END--"""

res = spot.remove_fin(aut)
assert(res.to_str('hoa') == exp)

# Different order for rabin_to_buchi_if_realizable() due to merge_edges() not
# being called.  This is on purpose: the edge order should match exactly the
# original automaton.
exp2 = """HOA: v1
States: 2
Start: 0
AP: 2 "p0" "p2"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels trans-acc complete
properties: deterministic
--BODY--
State: 0
[0] 1
[!0] 0 {0}
State: 1
[0] 1 {0}
[!0] 0 {0}
--END--"""
res = spot.rabin_to_buchi_if_realizable(aut)
assert(res.to_str('hoa') == exp2)

# Test 12.
aut = spot.automaton("""
HOA: v1
properties: deterministic
properties: complete
States: 2
Start: 0
Acceptance: 3 Fin(0) | Fin(1)&Inf(2)
AP: 2 "p2" "p0"
--BODY--
State: 1 "1"
[1] 1 {0 1 2}
[!1] 0 {0 2}
State: 0 "3"
[1] 1 {1 2}
[!1] 0 {2}
--END--
""")

exp = """HOA: v1
States: 4
Start: 0
AP: 2 "p0" "p2"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels state-acc
--BODY--
State: 0
[!0] 0
[0] 1
[!0] 2
[!0] 3
State: 1
[!0] 0
[0] 1
[!0] 3
State: 2 {0}
[!0] 2
State: 3 {0}
[!0] 3
--END--"""

res = spot.remove_fin(aut)
assert(res.to_str('hoa') == exp)

# Test 13.
aut = spot.automaton("""
HOA: v1
properties: deterministic
properties: complete
States: 2
Start: 0
Acceptance: 3 Inf(0) | Fin(1)&Inf(2)
AP: 2 "p3" "p2"
--BODY--
State: 1 "F(Gp3|GFp2)"
[0&1] 1 {0 2}
[0&!1] 0 {0}
[!0&1] 1 {0 1 2}
[!0&!1] 0 {0 1 2}
State: 0 "F(Gp3|GFp2)"
[0&1] 1 {2}
[0&!1] 0 {0}
[!0&1] 1 {}
[!0&!1] 0 {2}
--END--
""")

exp = """HOA: v1
States: 2
Start: 0
AP: 2 "p3" "p2"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels trans-acc complete
properties: deterministic
--BODY--
State: 0
[!1] 0 {0}
[1] 1
State: 1
[!1] 0 {0}
[1] 1 {0}
--END--"""

res = spot.remove_fin(aut)
assert(res.to_str('hoa') == exp)

# rabin_to_buchi_if_realizable() does not call merge_edges() on purpose: the
# edge order should match exactly the original automaton.
exp2 = """HOA: v1
States: 2
Start: 0
AP: 2 "p3" "p2"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels trans-acc complete
properties: deterministic
--BODY--
State: 0
[0&1] 1
[0&!1] 0 {0}
[!0&1] 1
[!0&!1] 0 {0}
State: 1
[0&1] 1 {0}
[0&!1] 0 {0}
[!0&1] 1 {0}
[!0&!1] 0 {0}
--END--"""

res = spot.rabin_to_buchi_if_realizable(aut)
assert(res.to_str('hoa') == exp2)

# Test 14.
aut = spot.automaton("""
HOA: v1
States: 1
Start: 0
Acceptance: 4 (Fin(0)&Inf(1)) | (Fin(2)&Inf(3))
AP: 2 "b" "a"
--BODY--
State: 0
 0 {3}          /*{}*/
 0 {1 3}        /*{b}*/
 0 {2}          /*{a}*/
 0 {2 1}        /*{b, a}*/
--END--""")

exp = """HOA: v1
States: 2
Start: 0
AP: 2 "b" "a"
acc-name: Buchi
Acceptance: 1 Inf(0)
properties: trans-labels explicit-labels trans-acc
--BODY--
State: 0
[!0] 0
[0] 0 {0}
[!0&!1] 1
[0&!1] 1 {0}
State: 1
[!1] 1 {0}
--END--"""

res = spot.remove_fin(aut)
assert(res.to_str('hoa') == exp)
assert spot.rabin_to_buchi_if_realizable(aut) is None
