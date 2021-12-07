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


# This file tests various error conditions on the twa API

import spot

env = spot.declarative_environment()
env.declare("a")
env.declare("b")

f1a = spot.parse_infix_psl("a U b")
f1b = spot.parse_infix_psl("a U b", env)
assert not f1a.errors
assert not f1b.errors
# In the past, atomic propositions requires via different environments were
# never equal, but this feature was never used and we changed that in Spot 2.0
# for the sake of simplicity.
assert f1a.f == f1b.f

f2 = spot.parse_infix_psl("(a U b) U c", env)
assert f2.errors
ostr = spot.ostringstream()
f2.format_errors(ostr)
err = ostr.str()
assert "unknown atomic proposition `c'" in err

f3 = spot.parse_prefix_ltl("R a d", env)
assert f3.errors
ostr = spot.ostringstream()
f3.format_errors(ostr)
err = ostr.str()
assert "unknown atomic proposition `d'" in err

f4 = spot.parse_prefix_ltl("R a b", env)
assert not f4.errors
