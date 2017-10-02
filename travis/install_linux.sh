#!/bin/bash

set -e

# Skip this run?
if [ -f build/skip.txt ]
then
  exit 0
fi

#sudo apt-get install -qq -y docker
