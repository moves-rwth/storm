---
title: DFTs for Vehicle Guidance Systems
category_weight: 6
layout: default
categories: [Publications]
---

On this page, you can find supplementary material for the dynamic fault tree analysis of vehicle guidance systems.

## Safety Analysis for Vehicle Guidance Systems with Dynamic Fault Trees

### Models

The generated DFTs for all 8 scenarios I-VIII are available (in anonymized form) on our [GitHub page](https://github.com/moves-rwth/dft-examples){:target="_blank"}.
The DFTs are provided in a JSON format and can be visualized using our [DFT tool](https://moves-rwth.github.io/dft-gui/).

### Analysis

The fault tree analysis is performed by [Storm](https://github.com/moves-rwth/storm/).

The computation of the failure probability within for example time `10,000` can be performed with the following call:
```console
$ storm-dft -dftjson sc_1.json --firstdep -symred --timebound 100000
```

Similarily, the computation of the mean-time-to-failure (MTTF) is computed by:
```console
$ storm-dft -dftjson sc_1.json --firstdep -symred -mttf
```

For more details on the DFT analysis with Storm we refer to our [documentation]({{ site.github.url }}/documentation/usage/running-storm-on-dfts.html).

