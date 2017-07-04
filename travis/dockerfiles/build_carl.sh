#!/bin/bash
echo "Building Carl..."
git clone -b singleton_fix https://github.com/smtrat/carl.git
cd carl
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release -DUSE_CLN_NUMBERS=ON -DUSE_GINAC=ON
make lib_carl -j2
echo "Building Carl finished"
