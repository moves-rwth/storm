# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2018-2020 Laboratoire de Recherche et Développement de l'Epita
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

# Test that the spot.gen package works, in particular, we want
# to make sure that the objects created from spot.gen methods
# are usable with methods from the spot package.

import spot

a1 = spot.automaton('''
HOA: v1  name: "aut"  States: 4  Start: 0  AP: 0
Acceptance: 4 Fin(0) & (Inf(1) | (Fin(2) & Inf(3)))
--BODY--
State: 0  [t] 0 {0}  [t] 1 {0 2}
State: 1  [t] 2
State: 2  [t] 1      [t] 0 {0}    [t] 3 {3}
State: 3  [t] 2 {1}  [t] 0
--END--''')

a2 = spot.automaton('''
HOA: v1 States: 7 Start: 0 AP: 3 "a" "b" "c" Acceptance: 3 Fin(0) & Fin(2)
& Inf(1) properties: trans-labels explicit-labels trans-acc --BODY--
State: 0 [2] 1 [!0&!2] 2 [0&!2] 3 [0&!2] 4 State: 1 [!0 | 1] 1 {1 2}
[0&!1] 5 {1} State: 2 [t] 2 {0} State: 3 [!0&!2] 2 [0&!2] 3 State: 4
[0&!1&2] 1 [!0&2 | 1&2] 1 {2} [!0&!2] 6 {2} [0&!1&!2] 4 [0&1&!2] 4 {2}
State: 5 [!0 | 1] 1 {2} [0&!1] 5 State: 6 [0&!1] 6 {0} [!0 | 1] 6 {0
2} --END--''')

a3 = spot.automaton('''
HOA: v1 States: 11 Start: 0 AP: 3 "a" "b" "c" Acceptance: 5 (Fin(0)
| Inf(1)) & (Fin(2) | Inf(3)) & Fin(4) properties: trans-labels
explicit-labels trans-acc --BODY-- State: 0 [0&!1&!2] 1 {1} [0&!1&!2]
2 {1} [!0&!2] 3 {1} [0&1&!2] 4 {1} [0&1&!2] 5 {1} [0&!1&2] 6 {1} [!0&2 |
1&2] 7 {1} State: 1 [0&!1&!2] 1 {0 2} [!0&!2] 3 {0 2} [0&1&!2] 4 {0 2}
State: 2 [0&!1&!2] 2 {0 2} [!0&!2] 8 {0 2 4} [0&1&!2] 5 {0 2 4} [0&!1&2]
6 {0 2} [!0&2 | 1&2] 7 {0 2 4} State: 3 [0&!1] 9 {1 2} [!0 | 1] 3 {1 2}
State: 4 [0&!1&!2] 1 {1 2} [!0&!2] 3 {1 2} [0&1&!2] 4 {1 2} State: 5
[0&!1&!2] 2 {1 2} [!0&!2] 8 {1 2 4} [0&1&!2] 5 {1 2 4} [0&!1&2] 6 {1 2}
[!0&2 | 1&2] 7 {1 2 4} State: 6 [0&!1] 6 {0 3} [!0 | 1] 7 {0 3 4} State:
7 [0&!1] 6 {1 3} [!0 | 1] 7 {1 3 4} State: 8 [0&!1] 10 {1 2} [!0 | 1]
8 {1 2 4} State: 9 [0&!1] 9 {0 2} [!0 | 1] 3 {0 2} State: 10 [0&!1]
10 {0 2} [!0 | 1] 8 {0 2 4} --END--''')

a4 = spot.automaton('''
HOA: v1 States: 8 Start: 0 AP: 3 "a" "b" "c" Acceptance: 6 ((Fin(0) &
Inf(1)) | (Fin(2) & Inf(3))) & Fin(4) & Inf(5) properties: trans-labels
explicit-labels state-acc complete properties: deterministic --BODY--
State: 0 {2 4} [0&2] 1 [!0&2] 2 [!0&!1&!2] 3 [!0&1&!2] 4 [0&!2] 5 State:
1 {1 2 4} [!0] 6 [0] 1 State: 2 {1 2 5} [!0] 6 [0] 1 State: 3 {1 2 4}
[t] 3 State: 4 {1 2 4} [!0&2] 6 [0&2] 1 [!1&!2] 3 [0&1&!2] 4 [!0&1&!2]
7 State: 5 {1 2 4} [!0&2] 6 [0&2] 1 [!0&!1&!2] 3 [0&!2] 5 [!0&1&!2]
7 State: 6 {2 5} [!0] 6 [0] 1 State: 7 {3 4} [!0&2] 6 [0&2] 1 [!1&!2]
3 [0&1&!2] 4 [!0&1&!2] 7 --END--''')

a5 = spot.automaton("""
HOA: v1 States: 10 Start: 0 AP: 2 "a" "b" Acceptance: 5 ((Inf(4)
| Fin(0)) & Fin(1)) | (Inf(2)&Inf(3)) properties: trans-labels
explicit-labels trans-acc --BODY-- State: 0 [0&!1] 2 State: 1 [!0&!1]
9 {3} [!0&1] 7 {0 1 2} State: 2 [!0&1] 1 [0&!1] 3 {1 2} [!0&1] 2 {3}
[0&!1] 5 {1 4} State: 3 [0&!1] 5 {0 3} [!0&!1] 9 [!0&!1] 2 [0&!1] 4 State:
4 [0&!1] 3 [!0&!1] 6 {0 1 2} State: 5 [!0&!1] 7 {0} [0&!1] 4 [0&!1] 1
[0&!1] 8 {0} State: 6 [!0&!1] 5 {2} [0&!1] 4 {3} State: 7 [!0&1] 5 [0&!1]
2 {2} [!0&1] 4 State: 8 [0&!1] 1 {1} [!0&!1] 6 {0} State: 9 [0&1] 0 {1}
[0&1] 3 {2} [0&1] 7 {1} [0&!1] 8 {2 3 4} --END--""")

a6 = spot.automaton("""
HOA: v1 States: 10 Start: 0 AP: 2 "a" "b" Acceptance: 5 (Inf(1) | (Fin(0)
& Inf(4)) | Fin(2)) & Fin(3) properties: trans-labels explicit-labels
trans-acc --BODY-- State: 0 [0&1] 9 {3} [!0&!1] 0 [0&!1] 5 {0 1} State:
1 [0&!1] 9 {4} [0&1] 8 {3} State: 2 [!0&!1] 8 {0} [!0&1] 6 {2 4} [0&1]
2 [!0&1] 7 State: 3 [0&!1] 2 {0 4} [!0&!1] 3 {1} [!0&1] 4 {0} State: 4
[0&!1] 5 {2} [0&1] 0 [!0&1] 1 {0} State: 5 [!0&!1] 0 [!0&!1] 6 State: 6
[0&1] 3 {2} [!0&1] 1 [0&1] 2 {0 1 3 4} State: 7 [0&1] 1 [!0&1] 7 {0 2}
State: 8 [!0&1] 7 [!0&!1] 9 {0} State: 9 [0&1] 8 {0} [0&!1] 5 [0&!1]
1 --END--""")

a7 = spot.automaton("""
HOA: v1 States: 10 Start: 0 AP: 2 "a" "b" acc-name: Rabin 3 Acceptance:
6 (Fin(0) & Inf(1)) | (Fin(2) & Inf(3)) | (Fin(4) & Inf(5)) properties:
trans-labels explicit-labels trans-acc --BODY-- State: 0 [0&!1] 8 {0}
[!0&!1] 6 {0 1} State: 1 [!0&1] 4 {2} [0&1] 8 {2} State: 2 [0&1] 6 {1 4}
[!0&!1] 3 {1 4} State: 3 [!0&!1] 8 {2 4} [0&1] 4 State: 4 [!0&!1] 8 {4}
[!0&!1] 7 State: 5 [!0&!1] 2 {0 5} [!0&!1] 8 {0 4} [!0&!1] 9 {4} State:
6 [!0&1] 1 {2 3 4} State: 7 [0&!1] 5 {0} [0&!1] 7 State: 8 [!0&1] 4 {0 2}
State: 9 [0&1] 3 {4} [!0&1] 5 {4} --END--""")

a8 = spot.automaton('''
HOA: v1 States: 10 Start: 0 AP: 2 "a" "b" Acceptance: 6 Fin(5) &
((Fin(1) & (Inf(3) | Inf(4))) | Fin(0) | Fin(2)) properties: trans-labels
explicit-labels trans-acc --BODY-- State: 0 [0&1] 8 {0} [0&!1] 6 {2}
State: 1 [!0&1] 9 {0 4 5} State: 2 [!0&1] 1 State: 3 [0&!1] 3 {2}
[0&1] 4 {3 5} State: 4 [0&1] 7 {5} [0&!1] 9 {2} [!0&1] 0 {0 2} State:
5 [!0&1] 1 [!0&1] 3 {2 3} State: 6 [0&!1] 8 {1 2 5} [!0&1] 7 {3} State:
7 [0&1] 2 {0} [!0&1] 5 State: 8 [0&!1] 3 {4 5} State: 9 [!0&1] 3 {1 2}
[0&1] 1 {4} [0&!1] 5 {2} --END--''')

a9 = spot.automaton("""
HOA: v1 States: 10 Start: 0 AP: 2 "a" "b" acc-name: Streett 3 Acceptance:
6 (Fin(0) | Inf(1)) & (Fin(2) | Inf(3)) & (Fin(4) | Inf(5)) properties:
trans-labels explicit-labels trans-acc deterministic --BODY-- State: 0
[0&1] 1 [0&!1] 9 {0 5} State: 1 [0&!1] 5 State: 2 [!0&!1] 4 {1} State: 3
[!0&!1] 8 {0} State: 4 [0&1] 6 {0 3} State: 5 [!0&!1] 7 State: 6 [!0&1]
4 State: 7 [!0&!1] 3 {2 5} State: 8 [0&!1] 1 {2} [!0&!1] 2 {2} State:
9 [!0&1] 6 {2 4} --END--""")

a10 = spot.automaton("""
HOA: v1 States: 2 Acceptance: 4 (Fin(0)|Fin(1))&(Fin(2)|Fin(3)) Start: 0
AP: 0 --BODY-- State: 0 [t] 0 {0 2 3} [t] 1 {1} State: 1 [t] 0 {2}
[t] 1 {3 0 1} --END--""")

a11 = spot.automaton("""
HOA: v1 States: 2 Acceptance: 6 (Fin(0)|Fin(1))&(Fin(2)|Fin(3))&
(Fin(4)|Fin(5)) Start: 0 AP: 0 --BODY-- State: 0 [t] 0 {0 2 3} [t]
1 {1 4} State: 1 [t] 0 {2 5} [t] 1 {3 0 1} --END--""")

# From issue #360.
a360 = spot.automaton("""HOA: v1 States: 2 Start: 0 AP: 2 "a"
"b" Acceptance: 8 Fin(5) & (Inf(4) | (Fin(3) & (Inf(2) | (Fin(1) &
Inf(0))))) & (Inf(6) | Inf(7)) & (Fin(6)|Fin(7)) properties: trans-labels
explicit-labels trans-acc complete properties: deterministic --BODY--
State: 0 [0&1] 0 {4 6 7} [0&!1] 1 {0 6} [!0&1] 0 {3 7} [!0&!1] 0 {0}
State: 1 [0&1] 0 {4 6 7} [0&!1] 1 {3 6} [!0&1] 0 {4 7} [!0&!1] 1 {0}
--END--""")


def generic_emptiness2_rec(aut):
    spot.cleanup_acceptance_here(aut, False)
    # Catching 'false' acceptance here is an optimization that could be removed.
    if aut.acc().is_f():
        return True
    # Catching Fin-less acceptance here to use a regular emptiness check is an
    # optimization.  This "if" block could be removed without breaking the
    # algorithm.
    if not aut.acc().uses_fin_acceptance():
        return aut.is_empty()
    si = spot.scc_info(aut, spot.scc_info_options_STOP_ON_ACC)
    acc_scc = si.one_accepting_scc()
    if acc_scc >= 0:
        return False
    nscc = si.scc_count()
    # Now recurse in all non-rejecting SCC
    for scc in range(nscc):
        if not si.is_rejecting_scc(scc):
            acc = aut.acc()
            sets = si.acc_sets_of(scc)
            acc = acc.restrict_to(sets)
            # Do we have any unit Fin?
            fu = acc.fin_unit()
            if fu:
                for part in si.split_on_sets(scc, fu):
                    if not generic_emptiness2(part):
                        return False
            else:
                # Find some Fin set, we necessarily have one, otherwise the SCC
                # would have been found to be either rejecting or accepting.
                fo = acc.fin_one()
                assert fo >= 0, acc
                for part in si.split_on_sets(scc, [fo]):
                    if not generic_emptiness2(part):
                        return False
                whole = si.split_on_sets(scc, [])[0]
                whole.set_acceptance(acc.force_inf([fo]))
                if not generic_emptiness2(whole):
                    return False
    return True

# A very old python version of spot.generic_emptiness_check()


def generic_emptiness2(aut):
    old_a = spot.acc_cond(aut.acc())
    res = generic_emptiness2_rec(aut)
    # Restore the original acceptance condition
    aut.set_acceptance(old_a)
    return res

# A more modern python version of spot.generic_emptiness_check()


def is_empty1(g):
    si = spot.scc_info_with_options(g, spot.scc_info_options_NONE)
    for scc_num in range(si.scc_count()):
        if si.is_trivial(scc_num):
            continue
        if not is_scc_empty1(si, scc_num):
            return False
    return True


def is_scc_empty1(si, scc_num, acc=None):
    if acc is None:  # acceptance isn't forced, get it from the automaton
        acc = si.get_aut().acc()
    occur, common = si.acc_sets_of(scc_num), si.common_sets_of(scc_num)
    acc = acc.restrict_to(occur)
    acc = acc.remove(common, False)
    if acc.is_t():
        return False
    if acc.is_f():
        return True
    if acc.accepting(occur):
        return False
    for cl in acc.top_disjuncts():
        fu = cl.fin_unit()  # Is there Fin at the top level
        if fu:
            with spot.scc_and_mark_filter(si, scc_num, fu) as filt:
                filt.override_acceptance(cl.remove(fu, True))
                if not is_empty1(filt):
                    return False
        else:
            # Pick some Fin term anywhere in the formula
            fo = cl.fin_one()
            # Try to solve assuming Fin(fo)=True
            with spot.scc_and_mark_filter(si, scc_num, [fo]) as filt:
                filt.override_acceptance(cl.remove([fo], True))
                if not is_empty1(filt):
                    return False
            # Try to solve assuming Fin(fo)=False
            if not is_scc_empty1(si, scc_num, acc.force_inf([fo])):
                return False
    return True


def is_empty2(g):
    return is_empty2_rec(spot.scc_and_mark_filter(g, g.acc().fin_unit()))


def is_empty2_rec(g):
    si = spot.scc_info_with_options(g, spot.scc_info_options_STOP_ON_ACC)
    if si.one_accepting_scc() >= 0:
        return False
    for scc_num in range(si.scc_count()):
        if si.is_rejecting_scc(scc_num):  # this includes trivial SCCs
            continue
        if not is_scc_empty2(si, scc_num):
            return False
    return True


def is_scc_empty2(si, scc_num, acc=None):
    if acc is None:  # acceptance isn't forced, get it from the automaton
        acc = si.get_aut().acc()
    occur, common = si.acc_sets_of(scc_num), si.common_sets_of(scc_num)
    acc = acc.restrict_to(occur)
    acc = acc.remove(common, False)
    # 3 stop conditions removed here, because they are caught by
    # one_accepting_scc() or is_rejecting_scc() in is_empty2_rec()
    for cl in acc.top_disjuncts():
        fu = cl.fin_unit()  # Is there Fin at the top level
        if fu:
            with spot.scc_and_mark_filter(si, scc_num, fu) as filt:
                filt.override_acceptance(cl.remove(fu, True))
                if not is_empty2_rec(filt):
                    return False
        else:
            # Pick some Fin term anywhere in the formula
            fo = cl.fin_one()
            # Try to solve assuming Fin(fo)=True
            with spot.scc_and_mark_filter(si, scc_num, [fo]) as filt:
                filt.override_acceptance(cl.remove([fo], True))
                if not is_empty2_rec(filt):
                    return False
            # Try to solve assuming Fin(fo)=False
            if not is_scc_empty2(si, scc_num, acc.force_inf([fo])):
                return False
    return True


def run_bench(automata):
    for aut in automata:
        # Make sure all our implementations behave identically
        res5 = is_empty2(aut)
        res4 = is_empty1(aut)
        spot.generic_emptiness_check_select_version("spot28")
        res3a = spot.generic_emptiness_check(aut)
        spot.generic_emptiness_check_select_version("atva19")
        res3b = spot.generic_emptiness_check(aut)
        spot.generic_emptiness_check_select_version("spot29")
        res3c = spot.generic_emptiness_check(aut)
        res2 = spot.remove_fin(aut).is_empty()
        res1 = generic_emptiness2(aut)
        res = (str(res1)[0] + str(res2)[0] + str(res3a)[0]
               + str(res3b)[0] + str(res3c)[0] + str(res4)[0] + str(res5)[0])
        print(res)
        assert res in ('TTTTTTT', 'FFFFFFF')
        if res == 'FFFFFFF':
            run3 = spot.generic_accepting_run(aut)
            assert run3.replay(spot.get_cout()) is True


run_bench([a1, a2, a3, a4, a5, a6, a7, a8, a9, a10, a11, a360])
