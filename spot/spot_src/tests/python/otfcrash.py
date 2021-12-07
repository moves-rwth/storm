# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2016, 2018  Laboratoire de Recherche et Développement
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
import spot.ltsmin
import spot.aux
import tempfile
import shutil
import sys

spot.ltsmin.require('divine')

# the test case actually starts here
with tempfile.NamedTemporaryFile(dir='.', suffix='.dve') as fp:
    fp.write(b"""int f = 3;
process R {
  int p = 1, found = 0;
  state i, e;
  init i;
  trans
    i -> i {guard p != f; effect p = p + 1;},
    i -> e {guard p == f; effect found = 1;},
    e -> e {};
}
system async;
""")
    fp.flush()
    m = spot.ltsmin.load(fp.name)
    spot.aux.rm_f(fp.name + '.cpp')
    spot.aux.rm_f(fp.name + '2C')

    def modelcheck(formula, model):
        a = spot.translate(formula)
        k = m.kripke([ap.ap_name() for ap in a.ap()])
        p = spot.otf_product(k, a)
        return p.is_empty()

    assert(modelcheck('X "R.found"', m) == True)
