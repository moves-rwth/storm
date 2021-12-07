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

opts="-1 -D -e 15 -n 1024 -t 0.5 -r -z -i $FORMULAE"

echo "WITHOUT ADDITIONAL ACCEPTANCE CONDITIONS"

for d in 0.001 0.002 0.01; do
  echo "density: $d"
  $RANDTGBA -A "$ALGORITHMS" -d $d $opts
done

echo "WITH 3 ADDITIONAL ACCEPTANCE CONDITIONS"

for d in 0.001 0.002 0.01; do
  echo "density: $d"
  $RANDTGBA -A "$ALGORITHMS" -a 3 0.0133333 -d $d $opts
done
