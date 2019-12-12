---
title: Benchmarks
navigation_weight: 5
layout: default
---

On this page, you can find (extended) benchmark results accompanying paper submissions.

### The Probabilistic Model Checker Storm (2019)[^1]

#### Setup
The benchmarks were run on 4 cores of an Intel Xeon Platinum 8160 Processor with 12GB of memory available. The timeout was 1800 seconds.

#### Benchmarks
All benchmarks from [QComp 2019](http://qcomp.org/competition/2019/index.html) were considered, except for 4 PTA Benchmarks that were not compatible with Storm.

#### Results
<a target="_blank" href="https://moves-rwth.github.io/storm-benchmark-logs/docs/2019-12/table.html" class="btn btn-primary btn-md active" role="button">Show interactive result table</a>
<a target="_blank" href="https://doi.org/10.5281/zenodo.3571210" class="btn btn-primary btn-md active" role="button">Download raw data and replication package</a>


### QComp 2019[^2]

Storm participated in the *2019 Comparison of Tools for the Analysis of Quantitative Formal Models (QComp 2019)*.
Details on the competition, the participating tools and the QComp benchmark set can be found on the [competition website](http://qcomp.org/competition/2019/index.html){:target="_blank"}.
Detailed results are available in the [interactive results table](http://qcomp.org/competition/2019/results/index.html){:target="_blank"}.

### A Storm is Coming: A Modern Probabilistic Model Checker (2017)[^3]

#### Setup

The benchmarks were conducted on a HP BL685C G7. All tools had up to eight cores with 2.0GHz and 8GB of memory available, but only the Java garbage collection of PRISM and EPMC used more than one core. The timeout was set to 1800 seconds.

#### Benchmarks

In this paper, we use all non-PTA models from the [PRISM benchmark suite](http://www.prismmodelchecker.org/benchmarks/){:target="_blank"} and the [IMCA](https://github.com/buschko/imca){:target="_blank"} benchmarks for MAs.

#### Results
In order to share the results, we provide them as a set of log files. To make the results more accessible, we also give four tables (one for each model type: DTMC, CTMC, MDP and MA). Using the buttons near the top of the table, you can select which of the configurations of the tools are displayed side-by-side (by default all configurations of participating tools are shown). For example, clicking `Storm-sparse` toggles whether Storm's sparse engine participates in the comparison or not. As argued in the paper, it makes sense to compare "matching engines" of the tools. (For an explanation which engines are comparable, please consult the paper.) This is why there are also buttons that let you select the tool configurations that are comparable with one click (`Show sparse`, `Show hybrid`, `Show dd` and `Show exact`). The best time for each instance is marked green. By clicking on the time for a particular experiment, you are taken to the log file of that experiment.

{:.alert .alert-warning}
The log files were obtained using a version of Storm that represents "infinity" as "-1" in `--exact` mode. This is,
however just a displaying issue. Newer versions of Storm correctly display "inf" as the result.

<a target="_blank" href="https://moves-rwth.github.io/storm-benchmark-logs/docs/index_dtmc.html" class="btn btn-primary btn-md active" role="button">Show DTMC table</a>
<a target="_blank" href="https://moves-rwth.github.io/storm-benchmark-logs/docs/index_ctmc.html" class="btn btn-primary btn-md active" role="button">Show CTMC table</a>
<a target="_blank" href="https://moves-rwth.github.io/storm-benchmark-logs/docs/index_mdp.html" class="btn btn-primary btn-md active" role="button">Show MDP table</a>
<a target="_blank" href="https://moves-rwth.github.io/storm-benchmark-logs/docs/index_ma.html" class="btn btn-primary btn-md active" role="button">Show MA table</a>
<a target="_blank" href="https://www.github.com/moves-rwth/storm-benchmark-logs/" class="btn btn-primary btn-md active" role="button">Show log files</a>

### References

[^1]: C. Hensel, S. Junges, J.-P. Katoen, T. Quatmann, M. Volk: The Probabilistic Model Checker Storm. To appear.

[^2]: [E. M. Hahn, A. Hartmanns, C. Hensel, M. Klauck, J. Klein, J. Křetínský, D. Parker, T. Quatmann, E. Ruijters, M. Steinmetz: *The 2019 Comparison of Tools for the Analysis of Quantitative Formal Models*, 2019](https://doi.org/10.1007/978-3-030-17502-3_5){:target="_blank"}

[^3]: [C. Dehnert, S. Junges, J.-P. Katoen, M. Volk: *A Storm is Coming: A Modern Probabilistic Model Checker*, 2017](https://arxiv.org/abs/1702.04311){:target="_blank"}
