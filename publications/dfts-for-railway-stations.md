---
title: DFTs for Railway Stations
category_weight: 6
layout: default
categories: [Publications]
---

On this page, you can find supplementary material for the DFT-based analysis approach on routing options in railway station areas.

## DFT Modelling Approach for Operational Risk Assessment of Railway Infrastructure

### Models

The generated DFTs for the railway stations are available (in anonymized form) on our [GitHub page](https://github.com/moves-rwth/dft-examples/tree/master/case_studies/railway_station_area){:target="_blank"}.
The DFTs are provided in a custom JSON format.

The DFTs can be visualized using our [DFT GUI](https://moves-rwth.github.io/dft-gui/){:target="_blank"}.
In the GUI, the corresponding JSON file first needs to be selected and can then be loaded via the `Load` button.
Further information on the DFT GUI can be found in the [documentation](https://github.com/moves-rwth/dft-gui/blob/master/doc/user_manual.md){:target="_blank"}.

### Analysis

The fault tree analysis can be performed by [Storm]({{ site.github.url }}/getting-started.html).

For example, the unreliability of the train station of Herzogenrath (with scheduled routes and single BEs for components) within 90 days can be computed with:
```console
$ storm-dft -dftjson Herzogenrath_scheduled_single.json --timebound 90 -nosymred
```

Similarly, the mean-time-to-failure (MTTF) is computed by:
```console
$ storm-dft -dftjson Herzogenrath_scheduled_single.json -mttf -nosymred
```

For more details on analysing DFTs with Storm we refer to our [documentation]({{ site.github.url }}/documentation/usage/running-storm-on-dfts.html).

