# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2010, 2012, 2013, 2014  Laboratoire de Recherche et
# Développement de l'Epita
# Copyright (C) 2004 Laboratoire d'Informatique de Paris 6 (LIP6),
# département Systèmes Répartis Coopératifs (SRC), Université Pierre
# et Marie Curie.
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

import buddy
import sys

buddy.bdd_init(10000, 10000)
buddy.bdd_setvarnum(3)

a = buddy.bdd_ithvar(0)
b = buddy.bdd_ithvar(1)
c = buddy.bdd_ithvar(2)

w = -a & -b | -c & b | a & -b

import spot

isop = spot.minato_isop(w)

i = isop.next()
l = []
while i != buddy.bddfalse:
    buddy.bdd_printset(i)
    spot.nl_cout()
    l.append(i)
    i = isop.next()

sys.exit(l == [-a, -c])
