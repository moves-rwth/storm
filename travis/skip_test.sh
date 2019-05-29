#!/bin/bash
# Skip this run?
if [ -f build/skip.txt ]
then
  exit 0
fi
