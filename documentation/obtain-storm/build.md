---
title: Build from Source
layout: default
documentation: true
category_weight: 1
categories: [Obtain Storm]
---

<h1>Build Storm from Source</h1>

{% include includes/toc.html %}

This guide shows you how to obtain the source code of Storm and build it.
This installation method is recommended if you either

* want to make changes to the Storm code base,
* can not use a supported package manager ([Homebrew](homebrew.html), [AUR](https://aur.archlinux.org/packages/stormchecker-git/)), or
* need to avoid overhead from virtualization due to [Docker](docker.html) or [VM](vm.html) usage.

While compiling the source code is not always a breeze (depending on your operating system), we spent some effort on making this process as easy as possible.


## Supported Operating Systems

Currently, we provide support for

- <i class="fa fa-apple" aria-hidden="true"></i> macOS 10.12 "Sierra" and higher on either x86- or [ARM-based](apple-silicon.html) CPUs
- <i class="icon-debian"></i> Debian 9 "Stretch" and higher
- <i class="icon-ubuntu"></i> Ubuntu 16.10 "Yakkety Yak" and higher

which are known to enable the easy installation of Storm. Other Linux distributions are likely to work too, but it may take significant effort to get the required versions of the dependencies up and running. For example, thanks to [Joachim Klein](http://www.inf.tu-dresden.de/index.php?node_id=1473){:target="_blank"}, there is a [script]({{ '/resources/scripts/installation/storm-build-debian-jessie.sh' | relative_url }}) that installs Storm and some crucial dependencies on Debian 8 "Jessie".

{:.alert .alert-danger}
Note that in particular <i class="icon-ubuntu"></i>Ubuntu 16.04 "Xenial Xerus" is *not* supported anymore as the shipped GCC version is too old.

{:.alert .alert-danger}
For ARM-based <i class="fa fa-apple" aria-hidden="true"></i> Apple Silicon CPUs [further steps are necessary](apple-silicon.html){:.alert-link}.

## Dependencies

We are going to assume that all necessary [dependencies](dependencies.html) have been installed on the machine in default locations so they can be found by our build machinery.
 
If you just want to run Storm and you want to run it natively on your machine, then we recommend installing it via a package manager or using the [Docker container](docker.html). However, if you want or need to make changes to the Storm code base, you have to obtain the source code and build it yourself. While this is not always a breeze (depending on your operating system), we spent some effort on making this process as easy as possible.

## Obtaining the Source Code

The source code of the latest stable release can be downloaded from [GitHub](https://github.com/moves-rwth/storm/releases/latest){:target="_blank"}. You can either clone the git repository
```console
$ git clone -b stable https://github.com/moves-rwth/storm.git
```
or download a zip archive with the latest stable release:
```console
$ wget https://github.com/moves-rwth/storm/archive/stable.zip
$ unzip stable.zip
```
or obtain one of the archived versions available at [![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.1181896.svg)](https://doi.org/10.5281/zenodo.1181896)
 
{:.alert .alert-info}
If you want the most recent version of Storm rather than the stable version, you can replace the `-b stable` with `-b master` when cloning the repository or use the archive [https://github.com/moves-rwth/storm/archive/master.zip](https://github.com/moves-rwth/storm/archive/master.zip){:.alert-link}, respectively.

In the following, we will use `STORM_DIR` to refer to the root directory of Storm. If you want, you can set an environment variable to ease the following steps via
```console
$ export STORM_DIR=<path to Storm root>
```


{:.alert .alert-danger}
Before proceeding with the following steps, make sure that you have set up all [dependencies as required](dependencies.html){:.alert-link}.

## Configuration Step

Below, we are going to build a standard version of Storm. There are plenty of configuration options, please check our [configuration guide](manual-configuration.html) if you want to build a non-standard version. Most notably, you will have to set additional options if you want to include solvers that are not shipped with Storm (for example Gurobi or MathSAT). However, the defaults should be suitable in most cases.

Switch to the directory `STORM_DIR` and create a build folder that will hold all files related to the build (in other words, building is done out-of source, in-source builds are strongly discouraged and are likely to break). Finally change to the `build` directory.

```console
$ cd $STORM_DIR
$ mkdir build
$ cd build
```

Then, use cmake to configure the build of Storm on your system by invoking

```console
$ cmake ..
```

Check the output carefully for errors and warnings. If all dependencies are properly installed and found, you are ready to build Storm and move to the next step. In case of errors, check the [dependencies](dependencies.html), consult the [troubleshooting guide](troubleshooting.html) and, if necessary, [file an issue](troubleshooting.html#file-an-issue).

## Build Step

If the configuration step went smoothly, the compilation step should run through. Feel free to use the compilation time for a coffee or stroll through the park.

To compile all of Storm's binaries including all tests, enter

```console
$ make
```

To only build the binaries (available from version 1.1 on), enter

```console
$ make binaries
```

{:.alert .alert-info}
If you just want to compile Storm's main command-line interface, typing `make storm-main` suffices. To see which targets you need to build, we refer to the table of [available executables]({{ '/documentation/usage/running-storm.html#storms-executables' | relative_url }}){:.alert-link}.

{:.alert .alert-info}
If you have multiple cores at your disposal and at least 8GB of memory, you can execute
`make storm-main -j${NUMBER_OF_CORES}` to speed up compilation. You will still be able to get the coffee, no worries.

## Adding Storm to your Path <span class="label label-info">optional</span>

If you want to be able to run Storm from anywhere, you may want to add it to your path (in the tutorial on how to [run Storm]({{ '/documentation/usage/running-storm.html' | relative_url }}) this is assumed). You can do so, by

```console
$ export PATH=$PATH:$STORM_DIR/build/bin
```

where `$STORM_DIR` is the environment variable set [earlier](#obtaining-the-source-code).

## Test Step <span class="label label-info">optional</span>

We recommend to execute it to verify that Storm produces correct results on your platform. Invoking

```console
$ make check
```

will build and run the tests. In case of errors, please do not hesitate to [file an issue](troubleshooting.html#file-an-issue).
