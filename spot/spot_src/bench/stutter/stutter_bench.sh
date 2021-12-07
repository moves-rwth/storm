#!/bin/sh

RANDLTL=../../bin/randltl
LTLFILT=../../bin/ltlfilt

echo 'ltl-user-bench.csv:; ./user.sh' > run.mk

pos="pos.states,pos.trans,pos.edges,pos.scc,pos.nondet"
neg="neg.states,neg.trans,neg.edges,neg.scc,neg.nondet"
algos="time1,time2,time3,time4,time5,time6,time7,time8"

OUTPUTF=bench_formulas.csv
(
all=
for ap in 1 2 3 4; do
    out=ltl-ap$ap.csv
    echo "$out:; $RANDLTL $ap --tree-size=..30 -n -1 | $LTLFILT --ap=$ap | $LTLFILT -v --nox -n 10000 | ./stutter_invariance_formulas -F- > \$@"
    all="$all $out"
done

echo "$OUTPUTF:$all; (echo 'formula,ap,$pos,$neg,$algos,res'; cat $all) > \$@"
) >> run.mk

OUTPUTG=bench_randgraph.csv
(
all=
for d in 0.0 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1.0; do
    for ap in 1 2 3 4; do
	out=graph-d$d-ap$ap.csv
	echo "$out:; ./stutter_invariance_randomgraph $d $ap > \$@"
	all="$all $out"
    done
done
echo "$OUTPUTG:$all; (echo 'd,ap,seed,$pos,$neg,$algos,res'; cat $all) > \$@"
) >> run.mk


make "$@" -f run.mk $OUTPUTF $OUTPUTG
# Do this one separately from the rest, because it will eat the memory
# and will either die from out-of-memory, or from needing more that 32
# acceptance sets.
make "$@" -f run.mk ltl-user-bench.csv
