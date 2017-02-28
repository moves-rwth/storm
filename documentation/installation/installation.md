---
title: Installation
layout: default
documentation: true
category_weight: 1
categories: [Installation]
---

This guide shows you the options you have to install Storm. For this, we are going to assume that all necessary [dependencies](requirements.html) have been installed on the machine in default locations so they can be found by our build machinery. Also, we are going to assume that your operating system is in the list of supported [operating systems](requirements.html#supported-os). If your operating system is not in this list but is Linux-based, chances are that you can install Storm but you may have to perform additional steps that we do not cover here. If you just want to quickly try Storm and/or are not able to install the dependencies, you might want to check out our [virtual machine]({{ site.baseurl }}/documentation/vm/vm.html) image.

We currently provide two ways of installing Storm:

- [homebrew](#homebrew) <span class="label label-info">new!</span>
- [from source](#building-storm-from-source)

If you just want to run Storm and you want to run it natively on your machine, then we recommend installing it via [homebrew](#homebrew). However, if you want or need to make changes to the Storm code base, you have to obtain the source code and [build it](#building-storm-from-source) yourself. While this is not always a breeze (depending on your operating system), we spent some effort on making this process as easy as possible.

## Homebrew

If you are running a version of macOS that is newer than Mavericks, you can use [homebrew](http://brew.sh/), the "missing package manager for macOS". Once you have installed homebrew, you need to make homebrew aware of how to install Storm. In brew-speak, you need to *tap* the Storm homebrew formulas

```shell
brew tap moves-rwth/storm
```

Then, installing Storm is as easy as

```shell
brew install stormchecker
```

This will install Storm and all necessary and some recommended dependencies. More options provided by the package can be seen by invoking

```shell
brew info stormchecker
```

After installing the package, you should directly be able to invoke

```shell
storm
```

and continue with the guide on how to [run Storm]({{ site.baseurl }}/documentation/usage/running-storm.html).

## Building Storm from source

This guide helps you building a standard version of storm. There are plenty of configuration options, please check our [configuration guide](documentation/installation/configuration-guide.html) if you want to build a non-standard version. Most notably, you will have to set additional options if you want to include solvers that are not shipped with Storm (for example Gurobi or MathSAT). However, the defaults should be suitable in most cases.

### Obtaining the source code

The source code can be downloaded from [GitHub](https://github.com/moves-rwth/storm). You can either clone the git repository
```shell
git clone https://github.com/moves-rwth/storm.git
```
or download a zip archive with the latest snapshot of the master branch:
```shell
wget https://github.com/moves-rwth/archive/master.zip
unzip master.zip
```
In the following, we will use `STORM_DIR` to refer to the root directory of Storm. If you want, you can set an environment variable to ease the following steps via
```shell
export STORM_DIR=<path to storm root>
```

### Configuration step

Switch to the directory `STORM_DIR` and create a build folder that will hold all files related to the build (in other words, building is done out-of source, in-source builds are discouraged and are likely to break). Finally change to the `build` directory.

```shell
cd STORM_DIR
mkdir build
cd build
```

Then, use cmake to configure the build of Storm on your system by invoking

```shell
cmake ..
```

Check the output carefully for errors and warnings. If all requirements are properly installed and found, you are ready to build Storm and move to the next step. In case of errors, check the [requirements](requirements.html), consult the [troubleshooting guide](troubleshooting.html) and, if necessary, [file an issue](documentation/installation/troubleshooting.html#file-an-issue).

### Build step

If the configuration step went smoothly, the compilation step should run through. Feel free to use the compilation time for a coffee or stroll through the park.

To compile just Storm's main command line interface, enter

```shell
make storm-main
```

{:.alert .alert-info}
If you have multiple cores at your disposal and at least 8GB of memory, you can execute
`make storm-main -j${NUMBER_OF_CORES}` to speed up compilation. You will still be able to get the coffee, no worries.

To build all binaries of Storm, use

```shell
make all
```

For an overview of the available binaries and for which tasks you need them, please have a look at our guide on [running Storm](running-storm.html#binaries)

### Adding Storm to your path

This step is optional. If you want to be able to run Storm from anywhere, you may want to add it to your path (in the tutorial on how to [run Storm](#running-storm) this is assumed). You can do so, by

```shell
export PATH=$PATH:$STORM_DIR/build/bin
```

where `$STORM_DIR` is the environment variable set [earlier](#obtaining-the-source-code).

### Test step

While this step is optional, we recommend to execute it to verify that Storm produces correct results on your platform. Invoking

```shell
make check
```

will build and run the tests.

In case of errors, please do not hesistate to [file an issue](troubleshooting.html#file-an-issue).
