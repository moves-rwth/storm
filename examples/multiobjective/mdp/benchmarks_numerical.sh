#!/bin/sh

mkdir benchmarks_numerical


executable=../../../build/src/Release/storm 
options='--precision 0.000001 --multiobjective:precision 0.0001 --debug'
modelcommand='-s'
propertycommand='-prop'
logfilepostfix='.storm.output'
$executable $modelcommand consensus/consensus2_3_2.nm $propertycommand consensus/consensus2_3_2_numerical.pctl $options | tee  benchmarks_numerical/1_consensus2_3_2$logfilepostfix 
$executable $modelcommand consensus/consensus2_4_2.nm $propertycommand consensus/consensus2_4_2_numerical.pctl $options | tee  benchmarks_numerical/1_consensus2_4_2$logfilepostfix 
$executable $modelcommand consensus/consensus2_5_2.nm $propertycommand consensus/consensus2_5_2_numerical.pctl $options | tee  benchmarks_numerical/1_consensus2_5_2$logfilepostfix 
$executable $modelcommand consensus/consensus3_3_2.nm $propertycommand consensus/consensus3_3_2_numerical.pctl $options | tee  benchmarks_numerical/1_consensus3_3_2$logfilepostfix 
$executable $modelcommand consensus/consensus3_4_2.nm $propertycommand consensus/consensus3_4_2_numerical.pctl $options | tee  benchmarks_numerical/1_consensus3_4_2$logfilepostfix 
$executable $modelcommand consensus/consensus3_5_2.nm $propertycommand consensus/consensus3_5_2_numerical.pctl $options | tee  benchmarks_numerical/1_consensus3_5_2$logfilepostfix 
$executable $modelcommand zeroconf/zeroconf4.nm $propertycommand zeroconf/zeroconf4_numerical.pctl $options | tee  benchmarks_numerical/2_zeroconf4$logfilepostfix 
$executable $modelcommand zeroconf/zeroconf6.nm $propertycommand zeroconf/zeroconf6_numerical.pctl $options | tee  benchmarks_numerical/2_zeroconf6$logfilepostfix 
$executable $modelcommand zeroconf/zeroconf8.nm $propertycommand zeroconf/zeroconf8_numerical.pctl $options | tee  benchmarks_numerical/2_zeroconf8$logfilepostfix 
$executable $modelcommand zeroconf-tb/zeroconf-tb2_14.nm $propertycommand zeroconf-tb/zeroconf-tb2_14_numerical.pctl $options | tee  benchmarks_numerical/3_zeroconf-tb2_14$logfilepostfix 
$executable $modelcommand zeroconf-tb/zeroconf-tb4_10.nm $propertycommand zeroconf-tb/zeroconf-tb4_10_numerical.pctl $options | tee  benchmarks_numerical/3_zeroconf-tb4_10$logfilepostfix 
$executable $modelcommand zeroconf-tb/zeroconf-tb4_14.nm $propertycommand zeroconf-tb/zeroconf-tb4_14_numerical.pctl $options | tee  benchmarks_numerical/3_zeroconf-tb4_14$logfilepostfix 
$executable $modelcommand team/team2obj_3.nm $propertycommand team/team2obj_3_numerical.pctl $options | tee  benchmarks_numerical/4_team2obj_3$logfilepostfix 
$executable $modelcommand team/team3obj_3.nm $propertycommand team/team3obj_3_numerical.pctl $options | tee  benchmarks_numerical/5_team3obj_3$logfilepostfix 
$executable $modelcommand team/team2obj_4.nm $propertycommand team/team2obj_4_numerical.pctl $options | tee  benchmarks_numerical/4_team2obj_4$logfilepostfix 
$executable $modelcommand team/team3obj_4.nm $propertycommand team/team3obj_4_numerical.pctl $options | tee  benchmarks_numerical/5_team3obj_4$logfilepostfix 
$executable $modelcommand team/team2obj_5.nm $propertycommand team/team2obj_5_numerical.pctl $options | tee  benchmarks_numerical/4_team2obj_5$logfilepostfix 
$executable $modelcommand team/team3obj_5.nm $propertycommand team/team3obj_5_numerical.pctl $options | tee  benchmarks_numerical/5_team3obj_5$logfilepostfix 
$executable $modelcommand scheduler/scheduler05.nm $propertycommand scheduler/scheduler05_numerical.pctl $options | tee  benchmarks_numerical/6_scheduler05$logfilepostfix 
$executable $modelcommand scheduler/scheduler25.nm $propertycommand scheduler/scheduler25_numerical.pctl $options | tee  benchmarks_numerical/6_scheduler25$logfilepostfix 
$executable $modelcommand scheduler/scheduler50.nm $propertycommand scheduler/scheduler50_numerical.pctl $options | tee  benchmarks_numerical/6_scheduler50$logfilepostfix 
$executable $modelcommand dpm/dpm100.nm $propertycommand dpm/dpm100_numerical.pctl $options | tee  benchmarks_numerical/7_dpm100$logfilepostfix 
$executable $modelcommand dpm/dpm200.nm $propertycommand dpm/dpm200_numerical.pctl $options | tee  benchmarks_numerical/7_dpm200$logfilepostfix 
$executable $modelcommand dpm/dpm300.nm $propertycommand dpm/dpm300_numerical.pctl $options | tee  benchmarks_numerical/7_dpm300$logfilepostfix 


executable=../../../../prism/prism-4.3-src/bin/prism 
options='-epsilon 0.000001 -paretoepsilon 0.0001 -sparse -javamaxmem 3g'
modelcommand=''
propertycommand=''
logfilepostfix='.prism.output'
$executable $modelcommand consensus/consensus2_3_2.nm $propertycommand consensus/consensus2_3_2_numerical.pctl $options | tee  benchmarks_numerical/1_consensus2_3_2$logfilepostfix 
$executable $modelcommand consensus/consensus2_4_2.nm $propertycommand consensus/consensus2_4_2_numerical.pctl $options | tee  benchmarks_numerical/1_consensus2_4_2$logfilepostfix 
$executable $modelcommand consensus/consensus2_5_2.nm $propertycommand consensus/consensus2_5_2_numerical.pctl $options | tee  benchmarks_numerical/1_consensus2_5_2$logfilepostfix 
$executable $modelcommand consensus/consensus3_3_2.nm $propertycommand consensus/consensus3_3_2_numerical.pctl $options | tee  benchmarks_numerical/1_consensus3_3_2$logfilepostfix 
$executable $modelcommand consensus/consensus3_4_2.nm $propertycommand consensus/consensus3_4_2_numerical.pctl $options | tee  benchmarks_numerical/1_consensus3_4_2$logfilepostfix 
$executable $modelcommand consensus/consensus3_5_2.nm $propertycommand consensus/consensus3_5_2_numerical.pctl $options | tee  benchmarks_numerical/1_consensus3_5_2$logfilepostfix 
$executable $modelcommand zeroconf/zeroconf4.nm $propertycommand zeroconf/zeroconf4_numerical.pctl $options | tee  benchmarks_numerical/2_zeroconf4$logfilepostfix 
$executable $modelcommand zeroconf/zeroconf6.nm $propertycommand zeroconf/zeroconf6_numerical.pctl $options | tee  benchmarks_numerical/2_zeroconf6$logfilepostfix 
$executable $modelcommand zeroconf/zeroconf8.nm $propertycommand zeroconf/zeroconf8_numerical.pctl $options | tee  benchmarks_numerical/2_zeroconf8$logfilepostfix 
$executable $modelcommand zeroconf-tb/zeroconf-tb2_14.nm $propertycommand zeroconf-tb/zeroconf-tb2_14_numerical.pctl $options | tee  benchmarks_numerical/3_zeroconf-tb2_14$logfilepostfix 
$executable $modelcommand zeroconf-tb/zeroconf-tb4_10.nm $propertycommand zeroconf-tb/zeroconf-tb4_10_numerical.pctl $options | tee  benchmarks_numerical/3_zeroconf-tb4_10$logfilepostfix 
$executable $modelcommand zeroconf-tb/zeroconf-tb4_14.nm $propertycommand zeroconf-tb/zeroconf-tb4_14_numerical.pctl $options | tee  benchmarks_numerical/3_zeroconf-tb4_14$logfilepostfix 
$executable $modelcommand team/team2obj_3.nm $propertycommand team/team2obj_3_numerical.pctl $options | tee  benchmarks_numerical/4_team2obj_3$logfilepostfix 
$executable $modelcommand team/team3obj_3.nm $propertycommand team/team3obj_3_numerical.pctl $options | tee  benchmarks_numerical/5_team3obj_3$logfilepostfix 
$executable $modelcommand team/team2obj_4.nm $propertycommand team/team2obj_4_numerical.pctl $options | tee  benchmarks_numerical/4_team2obj_4$logfilepostfix 
$executable $modelcommand team/team3obj_4.nm $propertycommand team/team3obj_4_numerical.pctl $options | tee  benchmarks_numerical/5_team3obj_4$logfilepostfix 
$executable $modelcommand team/team2obj_5.nm $propertycommand team/team2obj_5_numerical.pctl $options | tee  benchmarks_numerical/4_team2obj_5$logfilepostfix 
$executable $modelcommand team/team3obj_5.nm $propertycommand team/team3obj_5_numerical.pctl $options | tee  benchmarks_numerical/5_team3obj_5$logfilepostfix 
$executable $modelcommand scheduler/scheduler05.nm $propertycommand scheduler/scheduler05_numerical.pctl $options | tee  benchmarks_numerical/6_scheduler05$logfilepostfix 
$executable $modelcommand scheduler/scheduler25.nm $propertycommand scheduler/scheduler25_numerical.pctl $options | tee  benchmarks_numerical/6_scheduler25$logfilepostfix 
$executable $modelcommand scheduler/scheduler50.nm $propertycommand scheduler/scheduler50_numerical.pctl $options | tee  benchmarks_numerical/6_scheduler50$logfilepostfix 
$executable $modelcommand dpm/dpm100.nm $propertycommand dpm/dpm100_numerical.pctl $options | tee  benchmarks_numerical/7_dpm100$logfilepostfix 
$executable $modelcommand dpm/dpm200.nm $propertycommand dpm/dpm200_numerical.pctl $options | tee  benchmarks_numerical/7_dpm200$logfilepostfix 
$executable $modelcommand dpm/dpm300.nm $propertycommand dpm/dpm300_numerical.pctl $options | tee  benchmarks_numerical/7_dpm300$logfilepostfix 

executable=../../../../prism/prism-4.3-src/bin/prism 
options='-epsilon 0.000001 -paretoepsilon 0.0001 -sparse -javamaxmem 3g -gs'
modelcommand=''
propertycommand=''
logfilepostfix='.prism-gs.output'
$executable $modelcommand consensus/consensus2_3_2.nm $propertycommand consensus/consensus2_3_2_numerical.pctl $options | tee  benchmarks_numerical/1_consensus2_3_2$logfilepostfix 
$executable $modelcommand consensus/consensus2_4_2.nm $propertycommand consensus/consensus2_4_2_numerical.pctl $options | tee  benchmarks_numerical/1_consensus2_4_2$logfilepostfix 
$executable $modelcommand consensus/consensus2_5_2.nm $propertycommand consensus/consensus2_5_2_numerical.pctl $options | tee  benchmarks_numerical/1_consensus2_5_2$logfilepostfix 
$executable $modelcommand consensus/consensus3_3_2.nm $propertycommand consensus/consensus3_3_2_numerical.pctl $options | tee  benchmarks_numerical/1_consensus3_3_2$logfilepostfix 
$executable $modelcommand consensus/consensus3_4_2.nm $propertycommand consensus/consensus3_4_2_numerical.pctl $options | tee  benchmarks_numerical/1_consensus3_4_2$logfilepostfix 
$executable $modelcommand consensus/consensus3_5_2.nm $propertycommand consensus/consensus3_5_2_numerical.pctl $options | tee  benchmarks_numerical/1_consensus3_5_2$logfilepostfix 
$executable $modelcommand zeroconf/zeroconf4.nm $propertycommand zeroconf/zeroconf4_numerical.pctl $options | tee  benchmarks_numerical/2_zeroconf4$logfilepostfix 
$executable $modelcommand zeroconf/zeroconf6.nm $propertycommand zeroconf/zeroconf6_numerical.pctl $options | tee  benchmarks_numerical/2_zeroconf6$logfilepostfix 
$executable $modelcommand zeroconf/zeroconf8.nm $propertycommand zeroconf/zeroconf8_numerical.pctl $options | tee  benchmarks_numerical/2_zeroconf8$logfilepostfix 
$executable $modelcommand zeroconf-tb/zeroconf-tb2_14.nm $propertycommand zeroconf-tb/zeroconf-tb2_14_numerical.pctl $options | tee  benchmarks_numerical/3_zeroconf-tb2_14$logfilepostfix 
$executable $modelcommand zeroconf-tb/zeroconf-tb4_10.nm $propertycommand zeroconf-tb/zeroconf-tb4_10_numerical.pctl $options | tee  benchmarks_numerical/3_zeroconf-tb4_10$logfilepostfix 
$executable $modelcommand zeroconf-tb/zeroconf-tb4_14.nm $propertycommand zeroconf-tb/zeroconf-tb4_14_numerical.pctl $options | tee  benchmarks_numerical/3_zeroconf-tb4_14$logfilepostfix 
$executable $modelcommand team/team2obj_3.nm $propertycommand team/team2obj_3_numerical.pctl $options | tee  benchmarks_numerical/4_team2obj_3$logfilepostfix 
$executable $modelcommand team/team3obj_3.nm $propertycommand team/team3obj_3_numerical.pctl $options | tee  benchmarks_numerical/5_team3obj_3$logfilepostfix 
$executable $modelcommand team/team2obj_4.nm $propertycommand team/team2obj_4_numerical.pctl $options | tee  benchmarks_numerical/4_team2obj_4$logfilepostfix 
$executable $modelcommand team/team3obj_4.nm $propertycommand team/team3obj_4_numerical.pctl $options | tee  benchmarks_numerical/5_team3obj_4$logfilepostfix 
$executable $modelcommand team/team2obj_5.nm $propertycommand team/team2obj_5_numerical.pctl $options | tee  benchmarks_numerical/4_team2obj_5$logfilepostfix 
$executable $modelcommand team/team3obj_5.nm $propertycommand team/team3obj_5_numerical.pctl $options | tee  benchmarks_numerical/5_team3obj_5$logfilepostfix 
$executable $modelcommand scheduler/scheduler05.nm $propertycommand scheduler/scheduler05_numerical.pctl $options | tee  benchmarks_numerical/6_scheduler05$logfilepostfix 
$executable $modelcommand scheduler/scheduler25.nm $propertycommand scheduler/scheduler25_numerical.pctl $options | tee  benchmarks_numerical/6_scheduler25$logfilepostfix 
$executable $modelcommand scheduler/scheduler50.nm $propertycommand scheduler/scheduler50_numerical.pctl $options | tee  benchmarks_numerical/6_scheduler50$logfilepostfix 
$executable $modelcommand dpm/dpm100.nm $propertycommand dpm/dpm100_numerical.pctl $options | tee  benchmarks_numerical/7_dpm100$logfilepostfix 
$executable $modelcommand dpm/dpm200.nm $propertycommand dpm/dpm200_numerical.pctl $options | tee  benchmarks_numerical/7_dpm200$logfilepostfix 
$executable $modelcommand dpm/dpm300.nm $propertycommand dpm/dpm300_numerical.pctl $options | tee  benchmarks_numerical/7_dpm300$logfilepostfix 
