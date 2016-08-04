# Building Storm


## Requirements 
CMake >= 2.8.11
 CMake is required as it is used to generate the Makefiles or Projects/Solutions required to build StoRM.

### Compiler:
 A C++11 compliant compiler is required to build StoRM. It is tested and known to work with the following compilers:
 - GCC 5.3
 - Clang 3.5.0

 Other versions or compilers might work, but are not tested.

 The following Compilers are known NOT to work: 
 - Microsoft Visual Studio versions older than 2013,
 - GCC versions 4.9.1 and older.

Prerequisites:
 Boost >= 1.60
	Build using the Boost Build system, for x64 use "bjam address-model=64" or "bjam.exe address-model=64 --build-type=complete"

## Instructions

### General
 
> mkdir build


It is recommended to make an out-of-source build, meaning that the folder in which CMake generates its Cache, Makefiles and output files should not be the Project Root nor its Source Directory.
A typical build layout is to create a folder "build" in the project root alongside the CMakeLists.txt file, change into this folder and execute "cmake .." as this will leave all source files untouched
and makes cleaning up the build tree very easy.
There are several options available for the CMake Script as to control behaviour and included components.
If no error occured during the last CMake Configure round, press Generate.
Now you can build StoRM using the generated project/makefiles in the Build folder you selected.