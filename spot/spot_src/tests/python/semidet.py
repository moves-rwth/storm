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

import spot

formulas = [('(Gp0 | Fp1) M 1', False, True),
            ('(!p1 U p1) U X(!p0 -> Fp1)', False, True),
            ('(p1 | (Fp0 R (p1 W p0))) M 1', True, True),
            ('!G(F(p1 & Fp0) W p1)', False, True),
            ('X(!p0 W Xp1)', False, False),
            ('FG(p0)', False, True)]

for f, isd, issd in formulas:
    print(f)
    aut = spot.translate(f)
    # The formula with isd=True, issd=True is the only one
    # for which both properties are already set.
    assert (aut.prop_deterministic().is_maybe() or
            aut.prop_semi_deterministic().is_maybe() or
            isd == issd)
    spot.check_determinism(aut)
    assert aut.prop_deterministic() == isd
    assert aut.prop_semi_deterministic() == issd
