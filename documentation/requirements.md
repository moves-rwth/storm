---
title: Requirements
layout: default
---

### Requirements 
CMake >= 3.2
 CMake is required as it is used to generate the Makefiles or Projects/Solutions required to build StoRM.

#### Compiler:
 A C++11 compliant compiler is required to build StoRM. It is tested and known to work with the following compilers:
 - GCC 5.3
 - Clang 3.5.0

 Other versions or compilers might work, but are not tested.

 The following Compilers are known NOT to work: 
 - Microsoft Visual Studio versions older than 2013,
 - GCC versions 4.9.1 and older.

Prerequisites:
 Boost >= 1.61
        Build using the Boost Build system, for x64 use "bjam address-model=64" or "bjam.exe address-model=64 --build-type=complete"
