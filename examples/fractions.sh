#!/bin/bash
executable="timeout 3600 ../build/src/storm"
arguments=" -i 1000000 --parametric --parametricRegion --region:refinement 0.05 --region:samplemode off"
mkdir fractions
# pdtmcs
$executable -s ./pdtmc/crowds/crowds.pm -const CrowdSize=15,TotalRuns=5 --prop ./pdtmc/crowds/crowds.prctl --region:regionfile ./pdtmc/crowds/crowds_regions.txt $arguments | tee ./fractions/pdtmc_crowds.pm-constCrowdSize_15_TotalRuns_5.log 
$executable -s ./pdtmc/nand/nand.pm -const N=10,K=5 --prop ./pdtmc/nand/nand.prctl --region:regionfile ./pdtmc/nand/nand_regions.txt $arguments | tee ./fractions/pdtmc_nand.pm-constN_10_K_5.log 
$executable -s ./pdtmc/brp_rewards2/brp_rewards2.pm -const N=256,MAX=5 --prop ./pdtmc/brp_rewards2/brp_rewards2.prctl --region:regionfile ./pdtmc/brp_rewards2/brp_rewards2_regions.txt $arguments | tee ./fractions/pdtmc_brp_rewards2.pm-constN_256_MAX_5.log 
$executable -s ./pdtmc/brp_rewards4/brp_rewards4.pm -const N=256,MAX=5 --prop ./pdtmc/brp_rewards4/brp_rewards4.prctl --region:regionfile ./pdtmc/brp_rewards4/brp_rewards4_regions.txt $arguments | tee ./fractions/pdtmc_brp_rewards4.pm-constN_256_MAX_5.log 

# pmdps
$executable -s ./pmdp/brp/brp.pm -const N=256,MAX=5 --prop ./pmdp/brp/brp.prctl --region:regionfile ./pmdp/brp/brp_regions.txt $arguments | tee ./fractions/pmdp_brp.pm-constN_256_MAX_5.log 
$executable -s ./pmdp/coin4/coin4.pm -const K=2 --prop ./pmdp/coin4/coin4.prctl --region:regionfile ./pmdp/coin4/coin4_regions.txt $arguments | tee ./fractions/pmdp_coin4.pm-constK_4.log 
$executable -s ./pmdp/zeroconf/zeroconf.pm -const K=2 --prop ./pmdp/zeroconf/zeroconf.prctl --region:regionfile ./pmdp/zeroconf/zeroconf_regions.txt $arguments | tee ./fractions/pmdp_zeroconf.pm-constK_5.log 
$executable -s ./pmdp/reporter4/reporter4.pm -const Xsize=6,Ysize=6,MAXTRIES=2,B=2 --prop ./pmdp/reporter4/reporter4.prctl --region:regionfile ./pmdp/reporter4/reporter4_regions.txt $arguments | tee ./fractions/pmdp_reporter4.pm-constXsize_6_Ysize_6_MAXTRIES_2_B_2.log 
wait

echo "done"
