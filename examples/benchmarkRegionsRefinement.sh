#!/bin/bash

if [ "$#" != 1 ];
then
	echo "Wrong number of arguments! Provide a filename for the results!"
elif [ -a $1 ]; then
	echo "File for results already exists!"
else


DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

executable=$DIR/../build/src/storm

timeout="timeout 3600"

declare -a modeltypes=("pdtmc" "pmdp")

for modeltype in "${modeltypes[@]}"
do
if [ "$modeltype" == "pdtmc" ];
then	
	declare -a models=("crowds" "nand" "brp_rewards2" "brp_rewards4" "brp")
	dobisim="-bisim"
else
	declare -a models=("brp" "coin2" "coin4" "zeroconf" "reporter2" "reporter4")
	dobisim=""
fi
	for model in "${models[@]}"
	do
		modelfolder="$DIR/$modeltype/$model"
		suffix="-"
		while read instance;
		do
			suffix="1$suffix"
			echo "Working on $modelfolder/$instance"
			echo "___WORKING ON $modeltype: $instance""____________" >>$1$suffix
			echo "_________________________________________________________________________________" >> $1$suffix
			$timeout "$executable" -s $modelfolder/$instance $dobisim --prop $modelfolder/$model.prctl --parametric --parametricRegion --region:regionfile $modelfolder/$model"_space.txt" --region:refinement 0.05 --region:samplemode off >> "$1$suffix" &
		done < "$modelfolder/models"
		wait
		# write logs into result file
   		suffix="-"
		while read instance;
		do
			suffix="1$suffix"
			cat $1$suffix >> $1
			rm $1$suffix
		done < "$modelfolder/models"
	done
done
fi
