---
title: Requirements
layout: default
documentation: true
categories: [Installation]
---


# Dependencies


#### Compiler:
 A C++11 compliant compiler is required to build StoRM. It is tested and known to work with the following compilers:
 - GCC 5.3
 - Clang 3.5.0

 Other versions or compilers might work, but are not tested.

 The following Compilers are known NOT to work:
 - Microsoft Visual Studio versions older than 2013,
 - GCC versions 4.9.1 and older.




# OS Specific Preparations

We collected some specific hints to ease the installation of storm on several supported operating systems.

## Debian "Stretch" 

## macOS 10.12 "Sierra"

We recommend the usage of [homebrew](http://brew.sh) to install some packages.

- Required:
```
brew install cln
```

- Recommended:
```
brew install z3
```

## Ubuntu 16.04 LTS

## Ubuntu 16.10

- Required:
```
sudo apt-get install autoconf cmake git libhwloc-dev
```

- Recommended:
```
sudo apt-get install z3
```

{.alert alert-warning}
Add boost packages



