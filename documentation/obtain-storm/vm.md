---
title: Use Virtual Machine
layout: default
documentation: true
category_weight: 4
categories: [Obtain Storm]
---

<h1>Use a Virtual Machine (VM)</h1>

{% include includes/toc.html %}

{:.alert .alert-danger}
To reduce the virtualization overhead, it should be considered to use a [Docker container](docker.html){:.alert-link} instead of a VM.

On this page we provide virtual machine images containing pre-installed versions of Storm.

When you have downloaded an OVA image, you can import it into, for example, [VirtualBox](https://www.virtualbox.org){:target="_blank"}. Before the first run, you should review the hardware resources allocated to the VM. E.g., for VirtualBox open *Settings → System* and adjust the memory size and CPU count under *Motherboard* and *Processor*, respectively.

{:.alert .alert-info}
For all VMs on this page, the username and password are both *storm* 

## How to create your own VM
Since the VMs on this page might be outdated, we also provide the steps to create your own VM:

* Download an `.iso` file for the operating system. In the following, we assume [Ubuntu](https://ubuntu.com/download/desktop){:target="_blank"}. Other linux-based systems are [supported](build.html#supported-operating-systems) as well, but might need different [preparations](dependencies.html#os-specific-preparations).
* Set up a virtual machine using the `.iso` file and a virtualization software such as [VirtualBox](https://www.virtualbox.org/){:target="_blank"}.

{:.alert .alert-danger}
Make sure to assign enough resources to the VM. Since the build process is quite memory-intensive, we recommend to set the memory limit to more than 4 GB.

* Once the operating system inside the VM is ready, open a terminal and (assuming Ubuntu) execute the following command to install the dependencies of Storm:
```console
sudo apt-get install build-essential git cmake libboost-all-dev libcln-dev libgmp-dev libginac-dev automake libglpk-dev libhwloc-dev libz3-dev libxerces-c-dev libeigen3-dev
```

* We are now ready to download and compile the source code by executing:
``` console
git clone https://github.com/moves-rwth/storm.git
cd storm
git checkout stable
mkdir build
cd build
cmake .. -DSTORM_LOAD_QVBS=ON
make binaries
echo "export PATH=\$PATH:$(realpath bin)" >> ~/.bashrc
source ~/.bashrc
```
To build Storm in a specific version, just replace the line `git checkout stable` above with `git checkout 1.4.1` or any other version.
For more information on these steps, see our guide for [building Storm from source](build.html)

The build process takes roughly one hour. After that, you should be able to run
```console
storm --qvbs crowds
```
which checks an instance of the [Crowds protocol](http://qcomp.org/benchmarks/index.html#crowds){:target="_blank"}.

To check your installation, you may also run:
```console
cd ~/storm/build
make check
```

{:.alert .alert-danger}
If any problems occurr during this process (in particular when using a standard Ubuntu version) please [let us know](troubleshooting.html#file-an-issue){:.alert-link}.

## Storm 1.6.3 (2020/12)

A VM running Ubuntu 20.04 and Storm 1.6.3 can be found at [![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.4304439.svg)](https://doi.org/10.5281/zenodo.4304439). The root password is *storm*.

Storm is located at `/home/storm/storm` and the binaries can be found in `/home/storm/storm/build/bin`. For your convenience, the path containing the binaries is added to the `PATH`, meaning that you can run the Storm binaries from any location in the terminal. Moreover, the benchmarks from the [Quantitative Verification Benchmark Set](http://qcomp.org/benchmarks/) are included such that you can run, for example,
```console
storm --qvbs crowds
```
to check an instance of the [Crowds protocol](http://qcomp.org/benchmarks/index.html#crowds){:target="_blank"}.


## Storm 1.4.1 (2019/12)

A VM running Ubuntu 19.10 and Storm 1.4.1 can be found at [![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.3585795.svg)](https://doi.org/10.5281/zenodo.3585795). The root password is *storm*.

Storm is located at `/home/storm/storm` and the binaries can be found in `/home/storm/storm/build/bin`. For your convenience, the path containing the binaries is added to the `PATH`, meaning that you can run the Storm binaries from any location in the terminal. Moreover, the benchmarks from the [Quantitative Verification Benchmark Set](http://qcomp.org/benchmarks/) are included such that you can run, for example,
```console
storm --qvbs crowds
```
to check an instance of the [Crowds protocol](http://qcomp.org/benchmarks/index.html#crowds){:target="_blank"}.

## Storm 0.10 (2017/2)

We provide an outdated VM with pre-installed Storm 0.10 for historical reasons. The VM includes dependencies and other useful reference tools (like [PRISM](http://www.prismmodelchecker.org/){:target="_blank"} and [IMCA](https://github.com/buschko/imca){:target="_blank"} and the PRISM benchmark suite) on a Linux host system. You can download the virtual machine [here](https://rwth-aachen.sciebo.de/index.php/s/nthEAQL4o49zkYp){:target="_blank"}.
The root password is *storm*.

{:.alert .alert-info}
The virtual machine is hosted at [sciebo](https://www.sciebo.de/en/){:target="_blank" .alert-link}, an academic cloud hoster.

A `README` file is provided on the desktop. In the virtual machine, Storm is located at `/home/storm/storm` and the binaries can be found in `/home/storm/storm/build/bin`. For your convenience, an environment variable with the name `STORM_DIR` is set to the path containing the binaries and this directory is added to the `PATH`, meaning that you can run the Storm binaries from any location in the terminal and that
```console
$ cd $STORM_DIR
```
will take you to the folders containing Storm's binaries. For more information on how to run Storm, please read our [guide]({{ '/documentation/usage/running-storm.html' | relative_url }}).

<h3>Changelog</h3>

The VM has been updated to include bug fixes, new versions, and so on. When the image was most recently updated and what changes were made to the VM can be taken from the following changelog.

* Update on March 21, 2017
	- added scripts to re-run all benchmarks from the paper "[A Storm is Coming: A Modern Probabilistic Model Checker](http://doi.org/10.1007/978-3-319-63390-9_31)"
	- added description to README how to use the scripts
* Update on Feb 1, 2017
	- updated to newest Storm version
	- added files containing all tool invocations used in [benchmarks]({{ '/benchmarks.html' | relative_url }})
	- installed latest version of [IMCA](https://github.com/buschko/imca){:target="_blank"} and added its benchmark files
*  Update on January 22, 2017
	- installed Storm
	- installed [PRISM v4.3.1](http://www.prismmodelchecker.org/download.php){:target="_blank"}
	- added [PRISM benchmark suite](https://github.com/prismmodelchecker/prism-benchmarks/){:target="_blank"}
