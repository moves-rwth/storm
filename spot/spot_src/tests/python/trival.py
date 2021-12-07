# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2016, 2018  Laboratoire de Recherche et Développement
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

v1 = spot.trival()
v2 = spot.trival(False)
v3 = spot.trival(True)
v4 = spot.trival_maybe()
assert v1 != v2
assert v1 != v3
assert v2 != v3
assert v2 == spot.trival(spot.trival.no_value)
assert v2 != spot.trival(spot.trival.yes_value)
assert v4 != v2
assert v4 != v3
assert v2 == False
assert True == v3
assert v2 != True
assert False != v3
assert v4 == spot.trival_maybe()
assert v4 == spot.trival(spot.trival.maybe_value)
assert v3
assert -v2
assert not -v1
assert not v1
assert not -v3

for u in (v1, v2, v3):
    for v in (v1, v2, v3):
        assert (u & v) == -(-u | -v)
