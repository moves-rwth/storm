#!/bin/bash
executable="timeout 3600 ../build/src/storm"
arguments="-i 1000000 --parametric --parametricRegion --region:refinement 0.05 --region:samplemode off"
resultfolder=res

mkdir $resultfolder
# pdtmcs
$executable -s ./pdtmc/crowds/crowds.pm -const CrowdSize=10,TotalRuns=5 --prop ./pdtmc/crowds/crowds.prctl --region:regions "0.000010<=PF<=0.999990,0.000010<=badC<=0.999990;" $arguments | tee ./$resultfolder/pdtmc_crowds.pm-constCrowdSize_10_TotalRuns_5.log &
$executable -s ./pdtmc/crowds/crowds.pm -const CrowdSize=15,TotalRuns=5 --prop ./pdtmc/crowds/crowds.prctl --region:regions "0.000010<=PF<=0.999990,0.000010<=badC<=0.999990;" $arguments | tee ./$resultfolder/pdtmc_crowds.pm-constCrowdSize_15_TotalRuns_5.log &
$executable -s ./pdtmc/crowds/crowds.pm -const CrowdSize=15,TotalRuns=7 --prop ./pdtmc/crowds/crowds.prctl --region:regions "0.000010<=PF<=0.999990,0.000010<=badC<=0.999990;" $arguments | tee ./$resultfolder/pdtmc_crowds.pm-constCrowdSize_15_TotalRuns_7.log &
$executable -s ./pdtmc/crowds/crowds.pm -const CrowdSize=20,TotalRuns=5 --prop ./pdtmc/crowds/crowds.prctl --region:regions "0.000010<=PF<=0.999990,0.000010<=badC<=0.999990;" $arguments | tee ./$resultfolder/pdtmc_crowds.pm-constCrowdSize_20_TotalRuns_5.log &
wait
$executable -s ./pdtmc/crowds/crowds.pm -const CrowdSize=20,TotalRuns=7 --prop ./pdtmc/crowds/crowds.prctl --region:regions "0.000010<=PF<=0.999990,0.000010<=badC<=0.999990;" $arguments | tee ./$resultfolder/pdtmc_crowds.pm-constCrowdSize_20_TotalRuns_7.log &
$executable -s ./pdtmc/crowds/crowds.pm -const CrowdSize=20,TotalRuns=10 --prop ./pdtmc/crowds/crowds.prctl --region:regions "0.000010<=PF<=0.999990,0.000010<=badC<=0.999990;" $arguments | tee ./$resultfolder/pdtmc_crowds.pm-constCrowdSize_20_TotalRuns_10.log &

$executable -s ./pdtmc/nand/nand.pm -const N=10,K=5 --prop ./pdtmc/nand/nand.prctl --region:regions "0.000010<=perr<=0.999990,0.000010<=prob1<=0.999990;" $arguments | tee ./$resultfolder/pdtmc_nand.pm-constN_10_K_5.log &
$executable -s ./pdtmc/nand/nand.pm -const N=25,K=5 --prop ./pdtmc/nand/nand.prctl --region:regions "0.000010<=perr<=0.999990,0.000010<=prob1<=0.999990;" $arguments | tee ./$resultfolder/pdtmc_nand.pm-constN_25_K_5.log &
wait

$executable -s ./pdtmc/brp_rewards2/brp_rewards2.pm -const N=256,MAX=5 --prop ./pdtmc/brp_rewards2/brp_rewards2.prctl --region:regions "0.000010<=pL<=0.999990,0.000010<=pK<=0.999990;" $arguments | tee ./$resultfolder/pdtmc_brp_rewards2.pm-constN_256_MAX_5.log &
$executable -s ./pdtmc/brp_rewards2/brp_rewards2.pm -const N=512,MAX=5 --prop ./pdtmc/brp_rewards2/brp_rewards2.prctl --region:regions "0.000010<=pL<=0.999990,0.000010<=pK<=0.999990;" $arguments | tee ./$resultfolder/pdtmc_brp_rewards2.pm-constN_512_MAX_5.log &
$executable -s ./pdtmc/brp_rewards2/brp_rewards2.pm -const N=4096,MAX=5 --prop ./pdtmc/brp_rewards2/brp_rewards2.prctl --region:regions "0.000010<=pL<=0.999990,0.000010<=pK<=0.999990;" $arguments | tee ./$resultfolder/pdtmc_brp_rewards2.pm-constN_4096_MAX_5.log &

wait

$executable -s ./pdtmc/brp_rewards4/brp_rewards4.pm -const N=64,MAX=5 --prop ./pdtmc/brp_rewards4/brp_rewards4.prctl --region:regions "0.000010<=pL<=0.999990,0.000010<=pK<=0.999990,0.000010<=TOMsg<=0.999990,0.000010<=TOAck<=0.999990;" $arguments | tee ./$resultfolder/pdtmc_brp_rewards4.pm-constN_64_MAX_5.log &
$executable -s ./pdtmc/brp_rewards4/brp_rewards4.pm -const N=128,MAX=5 --prop ./pdtmc/brp_rewards4/brp_rewards4.prctl --region:regions "0.000010<=pL<=0.999990,0.000010<=pK<=0.999990,0.000010<=TOMsg<=0.999990,0.000010<=TOAck<=0.999990;" $arguments | tee ./$resultfolder/pdtmc_brp_rewards4.pm-constN_128_MAX_5.log &
$executable -s ./pdtmc/brp_rewards4/brp_rewards4.pm -const N=256,MAX=5 --prop ./pdtmc/brp_rewards4/brp_rewards4.prctl --region:regionso "0.000010<=pL<=0.999990,0.000010<=pK<=0.999990,0.000010<=TOMsg<=0.999990,0.000010<=TOAck<=0.999990;" $arguments | tee ./$resultfolder/pdtmc_brp_rewards4.pm-constN_256_MAX_5.log &
$executable -s ./pdtmc/brp_rewards4/brp_rewards4.pm -const N=5012,MAX=5 --prop ./pdtmc/brp_rewards4/brp_rewards4.prctl --region:regionso "0.000010<=pL<=0.999990,0.000010<=pK<=0.999990,0.000010<=TOMsg<=0.999990,0.000010<=TOAck<=0.999990;" $arguments | tee ./$resultfolder/pdtmc_brp_rewards4.pm-constN_5012_MAX_5.log &
wait


$executable -s ./pdtmc/brp/brp.pm -const N=256,MAX=5 --prop ./pdtmc/brp/brp.prctl --region:regions "0.000010<=pL<=0.999990,0.000010<=pK<=0.999990;" $arguments | tee ./$resultfolder/pdtmc_brp.pm-constN_256_MAX_5.log &
$executable -s ./pdtmc/brp/brp.pm -const N=4096,MAX=5 --prop ./pdtmc/brp/brp.prctl --region:regions "0.000010<=pL<=0.999990,0.000010<=pK<=0.999990;" $arguments | tee ./$resultfolder/pdtmc_brp.pm-constN_4096_MAX_5.log &

wait


# New instances!!!!! (tested here also with bisim)

arguments="-bisim -i 1000000 --parametric --parametricRegion --region:refinement 0.05 --region:samplemode off"
resultfolder=res_bisim
mkdir $resultfolder
$executable -s ./pdtmc/crowds/crowds.pm -const CrowdSize=15,TotalRuns=5 --prop ./pdtmc/crowds/crowds.prctl --region:regions "0.000010<=PF<=0.999990,0.000010<=badC<=0.999990;" $arguments | tee ./$resultfolder/pdtmc_crowds.pm-constCrowdSize_15_TotalRuns_5.log &
$executable -s ./pdtmc/crowds/crowds.pm -const CrowdSize=15,TotalRuns=7 --prop ./pdtmc/crowds/crowds.prctl --region:regions "0.000010<=PF<=0.999990,0.000010<=badC<=0.999990;" $arguments | tee ./$resultfolder/pdtmc_crowds.pm-constCrowdSize_15_TotalRuns_7.log &
$executable -s ./pdtmc/crowds/crowds.pm -const CrowdSize=20,TotalRuns=5 --prop ./pdtmc/crowds/crowds.prctl --region:regions "0.000010<=PF<=0.999990,0.000010<=badC<=0.999990;" $arguments | tee ./$resultfolder/pdtmc_crowds.pm-constCrowdSize_20_TotalRuns_5.log &
wait
$executable -s ./pdtmc/crowds/crowds.pm -const CrowdSize=20,TotalRuns=7 --prop ./pdtmc/crowds/crowds.prctl --region:regions "0.000010<=PF<=0.999990,0.000010<=badC<=0.999990;" $arguments | tee ./$resultfolder/pdtmc_crowds.pm-constCrowdSize_20_TotalRuns_7.log &
$executable -s ./pdtmc/crowds/crowds.pm -const CrowdSize=20,TotalRuns=10 --prop ./pdtmc/crowds/crowds.prctl --region:regions "0.000010<=PF<=0.999990,0.000010<=badC<=0.999990;" $arguments | tee ./$resultfolder/pdtmc_crowds.pm-constCrowdSize_20_TotalRuns_10.log &
wait

$executable -s ./pdtmc/brp_rewards2/brp_rewards2.pm -const N=256,MAX=5 --prop ./pdtmc/brp_rewards2/brp_rewards2.prctl --region:regions "0.000010<=pL<=0.999990,0.000010<=pK<=0.999990;" $arguments | tee ./$resultfolder/pdtmc_brp_rewards2.pm-constN_256_MAX_5.log &

$executable -s ./pdtmc/brp_rewards4/brp_rewards4.pm -const N=64,MAX=5 --prop ./pdtmc/brp_rewards4/brp_rewards4.prctl --region:regions "0.000010<=pL<=0.999990,0.000010<=pK<=0.999990,0.000010<=TOMsg<=0.999990,0.000010<=TOAck<=0.999990;" $arguments | tee ./$resultfolder/pdtmc_brp_rewards4.pm-constN_64_MAX_5.log &
$executable -s ./pdtmc/brp_rewards4/brp_rewards4.pm -const N=128,MAX=5 --prop ./pdtmc/brp_rewards4/brp_rewards4.prctl --region:regions "0.000010<=pL<=0.999990,0.000010<=pK<=0.999990,0.000010<=TOMsg<=0.999990,0.000010<=TOAck<=0.999990;" $arguments | tee ./$resultfolder/pdtmc_brp_rewards4.pm-constN_128_MAX_5.log &


wait


echo "done!"
