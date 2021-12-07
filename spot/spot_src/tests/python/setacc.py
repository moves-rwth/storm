#!/usr/bin/python3
# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2016, 2018 Laboratoire de Recherche et Développement de
# l'EPITA.
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


# Test case reduced from a report from Juraj Major <major@fi.muni.cz>.
a = spot.make_twa_graph(spot._bdd_dict)
a.set_acceptance(0, spot.acc_code("t"))
assert(a.prop_state_acc() == True)
a.set_acceptance(1, spot.acc_code("Fin(0)"))
assert(a.prop_state_acc() == spot.trival.maybe())


# Some tests for used_inf_fin_sets(), which return a pair of mark_t.
(inf, fin) = a.get_acceptance().used_inf_fin_sets()
assert inf == []
assert fin == [0]
(inf, fin) = spot.acc_code("(Fin(0)|Inf(1))&Fin(2)&Inf(0)").used_inf_fin_sets()
assert inf == [0, 1]
assert fin == [0, 2]

# is_rabin_like() returns (bool, [(inf, fin), ...])
(b, v) = spot.acc_cond("(Fin(0)&Inf(1))|(Fin(2)&Inf(0))").is_rabin_like()
assert b == True
assert len(v) == 2
assert v[0].fin == [0]
assert v[0].inf == [1]
assert v[1].fin == [2]
assert v[1].inf == [0]
(b, v) = spot.acc_cond("(Fin(0)|Inf(1))&(Fin(2)|Inf(0))").is_rabin_like()
assert b == False
assert len(v) == 0
(b, v) = spot.acc_cond("(Fin(0)|Inf(1))&(Fin(2)|Inf(0))").is_streett_like()
assert b == True
assert repr(v) == \
    '(spot.rs_pair(fin=[0], inf=[1]), spot.rs_pair(fin=[2], inf=[0]))'
v2 = (spot.rs_pair(fin=[0], inf=[1]), spot.rs_pair(fin=[2], inf=[0]))
assert v == v2

acc = spot.acc_cond("generalized-Rabin 1 2")
(b, v) = acc.is_generalized_rabin()
assert b == True
assert v == (2,)
(b, v) = acc.is_generalized_streett()
assert b == False
assert v == ()
(b, v) = acc.is_streett_like()
assert b == True
ve = (spot.rs_pair([0], []), spot.rs_pair([], [1]), spot.rs_pair([], [2]))
assert v == ve
assert acc.name() == "generalized-Rabin 1 2"

# At the time of writting, acc_cond does not yet recognize
# "generalized-Streett", as there is no definition for that in the HOA format,
# and defining it as follows (dual for gen.Rabin) would prevent Streett from
# being a generalized-Streett.  See issue #249.
acc = spot.acc_cond("Inf(0)|Fin(1)|Fin(2)")
(b, v) = acc.is_generalized_streett()
assert b == True
assert v == (2,)
(b, v) = acc.is_generalized_rabin()
assert b == False
assert v == ()
# FIXME: We should have a way to disable the following output, as it is not
# part of HOA v1.
assert acc.name() == "generalized-Streett 1 2"
