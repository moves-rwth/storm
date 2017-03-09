---
title: Running Storm
layout: default
documentation: true
category_weight: 4
categories: [Usage]
---

{% include toc.html %}

## Storm's executables

Storm takes [many languages](languages.html) as input. For some of these formats, domain-specific information is available and domain-specific properties are of key importance. Others are generic and require more flexibility in terms of options. As a result, there are several binaries that are each dedicated to a specific portion of the input languages. The following table gives an overview of the available executables and the targets that need to be built when compiling [Storm from source]({{ site.baseurl }}/documentation/installation/installation.html#build-step).

<table class="table table-striped table-hover">
  <thead>
    <tr class="bg-primary"><td>input language(s)</td><td>executable</td><td>make target</td></tr>
  </thead>
  <tbody>
    <tr><td><a href="languages.html#prism">PRISM</a>, <a href="languages.html#jani">JANI</a>, <a href="languages.html#explicit">explicit</a></td><td>storm</td><td>storm-main</td></tr>
    <tr><td><a href="languages.html#dfts">DFTs</a></td><td>storm-dft</td><td>storm-dft-cli</td></tr>
    <tr><td><a href="languages.html#gspns">GSPNs</a></td><td>storm-gspn</td><td>storm-gspn-cli</td></tr>
    <tr><td><a href="languages.html#cpgcl">cpGCL</a></td><td>storm-pgcl</td><td>storm-pgcl-cli</td></tr>
  </tbody>
</table>

Consequently, our guide on how to run Storm is structured accordingly. For every executable we illustrate the usage by one (or more) example(s).

## First steps

Many of Storm's executables have many options, only a fraction of which are covered in this guide. If you want to explore these options, invoke the executable with the `--help [hint]` option. If a hint is given, only those options are shown that match it.

Before we get started, let us check whether everything is set up properly. In all command-line examples we assume that this executable is in your PATH and can therefore be invoked without prefixing it with its path. Therefore, typing

```shell
storm
```

should produce output similar to

```shell
Storm 0.9.9

ERROR (cli.cpp:309): No input model.
ERROR (storm.cpp:39): An exception caused storm to terminate. The message of the exception is: No input model.
```

Of course, your version may differ, but the general picture should be the same. In particular, `storm` should complain about a missing input model. More information about the Storm version can be obtained by providing `--version`.

## Running Storm on PRISM, JANI or explicit input

These input languages can be treated by Storm's main executable `storm`.

### Running Storm on PRISM input

PRISM models can be provided with the `--prism <path/to/prism-file>` option.

{::options parse_block_html="true" /}
<div class="panel-group">
<div class="panel panel-default">
<div class="panel-heading">
<a href="#demo" data-toggle="collapse">
Show full example
</a>
<div class="pull-right">
<a href="#demo" data-toggle="collapse"><span class="glyphicon glyphicon-menu-down" aria-hidden="true"></span></a>
</div>
</div>
<div class="panel-collapse collapse" id="demo">
<div class="panel-body">
## Panel content
</div>
</div>  
</div>
</div>

### Specifying models

Depending on its type, the model needs to be provided via one of the following switches.

- `--prism <prism-file>`
- `--jani <jani-file>`
- `--explicit <tra-file> <lab-file>`

### Specifying Properties

Storm supports various [properties](properties.html). They can be passed to Storm by providing the `--prop <properties> <selection>` switch. The `<properties>` argument can be either a property as a string or the path to a file containing the properties. The `<selection>` argument is optional. If set, it can be used to indicate that only certain properties of the provided ones are to be checked. More specifically, this argument is either "all" or a comma-separated list of [names of properties](properties.html#naming-properties) and/or property indices. Note that named properties cannot be indexed by name, but need to be referred to by their name.


# Running storm

We will now discuss some examples to get you started. While they should illustrate how the tool is run, there are many more features and options to explore. For more details, be sure to check the [usage](documentation/usage/usage.html) and consult the help message of Storm. In this tutorial, we assume that `storm` is in your `PATH` (if you installed Storm via [Homebrew](#homebrew) this is already the case; if you built Storm yourself, you need to add it [manually](#adding-storm-to-your-path)).

### Example 1 (Analysis of a PRISM model of the Knuth-Yao die)

For this example, we assume that you obtained the corresponding PRISM model from the single die version of the model from the [PRISM website](http://www.prismmodelchecker.org/casestudies/dice.php) and that the model file is stored as `die.pm` in the current directory. You can download the file [here](http://www.prismmodelchecker.org/casestudies/examples/die.pm). If you are not familiar with this model, we recommend you to read the description on the PRISM website.

Let us start with a simple (exhaustive) exploration of the state space of the model.

```shell
storm --prism die.pm
```

This will tell you that the model is a [discrete-time Markov chain](models) with 13 states and 20 transitions, no reward model and two labels (`deadlock` and `init`). But wait, doesn't the PRISM model actually specify a reward model? Why does `storm` not find one? The reason is simple, `storm` doesn't build reward models or (custom) labels that are not referred to by properties unless you explicitly want all of them to be built:

```shell
storm --prism die.pm --buildfull
```

This gives you the same model, but this time there is a reward model `coin_flips` attached to it. Unless you want to know how many states satisfy a custom label, you can let `storm` take care of generating the needed reward models and labels. Note that by default, the model is stored in an sparse matrix representation (hence the `(sparse)` marker after the model type). There are other formats supported by Storm; please look at the [usage guide](documentation/usage/usage.html) for more details.

Now, let's say we want to check whether the probability to roll a one with our simulated die is as we'd expect. For this, we specify a reachability property

```shell
storm --prism die.pm --prop "P=? [F s=7&d=1]"
```

This will tell us that the probability for rolling a one is actually (very close to) 1/6. If, from the floating point figure, you are not convinced that the result is actually 1 over 6, try to provide the `--exact` flag. Congratulations, you have now checked your first property with `storm`! Now, say we are interested in the probability of rolling a one, provided that one of the outcomes "one", "two" or "three" were obtained, we can obtain this figure by using a conditional probability formula like this

```shell
storm --prism die.pm --prop "P=? [F s=7&d=1 || F s=7&d<4]"
```

which tells us that this probability is 1/3. So far the model seems to simulate a proper six-sided die! Finally, we are interested in the expected number of coin flips that need to be made until the simulated die returns an outcome:

```shell
storm --prism die.pm --prop "R{\"coin_flips\"}=? [F s=7]"
```

`storm` tells us that -- on average -- we will have to flip our fair coin 11/3 times. Note that we had to escape the quotes around the reward model name in the property string. If the property is placed within a file, there is no need to escape them.

### Example 2 (Analysis of a PRISM model of an asynchronous leader election protocol)

In this example, we consider the model described [here](http://www.prismmodelchecker.org/casestudies/asynchronous_leader.php). One particular instance of this model can be downloaded from [here](http://www.prismmodelchecker.org/casestudies/examples/leader4.nm). Just like in Example 1, we will assume that the file `leader4.nm` is located in the current directory.

As the name of the protocol suggests, it is supposed to elect a leader among a set of communicating agents. Well, let's see whether it lives up to it's name and check that almost surely (i.e. with probability 1) a leader will be elected eventually.

```shell
storm --prism leader4.nm --prop "P>=1 [F (s1=4 | s2=4 | s3=4 | s4=4) ]"
```

Apparently this is true. But what about the performance of the protocol? The property we just checked does not guarantee any upper bound on the number of steps that we need to make until a leader is elected. Suppose we have only 40 steps and want to know what's the probability to elect a leader *within this time bound*.

```shell
storm --prism leader4.nm --prop "P=? [F<=40 (s1=4 | s2=4 | s3=4 | s4=4) ]"
```

Likely, Storm will tell you that there is an error and that for nondeterministic models you need to specify whether minimal or maximal probabilities are to be computed. Why is that? Since the model is a [Markov Decision Process](documentation/usage/models.html), there are (potentially) nondeterministic choices in the model that need to be resolved. Storm doesn't know how to resolve them unless you tell it to either minimize or maximize (wrt. to the probability of the objective) whenever there is a nondeterministic choice.

```shell
storm --prism leader4.nm --prop "Pmin=? [F<=40 (s1=4 | s2=4 | s3=4 | s4=4) ]"
```

Storm should tell you that this probability is 0.375. So what does it mean? It means that even in the worst of all cases, so when every nondeterministic choice in the model is chosen to minimize the probability to elect a leader quickly, then we will elect a leader within our time bound in about 3 out of 8 cases.

{:.alert .alert-info}
For [nondeterministic models (MDPs and MAs)](documentation/usage/models.html), you will have to specify in which direction the nondeterminism is going to be resolved.

## Running Storm on DFTs

{:.alert .alert-info}
Coming soon.

## Running Storm on GSPNs

{:.alert .alert-info}
Coming soon.

## Running Storm on cpGCL

{:.alert .alert-info}
Coming soon.
