#!/bin/sh
RANDLTL=../../bin/randltl
LTLFILT=../../bin/ltlfilt
LTLDO=../../bin/ltldo

set -e -x

(
must_exit=false
echo ap,algo,time,matched,exit.status
for ap in 1 2 3; do
    $RANDLTL $ap --tree-size=..30 -n -1 | $LTLFILT --ap=$ap | $LTLFILT -v --nox -n 500 > formulas
    for algo in 1 2 3 4 5 6 7 8 0 9; do
	es=0
	SPOT_STUTTER_CHECK=$algo /usr/bin/time -o user-$ap-$algo.csv -f "$ap,$algo,%e" $LTLFILT --stutter-invariant formulas > matched-$ap-$algo.ltl || must_exit=true es=$?
	matched=`wc -l < matched-$ap-$algo.ltl`
	csv=`tail -n 1 user-$ap-$algo.csv`
	echo $csv,$matched,$es
	rm -f user-$ap-$algo.csv
	$must_exit && exit 0
    done
done
) > ltl-user-bench.csv
