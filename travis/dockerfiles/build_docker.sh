#!/bin/bash

# Build Ubuntu 16.10 "Yakkety Yak"
docker build -t mvolk/storm-basesystem:ubuntu-16.10 -f Dockerfile.ubuntu-16.10 .
docker push mvolk/storm-basesystem:ubuntu-16.10

# Build Debian 9 "Stretch"
docker build -t mvolk/storm-basesystem:debian-9 -f Dockerfile.debian-9 .
docker push mvolk/storm-basesystem:debian-9
