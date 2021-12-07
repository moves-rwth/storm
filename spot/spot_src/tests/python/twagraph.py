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
from buddy import bddtrue

aut = spot.make_twa_graph(spot.make_bdd_dict())

try:
    print(aut.to_str())
    exit(2)
except RuntimeError as e:
    assert "no state" in str(e)

try:
    aut.set_init_state(2)
except ValueError as e:
    assert "nonexisting" in str(e)

try:
    aut.set_univ_init_state([2, 1])
except ValueError as e:
    assert "nonexisting" in str(e)

aut.new_states(3)
aut.set_init_state(2)
assert aut.get_init_state_number() == 2
e = aut.set_univ_init_state([2, 1])
assert [2, 1] == list(aut.univ_dests(aut.get_init_state_number()))

try:
    aut.get_init_state()
except RuntimeError as e:
    s = str(e)
    assert "abstract interface" in s and "alternating automata" in s

all = aut.set_buchi()
aut.new_acc_edge(0, 1, bddtrue, True)

try:
    s = aut.state_acc_sets(0)
except RuntimeError as e:
    assert "state-based acceptance" in str(e)

try:
    s = aut.state_is_accepting(0)
except RuntimeError as e:
    assert "state-based acceptance" in str(e)

aut.prop_state_acc(True)

assert aut.state_acc_sets(0) == all
assert aut.state_is_accepting(0) == True

aut.set_init_state(0)
aut.purge_unreachable_states()
i = aut.get_init_state()
assert aut.state_is_accepting(i) == True

it = aut.succ_iter(i)
it.first()
assert aut.edge_number(it) == 1
assert aut.state_number(it.dst()) == 1
assert aut.edge_storage(it).src == 0
assert aut.edge_storage(1).src == 0
assert aut.edge_data(it).cond == bddtrue
assert aut.edge_data(1).cond == bddtrue
aut.release_iter(it)

aut.purge_dead_states()
i = aut.get_init_state()
assert aut.state_is_accepting(i) == False
