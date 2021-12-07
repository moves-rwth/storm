# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2010, 2011, 2012, 2014, 2019 Laboratoire de Recherche et
# Développement de l'EPITA.
# Copyright (C) 2003, 2004 Laboratoire d'Informatique de Paris 6
# (LIP6), département Systèmes Répartis Coopératifs (SRC), Université
# Pierre et Marie Curie.
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

# Python translation of the C++ example from the BuDDy distribution.
# (compare with buddy/examples/queen/queen.cxx)

import sys
from buddy import *


def build(i, j):
    """
    Build the requirements for all other fields than (i,j) assuming
    that (i,j) has a queen.
    """
    a = b = c = d = bddtrue

    # No one in the same column.
    for l in side:
        if l != j:
            a &= X[i][j] >> -X[i][l]

    # No one in the same row.
    for k in side:
        if k != i:
            b &= X[i][j] >> -X[k][j]

    # No one in the same up-right diagonal.
    for k in side:
        ll = k - i + j
        if ll >= 0 and ll < N:
            if k != i:
                c &= X[i][j] >> -X[k][ll]

    # No one in the same down-right diagonal.
    for k in side:
        ll = i + j - k
        if ll >= 0 and ll < N:
            if k != i:
                c &= X[i][j] >> -X[k][ll]

    global queen
    queen &= a & b & c & d


# Get the number of queens from the command-line, or default to 8.
if len(sys.argv) > 1:
    N = int(argv[1])
else:
    N = 8

side = range(N)

# Initialize with 100000 nodes, 10000 cache entries and NxN variables.
bdd_init(N * N * 256, 10000)
bdd_setvarnum(N * N)

queen = bddtrue

# Build variable array.
X = [[bdd_ithvar(i*N+j) for j in side] for i in side]

# Place a queen in each row.
for i in side:
    e = bddfalse
    for j in side:
        e |= X[i][j]
    queen &= e

# Build requirements for each variable(field).
for i in side:
    for j in side:
        sys.stdout.write("Adding position %d, %d\n" % (i, j))
        build(i, j)

# Print the results.
sys.stdout.write("There are %d solutions, one is:\n" %
                 bdd_satcount(queen))
solution = bdd_satone(queen)
bdd_printset(solution)

from spot import nl_cout
nl_cout()

# Cleanup all BDD variables before calling bdd_done(), otherwise
# bdd_delref will be called after bdd_done() and this is unsafe in
# optimized builds.
X = e = queen = solution = 0
bdd_done()
