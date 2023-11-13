---
title: Running Storm on Parametric Models
layout: default
documentation: true
category_weight: 3
categories: [Use Storm]
---

<h1>Running Storm on Parametric Models</h1>

{% include includes/toc.html %}

Many model descriptions contain constants which may be specified only by an interval of possible values. We refer to these constants as parameters. Such parametric models are supported by the binary `storm-pars`. A gentle overview is given in {% cite JJK22 %}.

Overall, for parametric systems, various questions (henceforth: queries) relating to model checking can be asked. These different queries are supported by different engines in `storm-pars`: We refer to this as the mode -- and the mode must always be specified.

The current support focusses on five modes, of which two are more experimental:
1. Feasibility -- Finding parameter values such that a property holds.
2. Verification -- Proving that for all parameter values in some region, a property holds.
3. Partitioning -- Finding regions for which the verification problem holds.
4. Solution Function computation -- Finding a closed-form representation of a solution function, mapping parameter values to, e.g., reachability probabilities.
5. Monotonicity analysis (experimental) -- Checking whether the solution function is monotonic in one of the parameters.
6. Sampling (experimental) -- Quickly sampling parametric models for different parameter valuations.

We outline these modes in more detail below.
In what follows, Storm typically assumes that all occurring parameters are graph-preserving, that is, they do not influence the topology of the underlying Markov model.

To illustrate the functionality, we use a bounded retransmission protocol. This example is an adaption of the Bounded Retransmission Protocol from the [PRISM website](http://www.prismmodelchecker.org/casestudies/brp.php){:target="_blank"}. Here, we have two channels whose reliabilities are represented by parameters `pL` and `pK`.

{% include includes/show_model.html name="parametric version of the Bounded Retransmission Protocol" class="parametric_brp" path="prism/brp.pm" %}

## Feasibility
Storm currently supports two feasibility engines:
1. Parameter lifting, or just pla
2. Gradient Descent, or just gd

While we work towards a unified interface, these support different options.

### Feasibility with pla

The pla engine is based on work in {% cite JAHJKQV19 %}. The feasibility computation is partially based on work in {% cite SJK21 %}.

```console
$ storm-pars  --mode feasibility --feasibility:method pla --prism brp.pm  --prop 'P<=0.01 [F s=5]' --region "0.1 <= pL <= 0.9, 0.1 <= pK <= 0.9"
```
We use the `--regions` option to set the parameter region. In this case, the equivalent region could have been given using the `--regionbound 0.1`, which restricts every parameter to be bounded between `regionbound` and 1-`regionbound`.

The complete output to be expected is:

{% include includes/show_output.html class="brp_feasibility_pla" path="parametric/brp_feasibility_pla.out" %}

In particular, the output says

```console
Result at initial state: 947822915828163/288230376151711744 ( approx. 0.003288421326) at [pK=17/20, pL=17/20].
```

which means that setting pK and pL to 17/20 yields a Markov chain such that `P<=0.01 [F s=5]` is satisfied.

Instead of giving a bound, pla also support optimization:

```console
$ storm-pars  --mode feasibility --feasibility:method pla --prism brp.pm  --prop 'P=? [F s=5]' --direction min --region "0.1 <= pL <= 0.9, 0.1 <= pK <= 0.9" --guarantee 0.0001 abs
```

In this case, we do not give a bound and specify with `--direction min` that we want to minimize the reachability probability.
Additionally, we give a `--guarantee 0.0001 abs`, specifying that we want to find a solution which is within 0.0001 of the optimal value, where the distance is measured in absolute terms.

The complete output to be expected is:

{% include includes/show_output.html class="brp_feasibility_pla_guarantee" path="parametric/brp_feasibility_pla_guarantee.out" %}

In particular, the output says

```console
Result at initial state: 5282881788238101/9223372036854775808 ( approx. 0.0005727711912) at [pK=287/320, pL=287/320].
```

This output indicates not only how to instantiate pK and pL to obtain `P=? [F s=5] = approx. 0.0005727711912`, but also that this value is within 0.0001 from the absolute lower bound (within the given region).

### Feasibility with gd

The gd engine is based on work in {% cite HSJMK22 %}.

```console
$ storm-pars  --mode feasibility --feasibility:method gd --prism brp.pm  --prop 'P<=0.01 [F s=5]'
```
In this case, the region for searching cannot be given.

The complete output to be expected is:

{% include includes/show_output.html class="brp_feasibility_pla" path="parametric/brp_feasibility_pla.out" %}

In particular, the output says

```console
Result at initial state: 0.001706273496 ( approx. 0.001706273496) at [pK=62679533432486317/72057594037927936, pL=62679533432486317/72057594037927936].
```

 which gives values for pK and pL such that `P<=0.01 [F s=5]`  is satisfied.

## Verification
To prove the absence of a solution, we can use parameter lifting. A comprehensive overview on the theoretical backgrounds is given in {% cite JAHJKQV19 %}.

```console
$ storm-pars  --mode verification --prism brp.pm  --prop 'P<=0.9999 [F s=5]' --region "0.1 <= pL <= 0.9, 0.1 <= pK <= 0.9"
```
will provide output

{% include includes/show_output.html class="brp_verification_valid" path="parametric/brp_verification_valid.out" %}

In contrast, running
```console
$ storm-pars  --mode verification --prism brp.pm  --prop 'P<=0.99 [F s=5]' --region "0.1 <= pL <= 0.9, 0.1 <= pK <= 0.9"
```
will yield output that says that the statement is *not* true:

{% include includes/show_output.html class="brp_verification_invalid" path="parametric/brp_verification_invalid.out" %}

To find a counterexample, one can use the feasibility mode above.


## Parameter Space Partitioning

The verification result may be too coarse: Potentially, we do want to find subregions that satisfy the property.
While we generally advocate to use our (Python) API for flexible support of such scenarios, the Storm command-line interface does yield some support:

```console
$ storm-pars  --mode partitioning --prism brp.pm  --prop 'P<=0.99 [F s=5]' --region "0.1 <= pL <= 0.9, 0.1 <= pK <= 0.9"
```

This produces the following output:

{% include includes/show_output.html class="brp_partitioning" path="parametric/brp_partitioning.out" %}

We can see that for two parameters, Storm even visulalizes the acquired data to give an initial understanding of the region partition. To cover more (or less) parts of the parameter space, use `--partitioning:terminationCondition 0.01` to say that 99% of the parameter space should be covered.


## Computing the exact solution function

We can run Storm to obtain closed-form solutions, i.e. a rational function that represents, e.g., the probability with which the given property is satisfied for certain parameter valuations.  A comprehensive overview on the theoretical backgrounds is given in {% cite JAHJKQV19 %}.

```console
$ storm-pars --mode solutionfunction  --prism brp.pm  --prop 'P=? [F s=5]'  
```

The result is an expression over the parameter pL and pK:

```console
(-1 * (pK^10*pL^10+(-120)*pK^7*pL^7+(-10)*pK^9*pL^9+45*pK^8*pL^8+210*pK^6*pL^6+(-250)*pK^5*pL^5+(-100)*pK^3*pL^3+25*pK^2*pL^2+200*pK^4*pL^4+(-1)))/(1)
```

{% include includes/show_output.html class="closed_form" path="parametric/closed_form.out" %}

## Monotonicity analysis

Storm can also check if the parameters of a model are monotonic in regard to a certain property {% cite SJK19 SJK21 %}. For this, we use the `monotonicity` mode.


```console
$ ./storm-pars  --mode monotonicity  --prism brp.pm  --prop 'P=? [F s=5]' --bisimulation
```

We use `--bisimulation` to apply further model simplification and significantly reduce the workload of the model checker.
This results in the following output:

{% include includes/show_output.html class="brp_mon" path="parametric/brp_mon.out" %}

In particular, the output specifies that both pL and pK are monotonically decreasing.

If we want to take a look at the reachability order that is built during monotonicity checking, we can add the option `--mon:dotOutput` followed by an optional filename. Storm will then write a dot output of the order into the file which can then be processed using, e.g., [Graphviz](https://graphviz.org/). In this case, it results in the following graph:

![Reachability Order]({{ '/pics/parametric-brp-mon-dot.png' | relative_url }} "Reachability Order"){: .center-image width="300"}
