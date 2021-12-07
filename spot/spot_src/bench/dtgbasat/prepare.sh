#!/bin/sh

ltlfilt=../../bin/ltlfilt
ltl2tgba=../../bin/ltl2tgba
dstar2tgba=../../bin/dstar2tgba

# Rename all formulas using a b c... suppress duplicates.
$ltlfilt --ignore-errors --relabel=abc -u formulas > nodups.ltl

while read f; do
  acc=`$ltl2tgba "$f" --low -a --stats="%a"`
  acc2=`$ltl2tgba "$f" -D --stats="%a"`
  if $ltlfilt -f "$f" --obligation >/dev/null; then
     echo "$f, WDBA, $acc, $acc2"
  elif test `$ltl2tgba "$f" -D --stats="%d"` = 1; then
     echo "$f, trad, $acc, $acc2"
  elif test `$ltl2tgba "$f" -x tba-det -D --stats="%d"` = 1; then
     echo "$f, TCONG, $acc, $acc2"
  elif test `$ltlfilt --remove-wm -f "$f" -l | ltl2dstar --ltl2nba=spin:$ltl2tgba@-Ds - - | $dstar2tgba -D --low --stats="%d"` = 1; then
     echo "$f, DRA, $acc, $acc2"
  else
     echo "$f, not DBA-realizable, $acc, $acc2"
  fi
done < nodups.ltl | tee info.ltl
