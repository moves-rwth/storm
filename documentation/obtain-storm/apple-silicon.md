---
title: Building for Apple Silicon
layout: default
---

<h1>Building for ARM-based Apple Silicon Systems </h1>

{:.alert .alert-info}
_Native_ code compilation on ARM-based Apple Silicon systems is not yet supported. However, compilation with x86 emulation (Rosetta 2) does work just fine. We provide detailed installation steps for that below.
Native ARM compilation is work in progress.

##  Installing Dependencies

### Development Tools
Similarly to the x86-based preperations, you first need to download either the command line tools (CLT) or Xcode. Installing the CLT can be done using the terminal by executing
``` console
$ xcode-select --install
```

{:.alert .alert-danger}
Older versions of the CLT are known to cause issues during the installation. The following steps have successfully been tested using the CLT in version ```12.4.0.0.1.1610135815```. More recent versions should work as well.

### Homebrew
Furthermore, we recommend having [seperate homebrew installations](https://docs.brew.sh/Installation#multiple-installations){:target="_blank"} that use default paths for x86 and ARM, respectively. One valid configuration would be to have 

- one homebrew installation for x86 compilation in ```/usr/local/``` and
- one for ARM compilation in ```/opt/homebrew/```.

For installing Storm, you only need a homebrew installation for x86 compilation, preferably in ```/usr/local/```.
We set an environment variable to ensure that the x86 homebrew installation is invoked.
```console
$ export X86_BREW=/usr/local/bin/brew # change path if necessary.
````

Confirm that homebrew recognizes CLT _or_ Xcode by querying
``` console
$ $X86_BREW config | grep 'CLT\|Xcode'
```
If the output provides no information for _both_ entries, like so:
``` console
CLT: N/A
Xcode: N/A
```
you need to reinstall either suites. Downloading CLT [directly from Apples website](https://developer.apple.com/downloads/more/){:target="_blank"} fixes this issue for some users.

Finally, check that Rosetta 2 can be detected by Homebrew:
``` console
$ arch -x86_64 /usr/local/bin/brew config | grep 'Rosetta 2'
```
If you have never run any application using Rosetta 2, a popup will appear asking you to install it. If the above command yields
``` console
Rosetta 2: true
```
you are ready to continue.

### General Dependencies
Install the x86 version of the [general dependencies](dependencies.html#general-dependencies) of Storm using homebrew. You need to enable x86 emulation (through Rosetta 2) by prefixing the brew command with `arch -x86_64`, i.e. execute one of the following

- Required:
``` console
$ arch -x86_64 $X86_BREW install cln ginac automake cmake boost gmp glpk hwloc
```

- Recommended:
``` console
$ arch -x86_64 $X86_BREW install cln ginac automake cmake boost gmp glpk hwloc z3 xerces-c
```
 
{:.alert .alert-info}
Make sure to use the correct x86 installation of Homebrew. If the wrong installation is specified or the installations would be done to paths used for ARM binaries, Homebrew will notify you and cancel the installation.

### CMake
ARM versions of CMake do often not play nicely with the compilation of dependencies Storm is using, which is why we recommend to use a x86 version of cmake. Check which architecture your cmake executable is targeting by executing:
```console
$ file $(which cmake)
```
Ensure that the output is ```Mach-O 64-bit executable x86_64``` or ```Mach-O universal binary with 2 architectures: [x86_64:Mach-O 64-bit executable x86_64] [arm64]```. If instead ```Mach-O 64-bit executable arm64``` is prompted, you need to locate a cmake executable that is either targeting x86 architectures or an universal binary with x86 support. The following command iterates over registered cmake installations and displays those that are compatible:
```console
$ where cmake | sort | uniq | while read l; do if [[ $(file $line) == *"x86"* ]] then echo "x86 compatible CMake at: $l"; fi; done
```
If you have used Homebrew to install cmake, you can also find the installation using this command instead:
```console
$ file $(arch -x86_64 $X86_BREW --prefix cmake)/bin/cmake
```
where ```$X86_BREW``` is as explained [above](apple-silicon.html#homebrew).

{:.alert .alert-danger}
There are known issues when compiling Storm using older versions of CMake.
These issues seem to have been fixed with cmake version ```3.19.5```. Make sure to use the latest version of CMake.

## Building Storm from Source.

You can now obtain, configure, and build Storm as outlined [here](build.html#obtaining-the-source-code).
However, you need to ensure that you invoke universal binaries, in particular `cmake` and `make`, using ```arch -x86_64``` as a prefix.


{:.alert .alert-info}
The prefix ```arch -x86_64``` is not necessary for non-universal x86 binaries. However, if you are not sure, you can just apply the prefix to all commands.
