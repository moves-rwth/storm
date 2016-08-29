#!/bin/bash
executable="timeout 3600 ../build/src/storm"
arguments="-i 1000000 --parametric --parametricRegion --region:samplemode off"
mkdir plotresults

declare -a brpPars=("16" "128" "256" "384" "512" "640" "768" "896" "1024" "1152" "1280" "1408" "1536" "1664" "1792" "1920" "2048" "2176" "2304" "2432" "2560" "2688" "2816" "2944" "3072" "3200" "3328" "3456" "3584" "3712" "3840" "3968" "4096")


declare -a repPars=("3" "6" "9" "12" "15" "18" "21" "24" "27" "30" "33" "36" "39" "42" "45" "48" "51" "54" "57" "60" "63" "66" "69" "72" "75" "78" "81" "84" "87" "90" "93" "96" "99" "100")

 



plot_brp2 () {
for brp2par in "${brpPars[@]}"
do
$executable -s ./pdtmc/brp_rewards2/brp_rewards2.pm -const N="$brp2par",MAX=5 --prop ./pdtmc/brp_rewards2/brp_rewards2.prctl --region:regionfile ./pdtmc/brp_rewards2/brp_rewards2_regions.txt $arguments | tee -a ./plotresults/pdtmc_brp_rewards2.log 
done
}

plot_brp4 () {
for brp4par in "${brpPars[@]}"
do
$executable -s ./pdtmc/brp_rewards4/brp_rewards4.pm -const N="$brp4par",MAX=5 --prop ./pdtmc/brp_rewards4/brp_rewards4.prctl --region:regionfile ./pdtmc/brp_rewards4/brp_rewards4_regions.txt  $arguments | tee -a ./plotresults/pdtmc_brp_rewards4.log
done
}


plot_rep2 () {
for rep2par in "${repPars[@]}"
do
$executable -s ./pmdp/reporter2/reporter2.pm -const Xsize="$rep2par",Ysize="$rep2par",MAXTRIES=2,B=2 --prop ./pmdp/reporter2/reporter2.prctl --region:regionfile ./pmdp/reporter2/reporter2_regions.txt $arguments | tee -a ./plotresults/pmdp_reporter2.log
done
}


plot_rep4 () {
for rep4par in "${repPars[@]}"
do
$executable -s ./pmdp/reporter4/reporter4.pm -const Xsize="$rep4par",Ysize="$rep4par",MAXTRIES=2,B=2 --prop ./pmdp/reporter4/reporter4.prctl --region:regionfile ./pmdp/reporter4/reporter4_regions.txt $arguments | tee -a ./plotresults/pmdp_reporter4.log
done
}

plot_brp2 &
plot_brp4 &
plot_rep2 &
plot_rep4 &
wait

echo "done!"
