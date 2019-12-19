---
title: Getting Started
navigation_weight: 2
layout: default
---


## 1. Obtain Storm

To be able to run Storm, you need to obtain it and run it on your system.
Currently, you can choose one of the following options:

* Build Storm [from source](documentation/obtain-storm/build.html),
* Install Storm via a supported package manager
	* [Homebrew](documentation/obtain-storm/homebrew.html)
	* [AUR](https://aur.archlinux.org/packages/stormchecker-git/) (thanks to [Sascha Wunderlich](https://www.saschawunderlich.de/university/))
* Use a [Docker container](documentation/obtain-storm/docker.html)
* Use a [virtual machine](documentation/obtain-storm/vm.html)

## 2. Prepare a model checking query

After you have obtained Storm, you need to make sure that your input model has the right form. That is, on a fundamental level, you need to ensure that your input model falls into one of the [model types](documentation/background/models.html) supported by Storm.

If your model indeed does, then the next thing is to have the model available in an [input language](documentation/background/languages.html) of Storm. If you don't have such a model yet, you need to first model the system you are interested in or transcribe it from a different input format.

However, the input model is only "half" of the input you need to provide, the other "half" being the property you want to verify. Please consult our [guide to properties](documentation/background/properties.html) for details on how to specify them.

An extensive list of example models and properties is available at the [Quantitative Verification Benchmark Set](https://qcomp.org/benchmarks).


## 3. Run Storm

Finally, if both the input model as well as the property are captured in an appropriate format, then you are ready to run Storm!

Our [guide](documentation/usage/running-storm.html) illustrates how you can do so. Since the calls (and even the binaries) you need to invoke depend on the input language for each of the input languages, the guide shows how to run Storm depending on the input you have.
