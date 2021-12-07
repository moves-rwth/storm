# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2017 Laboratoire de Recherche et Développement
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

a = spot.vector_rs_pair()

m0 = spot.mark_t([0])
m1 = spot.mark_t([1])
m2 = spot.mark_t([2])
m3 = spot.mark_t([3])
mall = spot.mark_t()


def test_rs(acc, rs, expected_res, expected_pairs):
    res, p = getattr(acc, 'is_' + rs + '_like')()
    assert res == expected_res
    if expected_res:
        expected_pairs.sort()
        p = sorted(p)
        for a, b in zip(p, expected_pairs):
            assert a.fin == b.fin and a.inf == b.inf


def switch_pairs(pairs):
    if pairs == None:
        return None
    r = []
    for p in pairs:
        r.append(spot.rs_pair(p.inf, p.fin))
    return r


def test_streett(acc, expected_streett_like, expected_pairs):
    test_rs(acc, 'streett', expected_streett_like, expected_pairs)
    o_acc = spot.acc_cond(acc.get_acceptance().complement())
    test_rs(o_acc, 'rabin', expected_streett_like,
            switch_pairs(expected_pairs))


def test_rabin(acc, expected_rabin_like, expected_pairs):
    test_rs(acc, 'rabin', expected_rabin_like, expected_pairs)
    o_acc = spot.acc_cond(acc.get_acceptance().complement())
    test_rs(o_acc, 'streett', expected_rabin_like,
            switch_pairs(expected_pairs))


acc = spot.acc_cond(spot.acc_code('Fin(0)'))
test_streett(acc, True, [spot.rs_pair(m0, mall)])

acc = spot.acc_cond(spot.acc_code('Fin(0)|Inf(1)'))
test_streett(acc, True, [spot.rs_pair(m0, m1)])

acc = spot.acc_cond(spot.acc_code('(Fin(0)|Inf(1))&(Fin(2)|Inf(3))'))
test_streett(acc, True, [spot.rs_pair(m0, m1), spot.rs_pair(m2, m3)])

acc = spot.acc_cond(spot.acc_code('(Fin(0)|Inf(1))&(Inf(3)|Fin(2))'))
test_streett(acc, True, [spot.rs_pair(m0, m1), spot.rs_pair(m2, m3)])

acc = spot.acc_cond(spot.acc_code('(Fin(0)|Inf(1))&(Fin(3)|Inf(2))'))
test_streett(acc, True, [spot.rs_pair(m0, m1), spot.rs_pair(m3, m2)])

acc = spot.acc_cond(spot.acc_code('(Fin(0)|Inf(1))&(Fin(0)|Inf(2))'))
test_streett(acc, True, [spot.rs_pair(m0, m1), spot.rs_pair(m0, m2)])

acc = spot.acc_cond(spot.acc_code('(Fin(0)|Inf(1))&(Fin(1)|Inf(2))'))
test_streett(acc, True, [spot.rs_pair(m0, m1), spot.rs_pair(m1, m2)])

acc = spot.acc_cond(spot.acc_code('(Fin(0)|Inf(1))&(Fin(1)|Inf(2))'
                                  '&(Fin(3)&Inf(3))'))
test_streett(acc, True, [spot.rs_pair(m0, m1), spot.rs_pair(m1, m2),
                         spot.rs_pair(m3, mall), spot.rs_pair(mall, m3)])

acc = spot.acc_cond(spot.acc_code('(Fin(0)|Inf(1))&(Fin(1)|Inf(2))'
                                  '&(Fin(3)&Inf(3))&(Fin(4)|Inf(5)|Inf(6))'))
test_streett(acc, False, None)

acc = spot.acc_cond(spot.acc_code('(Fin(0)&Inf(1))'))
test_rabin(acc, True, [spot.rs_pair(m0, m1)])

acc = spot.acc_cond(spot.acc_code('(Fin(0)&Inf(1))|(Fin(2)&Inf(3))'))
test_rabin(acc, True, [spot.rs_pair(m0, m1), spot.rs_pair(m2, m3)])

acc = spot.acc_cond(spot.acc_code('(Fin(0)&Inf(1))|(Inf(3)&Fin(2))'))
test_rabin(acc, True, [spot.rs_pair(m0, m1), spot.rs_pair(m2, m3)])

acc = spot.acc_cond(spot.acc_code('(Fin(0)&Inf(1))|(Fin(3)&Inf(2))'))
test_rabin(acc, True, [spot.rs_pair(m0, m1), spot.rs_pair(m3, m2)])

acc = spot.acc_cond(spot.acc_code('(Fin(0)&Inf(1))|(Fin(0)&Inf(2))'))
test_rabin(acc, True, [spot.rs_pair(m0, m1), spot.rs_pair(m0, m2)])

acc = spot.acc_cond(spot.acc_code('(Fin(0)&Inf(1))|(Fin(1)&Inf(2))'))
test_rabin(acc, True, [spot.rs_pair(m0, m1), spot.rs_pair(m1, m2)])

acc = spot.acc_cond(spot.acc_code('(Fin(0)&Inf(1))|(Fin(1)&Inf(2))'
                                  '|(Fin(3)|Inf(3))'))
test_rabin(acc, True, [spot.rs_pair(m0, m1), spot.rs_pair(m1, m2),
                       spot.rs_pair(m3, mall), spot.rs_pair(mall, m3)])

acc = spot.acc_cond(spot.acc_code('(Fin(0)&Inf(1))|(Fin(1)&Inf(2))'
                                  '|(Fin(3)|Inf(3))|(Fin(4)&Inf(5)&Inf(6))'))
test_rabin(acc, False, None)
