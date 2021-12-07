# -*- mode: python; coding: utf-8 -*-
# Copyright (C) 2019 Laboratoire de Recherche et Développement de l'Epita
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

# Make sure we can leep track of BDD association in Python using bdd_dict, as
# discussed in issue #372.


import spot


class bdd_holder:
    def __init__(self, aut):
        self.bdddict = d = aut.get_dict()
        for ap in aut.ap():
            d.register_proposition(ap, self)

    def __del__(self):
        self.bdddict.unregister_all_my_variables(self)


class bdd_holder2:
    def __init__(self, aut):
        self.bdddict = d = aut.get_dict()
        d.register_all_variables_of(aut, self)

    def __del__(self):
        self.bdddict.unregister_all_my_variables(self)


class bdd_holder3:
    def __init__(self, h2):
        self.bdddict = d = h2.bdddict
        d.register_all_variables_of(h2, self)

    def __del__(self):
        self.bdddict.unregister_all_my_variables(self)


def check_ok():
    assert type(bdict.varnum(spot.formula.ap("a"))) is int


def check_nok():
    try:
        bdict.varnum(spot.formula.ap("a"))
    except IndexError as e:
        pass
    else:
        raise RuntimeError("missing exception")


def debug(txt):
    # print(txt)
    # bdict.dump(spot.get_cout())
    pass


aut = spot.translate("a U b")
bdict = aut.get_dict()
debug("aut")

word = aut.accepting_word()
debug("word")
check_ok()

h = bdd_holder(aut)
debug("h")
check_ok()

h2 = bdd_holder2(aut)
debug("h2")
check_ok()

del aut
debug("-aut")
check_ok()

del word
debug("-word")
check_ok()

del h
debug("-h")
check_ok()

del h2
debug("-h2")
check_nok()

h2 = bdd_holder2(spot.translate("a U b").accepting_word())
debug("h2")
h3 = bdd_holder3(h2)
var = bdict.register_anonymous_variables(1, h3)
debug("h3")
assert var == 2
del h2
debug("-h2")
check_ok()
del h3
debug("-h3")
check_nok()

h2 = bdd_holder2(spot.translate("a U b").accepting_word())
debug("h2")
bdict.unregister_variable(bdict.varnum("b"), h2)
bdict.unregister_variable(bdict.varnum("a"), h2)
debug("-b,-a")
check_nok()
