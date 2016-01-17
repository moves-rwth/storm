#!/bin/bash

if [ "$#" != 1 ];
then
	echo "Wrong number of arguments! Provide a filename for the results!"
elif [ -a $1 ]; then
	echo "File for results already exists!"
else
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
executable=$DIR/../build/src/storm

declare -a modeltypes=("pdtmc" "pmdp")

for modeltype in "${modeltypes[@]}"
do
if [ "$modeltype" == "pdtmc" ];
then	
#	declare -a models=("crowds" "nand" "brp_rewards2" "brp_rewards4" "brp")
	dobisim="-bisim"
else
#	declare -a models=("brp" "coin2" "coin4" "zeroconf" "reporter2" "reporter4")
	declare -a models=( "zeroconf" "reporter2" "reporter4")
	dobisim=""
fi
	for model in "${models[@]}"
	do
		modelfolder="$DIR/$modeltype/$model"
		while read instance;
		do
		
		echo "Working on $modelfolder/$instance"
		echo >> $1
		echo "-------------------------------------------------------------" >> $1
		echo "---- WORKING ON: $modelfolder/$instance ----" >>$1
		echo "-------------------------------------------------------------" >> $1
		"$executable" -s $modelfolder/$instance $dobisim --prop $modelfolder/$model.prctl --parametric --parametricRegion --region:regionfile $modelfolder/$model"_regions.txt" >> $1
		done < "$modelfolder/models"

	done
done
fi
