# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2018-2020 Laboratoire de Recherche et Développement de
# l'Epita (LRDE).
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


# Test some function that must return exceptions on error.  Doing
# so is mostly a way to improve the coverage report.


import spot
import buddy


def report_missing_exception():
    raise RuntimeError("missing exception")


aut = spot.translate('GFa & GFb & GFc')
aut.set_acceptance(spot.acc_cond("parity min even 4"))
try:
    spot.iar(aut)
except RuntimeError as e:
    assert 'iar() expects Rabin-like or Streett-like input' in str(e)
else:
    report_missing_exception()

alt = spot.dualize(spot.translate('FGa | FGb'))
try:
    spot.tgba_determinize(alt)
except RuntimeError as e:
    assert 'tgba_determinize() does not support alternation' in str(e)
else:
    report_missing_exception()

aut = spot.translate('a U b U c')
aps = aut.ap()
rem = spot.remove_ap()
rem.add_ap('"a"=0,b')
aut = rem.strip(aut)
assert aut.ap() == aps[2:]
try:
    rem.add_ap('"a=0,b')
except ValueError as e:
    assert """missing closing '"'""" in str(e)
else:
    report_missing_exception()

try:
    rem.add_ap('a=0=b')
except ValueError as e:
    assert """unexpected '=' at position 3""" in str(e)
else:
    report_missing_exception()

si = spot.scc_info(aut)
for meth in ('scc_has_rejecting_cycle', 'is_inherently_weak_scc',
             'is_weak_scc', 'is_complete_scc', 'is_terminal_scc'):
    try:
        getattr(spot, meth)(si, 20)
    except ValueError as e:
        assert "invalid SCC number" in str(e)
    else:
        report_missing_exception()


s1 = alt.new_state()
s2 = alt.new_state()
alt.new_edge(0, s1, buddy.bddtrue)
alt.new_edge(s1, s2, buddy.bddtrue, [0])
alt.new_edge(s2, s1, buddy.bddtrue)
alt.prop_inherently_weak(False)
alt.prop_state_acc(False)
si = spot.scc_info(alt)
try:
    si.determine_unknown_acceptance()
except RuntimeError as e:
    assert "scc_info::determine_unknown_acceptance() does not supp" in str(e)
else:
    report_missing_exception()

try:
    alt.set_init_state(999)
except ValueError as e:
    assert "set_init_state()" in str(e)
else:
    report_missing_exception()

alt.set_univ_init_state([s1, s2])
u = alt.get_init_state_number()
alt.set_init_state(u)

try:
    alt.set_init_state(u - 1)
except ValueError as e:
    assert "set_init_state()" in str(e)
else:
    report_missing_exception()


r = spot.twa_run(aut)
try:
    a = r.as_twa()
except RuntimeError as e:
    assert "empty cycle" in str(e)
else:
    report_missing_exception()

try:
    a = r.replay(spot.get_cout())
except RuntimeError as e:
    assert "empty cycle" in str(e)
else:
    report_missing_exception()

try:
    a = r.reduce()
except RuntimeError as e:
    assert "empty cycle" in str(e)
else:
    report_missing_exception()

f = spot.formula('GF(a | Gb)')
try:
    spot.gf_guarantee_to_ba(f, spot._bdd_dict)
except RuntimeError as e:
    assert "guarantee" in str(e)
else:
    report_missing_exception()

f = spot.formula('FG(a | Fb)')
try:
    spot.fg_safety_to_dca(f, spot._bdd_dict)
except RuntimeError as e:
    assert "safety" in str(e)
else:
    report_missing_exception()

n = spot.mark_t.max_accsets()
m = spot.mark_t([n - 1])
try:
    m = spot.mark_t([0]) << n
except RuntimeError as e:
    assert "Too many acceptance sets" in str(e)
else:
    report_missing_exception()

try:
    m.set(n)
except RuntimeError as e:
    assert "bit index is out of bounds" in str(e)
else:
    report_missing_exception()

try:
    m = spot.mark_t([0, n, 1])
except RuntimeError as e:
    assert "Too many acceptance sets used.  The limit is" in str(e)
else:
    report_missing_exception()

try:
    spot.complement_semidet(spot.translate('Gb R a', 'ba'))
except RuntimeError as e:
    assert "requires a semi-deterministic input" in str(e)
else:
    report_missing_exception()

try:
    spot.translate('F(G(a | !a) & ((b <-> c) W d))', 'det', 'any')
except ValueError as e:
    s = str(e)
    assert 'det' in s
    assert 'any' in s
else:
    report_missing_exception()

a1 = spot.translate('FGa')
a2 = spot.translate('Gb')
assert not spot.is_deterministic(a1)
assert spot.is_deterministic(a2)
try:
    spot.product_xor(a1, a2)
except RuntimeError as e:
    assert "product_xor() only works with deterministic automata"
else:
    report_missing_exception()
try:
    spot.product_xor(a2, a1)
except RuntimeError as e:
    assert "product_xor() only works with deterministic automata"
else:
    report_missing_exception()
try:
    spot.product_xnor(a1, a2)
except RuntimeError as e:
    assert "product_xnor() only works with deterministic automata"
else:
    report_missing_exception()
try:
    spot.product_xnor(a2, a1)
except RuntimeError as e:
    assert "product_xnor() only works with deterministic automata"
else:
    report_missing_exception()
