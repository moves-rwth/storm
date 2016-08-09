#!/bin/bash

if [ "$#" != 1 ];
then
	echo "Wrong number of arguments! Provide a filename for the results!"
elif [ -a $1 ]; then
	echo "File for results already exists!"
else


DIR="."

echo '#!/bin/bash' >> $1
echo 'executable="timeout 3600 ../build/src/storm"' >> $1
echo 'arguments="-bisim -i 1000000 --parametric --parametricRegion --region:refinement 0.05 --region:samplemode off"' >> $1
echo "mkdir results" >> $1


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
        echo "# $modeltype""s" >> $1
	for model in "${models[@]}"
	do
		modelfolder="$DIR/$modeltype/$model"
		suffix="-"
		while read instance;
		do
		
			output='$executable '
			output="$output""-s $modelfolder/$instance --prop $modelfolder/$model.prctl --region:regions "
			region=$(head -n 1 $modelfolder/$model"_space.txt")
			region="$(echo -e "${region}" | tr -d '[[:space:]]')"
			output="$output"'"'$region'" $arguments | tee '
			instanceString="$(echo -e "${instance}" | tr -d '[[:space:]]')"
                        instanceString=${instanceString//[,=]/_}
			output="$output""./results/$modeltype""_$instanceString.log &"
			echo $output >> $1

		done < "$modelfolder/models"
		
	done
done
echo 'wait' >> $1
fi
