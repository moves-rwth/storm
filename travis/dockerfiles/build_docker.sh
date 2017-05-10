#!/bin/bash
docker build -t mvolk/storm-basesystem:ubuntu-16.10 -f Dockerfile.ubuntu-16.10 .
docker push mvolk/storm-basesystem:ubuntu-16.10
