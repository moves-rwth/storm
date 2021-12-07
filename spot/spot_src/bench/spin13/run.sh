#!/bin/sh
# -*- coding: utf-8 -*-
# Copyright (C) 2013, 2015 Laboratoire de Recherche et Développement
# de l'Epita (LRDE).
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

# This file produces a makefile (called run.mk) that runs all the
# configurations for our benchmarks possibly in parallel (using make
# -j8, for instance).  The makefile in turn calls this script to run
# ltlcross, because the configuration of ltlcross is easier done in
# this shell script.  We distinguish between these two uses of run.sh
# with the number of arguments: when no argument is given, we build a
# Makefile; when two arguments are given, we run ltlcross, reading
# formulas from standard input.

set -e

ltl2tgba=../../src/bin/ltl2tgba
ltlfilt=../../src/bin/ltlfilt
ltlcross=../../src/bin/ltlcross


# If two arguments are given, they should be
#   ./run.sh  formula.ltl  prefix
# where prefix is "tgba-" or "ba-".
# The first argument is only used to name the resulting
# files and to decide if we should disable ltlcross
# check routines in order to check only positive or
# negative formulas.
if test $# = 2; then
    f=$1
    prefix=$2
    case $2 in
	tgba-)
	    opt=' --lbtt>%T'
	    ba=false
	    D=
	    d=
	    DB=
	    ;;
	ba-)
	    opt=' --spin>%N'
	    ba=:
	    D=D
	    d=d
	    DB=DB
	    ;;
    esac

    opt="$opt --high"
    nosim='-x !simul,!ba-simul'

    set x

    # Deter

    $ba && set "$@" \
	": LTL3BA NoSim NoSusp Deter; ltl3ba -S0 -A -M1 -f %s >%N" \
	": LTL3BA Sim NoSusp Deter; ltl3ba -S2 -A -M1 -f %s >%N" \
	": Cou99 '(Ad)' Deter; $ltl2tgba $opt $nosim --deter -x !degen-reset,!degen-lcache %f"
    set "$@" \
	": Cou99 '(A${D})' Deter; $ltl2tgba $opt $nosim --deter %f"
    $ba && set "$@" \
	": Cou99 '(ASd)' Deter; $ltl2tgba $opt -x !ba-simul,!degen-reset,!degen-lcache --deter %f" \
	": Cou99 '(ASD)' Deter; $ltl2tgba $opt -x !ba-simul --deter %f"
    set "$@" \
	": Cou99 '(AS${DB})' Deter; $ltl2tgba $opt --deter %f"
    $ba && set "$@" \
	": Cou99 '(ASdB)' Deter; $ltl2tgba $opt --deter -x !degen-reset,!degen-lcache %f" \
	": LTL3BA NoSim Susp Deter; ltl3ba -S0 -M1 -f %s >%N" \
	": LTL3BA Sim Susp Deter; ltl3ba -S2 -M1 -f %s >%N"
    set "$@" \
	": Comp '(A${D})' Deter; $ltl2tgba -x comp-susp,!skel-simul $opt $nosim --deter %f" \
	": Comp '(A${D})' Deter Early; $ltl2tgba -x comp-susp,!skel-simul,early-susp $opt $nosim --deter %f"
    $ba && set "$@" \
	": Comp '(ASD)' Deter; $ltl2tgba -x comp-susp,!ba-simul $opt --deter %f" \
	": Comp '(ASD)' Deter Early; $ltl2tgba -x comp-susp,early-susp,!ba-simul $opt --deter %f"
    set "$@" \
	": Comp '(AS${DB})' Deter; $ltl2tgba -x comp-susp $opt --deter %f" \
	": Comp '(AS${DB})' Deter Early; $ltl2tgba -x comp-susp,early-susp $opt --deter %f"

    # Small

    $ba && set "$@" \
	": LTL3BA NoSim NoSusp; ltl3ba -S0 -M0 -A -f %s >%N" \
	": LTL3BA Sim NoSusp; ltl3ba -S2 -M0 -A -f %s >%N" \
	": Cou99 '(Ad)' Small; $ltl2tgba $opt $nosim --small -x !degen-reset,!degen-lcache %f"
    set "$@" \
	": Cou99 '(A${D})' Small; $ltl2tgba $opt $nosim --small %f"
    $ba && set "$@" \
	": Cou99 '(ASd)' Small; $ltl2tgba $opt -x !ba-simul,!degen-reset,!degen-lcache --small %f" \
	": Cou99 '(ASD)' Small; $ltl2tgba $opt -x !ba-simul --small %f"
    set "$@" \
	": Cou99 '(AS${DB})' Small; $ltl2tgba $opt --small %f"
    $ba && set "$@" \
	": Cou99 '(ASdB)' Small; $ltl2tgba $opt --small -x !degen-reset,!degen-lcache %f" \
	": LTL3BA NoSim Susp; ltl3ba -S0 -M0 -f %s >%N" \
	": LTL3BA Sim Susp; ltl3ba -S2 -M0 -f %s >%N"
    set "$@" \
	": Comp '(A${D})' Small; $ltl2tgba -x comp-susp,!skel-wdba,!skel-simul $opt $nosim --small %f" \
	": Comp '(A${D})' Small Early; $ltl2tgba -x comp-susp,!skel-wdba,!skel-simul,early-susp $opt $nosim --small %f"
    $ba && set "$@" \
	": Comp '(ASD)' Small; $ltl2tgba -x comp-susp,!skel-wdba,!ba-simul $opt --small %f" \
	": Comp '(ASD)' Small Early; $ltl2tgba -x comp-susp,!no-skel-wdba,early-susp,!ba-simul $opt --small %f"
    set "$@" \
	": Comp '(AS${DB})' Small; $ltl2tgba -x comp-susp,!skel-wdba $opt --small %f" \
	": Comp '(AS${DB})' Small Early; $ltl2tgba -x comp-susp,!skel-wdba,early-susp $opt --small %f"

    # If the formulas are only positive or negative, disable checks so
    # that ltlcross does not process the negation as well.
    case $f in
	*pos*|*neg*) set "$@" --no-check;;
    esac

    set "$@" --timeout=1200
    shift

    $ltlcross "$@" --json=$prefix$f.json --csv=$prefix$f.csv 2>$prefix$f.log

    # Edit the JSON file to keep only the part of the name between ':' and ';',
    # it's much more readable than the list of options...
    perl -pi -e 's/":\s*(.*)\s*;.*"/"$1 "/' $prefix$f.json

    exit $?
fi

if test $# -ne 0; then
    echo "Wrong number of arguments." >&2
    exit 1;
fi

# If no argument have been supplied, build a Makefile.

cat >run.mk <<\EOF
.PHONY: all _all
all: _all

.SUFFIXES: .json .html .tex .pdf
.json.html:
	cat html.top $< html.bottom > $@.tmp
	mv $@.tmp $@
.tex.pdf:
	latexmk --pdf $<
EOF
all=
for prefix in tgba- ba-; do
    presults=
    for f in known.ltl; do
	cat >>run.mk <<EOF
$prefix$f.stamp: $f $ltlcross $ltl2tgba
	./run.sh $f $prefix <$f && touch \$@

$prefix$f.json $prefix$f.csv $prefix$f.log: $prefix$f.stamp
	@:
EOF
	presults="$presults $prefix$f.json"
    done

    for f in new.ltl new-fair2.ltl new-weak3.ltl new-strong1.ltl new-strong2.ltl; do

	cat >>run.mk <<EOF
$prefix$f.pos.stamp: $f $ltlcross $ltl2tgba
	./run.sh $f.pos $prefix <$f && touch \$@

$prefix$f.pos.json $prefix$f.pos.csv $prefix$f.pos.log: $prefix$f.pos.stamp
	@:

$prefix$f.neg.stamp: $f $ltlcross $ltl2tgba
	$ltlfilt --negate -F $f | ./run.sh $f.neg $prefix && touch \$@

$prefix$f.neg.json $prefix$f.neg.csv $prefix$f.neg.log: $prefix$f.neg.stamp
	@:

EOF
	presults="$presults $prefix$f.pos.json $prefix$f.neg.json"
   done

   v=`git describe --always --dirty 2>/dev/null ||
      ../../config.status --version | head -n 1 | cut -d' ' -f 3`
   h=`hostname -f`

   cat >>run.mk <<EOF
${prefix}sum.tex: $presults
	../ltl2tgba/sum.py --intro "Version: $v; Date: `date +%Y-%m-%d`; Host: $h." \\
	$presults >${prefix}sum.tex

EOF

    log=`echo "$presults" | sed 's/json/log/g'`
    csv=`echo "$presults" | sed 's/json/csv/g'`
    html=`echo "$presults" | sed 's/json/html/g'`

    all="$all ${prefix}sum.pdf ${prefix}sum.tex $results $csv $log $html"
done

arch=`date +%Y-%m-%d`.tar.xz
cat >>run.mk <<EOF
_all: $arch

FILES = $all
$arch: \$(FILES)
	tar Jcvf \$@ \$(FILES)
	@echo === \$@ ready ===
EOF

echo 'Now run "make -f run.mk" or preferably "make -f run.mk -j8".'
echo '(Adjusting -j8 to the number of processors on your machine.)'
