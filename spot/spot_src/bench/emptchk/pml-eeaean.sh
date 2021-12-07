#!/bin/sh
# Copyright (C) 2005  Laboratoire d'Informatique de Paris 6 (LIP6),
# département Systèmes Répartis Coopératifs (SRC), Université Pierre
# et Marie Curie.
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

. ./defs
set -e

FORMULAE=$srcdir/models/eeaean.ltl

opts='-f -x -m'

for model in eeaean1.tgba eeaean2.tgba eeaean1R.tgba eeaean2R.tgba
do
  echo "+++++++++++++++++++++"
  echo "  $model"
  echo "+++++++++++++++++++++"

  cat "$FORMULAE" |
  while read formula; do
    echo "-----------------------------------------------------------"
    echo "###  formula: $formula"
    cat "$ALGORITHMS" |
    while read algo; do
      "$LTL2TGBA" -0 -e"$algo" $opts -Pmodels/$model "$formula"
    done
  done
done
