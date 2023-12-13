---
title: Running Storm
layout: default
documentation: true
category_weight: 1
categories: [Use Storm]
---

<h1>Running Storm</h1>

{% include includes/toc.html %}

## Storm's executables

Storm takes [many languages]({{ '/documentation/background/languages.html' | relative_url }}) as input. For some of these formats, domain-specific information is available and domain-specific properties are of key importance. Others are generic and require more flexibility in terms of options. As a result, there are several binaries that are each dedicated to a specific portion of the input languages. The following table gives an overview of the available executables, the targets that need to be built when compiling [Storm from source]({{ '/documentation/obtain-storm/build.html#build-step' | relative_url }}) and the documentation for each executable.

<table class="table table-striped table-hover">
  <thead>
    <tr class="bg-primary"><td>input language(s)</td><td>executable</td><td>make target</td><td>Documentation</td></tr>
  </thead>
  <tbody>
    <tr>
        <td><a href="{{ '/documentation/background/languages.html#prism' | relative_url }}">PRISM</a>, <a href="{{ '/documentation/background/languages.html#jani' | relative_url }}">JANI</a>, <a href="{{ '/documentation/background/languages.html#explicit' | relative_url }}">explicit</a></td>
        <td>storm</td>
        <td>storm-main</td>
        <td><a href="running-storm.html#running-storm-on-prism-jani-or-explicit-input">Running Storm on PRISM, JANI or explicit input</a></td>
    </tr>
    <tr>
        <td><a href="{{ '/documentation/background/languages.html#prism' | relative_url }}">PRISM</a>, <a href="{{ '/documentation/background/languages.html#jani' | relative_url }}">JANI</a>, <a href="{{ '/documentation/background/languages.html#explicit' | relative_url }}">explicit</a></td>
        <td>storm-pars</td>
        <td>storm-pars-cli</td>
        <td><a href="running-storm-on-parametric-models">Running Storm on parametric models</a></td>
    </tr>
    <tr>
        <td><a href="{{ '/documentation/background/languages.html#dfts' | relative_url }}">DFTs</a>
        </td><td>storm-dft</td>
        <td>storm-dft-cli</td>
        <td><a href="running-storm-on-dfts.html">Running Storm on DFTs</a></td>
    </tr>
    <tr>
        <td><a href="{{ '/documentation/background/languages.html#gspns' | relative_url }}">GSPNs</a></td>
        <td>storm-gspn</td>
        <td>storm-gspn-cli</td>
        <td><a href="running-storm.html#running-storm-on-gspns">Running Storm on GSPNs</a></td>
    </tr>
  </tbody>
</table>

Consequently, our guide on how to run Storm is structured accordingly. For every executable we illustrate the usage by one (or more) example(s).

## First steps

Many of Storm's executables have many options, only a fraction of which are covered in this guide. If you want to explore these options, invoke the executable with the `--help [hint]` option. If a hint is given, only those options are shown that match it.

Before we get started, let us check whether everything is set up properly. In all command-line examples we assume that the executables are in your PATH and can therefore be invoked without prefixing them with their path. If you installed Storm via [Homebrew]({{ '/documentation/obtain-storm/homebrew.html' | relative_url }}), this is automatically the case; if you built Storm yourself, you have to [manually add it to your PATH]({{ '/documentation/obtain-storm/build.html#adding-storm-to-your-path-optional' | relative_url }}). Typing

```console
$ storm
```

should produce output similar to

```text
Storm 1.0.0

ERROR (cli.cpp:309): No input model.
ERROR (storm.cpp:39): An exception caused Storm to terminate. The message of the exception is: No input model.
```

Of course your version may differ, but the general picture should be the same. In particular, `storm` should complain about a missing input model. More information about the Storm version can be obtained by providing `--version`.

## Running Storm on PRISM, JANI or explicit input

These input languages can be treated by Storm's main executable `storm`. Storm supports various [properties]({{ '/documentation/background/properties.html' | relative_url }}). They can be passed to Storm by providing the `--prop <properties> <selection>` switch. The `<properties>` argument can be either a property as a string or the path to a file containing the properties. The `<selection>` argument is optional. If set, it can be used to indicate that only certain properties of the provided ones are to be checked. More specifically, this argument is either "all" or a comma-separated list of [names of properties]({{ '/documentation/background/properties.html#naming-properties' | relative_url }}) and/or property indices. Note that named properties cannot be indexed by name, but need to be referred to by their name.

### Running Storm on PRISM input

[PRISM]({{ '/documentation/background/languages.html#prism' | relative_url }}) models can be provided with the `--prism <path/to/prism-file>` option.

#### Example 1 (Analysis of a PRISM model of the Knuth-Yao die)

In our first example, we are going to analyze a small [DTMC]({{ '/documentation/background/models.html#discrete-time-markov-chains-dtmcs' | relative_url }}) in the PRISM format. More specifically, the model represents a protocol to simulate a six-sided die with the use of a fair coin only. The model and more information can be found at the [PRISM website](http://www.prismmodelchecker.org/casestudies/dice.php){:target="_blank"}, but for your convenience, you can view the model and the download link below.

{% include includes/show_model.html name="PRISM model of Knuth-Yao die" class="prism_die_dtmc" path="prism/die.pm" %}

From now on, we will assume that the model file is stored as `die.pm` in the current directory. Let us start with a simple (exhaustive) exploration of the state space of the model:

```console
$ storm --prism die.pm
```

{% include includes/show_output.html class="prism_die_dtmc_output_exploration" path="prism/die_exploration.out" %}

This will tell you that the model is a [sparse]({{ '/documentation/background/engines.html#sparse' | relative_url }}) [discrete-time Markov chain]({{ '/documentation/background/models.html#discrete-time-markov-chains-dtmcs' | relative_url }}) with 13 states and 20 transitions, no reward model and two labels (`deadlock` and `init`). But wait, doesn't the PRISM model actually specify a reward model? Why does `storm` not find one? The reason is simple, `storm` doesn't build reward models or (custom) labels that are not referred to by properties unless you explicitly want all of them to be built:

```console
$ storm --prism die.pm --buildfull
```

{% include includes/show_output.html class="prism_die_dtmc_output_exploration_buildfull" path="prism/die_exploration_buildfull.out" %}

This gives you the same model, but this time there is a reward model `coin_flips` attached to it. Unless you want to know how many states satisfy a custom label, you can let `storm` take care of generating the needed reward models and labels. Note that by default, the model is stored in an *sparse matrix* representation (hence the `(sparse)` marker after the model type). There are other formats supported by Storm; please look at the [engines guide]({{ '/documentation/background/engines.html' | relative_url }}) for more details.

Now, let's say we want to check whether the probability to roll a one with our simulated die is as we'd expect. As the protocol states the simulated die shows a one if it ends up in a state where `s=7&d=1`, we formulate a reachability property like this:

```console
$ storm --prism die.pm --prop "P=? [F s=7&d=1]"
```

{% include includes/show_output.html class="prism_die_dtmc_output_prob_one" path="prism/die_prob_one.out" %}

This will tell us that the probability for rolling a one is actually (very close to) 1/6.

{:.alert .alert-info}
If, from the floating point figure, you are not convinced that the result is actually 1 over 6, try to additionally provide the `--exact` flag.

Congratulations, you have now checked your first property with Storm! Now, say we are interested in the probability of rolling a one, provided that one of the outcomes "one", "two" or "three" were obtained, we can obtain this figure by using a conditional probability formula like this

```console
$ storm --prism die.pm --prop "P=? [F s=7&d=1 || F s=7&d<4]"
```

{% include includes/show_output.html class="prism_die_dtmc_output_prob_one_conditional" path="prism/die_prob_one_conditional.out" %}

which tells us that this probability is 1/3. So far the model seems to simulate a proper six-sided die! Finally, we are interested in the expected number of coin flips that need to be made until the simulated die returns an outcome:

```console
$ storm --prism die.pm --prop "R{\"coin_flips\"}=? [F s=7]"
```

{% include includes/show_output.html class="prism_die_dtmc_output_expected_coinflips" path="prism/die_expected_coinflips.out" %}

`storm` tells us that -- on average -- we will have to flip our fair coin 11/3 times. Note that we had to escape the quotes around the reward model name in the property string. If the property is placed within a file, there is no need to escape them.

{:.alert .alert-info}
More information on how to define properties can be found [here]({{ '/documentation/background/properties.html' | relative_url }}){:.alert-link}.

#### Example 2 (Analysis of a PRISM model of an asynchronous leader election protocol)

In this example, we consider another model available from the [PRISM website](http://www.prismmodelchecker.org/casestudies/asynchronous_leader.php){:target="_blank"}: the asynchronous leader election protocol.

{% include includes/show_model.html name="PRISM model of asynchronous leader election protocol" class="prism_async_leader_mdp" path="prism/leader4.nm" %}

Just like in [Example 1](#example-1-analysis-of-a-prism-model-of-the-knuth-yao-die), we will assume that the file `leader4.nm` is located in the current directory.

As the name of the protocol suggests, it is supposed to elect a leader among a set of communicating agents (four in this particular instance). Well, let's see whether it lives up to it's name and check that almost surely (i.e. with probability 1) a leader will be elected eventually.

```console
$ storm --prism leader4.nm --prop "P>=1 [F (s1=4 | s2=4 | s3=4 | s4=4) ]"
```

{% include includes/show_output.html class="prism_async_leader_mdp_eventually_elected" path="prism/async_leader_eventually_elected.out" %}

Apparently this is true. But what about the performance of the protocol? The property we just checked does not guarantee any upper bound on the number of steps that we need to make until a leader is elected. Suppose we have only 40 steps and want to know what's the probability to elect a leader *within this time bound*.

```console
$ storm --prism leader4.nm --prop "P=? [F<=40 (s1=4 | s2=4 | s3=4 | s4=4) ]"
```

{% include includes/show_output.html class="prism_async_leader_mdp_bounded_eventually_elected_error" path="prism/async_leader_bounded_eventually_elected_error.out" %}

Likely, Storm will tell you that there is an error and that for nondeterministic models you need to specify whether minimal or maximal probabilities are to be computed. Why is that? Since the model is a [Markov Decision Process]({{ '/documentation/background/models.html#discrete-time-markov-decision-processes-mdps' | relative_url }}), there are (potentially) nondeterministic choices in the model that need to be resolved. Storm doesn't know how to resolve them unless you tell it to either minimize or maximize (w.r.t. the probability of the objective) whenever there is a nondeterministic choice.

```console
$ storm --prism leader4.nm --prop "Pmin=? [F<=40 (s1=4 | s2=4 | s3=4 | s4=4) ]"
```

{% include includes/show_output.html class="prism_async_leader_mdp_bounded_eventually_elected" path="prism/async_leader_bounded_eventually_elected.out" %}

Storm should tell you that this probability is 0.3828125. So what does it mean? It means that even in the worst of all cases, so when every nondeterministic choice in the model is chosen to minimize the probability to elect a leader quickly, then we will elect a leader within our time bound in about 49 out of 128 cases.

{:.alert .alert-info}
For [nondeterministic models (MDPs and MAs)]({{ '/documentation/background/models.html#models-with-nondeterminism' | relative_url }}){:.alert-link}, you will have to specify in which direction the nondeterminism is going to be resolved.


### Multi-objective Model Checking of MDPs and Markov Automata

Storm supports multi-objective model checking: In nondeterministic models, different objectives might require different choices of actions in order to satisfy the property.
This induces trade-offs between different strategies. Multi-objective model checking reveals such trade-offs by computing the Pareto curve. An example is given below.

#### Example 3 (Pareto Curves)
Consider an instance of stochastic job scheduling, where a number of jobs with exponential run time needs to be handled by a number of servers.
On one hand, we are interested in reducing the expected time until all jobs are done. On the other hand, we want to maximize the probability that a first
batch of jobs has been treated within an hour. This yields a trade-off, as the optimal strategy for minimizing the expected time is to run the slowest jobs first.

The trade-off for 12 jobs and 3 servers is depicted by the following curve:

![Pareto Curve]({{ '/pics/multi-objective.png' | relative_url }} 'Pareto Curve'){: .img-thumbnail .col-sm .center-image width="300"}

{% include includes/show_model.html name="PRISM file for stochastic job scheduling (3 jobs and 2 servers)" class="job_sched_file" path="ma/jobs03_2.ma" %}

Again, we assume that the file `jobs03_2.ma` is located in the current directory.
We obtain the data for tha Pareto plot as above by the following call:

```console
$ storm --prism jobs03_2.ma --prop "multi(Tmin=? [ F \"all_jobs_finished\"], Pmax=? [ F<=(N/(4*K)) \"half_of_jobs_finished\"])" --multiobjective:precision 0.01 --multiobjective:exportplot plot/
```

`--prop` now contains a multi-objective query with two dimensions: A call for the expected time, and a maximum time-bounded probability.
Notice that for Markov automata, the algorithm necessarily can only approximate the result: `--multiobjective:precision` reflects the area that remains undecided.
`--multiobjective:exportplot plot/` specifies that the directory `plot` will contain the Pareto-optimal points in a CSV format. The plot can be generated from this file.

More benchmarks and further details for multi-objective model checking can also be found in [this](https://github.com/tquatmann/multiobjective-ma) repository.

### Running Storm on JANI input

[JANI]({{ '/documentation/background/languages.html#jani' | relative_url }}) models can be provided with the `--jani <path/to/jani-file>` option.
In case you also want to read the properties from the jani file, the option `--janiproperty` needs to be added.

#### Example 4 (Analysis of a rejection-sampling algorithm to approximate $$\pi$$)

Here, we are going to analyze a model of an algorithm that approximates $$\pi$$. It does so by repeated sampling according to a uniform distribution. While this model is a JANI model, the original model was written in the probabilistic guarded command language (pGCL) and has been translated to JANI. The JANI model and the original pGCL code is available from the [JANI models repository](https://github.com/ahartmanns/jani-models){:target="_blank"}.

{% include includes/show_model.html name="original pGCL program" class="jani_approxpi_pgcl" path="jani/approx_pi_00100_010_full.pgcl" %}

{% include includes/show_model.html name="JANI model of rejection-sampling algorithm" class="jani_approxpi_jani" path="jani/approx_pi_00100_010_full.jani" %}

Again, we will assume that the file `approx_pi_00100_010_full.jani` is located in the current directory. Let's see how many states the underlying MDP has. For the sake of illustration, we are going to use the [hybrid engine]({{ '/documentation/background/engines.html#hybrid' | relative_url }}) for this example.

```console
$ storm --jani approx_pi_00100_010_full.jani --engine hybrid
```

{% include includes/show_output.html class="jani_approxpi_jani_output_exploration" path="jani/approxpi_exploration.out" %}

As we selected the *hybrid* engine, Storm builds the MDP in terms of a symbolic data structure ((MT)BDDs), hence the `(symbolic)` marker. For this representation, Storm also reports the sizes of the state and transition DDs in terms of the number of nodes.

The algorithm uses a sampling-based technique to approximate $$\pi$$. More specifically, it repeatedly (100 times in this particular instance) samples points in a square and checks whether they are in a circle whose diameter is the edge length of the square (which is called a hit). From this, we can derive $$\pi \approx 4 \frac{hits}{100}$$ (for more details, we refer to [this explanation](https://theclevermachine.wordpress.com/tag/rejection-sampling/){:target="_blank"}). We are therefore interested in the expected number of hits until termination of the algorithm. The program has a transient Boolean variable `_ret0_` that marks termination of the pGCL program; this transient variable can be used as a label in properties:

```console
$ storm --jani approx_pi_00100_010_full.jani --engine hybrid --prop "Rmax=? [F \"_ret0_\"]"
```
{% include includes/show_output.html class="jani_approxpi_jani_output_expected_hits" path="jani/approxpi_expected_hits.out" %}

Plugging this value in our formula yields $$\pi \approx 4 \frac{hits}{100} = 4 \frac{72.60088512}{100} \approx 2.90404$$. Of course, this is a crude approximation, but it can be refined by adjusting the size of the circle and the number of samples (see the other instances of this model in the [JANI models repository](https://github.com/ahartmanns/jani-models/tree/master/ApproxPi/NaiveRejectionSampling){:target="_blank"}).

### Running Storm on Benchmarks from the Quantitative Verification Benchmark Set

To run Storm on a benchmark from the [Quantitative Verification Benchmark Set (QVBS)](http://qcomp.org/benchmarks){:target="_blank"} (say, for example, the model [jobs](http://qcomp.org/benchmarks/index.html#jobs){:target="_blank"}, type:

```console
$ storm --qvbsroot QCOMP_DIR/benchmarks --qvbs jobs
```
{% include includes/show_output.html class="qvbs_jobs" path="qvbs/jobs.out" %}

Here, we assume that the [QComp git repository](https://github.com/ahartmanns/qcomp){:target="_blank"} has been cloned into the directory `QCOMP_DIR`. If you built Storm from [source]({{ '/documentation/obtaion-storm/build.html' | relative_url }}), you can also set the [cmake option]({{ '/documentation/obtain-storm/manual-configuration' | relative_url }}) `-DSTORM_LOAD_QVBS=ON` which will automatically download the complete benchmark set during the build step, allowing you to omit the `--qvbsroot QCOMP_DIR/benchmarks` command line option.

The above command checks all available properties for the first instance of the model [jobs](http://qcomp/org/benchmarks/index.html#jobs){:target="_blank"}.
The model checking call is equivalent to invoking Storm on the respective JANI file.
The output also indicates the other available instances and the available properties. You can append an instance index and a comma seperated list of property names to the command above:

```console
$ storm --qvbsroot QCOMP_DIR/benchmarks --qvbs jobs 2 completiontime,avgtime
```
{% include includes/show_output.html class="qvbs_jobs_1_completiontime_avgtime" path="qvbs/jobs_1_completiontime_avgtime.out" %}

### Running Storm on explicit input

Sometimes, it is convenient to specify your model in terms of an explicit enumeration of states and transitions (for example if your model is generated by another tool). For this, Storm offers the [explicit input format]({{ '/documentation/background/languages.html#explicit' | relative_url }}). Models in this format consist of (at least) two files and can be provided with the `--explicit <path/to/tra-file> <path/to/lab-file>` option. Additionally, the options `--staterew <path/to/state-rewards-file>` and `--transrew <path/to/transition-rewards-file>` can be used to specify state and transition rewards.

#### Example 4 (Analysis of an explicit model of the Knuth-Yao die)

Here, we take the same input model as for [Example 1](#example-1-analysis-of-a-prism-model-of-the-knuth-yao-die) of the PRISM input section. However, this time the input model is given in the explicit format.

{% include includes/show_model.html name="explicit transition file of Knuth-Yao die" class="explicit_die_dtmc_tra" path="explicit/die.tra" %}

{% include includes/show_model.html name="explicit label file of Knuth-Yao die" class="explicit_die_dtmc_lab" path="explicit/die.lab" %}

{% include includes/show_model.html name="explicit (transition) reward file of Knuth-Yao die" class="explicit_die_dtmc_rew" path="explicit/die.tra.rew" %}

Again, we assume that all three files are located in the current directory. We proceed analogously to the example in the PRISM input section and start by loading the model:

```console
$ storm --explicit die.tra die.lab --transrew die.tra.rew
```

{% include includes/show_output.html class="explicit_die_dtmc_exploration" path="explicit/die_exploration.out" %}

Note that in contrast to the PRISM input model, the explicit version of the Knuth-Yao die does not have symbolic variables. Therefore, we need to rephrase the properties from before. Computing the probability of rolling a one thus becomes

```console
$ storm --explicit die.tra die.lab --transrew die.tra.rew --prop "P=? [F \"one\"]"
```

{% include includes/show_output.html class="explicit_die_dtmc_output_prob_one" path="explicit/die_prob_one.out" %}

{:.alert .alert-danger}
Unlike for PRISM and JANI models, there is no `--exact` mode for explicit input as it's already imprecise because of floating point numbers.

Note that the model defines two labels `one` and `done` that can be used in properties. For reward properties, the only difference in property specification is that formulae must not refer to a specific reward model but rather to the implicit default one. This is because in the explicit input format, every model can have only up to one reward model and there is no way to specify its name in the input format. Consequently, computing the expected number of coin flips that are necessary until the simulated die has terminated with an outcome in {1, ..., 6} can be done like this:

```console
$ storm --explicit die.tra die.lab --transrew die.tra.rew --prop "R=? [F \"done\"]"
```

{% include includes/show_output.html class="explicit_die_dtmc_output_expected_coinflips" path="explicit/die_expected_coinflips.out" %}

## Running Storm on GSPNs

The binary `storm-gsnp` handles [Generalized Stochastic Petri Nets (GSPNs)]({{ '/documentation/background/languages.html#gspns' | relative_url }}).
GSPNs can be analysed by first converting them to the [JANI format]({{ '/documentation/background/languages.html#jani' | relative_url }}) and then analyzing the JANI model as [shown before](#running-storm-on-jani-input).

### Running Storm on Pnpro input

#### Example 6 (Analysis of a GSPN)

We start by parsing a GSPN given in the [pnpro format]({{ '/documentation/background/languages.html#greatspn-editor-projects' | relative_url }}) used by the GreatSPN editor.
The GSPN models four dining philosophers.

{% include includes/show_model.html name="GSPN model of dining philosophers" class="gspn_dining_philosophers" path="gspn/philosophers.pnpro" %}

We convert the GSPN into a JANI file with the following command:

```console
$ storm-gspn --gspnfile philosophers.pnpro --to-jani philosophers.jani --addprops --capacity 1
```

The argument `--capacity <value>` specifies the maximal capacity for all places.
Alternatively, it is possible to provide a file specifying the capacity for each place with `--capacitiesfile <filename>`.
The flag `-addprops` automatically adds some properties which are commonly used in Petri net analysis.

On the resulting JANI file we compute for example the maximal probability of reaching a deadlock within 10 time units:

```console
$ storm --jani philosophers.jani --constants TIME_BOUND=10 --janiproperty MaxPrReachDeadlockTB
```

{% include includes/show_output.html class="gspn_philosophers_output_deadlock_tb" path="gspn/philosophers_deadlock_tb.out" %}

For additional command-line options see the help for GSPNs with:

```console
$ storm-gspn --help gspn
```
