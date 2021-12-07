# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2020 Laboratoire de Recherche et Développement de l'Epita
# (LRDE).
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

# This issue was reported by Florian Renkin.  The reduce() call used in
# intersecting_run() was bogus, and could incorrectly reduce a word
# intersecting the product into a word not intersecting the product if the
# acceptance condition uses some Fin.
a, b = spot.automata("""HOA: v1 States: 5 Start: 0 AP: 2 "p0" "p1" acc-name:
Rabin 2 Acceptance: 4 (Fin(0) & Inf(1)) | (Fin(2) & Inf(3)) properties:
trans-labels explicit-labels trans-acc complete properties: deterministic
--BODY-- State: 0 [t] 3 State: 1 [t] 4 {1} State: 2 [0] 2 {1} [!0] 1 {0} State:
3 [t] 1 {1} State: 4 [!0&1] 4 {3} [!0&!1] 3 [0] 2 {0} --END-- HOA: v1 States: 5
Start: 0 AP: 2 "p0" "p1" Acceptance: 3 Inf(2) | (Fin(0) & Inf(1)) properties:
trans-labels explicit-labels trans-acc complete properties: deterministic
--BODY-- State: 0 [t] 3 State: 1 [t] 4 {1 2} State: 2 [0] 2 {1 2} [!0] 1 {0 2}
State: 3 [t] 1 {1 2} State: 4 [!0&1] 4 {2} [!0&!1] 3 {2} [0] 2 {0 2} --END--""")
r = b.intersecting_run(spot.complement(a));
c = spot.twa_word(r).as_automaton()
assert c.intersects(b)
assert not c.intersects(a)
