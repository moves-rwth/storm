#!/bin/bash
executable="timeout 3600 ../build/src/storm"
arguments="-bisim -i 1000000 --parametric --parametricRegion --region:refinement 0.05 --region:samplemode off"
mkdir results
# pdtmcs
$executable -s ./pdtmc/crowds/crowds.pm -const CrowdSize=10,TotalRuns=5 --prop ./pdtmc/crowds/crowds.prctl --region:regions "0.000010<=PF<=0.999990,0.000010<=badC<=0.999990;" $arguments | tee ./results/pdtmc_crowds.pm-constCrowdSize_10_TotalRuns_5.log &
$executable -s ./pdtmc/crowds/crowds.pm -const CrowdSize=20,TotalRuns=10 --prop ./pdtmc/crowds/crowds.prctl --region:regions "0.000010<=PF<=0.999990,0.000010<=badC<=0.999990;" $arguments | tee ./results/pdtmc_crowds.pm-constCrowdSize_20_TotalRuns_10.log &


$executable -s ./pdtmc/nand/nand.pm -const N=10,K=5 --prop ./pdtmc/nand/nand.prctl --region:regions "0.000010<=perr<=0.999990,0.000010<=prob1<=0.999990;" $arguments | tee ./results/pdtmc_nand.pm-constN_10_K_5.log &
$executable -s ./pdtmc/nand/nand.pm -const N=25,K=5 --prop ./pdtmc/nand/nand.prctl --region:regions "0.000010<=perr<=0.999990,0.000010<=prob1<=0.999990;" $arguments | tee ./results/pdtmc_nand.pm-constN_25_K_5.log &

wait

$executable -s ./pdtmc/brp_rewards2/brp_rewards2.pm -const N=512,MAX=5 --prop ./pdtmc/brp_rewards2/brp_rewards2.prctl --region:regions "0.000010<=pL<=0.999990,0.000010<=pK<=0.999990;" $arguments | tee ./results/pdtmc_brp_rewards2.pm-constN_512_MAX_5.log &
$executable -s ./pdtmc/brp_rewards2/brp_rewards2.pm -const N=4096,MAX=5 --prop ./pdtmc/brp_rewards2/brp_rewards2.prctl --region:regions "0.000010<=pL<=0.999990,0.000010<=pK<=0.999990;" $arguments | tee ./results/pdtmc_brp_rewards2.pm-constN_4096_MAX_5.log &


$executable -s ./pdtmc/brp_rewards4/brp_rewards4.pm -const N=256,MAX=5 --prop ./pdtmc/brp_rewards4/brp_rewards4.prctl --region:regions "0.000010<=pL<=0.999990,0.000010<=pK<=0.999990,0.000010<=TOMsg<=0.999990,0.000010<=TOAck<=0.999990;" $arguments | tee ./results/pdtmc_brp_rewards4.pm-constN_256_MAX_5.log &
$executable -s ./pdtmc/brp_rewards4/brp_rewards4.pm -const N=5012,MAX=5 --prop ./pdtmc/brp_rewards4/brp_rewards4.prctl --region:regions "0.000010<=pL<=0.999990,0.000010<=pK<=0.999990,0.000010<=TOMsg<=0.999990,0.000010<=TOAck<=0.999990;" $arguments | tee ./results/pdtmc_brp_rewards4.pm-constN_5012_MAX_5.log &

$executable -s ./pdtmc/brp/brp.pm -const N=256,MAX=5 --prop ./pdtmc/brp/brp.prctl --region:regions "0.000010<=pL<=0.999990,0.000010<=pK<=0.999990;" $arguments | tee ./results/pdtmc_brp.pm-constN_256_MAX_5.log &
$executable -s ./pdtmc/brp/brp.pm -const N=4096,MAX=5 --prop ./pdtmc/brp/brp.prctl --region:regions "0.000010<=pL<=0.999990,0.000010<=pK<=0.999990;" $arguments | tee ./results/pdtmc_brp.pm-constN_4096_MAX_5.log &

wait

# pmdps
arguments="-i 1000000 --parametric --parametricRegion --region:refinement 0.05 --region:samplemode off"

$executable -s ./pmdp/brp/brp.pm -const N=256,MAX=5 --prop ./pmdp/brp/brp.prctl --region:regions "0.000010<=pL<=0.999990,0.000010<=pK<=0.999990;" $arguments | tee ./results/pmdp_brp.pm-constN_256_MAX_5.log &
$executable -s ./pmdp/brp/brp.pm -const N=4096,MAX=5 --prop ./pmdp/brp/brp.prctl --region:regions "0.000010<=pL<=0.999990,0.000010<=pK<=0.999990;" $arguments | tee ./results/pmdp_brp.pm-constN_4096_MAX_5.log &

$executable -s ./pmdp/coin2/coin2.pm -const K=2 --prop ./pmdp/coin2/coin2.prctl --region:regions "0.000010<=p1<=0.999990,0.000010<=p2<=0.999990;" $arguments | tee ./results/pmdp_coin2.pm-constK_2.log &
$executable -s ./pmdp/coin2/coin2.pm -const K=32 --prop ./pmdp/coin2/coin2.prctl --region:regions "0.000010<=p1<=0.999990,0.000010<=p2<=0.999990;" $arguments | tee ./results/pmdp_coin2.pm-constK_32.log &

$executable -s ./pmdp/coin4/coin4.pm -const K=2 --prop ./pmdp/coin4/coin4.prctl --region:regions "0.000010<=p1<=0.999990,0.000010<=p2<=0.999990,0.000010<=p3<=0.999990,0.000010<=p4<=0.999990;" $arguments | tee ./results/pmdp_coin4.pm-constK_2.log &
$executable -s ./pmdp/coin4/coin4.pm -const K=4 --prop ./pmdp/coin4/coin4.prctl --region:regions "0.000010<=p1<=0.999990,0.000010<=p2<=0.999990,0.000010<=p3<=0.999990,0.000010<=p4<=0.999990;" $arguments | tee ./results/pmdp_coin4.pm-constK_4.log &

wait

$executable -s ./pmdp/zeroconf/zeroconf.pm -const K=2 --prop ./pmdp/zeroconf/zeroconf.prctl --region:regions "0.000010<=loss<=0.999990,0.000010<=old<=0.999990;" $arguments | tee ./results/pmdp_zeroconf.pm-constK_2.log &
$executable -s ./pmdp/zeroconf/zeroconf.pm -const K=5 --prop ./pmdp/zeroconf/zeroconf.prctl --region:regions "0.000010<=loss<=0.999990,0.000010<=old<=0.999990;" $arguments | tee ./results/pmdp_zeroconf.pm-constK_5.log &

$executable -s ./pmdp/reporter2/reporter2.pm -const Xsize=6,Ysize=6,MAXTRIES=2,B=2 --prop ./pmdp/reporter2/reporter2.prctl --region:regions "0.000010<=pL<=0.999990,0.000010<=pH<=0.999990;" $arguments | tee ./results/pmdp_reporter2.pm-constXsize_6_Ysize_6_MAXTRIES_2_B_2.log &
$executable -s ./pmdp/reporter2/reporter2.pm -const Xsize=100,Ysize=100,MAXTRIES=10,B=10 --prop ./pmdp/reporter2/reporter2.prctl --region:regions "0.000010<=pL<=0.999990,0.000010<=pH<=0.999990;" $arguments | tee ./results/pmdp_reporter2.pm-constXsize_100_Ysize_100_MAXTRIES_10_B_10.log &

$executable -s ./pmdp/reporter4/reporter4.pm -const Xsize=6,Ysize=6,MAXTRIES=2,B=2 --prop ./pmdp/reporter4/reporter4.prctl --region:regions "0.000010<=pL<=0.999990,0.000010<=pH<=0.999990,0.000010<=pLDiff<=0.999990,0.000010<=pHDiff<=0.999990;" $arguments | tee ./results/pmdp_reporter4.pm-constXsize_6_Ysize_6_MAXTRIES_2_B_2.log &
$executable -s ./pmdp/reporter4/reporter4.pm -const Xsize=10,Ysize=10,MAXTRIES=3,B=3 --prop ./pmdp/reporter4/reporter4.prctl --region:regions "0.000010<=pL<=0.999990,0.000010<=pH<=0.999990,0.000010<=pLDiff<=0.999990,0.000010<=pHDiff<=0.999990;" $arguments | tee ./results/pmdp_reporter4.pm-constXsize_10_Ysize_10_MAXTRIES_3_B_3.log &

wait
echo "done!"
