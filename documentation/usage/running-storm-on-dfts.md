---
title: Running Storm on DFTs
layout: default
documentation: true
category_weight: 6
categories: [Usage]
---

{% include includes/toc.html %}


## Running Storm on DFTs

The support for fault tree analysis is bundled in the `storm-dft` binary which should be created during the [build]({{ site.github.url }}/documentation/installation/installation.html#building-storm-from-source). The input of Dynamic fault trees (DFTs) is given in the [Galileo format](languages.html#dfts).

#### Example 1 (Fault tree analysis of a motorbike)

We consider the example of a motorbike with a front wheel and a rear wheel. When one of the wheels fails, the spare wheel can be used as a replacement. If more than one wheel fails, the whole motorbike fails. The fault tree for the motor bike is given in the following.

{% include includes/show_model.html name="DFT file of motorbike" class="dft_motorbike" path="dft/motorbike.dft" %}

For the fault tree analysis two measures are of importance: *reliability* and *mean-time-to-failure (MTTF)*. The reliability at time t=1 can be computed with the following call:

```console
$ storm-dft -dft motorbike.dft --timebound 1
```

{% include includes/show_output.html class="dft_motorbike_timebound" path="dft/motorbike_timebound.out" %}

At time t=1 the probability of having a failure of the motorbike is 35.78%.

For a better indication of the reliability over time multiple time points can be computed at once:
```console
$ storm-dft -dft motorbike.dft --timepoints 1 5 0.1
```

{% include includes/show_output.html class="dft_motorbike_timepoints" path="dft/motorbike_timepoints.out" %}

Here the reliability for all time points $$1+i\cdot0.1, i \in \{0, ..., 40\}$$ is computed.

The second measure MTTF computes the expected time of the failure of the motorbike.

```console
$ storm-dft -dft motorbike.dft -mttf
```

{% include includes/show_output.html class="dft_motorbike_mttf" path="dft/motorbike_mttf.out" %}

The motorbike has an expected lifetime of 1.61.

#### Example 2 (Fault tree analysis of the Hypothetical Example Computer System (HECS))

In this example we consider the *Hypothetical Example Computer System (HECS)* from the NASA handbook on fault trees. It models a computer system consisting of a processor unit with two processors and a spare processor, a memory unit with 5 memory slots, an operator interface consisting of hardware and software, and a 2-redundant bus.

{% include includes/show_model.html name="DFT file of HECS" class="dft_hecs" path="dft/hecs.dft" %}

We compute the MTTF again.

```console
$ storm-dft -dft motorbike.dft -mttf
```

{% include includes/show_output.html class="dft_hecs_mttf" path="dft/hecs_mttf.out" %}

As the system is larger, it makes sense to use *symmetry reduction* as an optimization via `-symred`.

```console
$ storm-dft -dft motorbike.dft -mttf -symred
```

{% include includes/show_output.html class="dft_hecs_mttf_symred" path="dft/hecs_mttf_symred.out" %}

This reduces the model size drastically and improves the analysis time.

For reliability another optimization can be enabled: *modularisation* via `--modularisation`.

```console
$ storm-dft -dft motorbike.dft --timebound 500 -symred --modularisation
```

{% include includes/show_output.html class="dft_hecs_timebound_mod" path="dft/hecs_timebound_mod.out" %}

Here independent subtrees are analysed separately and the results are combined yielding the total reliability.
