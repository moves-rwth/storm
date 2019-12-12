---
title: VM
layout: default
documentation: true
categories: [VM]
---

We provide an outdated VM with pre-installed Storm 0.10 for historical reasons. The VM includes dependencies and other useful reference tools (like [PRISM](http://www.prismmodelchecker.org/){:target="_blank"} and [IMCA](https://github.com/buschko/imca){:target="_blank"} and the PRISM benchmark suite) on a Linux host system. You can download the latest version of the virtual machine [here](https://rwth-aachen.sciebo.de/index.php/s/nthEAQL4o49zkYp){:target="_blank"}.

{:.alert .alert-danger}
Note that the provided virtual machine images is outdated. We recommend to use the [Docker container]({{ site.github.url }}/documentation/installation/installation.html#docker){:.alert-link} instead.

{:.alert .alert-info}
The virtual machine is hosted at [sciebo](https://www.sciebo.de/en/){:target="_blank" .alert-link}, an academic cloud hoster. We are not able to trace the identity of downloaders, so reviewers can use this link without revealing their identity.


## Importing

When you have downloaded the OVA image, you can import it into, for example, [VirtualBox](https://www.virtualbox.org){:target="_blank"}. Before the first run, you should review the hardware resources allocated to the VM. E.g., for VirtualBox open *Settings → System* and adjust the memory size and CPU count under *Motherboard* and *Processor*, respectively.

The username and password are both *storm* and a `README` file is provided on the desktop. In the virtual machine, Storm is located at `/home/storm/storm` and the binaries can be found in `/home/storm/storm/build/bin`. For your convenience, an environment variable with the name `STORM_DIR` is set to the path containing the binaries and this directory is added to the `PATH`, meaning that you can run the Storm binaries from any location in the terminal and that
```console
$ cd $STORM_DIR
```
will take you to the folders containing Storm's binaries. For more information on how to run Storm, please read our [guide]({{ site.github.url }}/documentation/usage/running-storm.html).

## Changelog

The VM may be updated periodically to include bug fixes, new versions, and so on. When the image was most recently updated and what changes were made to the VM can be taken from the following changelog.

#### Update on March 21, 2017

- added scripts to re-run all benchmarks from paper submission "A Storm is Coming: A Modern Probabilistic Model Checker"
- added description to README how to use the scripts

#### Update on Feb 1, 2017

- updated to newest Storm version
- added files containing all tool invocations used in [benchmarks]({{ site.github.url }}/benchmarks.html)
- installed latest version of [IMCA](https://github.com/buschko/imca){:target="_blank"} and added its benchmark files

#### Update on January 22, 2017

- installed Storm
- installed [PRISM v4.3.1](http://www.prismmodelchecker.org/download.php){:target="_blank"}
- added [PRISM benchmark suite](https://github.com/prismmodelchecker/prism-benchmarks/){:target="_blank"}
