# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2010, 2012, 2014, 2015, 2016 Laboratoire de Recherche et
# Développement de l'EPITA.
# Copyright (C) 2003, 2004 Laboratoire d'Informatique de Paris 6
# (LIP6), département Systèmes Répartis Coopératifs (SRC), Université
# Pierre et Marie Curie.
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

# Make sure that interdependencies between the spot and buddy wrappers
# are not problematic.
import buddy
import spot
import sys

simp = spot.tl_simplifier()

e = spot.default_environment.instance()
pf = spot.parse_infix_psl('GFa', e)
d = simp.get_dict()
a = spot.ltl_to_tgba_fm(pf.f, d)
g = spot.parse_infix_boolean('b&c', e)
b = simp.as_bdd(g.f)
buddy.bdd_printset(b)
spot.nl_cout()
del g

s0 = a.get_init_state()
it = a.succ_iter(s0)
it.first()
while not it.done():
    c = it.cond()
    sys.stdout.write("%s\n" % c)
    b &= c  # `&=' is defined only in buddy.  So if this statement works
    # it means buddy can grok spot's objects.
    buddy.bdd_printset(c)
    spot.nl_cout()
    it.next()
buddy.bdd_printset(b)
spot.nl_cout()
sys.stdout.write("%s\n" % b)
del it
del s0
del b
del c
del pf
del simp
