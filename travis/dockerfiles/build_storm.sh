#!/bin/bash
echo "Building Storm..."
git clone https://github.com/moves-rwth/storm.git
cd storm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make storm storm-dft storm-pars -j1
echo "Building Storm finished"
