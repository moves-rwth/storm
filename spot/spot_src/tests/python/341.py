# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2017 Laboratoire de Recherche et Développement de l'Epita
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
from subprocess import _active


def two_intersecting_automata():
    """return two random automata with a non-empty intersection"""
    g = spot.automata('randaut -A4 -Q5 -n-1 2 |')
    for a, b in zip(g, g):
        if a.intersects(b):
            return a, b


for i in range(5):
    two_intersecting_automata()

n = len(_active)
print(n, "active processes")
assert(n == 0)
