---
title: Running Storm on Parametric Models
layout: default
documentation: true
category_weight: 3
categories: [Use Storm]
---

<h1>Running Storm on Parametric Models</h1>

{% include includes/toc.html %}


Many model descriptions contain constants which may be specified only by an interval of possible values. We refer to these constants as parameters. Such parametric models are supported by the binary `storm-pars`.
In what follows, we assume that all occurring parameters are graph-preserving, that is, they do not influence the topology of the underlying Markov model.

A comprehensive overview on the theoretical backgrounds is given in {% cite JAHJKQV19 %}.

## Computing the exact solution function

We can run storm to obtain closed-form solutions, i.e. a rational function that represents, e.g., the probability with which the given property is satisfied for certain parameter valuations.

### Example 1 (Obtaining a closed-form solution of the parametric Knuth-Yao die)

The following model is an adaption of the Knuth-Yao die used in previous examples.
The probability of flipping heads is now given by a parameter `p`.

{% include includes/show_model.html name="parametric version of Knuth-Yao die" class="parametric_knuth_yao" path="prism/parametric_die.pm" %}

We can consider the probability of a die roll reflecting an outcome of one.

```console
$ storm-pars --prism parametric-die.pm --prop "P=? [F s=7&d=1]"
```

The result is an expression over the parameter p.

{% include includes/show_output.html class="closed_form_parametric_models" path="parametric/closed_form.out" %}

## Parameter lifting for region refinement
For bounded properties we can use *parameter lifting* to partition a given region into safe, unsafe and ambiguous subregions.

### Example 2 (Region refinement for a Bounded retransmission protocol)
This example is an adaption of the Bounded Retransmission Protocol from the [PRISM website](http://www.prismmodelchecker.org/casestudies/brp.php){:target="_blank"}. Here, we have two channels whose reliabilities are represented by parameters `pL` and `pK`.

{% include includes/show_model.html name="parametric version of the Bounded Retransmission Protocol" class="parametric_brp" path="prism/brp.pm" %}

We want to split our region into those subregions that result in a chance for a successful transmission that is greater than 0.5 and those that do not.

```console
$ storm-pars --prism brp.pm --prop "P>0.5 [F s=5]" --region "0.1 <= pL <= 0.9, 0.1 <=pK <=0.9" --refine 0.01 10
```

We use the `--regions` option to set the parameter region.
The arguments we give to `--refine` are the coverage threshold and the depth limit. The coverage threshold decides how precisely we want to partition our region. The value  0.01 means that Storm will continue to refine until only 1% of the given region remains ambiguous while the rest has been determined to be either safe or unsafe. The depth limit is an optional second argument. If the level at which regions are split reaches this bound, the region is not refined any further, regardless of the coverage achieved so far.

This produces the following output:

{% include includes/show_output.html class="pla_parametric_models" path="parametric/brp_pla.out" %}

We can see that for two parameters, Storm even visulalizes the acquired data to give an initial understanding of the region partition.

Further options on this can be found with the following two queries:

```console
$ storm-pars --help region
$ storm-pars --help parametric
```

## Monotonicity analysis

Storm can also check if the parameters of a model are monotonic in regard to a certain property. For this, we use the `--monotonicity-analysis` option.

### Example 3 (Checking the monotonicity of a parameter)
Again, we use the BRP model and check if any of the parameters are monotonic:

```console
$ storm-pars --prism brp.pm --prop 'P=? [F s=5]' --region '0.1 <= pL <= 0.9, 0.1 <=pK <=0.9' --monotonicity-analysis --bisimulation
```
We use `--bisimulation` to apply further model simplification and significantly reduce the workload of the modelchecker.
This results in the following output:

{% include includes/show_output.html class="mon_parametric_models" path="parametric/brp_mon.out" %}

If we want to take a look at the reachability order that is built during monotonicity checking, we can add the option `--mon:dotOutput` followed by an optional filename. Storm will then write a dot output of the order into the file which can then be processed using, e.g., [Graphviz](https://graphviz.org/). In this case, it results in the following graph:

![Reachability Order]({{ '/pics/parametric-brp-mon-dot.png' | relative_url }} "Reachability Order"){: .center-image width="300"}

For checking the monotonicity on samples, we use `--mon:samples` and provide it with the number of samples we want to be taken.

For more options on monotonictiy checking and their usages, we can use the `--help` option:

```console
$ storm-pars --help mon
```

## Computing extrema
Another feature Storm offers is the computations of extrema for a property within a given region.

### Example 4 (Extremum computation for a bounded retransmission protocol)
We reuse our BRP model from earlier to compute the maximum probability for a successful transmission that is possible in the given region.

```console
$ storm-pars --prism brp.pm --prop "P=? [F s=5]" --region "0.1 <= pL <= 0.9, 0.1 <= pK <= 0.9" --extremum max 0.01
```
We use `--extremum` and give it first a direction, in this case `max` to compute the maximum, and then a precision with which we want the maximum to be computed.

The result is the extremum as well as the specific valuation with which it was achieved.

{% include includes/show_output.html class="extremum_parametric_models" path="parametric/brp_extremum.out" %}

More relevant options can be found via

```console
$ storm-pars --help region
$ storm-pars --help parametric
```