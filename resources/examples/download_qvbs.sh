#!/bin/bash

# Get arguments
if [ $# -ne 1 ]; then
    # Print usage
    echo "Download helper for the 'Quantitative Verification Benchmark Set' (QVBS) from http://qcomp.org/benchmarks/"
    FILE_NAME=$(basename $BASH_SOURCE)
    echo "- Usage:"
    echo -e "\t./$FILE_NAME <destination directory>"
    exit 1
fi

DIR=$1

# Check if directory already exists
if [ -d "$DIR" ]; then
    # Check if directory already contains git repo
    git -C $DIR rev-parse
    GIT_RET=$?
    if [ $GIT_RET -ne 0 ]; then
        echo "- Directory already exists."
        exit 2
    fi
    # Directory contains git repo
    GIT_URL=$(git -C $DIR config --get remote.origin.url)
    if [ "$GIT_URL" = "https://github.com/ahartmanns/qcomp.git" ]; then
        echo "- QVBS repo already exists. Updating the repo instead."
        git -C $DIR pull
    else
        echo "- Unknown git repo already exists in directory."
        exit 3
    fi
else
    echo "- Will clone repo from https://github.com/ahartmanns/qcomp.git"
    git clone https://github.com/ahartmanns/qcomp.git $DIR
    echo "- QVBS successfully downloaded to $DIR"
fi

echo "- Integrate QVBS into Storm by adding the following flag to CMake:"
echo "-DSTORM_QVBS_ROOT=$DIR/benchmarks"
