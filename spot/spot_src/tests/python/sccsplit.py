#!/usr/bin/python3
# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2018 Laboratoire de Recherche et Développement de
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

aut = spot.translate('GF(a <-> Xa) & GF(b <-> XXb)')
si = spot.scc_info(aut)
s = ""
for aut2 in si.split_on_sets(0, [0]):
    # This call to to_str() used to fail because split_on_sets had not
    # registered the atomic propositions of aut
    s += aut2.to_str()
assert spot.automaton(s).num_states() == 8
