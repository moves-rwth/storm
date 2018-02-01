---
title: GSPN semantics for DFTs
category_weight: 6
layout: default
categories: [Publications]
---

On this page, you can find supplementary material for the GSPN semantics for DFTs.

## One Net Fits All
### A unifying semantics of Dynamic Fault Trees using GSPNs

The translation from DFTs to GSPN is implemented in Storm in version [1.2.1](https://github.com/moves-rwth/storm/releases/tag/1.2.1).

The obtained GSPNs can be exported into the project format used by the [GreatSPN Editor](http://www.di.unito.it/~amparore/mc4cslta/editor.html).

The translation into a GSPN can be enabled in Storm with the commandline flag `--gspn` and then the export into the GreatSPN Editor format can be triggered with `--to-pnpro`.
An example call would be as follows:
```console
$ storm-dft -dft dft_model.dft --gspn --to-pnpro gspn.pnpro
```

### Models

We provide GSPNs for sample DFTs on our [GitHub page](https://github.com/moves-rwth/dft-gspn-examples){:target="_blank"}.

