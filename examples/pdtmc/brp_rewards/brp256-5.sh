#!/bin/bash

smtcommand=$(cat smtcommand.txt)

/home/tim/git/storm/build/storm -s "/home/tim/git/paramagic/benchmarkfiles/pdtmc/brp/brp_256-5.pm" --prop 'P<0.5 [F "target"]' --parametric --parametricRegion --smt2:exportscript "/home/tim/Desktop/smtlibcommand.smt2" --smt2:solvercommand "$smtcommand" --region:regionfile /home/tim/Desktop/brpRegions.txt $1


