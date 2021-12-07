# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2012, 2014, 2015 Laboratoire de Recherche et Développement
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

import os
import spot

contents = '''
HOA: v1 name: "a U b" States: 2 Start: 1 AP: 2 "a" "b" acc-name: Buchi
Acceptance: 1 Inf(0) properties: trans-labels explicit-labels state-acc
deterministic --BODY-- State: 0 {0} [t] 0 State: 1 [1] 0 [0&!1] 1 --END--
'''

filename = 'parsetgba.hoa'

out = open(filename, 'w+')
out.write(contents)
out.close()

a = spot.parse_aut(filename, spot.make_bdd_dict())

assert not a.errors

spot.print_dot(spot.get_cout(), a.aut)

del a

os.unlink(filename)
