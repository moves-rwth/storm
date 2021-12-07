# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2015  Laboratoire de Recherche et Développement
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


aut = spot.translate('GFa & GFb', 'BA')
assert aut.num_sets() == 1
assert aut.num_states() == 3
assert aut.is_deterministic()


min1 = spot.sat_minimize(aut, acc='Rabin 1')
assert min1.num_sets() == 2
assert min1.num_states() == 2
min1 = spot.sat_minimize(aut, acc='Rabin 1', sat_langmap=True)
assert min1.num_sets() == 2
assert min1.num_states() == 2
min1 = spot.sat_minimize(aut, acc='Rabin 1', sat_incr=1)
assert min1.num_sets() == 2
assert min1.num_states() == 2
min1 = spot.sat_minimize(aut, acc='Rabin 1', sat_incr=1, sat_incr_steps=0)
assert min1.num_sets() == 2
assert min1.num_states() == 2
min1 = spot.sat_minimize(aut, acc='Rabin 1', sat_incr=1, sat_incr_steps=1)
assert min1.num_sets() == 2
assert min1.num_states() == 2
min1 = spot.sat_minimize(aut, acc='Rabin 1', sat_incr=1, sat_incr_steps=2)
assert min1.num_sets() == 2
assert min1.num_states() == 2
min1 = spot.sat_minimize(aut, acc='Rabin 1', sat_incr=1, sat_incr_steps=50)
assert min1.num_sets() == 2
assert min1.num_states() == 2
min1 = spot.sat_minimize(aut, acc='Rabin 1', sat_incr=2)
assert min1.num_sets() == 2
assert min1.num_states() == 2
min1 = spot.sat_minimize(aut, acc='Rabin 1', sat_incr=2, sat_incr_steps=-1)
assert min1.num_sets() == 2
assert min1.num_states() == 2
min1 = spot.sat_minimize(aut, acc='Rabin 1', sat_incr=2, sat_incr_steps=0)
assert min1.num_sets() == 2
assert min1.num_states() == 2
min1 = spot.sat_minimize(aut, acc='Rabin 1', sat_incr=2, sat_incr_steps=1)
assert min1.num_sets() == 2
assert min1.num_states() == 2
min1 = spot.sat_minimize(aut, acc='Rabin 1', sat_incr=2, sat_incr_steps=2)
assert min1.num_sets() == 2
assert min1.num_states() == 2
min1 = spot.sat_minimize(aut, acc='Rabin 1', sat_incr=2, sat_incr_steps=50)
assert min1.num_sets() == 2
assert min1.num_states() == 2
min1 = spot.sat_minimize(aut, acc='Rabin 1', sat_naive=True)
assert min1.num_sets() == 2
assert min1.num_states() == 2


min2 = spot.sat_minimize(aut, acc='Streett 2')
assert min2.num_sets() == 4
assert min2.num_states() == 1
min2 = spot.sat_minimize(aut, acc='Streett 2', sat_langmap=True)
assert min2.num_sets() == 4
assert min2.num_states() == 1
min2 = spot.sat_minimize(aut, acc='Streett 2', sat_incr=1)
assert min2.num_sets() == 4
assert min2.num_states() == 1
min2 = spot.sat_minimize(aut, acc='Streett 2', sat_incr=1, sat_incr_steps=0)
assert min2.num_sets() == 4
assert min2.num_states() == 1
min2 = spot.sat_minimize(aut, acc='Streett 2', sat_incr=1, sat_incr_steps=1)
assert min2.num_sets() == 4
assert min2.num_states() == 1
min2 = spot.sat_minimize(aut, acc='Streett 2', sat_incr=1, sat_incr_steps=2)
assert min2.num_sets() == 4
assert min2.num_states() == 1
min2 = spot.sat_minimize(aut, acc='Streett 2', sat_incr=1, sat_incr_steps=50)
assert min2.num_sets() == 4
assert min2.num_states() == 1
min2 = spot.sat_minimize(aut, acc='Streett 2', sat_incr=2)
assert min2.num_sets() == 4
assert min2.num_states() == 1
min2 = spot.sat_minimize(aut, acc='Streett 2', sat_incr=2, sat_incr_steps=-1)
assert min2.num_sets() == 4
assert min2.num_states() == 1
min2 = spot.sat_minimize(aut, acc='Streett 2', sat_incr=2, sat_incr_steps=0)
assert min2.num_sets() == 4
assert min2.num_states() == 1
min2 = spot.sat_minimize(aut, acc='Streett 2', sat_incr=2, sat_incr_steps=1)
assert min2.num_sets() == 4
assert min2.num_states() == 1
min2 = spot.sat_minimize(aut, acc='Streett 2', sat_incr=2, sat_incr_steps=2)
assert min2.num_sets() == 4
assert min2.num_states() == 1
min2 = spot.sat_minimize(aut, acc='Streett 2', sat_incr=2, sat_incr_steps=50)
assert min2.num_sets() == 4
assert min2.num_states() == 1
min2 = spot.sat_minimize(aut, acc='Streett 2', sat_naive=True)
assert min2.num_sets() == 4
assert min2.num_states() == 1


min3 = spot.sat_minimize(aut, acc='Rabin 2',
                         state_based=True, max_states=5)
assert min3.num_sets() == 4
assert min3.num_states() == 3
min3 = spot.sat_minimize(aut, acc='Rabin 2',
                         state_based=True, max_states=5, sat_langmap=True)
assert min3.num_sets() == 4
assert min3.num_states() == 3
min3 = spot.sat_minimize(aut, acc='Rabin 2',
                         state_based=True, max_states=5, sat_incr=1)
assert min3.num_sets() == 4
assert min3.num_states() == 3
min3 = spot.sat_minimize(aut, acc='Rabin 2',
                         state_based=True, max_states=5, sat_incr=1,
                         sat_incr_steps=0)
assert min3.num_sets() == 4
assert min3.num_states() == 3
min3 = spot.sat_minimize(aut, acc='Rabin 2',
                         state_based=True, max_states=5, sat_incr=1,
                         sat_incr_steps=1)
assert min3.num_sets() == 4
assert min3.num_states() == 3
min3 = spot.sat_minimize(aut, acc='Rabin 2',
                         state_based=True, max_states=5, sat_incr=1,
                         sat_incr_steps=2)
assert min3.num_sets() == 4
assert min3.num_states() == 3
min3 = spot.sat_minimize(aut, acc='Rabin 2',
                         state_based=True, max_states=5, sat_incr=1,
                         sat_incr_steps=50)
assert min3.num_sets() == 4
assert min3.num_states() == 3
min3 = spot.sat_minimize(aut, acc='Rabin 2',
                         state_based=True, max_states=5, sat_incr=2)
assert min3.num_sets() == 4
assert min3.num_states() == 3
min3 = spot.sat_minimize(aut, acc='Rabin 2',
                         state_based=True, max_states=5, sat_incr=2,
                         sat_incr_steps=-1)
assert min3.num_sets() == 4
assert min3.num_states() == 3
min3 = spot.sat_minimize(aut, acc='Rabin 2',
                         state_based=True, max_states=5, sat_incr=2,
                         sat_incr_steps=0)
assert min3.num_sets() == 4
assert min3.num_states() == 3
min3 = spot.sat_minimize(aut, acc='Rabin 2',
                         state_based=True, max_states=5, sat_incr=2,
                         sat_incr_steps=1)
assert min3.num_sets() == 4
assert min3.num_states() == 3
min3 = spot.sat_minimize(aut, acc='Rabin 2',
                         state_based=True, max_states=5, sat_incr=2,
                         sat_incr_steps=2)
assert min3.num_sets() == 4
assert min3.num_states() == 3
min3 = spot.sat_minimize(aut, acc='Rabin 2',
                         state_based=True, max_states=5, sat_incr=2,
                         sat_incr_steps=50)
assert min3.num_sets() == 4
assert min3.num_states() == 3
min3 = spot.sat_minimize(aut, acc='Rabin 2',
                         state_based=True, max_states=5, sat_naive=True)
assert min3.num_sets() == 4
assert min3.num_states() == 3


min4 = spot.sat_minimize(aut, acc='parity max odd 3',
                         colored=True)
assert min4.num_sets() == 3
assert min4.num_states() == 2
min4 = spot.sat_minimize(aut, acc='parity max odd 3',
                         colored=True, sat_langmap=True)
assert min4.num_sets() == 3
assert min4.num_states() == 2
min4 = spot.sat_minimize(aut, acc='parity max odd 3',
                         colored=True, sat_incr=1)
assert min4.num_sets() == 3
assert min4.num_states() == 2
min4 = spot.sat_minimize(aut, acc='parity max odd 3',
                         colored=True, sat_incr=1, sat_incr_steps=0)
assert min4.num_sets() == 3
assert min4.num_states() == 2
min4 = spot.sat_minimize(aut, acc='parity max odd 3',
                         colored=True, sat_incr=1, sat_incr_steps=1)
assert min4.num_sets() == 3
assert min4.num_states() == 2
min4 = spot.sat_minimize(aut, acc='parity max odd 3',
                         colored=True, sat_incr=1, sat_incr_steps=2)
assert min4.num_sets() == 3
assert min4.num_states() == 2
min4 = spot.sat_minimize(aut, acc='parity max odd 3',
                         colored=True, sat_incr=1, sat_incr_steps=50)
assert min4.num_sets() == 3
assert min4.num_states() == 2
min4 = spot.sat_minimize(aut, acc='parity max odd 3',
                         colored=True, sat_incr=2)
assert min4.num_sets() == 3
assert min4.num_states() == 2
min4 = spot.sat_minimize(aut, acc='parity max odd 3',
                         colored=True, sat_incr=2, sat_incr_steps=-1)
assert min4.num_sets() == 3
assert min4.num_states() == 2
min4 = spot.sat_minimize(aut, acc='parity max odd 3',
                         colored=True, sat_incr=2, sat_incr_steps=0)
assert min4.num_sets() == 3
assert min4.num_states() == 2
min4 = spot.sat_minimize(aut, acc='parity max odd 3',
                         colored=True, sat_incr=2, sat_incr_steps=1)
assert min4.num_sets() == 3
assert min4.num_states() == 2
min4 = spot.sat_minimize(aut, acc='parity max odd 3',
                         colored=True, sat_incr=2, sat_incr_steps=2)
assert min4.num_sets() == 3
assert min4.num_states() == 2
min4 = spot.sat_minimize(aut, acc='parity max odd 3',
                         colored=True, sat_incr=2, sat_incr_steps=50)
assert min4.num_sets() == 3
assert min4.num_states() == 2
min4 = spot.sat_minimize(aut, acc='parity max odd 3',
                         colored=True, sat_naive=True)
assert min4.num_sets() == 3
assert min4.num_states() == 2
