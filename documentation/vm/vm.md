---
title: VM
layout: default
documentation: true
categories: [VM]
---

If you just want to try Storm without installing it or its dependencies, the virtual machine image we provide might be the right fit for you. We pre-installed Storm, its dependencies and other useful reference tools (like [PRISM](http://www.prismmodelchecker.org/) and [IMCA](https://github.com/buschko/imca) and the PRISM benchmark suite) on a Linux host system. You can download the latest version of the virtual machine [here](https://rwth-aachen.sciebo.de/index.php/s/nthEAQL4o49zkYp).

{:.alert .alert-info}
The virtual machine is hosted at sciebo, an academic cloud hoster. We are not able to trace the identity of downloaders, so reviewers can use this link without revealing their identity.

# Changelog

When the image was most recently updated and what changes were made to the VM can be taken from the following changelog.

#### Update on Feb 1, 2017

- updated to newest Storm version
- added files containing all tool invocations used in [benchmarks]({{ site.baseurl }}/benchmarks.html)
- installed latest version of [IMCA](https://github.com/buschko/imca) and added its benchmark files

#### Update on Jan 22, 2017

- installed Storm
- installed [PRISM v4.3.1](http://www.prismmodelchecker.org/download.php)
- added [PRISM benchmark suite](https://github.com/prismmodelchecker/prism-benchmarks/)
