---
title: DFTs for Vehicle Guidance Systems
category_weight: 6
layout: default
categories: [Publications]
---

On this page, you can find supplementary material for the dynamic fault tree analysis of vehicle guidance systems.

## Safety Analysis for Vehicle Guidance Systems with Dynamic Fault Trees

### Models

The generated DFTs for all 8 scenarios I-VIII are available (in anonymized form) on our [GitHub page](https://github.com/moves-rwth/dft-examples/tree/master/case_studies/iso26262_vehicle_guidance_system){:target="_blank"}.
The DFTs are provided in a custom JSON format.

The DFTs can be visualized using our [DFT GUI](https://moves-rwth.github.io/dft-gui/){:target="_blank"}.
In the GUI, first the corresponding JSON file must be selected and then it can be visualized via the `Load` button.
Further information on the DFT GUI can be found in the [documentation](https://github.com/moves-rwth/dft-gui/blob/master/doc/user_manual.md){:target="_blank"}.

### Analysis

The fault tree analysis is performed by [Storm](https://github.com/moves-rwth/storm/){:target="_blank"}.

The computation of the failure probability within for example time `10,000` can be performed with the following call:
```console
$ storm-dft -dftjson sc_1.json --firstdep --timebound 100000
```
(Note that symmetry reduction is applied by default from Storm version 1.5.0 on.
In earlier versions, the flag `-symred` needs to be provided.)

Similarly, the computation of the mean-time-to-failure (MTTF) can be performed with:
```console
$ storm-dft -dftjson sc_1.json --firstdep -mttf
```

For more details on analysing DFTs with Storm we refer to our [documentation]({{ '/documentation/usage/running-storm-on-dfts.html' | relative_url }}).

