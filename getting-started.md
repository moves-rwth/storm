---
title: Getting Started
navigation_weight: 2
layout: default
---

This document shows you the steps to get started with storm.

- list
{:toc}

# Obtaining storm

We currently provide two ways of running storm. 

- a Virtual Machine image 
- source code

If you just want to have a peek at what Storm can do, the easiest way is to download the VM as it comes with the necessary dependencies preinstalled and let's you just run the tool. If, however, performance in terms of time and memory consumption is an issue or you want to implement something on top of Storm, you need to obtain the source code and [build it](#building-storm-from-source) yourself. While this is not always easy, we spent some effort on making this process easy. Please also consult our list of [requirements](documentation/installation/requirements) to see whether building Storm on your system promises to be successful. 

## Obtaining the VM

The virtual machine image can be found [here](https://rwth-aachen.sciebo.de/index.php/s/nthEAQL4o49zkYp).

{:.alert .alert-info}
The virtual machine is hosted at sciebo, an academic cloud hoster. We are not able to trace the identity of downloaders, so reviewers can use this link without revealing their identity.

When you have downloaded the OVA image, you can import it into, for example, [VirtualBox](link) and run it. The username and password are both *storm* and a `README` file is provided in the home folder of the user *storm*. In the virtual machine, Storm is installed into `/home/storm/storm` and the binaries can be found in `/home/storm/storm/build/bin`. For your convenience, an environment variable with the name `STORM_DIR` is set to the path containing the binaries and this directory is added to the `PATH`, meaning that you can run the Storm binaries from any location in the terminal and that `cd $STORM_DIR` will take you to the folders containing Storm's binaries. For more information on how to run Storm, please see [below](#running-storm). 

The VM is periodically updated to include bug fixes, new versions, etc. You can find the history of updates [here](documentation/installation/vmchangelog.html).

## Obtaining the source code

The source code can be downloaded from [github](https://github.com/moves-rwth/storm). You can either clone the git repository
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

# Building storm from source

This guide helps you building a standard version of storm. There are plenty of configuration options, please check our [configuration guide](documentation/installation/configuration-guide.html) if you want to build a non-standard version. Most notably, you will have to set additional options if you want to include solvers that are not shipped with storm (for example Gurobi or MathSAT). However, the defaults should be suitable in most cases.

## Configuration step

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

## Build step

If the configuration step went smoothly, the compilation step should run through. Feel free to use the compilation time for a coffee or stroll through the park.

To compile just storm's main command line interface, enter

```bash
make storm-main
```

{:.alert .alert-info}
If you have multiple cores at your disposal and at least 8GB of memory, you can execute 
`make storm-main -j${NUMBER_OF_CORES}` to speed up compilation. You will still be able to get the coffee, no worries.

If you are interested in one of the other binaries, replace `storm-main` with the appropriate target:

|-------------|----------------+----------------|
| purpose     | target         | binary         |
|-------------|----------------+----------------|
| DFTs        | storm-dft-cli  |                |
| GSPNs       | storm-gspn-cli |                |
| cpGCL       | storm-pgcl-cli |                |
|-------------|----------------+----------------|


## Test step (optional)

While this step is optional, we recommend to execute it to verify that storm produces correct results on your platform. Invoking

```shell
make check
```

will build and run the tests. 

In case of errors, please do not hesistate to [notify us](documentation/installation/troubleshooting.html#file-an-issue).

# Running Storm

Congratulations, you are now ready to run storm!

We will now discuss some examples to get you started. While they should illustrate how the tool is run, there are many more features and options to explore. For more details, be sure to check the [usage](documentation/usage/usage.html) and consult the help message of Storm.

## Standard model checking

For this task, you built the `storm-main` target and therefore produced the `storm` binary. So let's make sure that you can run storm:

```shell
cd STORM_DIR/build/bin
./storm
```

If the storm binary was correctly built and is correctly located in the directory `STORM_DIR/build/bin`, this should produce output similar to

```shell
Storm
---------------

version: 0.10.1 (+398 commits) build from revision ga183b72.

Linked with GNU Linear Programming Kit v4.60.
Linked with Microsoft Z3 Optimizer v4.5 Build 0 Rev 0.
Linked with MathSAT5 version 5.3.14 (0b98b661254c) (Nov 17 2016 11:27:06, gmp 5.1.3, clang/LLVM 6.0, 64-bit).
Linked with CARL.
Command line arguments: 
Current working directory: /Users/chris/work/storm4/build_xcode

ERROR (cli.cpp:306): No input model.
ERROR (storm.cpp:39): An exception caused Storm to terminate. The message of the exception is: No input model.
```

Of course, your version, the git revision and the linked libraries are likely to differ, but the general picture should be the same. In particular, `storm` should complain about a missing input model. 

### Example 1 (Analysis of a PRISM model of the Knuth-Yao die)

For this example, we assume that you obtained the corresponding PRISM model from the single die version of the model from the [PRISM website](http://www.prismmodelchecker.org/casestudies/dice.php) and that the model file is stored as `die.pm` in `STORM_DIR/build/bin`. You can download the file [here](http://www.prismmodelchecker.org/casestudies/examples/die.pm). If you are not familiar with this model, we recommend you to read the description on the PRISM website.

Let us start with a simple (exhaustive) exploration of the state space of the model.

```shell
./storm --prism die.pm
```

This will tell you that the model is a [discrete-time Markov chain](models) with 13 states and 20 transitions, no reward model and two labels (`deadlock` and `init`). But wait, doesn't the PRISM model actually specify a reward model? Why does `storm` not find one? The reason is simple, `storm` doesn't build reward models or (custom) labels that are not referred to by properties unless you explicitly want all of them to be built:

```shell
./storm --prism die.pm --buildfull
```

This gives you the same model, but this time there is a reward model `coin_flips` attached to it. Unless you want to know how many states satisfy a custom label, you can let `storm` take care of generating the needed reward models and labels. Note that by default, the model is stored in an sparse matrix representation (hence the `(sparse)` marker after the model type). There are other formats supported by Storm; please look at the [usage guide](documentation/usage/usage.html) for more details.

Now, let's say we want to check whether the probability to roll a one with our simulated die is as we'd expect. For this, we specify a reachability property

```shell
./storm --prism die.pm --prop "P=? [F s=7&d=1]"
```

This will tell us that the probability for rolling a one is actually (very close to) 1/6. If, from the floating point figure, you are not convinced that the result is actually 1 over 6, try to provide the `--exact` flag. Congratulations, you have now checked your first property with `storm`! Now, say we are interested in the probability of rolling a one, provided that one of the outcomes "one", "two" or "three" were obtained, we can obtain this figure by using a conditional probability formula like this

```shell
./storm --prism die.pm --prop "P=? [F s=7&d=1 || F s=7&d<4]"
```

which tells us that this probability is 1/3. So far the model seems to simulate a proper six-sided die! Finally, we are interested in the expected number of coin flips that need to be made until the simulated die returns an outcome:

```shell
./storm --prism die.pm --prop "R{\"coin_flips\"}=? [F s=7]"
```

`storm` tells us that -- on average -- we will have to flip our fair coin 11/3 times.

### Example 2 (Analysis of a PRISM model of an asynchronous leader election protocol)

In this example, we consider the model described [here](http://www.prismmodelchecker.org/casestudies/asynchronous_leader.php). One particular instance of this model can be downloaded from [here](http://www.prismmodelchecker.org/casestudies/examples/leader4.nm). Just like in Example 1, we will assume that the file `leader4.nm` is located in `STORM_DIR/build/bin`.

As the name of the protocol suggests, it is supposed to elect a leader among a set of communicating agents. Well, let's see whether it lives up to it's name and check that almost surely (i.e. with probability 1) a leader will be elected eventually.

```shell
./bin/Debug/storm --prism leader4.nm --prop "P>=1 [F (s1=4 | s2=4 | s3=4 | s4=4) ]"
```

Apparently this is true. But what about the performance of the protocol? The property we just checked does not guarantee any upper bound on the number of steps that we need to make until a leader is elected. Suppose we have only 40 steps and want to know what's the probability to elect a leader *within this time bound*.

```shell
./bin/Debug/storm --prism leader4.nm --prop "P=? [F<=40 (s1=4 | s2=4 | s3=4 | s4=4) ]"
```

Likely, Storm will tell you that there is an error and that for nondeterministic models you need to specify whether minimal or maximal probabilities are to be computed. Why is that? Since the model is a [Markov Decision Process](documentation/usage/models.html), there are (potentially) nondeterministic choices in the model that need to be resolved. Storm doesn't know how to resolve them unless you tell it to either minimize or maximize (wrt. to the probability of the objective) whenever there is a nondeterministic choice.

```shell
./bin/Debug/storm --prism leader4.nm --prop "Pmin=? [F<=40 (s1=4 | s2=4 | s3=4 | s4=4) ]"
```

Storm should tell you that this probability is 0.375. So what does it mean? It means that even in the worst of all cases, so when every nondeterministic choice in the model is chosen to minimize the probability to elect a leader quickly, then we will elect a leader within our time bound in about 3 out of 8 cases.

{:.alert .alert-info}
For [nondeterministic models (MDPs and MAs)](documentation/usage/models.html), you will have to specify in which direction the nondeterminism is going to be resolved.

## DFT Analysis

{:.alert .alert-danger}
The tutorial on DFT analysis is currently under development.
