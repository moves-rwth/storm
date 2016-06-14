#!/bin/sh
echo " " >> results.csv 
cat *.output | grep construc -A3 | grep States >> results.csv 
echo " " >> results.csv 
cat *.output | grep "Number of weight vectors used:" >> results.csv 
echo " " >> results.csv 
cat *.output | grep "value iteration(s) took" >> results.csv
echo " " >> results.csv 
cat *.output | grep "Time for model checking:" >> results.csv
echo " " >> results.csv 
cat *.output | grep "Result:" >> results.csv
