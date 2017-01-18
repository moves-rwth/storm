---
title: Getting Started
navigation_weight: 2
layout: default
---

This document shows you the steps to get started with storm.

- list
{:toc}

# Obtain Storm

We currently provide two ways of running storm. 

- A Virtual Machine image 
- Source code

## Obtain the VM

{:.alert .alert-danger} 
The VM is not yet available.

## Obtain the source

The source code can be obtaind from [github](https://github.com/moves-rwth/storm).

- via git: `git clone https://github.com/moves-rwth/storm.git STORM_DIR`
-  as an archive `wget https://github.com/moves-rwth/archive/master.zip`


# Build Storm

This guide helps you building a standard version of storm. There are plenty of configuration options, please check our [configuration guide](documentation/installation/configuration-guide.html).

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


In case of errors, check the [requirements](documentation/installation/requirements.html).

## Build Step

If configuration went smoothly, the compilation step should run through. We recommend some coffee though.

To compile just the storm main command line interface, do

```bash
make storm-main
```

{:.alert .alert-info}
If you have multiple cores at your disposal and 8GB of memory, you can execute 
`make -jNUMBER_OF_CORES storm-main`

If you are interested in one of the other binaries, replace storm with the appropriate target

|-------------+----------------|
| binary      | target         |
|-------------+----------------|
| DFTs        | storm-dft-cli  |
| GSPNs       | storm-gspn-cli |
| PGCL        | storm-pgcl-cli |
|-------------+----------------| 


## Test Step (optional)

We recommend this step in order to make sure storm produces correct results on your platform.

```bash
make check
```

In case of errors, please do not hesistate to [contact us](about.html#people-behind-storm). Please provide the output of ```make check-verbose``` and the output obtained by running ```cmake .. ``` from the build folder.


# Run Storm

In order to get you started, we discuss some common scenarios here.

## Standard Model Checking



## DFT Analysis




