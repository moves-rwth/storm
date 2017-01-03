---
title: Getting Started
navigation_weight: 2
layout: default
---

This document shows you the steps to get started with storm.

# TOC
 {:.no_toc}
- list
{:toc}

# Obtain Storm

We currently provide two ways of running storm. 

- A Virtual Machine image 
- Source code

## Obtain the VM

The VM is not yet available.
{:.todo} 

## Obtain the source

The source code can be obtaind from [github](https://github.com/moves-rwth/storm).

- via git:
```bash
git clone https://github.com/moves-rwth/storm.git STORM_DIR
```
-  as an archive
```bash
wget https://github.com/moves-rwth/archive/master.zip
````

Currently, the source code is only available from our private repo.
{:.todo} 


# Build Storm

## Configuration Step
Switch to the directory where you put storm in the previous step.

```bash
cd STORM_DIR
```

From there, create a build directory.

```bash
mkdir build
cd build
```

Configure storm using 

```bash
cmake ..
```


In case of errors, check the [requirements](documentation/requirements.html).

## Build Step

If configuration went smoothly, the compilation step should run through. We recommend some coffee though.

To compile just the storm main command line interface, do

```bash
make storm
```

If you have multiple cores at your disposal and 8GB of memory, you can do 

```bash
make -jNUMBER_OF_CORES storm
```

If you are interested in one of the other binaries, replace storm with the appropriate target

|-------------+----------------|
| binary      | target         |
|-------------+----------------|
| DFTs        | storm-dft-cli  |
| GSPNs       | storm-gspn-cli |
| PGCL        | storm-pgcl-cli |
|-------------+----------------| 


## Test Step

This step is optional. We recommend you run it in order to make sure storm produces correct results on your platform.


# Run Storm

In order to get you started, we discuss some common scenarios here.

## Standard Model Checking

## DFT Analysis




