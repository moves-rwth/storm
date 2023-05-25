cmake_minimum_required(VERSION 3.17)
project(storm)

set(CMAKE_CXX_STANDARD 17)

add_executable(storm main.cpp src/storm/modelchecker/blackbox/bMDP.cpp src/storm/modelchecker/blackbox/bMDP.h src/storm/modelchecker/blackbox/eMDP.cpp src/storm/modelchecker/blackbox/eMDP.h src/storm/modelchecker/blackbox/blackBoxExplorer.cpp src/storm/modelchecker/blackbox/blackBoxExplorer.h src/storm/modelchecker/blackbox/eMDP2bMDP.cpp src/storm/modelchecker/blackbox/eMDP2bMDP.h src/storm/modelchecker/blackbox/blackBoxChecker.cpp src/storm/modelchecker/blackbox/blackBoxChecker.h src/storm/modelchecker/blackbox/bound-functions/boundFunc.cpp src/storm/modelchecker/blackbox/bound-functions/boundFunc.h "src/storm/modelchecker/blackbox/valueFunctions /valueFunc.cpp" "src/storm/modelchecker/blackbox/valueFunctions /valueFunc.h" src/storm/modelchecker/blackbox/heurstic-simulate/heuristicSim.cpp src/storm/modelchecker/blackbox/heurstic-simulate/heuristicSim.h)