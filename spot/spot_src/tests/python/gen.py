# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2017 Laboratoire de Recherche et Développement de
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

# Test that the spot.gen package works, in particular, we want
# to make sure that the objects created from spot.gen methods
# are usable with methods from the spot package.

import spot.gen as gen
from sys import exit

k2 = gen.aut_pattern(gen.AUT_KS_NCA, 2)
assert k2.prop_state_acc()
assert k2.num_states() == 5
assert k2.prop_universal().is_false()
assert k2.prop_inherently_weak().is_false()
assert k2.prop_stutter_invariant().is_false()
assert k2.prop_semi_deterministic().is_false()
assert k2.prop_deterministic().is_false()
assert k2.prop_terminal().is_false()

# to_str is defined in the spot package, so this makes sure
# the type returned by spot.gen.ks_nca() is the correct one.
assert 'to_str' in dir(k2)

k3 = gen.aut_pattern(gen.AUT_L_NBA, 3)
assert k3.num_states() == 10
assert k3.prop_state_acc()
assert k3.prop_universal().is_false()
assert k3.prop_inherently_weak().is_false()
assert k3.prop_stutter_invariant().is_false()
assert k3.prop_semi_deterministic().is_false()
assert k3.prop_deterministic().is_false()
assert k3.prop_terminal().is_false()

assert k2.get_dict() == k3.get_dict()

try:
    gen.aut_pattern(gen.AUT_KS_NCA, 0)
except RuntimeError as e:
    assert 'positive argument' in str(e)
else:
    exit(2)

f = gen.ltl_pattern(gen.LTL_AND_F, 3)
assert f.size() == 3
assert gen.ltl_pattern_name(gen.LTL_AND_F) == "and-f"

try:
    gen.ltl_pattern(1000, 3)
except RuntimeError as e:
    assert 'unsupported pattern' in str(e)
else:
    exit(2)

try:
    gen.ltl_pattern(gen.LTL_OR_G, -10)
except RuntimeError as e:
    assert 'or-g' in str(e)
    assert 'positive' in str(e)
else:
    exit(2)

assert 40 == sum(p.size() for p in gen.ltl_patterns((gen.LTL_OR_G, 1, 5),
                                                    (gen.LTL_GH_Q, 3),
                                                    gen.LTL_EH_PATTERNS))

assert 32 == sum(p.num_states()
                 for p in gen.aut_patterns((gen.AUT_L_NBA, 1, 3),
                                           (gen.AUT_KS_NCA, 5)))
