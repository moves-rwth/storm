# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2019  Laboratoire de Recherche et Développement
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


aut = spot.translate('a U (c & Gb)')
assert not spot.is_terminal_automaton(aut)
assert aut.prop_terminal().is_false()
rem = spot.remove_ap()
rem.add_ap("b")
aut = rem.strip(aut)
assert not aut.prop_terminal().is_false()
assert spot.is_terminal_automaton(aut)
assert aut.prop_terminal().is_true()
aut = rem.strip(aut)
assert aut.prop_terminal().is_true()
