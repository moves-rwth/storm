# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2016 Laboratoire de Recherche et Développement de
# l'Epita
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

lcc = spot.language_containment_checker()

formulas = ['GFa', 'FGa', '(GFa) U b',
            '(a U b) U c', 'a U (b U c)',
            '(a W b) W c', 'a W (b W c)',
            '(a R b) R c', 'a R (b R c)',
            '(a M b) M c', 'a M (b M c)',
            '(a R b) U c', 'a U (b R c)',
            '(a M b) W c', 'a W (b M c)',
            '(a U b) R c', 'a R (b U c)',
            '(a W b) M c', 'a M (b W c)',
            ]

# The rewriting assume the atomic proposition will not change
# once we reache the non-alive part.
cst = spot.formula('G(X!alive => ((a <=> Xa) && (b <=> Xb) && (c <=> Xc)))')

for f in formulas:
    f1 = spot.formula(f)
    f2 = f1.unabbreviate()
    f3 = spot.formula_And([spot.from_ltlf(f1), cst])
    f4 = spot.formula_And([spot.from_ltlf(f2), cst])
    print("{}\t=>\t{}".format(f1, f3))
    print("{}\t=>\t{}".format(f2, f4))
    assert lcc.equal(f3, f4)
    print()
