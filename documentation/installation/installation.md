---
title: Installation
layout: default
documentation: true
category_weight: 1
categories: [Installation]
---

# Obtaining storm

We currently provide three ways of obtaining and running storm.

- [Homebrew](#homebrew) (**new!**)
- a [virtual machine](#virtual-machine)
- [source code](#building-storm-from-source)

If you just want to run storm and you want to run it natively on your machine, then we recommend installing it via [Homebrew](#homebrew). If you just want to have a peek at what storm can do and you cannot use the Homebrew installation, then downloading the VM is the best choice as it comes with the necessary dependencies preinstalled and let's you just run the tool. However, if you want or need to make changes to the storm code base, you have to obtain the source code and [build it](#building-storm-from-source) yourself. While this is not always easy, we spent some effort on making this process easy. Please also consult our list of [requirements](documentation/installation/requirements) to see whether building storm on your system promises to be successful.

## Homebrew

If you are running a version of macOS that is newer than Mavericks, you can use [Homebrew](http://brew.sh/), a tool that provides easy access to packages. Once you have installed Homebrew, you need to *tap* the storm Homebrew formulas

```shell
brew tap moves-rwth/storm
```

to make Homebrew aware of how to install storm. Then, installing storm is as easy as

```shell
brew install stormchecker
```

This will install storm and all necessary and some recommended dependencies. More options provided by the package can be seen by invoking

```shell
brew info stormchecker
```

After installing the package, you should directly be able to invoke

```shell
storm
```

and continue with the guide on how to [invoke storm](#running-storm).

## Virtual Machine

The virtual machine image can be found [here](https://rwth-aachen.sciebo.de/index.php/s/nthEAQL4o49zkYp).

{:.alert .alert-info}
The virtual machine is hosted at sciebo, an academic cloud hoster. We are not able to trace the identity of downloaders, so reviewers can use this link without revealing their identity.

When you have downloaded the OVA image, you can import it into, for example, [VirtualBox](link) and run it. The username and password are both *storm* and a `README` file is provided in the home folder of the user *storm*. In the virtual machine, storm is installed into `/home/storm/storm` and the binaries can be found in `/home/storm/storm/build/bin`. For your convenience, an environment variable with the name `STORM_DIR` is set to the path containing the binaries and this directory is added to the `PATH`, meaning that you can run the storm binaries from any location in the terminal and that `cd $STORM_DIR` will take you to the folders containing storm's binaries. For more information on how to run storm, please see [below](#running-storm).

The VM is periodically updated to include bug fixes, new versions, etc. You can find the history of updates [here](documentation/installation/vmchangelog.html).

## Building storm from source

This guide helps you building a standard version of storm. There are plenty of configuration options, please check our [configuration guide](documentation/installation/configuration-guide.html) if you want to build a non-standard version. Most notably, you will have to set additional options if you want to include solvers that are not shipped with storm (for example Gurobi or MathSAT). However, the defaults should be suitable in most cases.

### Obtaining the source code

The source code can be downloaded from [GitHub](https://github.com/moves-rwth/storm). You can either clone the git repository
```shell
git clone https://github.com/moves-rwth/storm.git STORM_DIR
```
or download a zip archive with the latest snapshot of the master branch:
```shell
wget https://github.com/moves-rwth/archive/master.zip
unzip master.zip
```
In the following, we will use `STORM_DIR` to refer to the root directory of storm. If you want, you can set an environment variable to ease the following steps via
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

Then, use cmake to configure the build of storm on your system by invoking

```shell
cmake ..
```

Check the output carefully for errors and warnings. If all requirements are properly installed and found, you are ready to build storm and move to the next step. In case of errors, check the [requirements](documentation/installation/requirements.html), consult the [troubleshooting guide](documentation/installation/troubleshooting) and, if necessary, [file a bug](documentation/installation/troubleshooting.html#file-an-issue).

### Build step

If the configuration step went smoothly, the compilation step should run through. Feel free to use the compilation time for a coffee or stroll through the park.

To compile just storm's main command line interface, enter

```bash
make storm-main
```

{:.alert .alert-info}
If you have multiple cores at your disposal and at least 8GB of memory, you can execute
`make storm-main -j${NUMBER_OF_CORES}` to speed up compilation. You will still be able to get the coffee, no worries.

### Other Binaries

If you are interested in one of the other binaries, replace `storm-main` with the appropriate target:

|------------------------|----------------+----------------|
| purpose                | target         | binary         |
|------------------------|----------------+----------------|
| PRISM, JANI, explicit  | storm-main     | storm          |
| DFTs                   | storm-dft-cli  |                |
| GSPNs                  | storm-gspn-cli |                |
| cpGCL                  | storm-pgcl-cli |                |
|------------------------|----------------+----------------|

### Adding storm to your path

This step is optional. If you want to be able to run storm from anywhere, you may want to add storm to your path (in the tutorial on how to [run storm](#running-storm) this is assumed). You can do so, by

```shell
export PATH=$PATH:$STORM_DIR/build/bin
```

### Test step

While this step is optional, we recommend to execute it to verify that storm produces correct results on your platform. Invoking

```shell
make check
```

will build and run the tests.

In case of errors, please do not hesitate to [notify us](documentation/installation/troubleshooting.html#file-an-issue).
