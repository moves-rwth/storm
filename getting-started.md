---
title: Getting Started
navigation_weight: 2
layout: default
---

This document shows you the steps to get started with storm.




## Building Storm

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


In case of errors, check the [requirements](requirements.html).







### Instructions

### General

```bash
mkdir build
cd build
``` 

It is recommended to make an out-of-source build, meaning that the folder in which CMake generates its Cache, Makefiles and output files should not be the Project Root nor its Source Directory.
A typical build layout is to create a folder "build" in the project root alongside the CMakeLists.txt file, change into this folder and execute "cmake .." as this will leave all source files untouched
and makes cleaning up the build tree very easy.
There are several options available for the CMake Script as to control behaviour and included components.
If no error occured during the last CMake Configure round, press Generate.
Now you can build StoRM using the generated project/makefiles in the Build folder you selected.%                
