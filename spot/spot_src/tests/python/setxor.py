# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2010, 2011 Laboratoire de Recherche et Développement
# de l'EPITA.
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

import sys
from buddy import *

bdd_init(10000, 10000)
bdd_setvarnum(5)

V = [bdd_ithvar(i) for i in range(5)]

a = V[0] & -V[1] & V[2] & -V[3]
b = V[0] & V[1] & V[2] & -V[3]
c = -V[0] & V[1] & -V[2] & -V[3]

assert(c == bdd_setxor(a, b))
assert(c == bdd_setxor(b, a))
assert(a == bdd_setxor(b, c))
assert(a == bdd_setxor(c, b))
assert(b == bdd_setxor(a, c))
assert(b == bdd_setxor(c, a))

d = V[1] & V[2] & -V[3] & V[4]
e = V[0] & V[1] & -V[2] & -V[3] & V[4]

assert(e == bdd_setxor(a, d))
assert(e == bdd_setxor(d, a))

# Cleanup all BDD variables before calling bdd_done(), otherwise
# bdd_delref will be called after bdd_done() and this is unsafe in
# optimized builds.
V = a = b = c = d = e = 0
bdd_done()
