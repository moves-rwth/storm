---
title: Building for Apple Silicon
layout: default
---

<h1>Building for ARM-based Apple Silicon Systems </h1>

## Native Compilation to an ARM binary (recommended)

Storm compiles natively on ARM-based Apple Silicon processors. 
However, there are a few points to consider for troubleshooting.

* Make sure you have the latest version of Storm (Version 1.8.0 or later)
* Make sure that you use an ARM compiled `cmake` binary (check with `where cmake` and `file path/to/cmake`)
* Dependencies need to be installed using the *ARM* version of homebrew. Skip `cln` and `ginac` since (at the time of writing) they do not have an ARM version supported by homebrew
* Carl also needs to be build without `cln` and `ginac`.
* If you previously had an x86 version installed, you need to re-build Storm (and carl, if installed manually) from scratch. For this, remove the corresponding `build` folders and invoke the building steps again.
* Examine the `cmake` output. It might have found an `x86` version of carl or another dependency. Homebrew dependencies should normally be found in `/opt/homebrew`, *not* in `/usr/local/`.
* Contact us at [support@stormchecker.org](mailto:support@stormchecker.org) or on [GitHub](https://github.com/moves-rwth/storm) in case of issues.


##  Compiling with x86 Emulation

In some cases, e.g. when an older Storm version needs to be used, it might be necessary to compile an x86 version of Storm that can be run on Apple Silicon systems using Emulation via Rosetta 2.

### x86 Emulation and Development Tools
Install Rosetta 2 and either the command line tools (CLT) or Xcode. Installing Rosetta 2 and the CLT can be done using the terminal by executing
``` console
$ softwareupdate --install-rosetta
$ xcode-select --install
```

The following steps should be performed inside a x86 shell so that universal binaries are invoked correctly.
If you are using `zsh` (which is the default shell on macOS), this can be done by executing `arch -x86_64 zsh`.
To easily see that your shell runs in x86 mode, you can add the following lines to your `~/.zshrc` file:

```console
# modify the prompt if in x86 mode
if [[ $(uname -m) == 'x86_64' ]]; then
	expot PROMPT="%F{cyan}x86%f:$PROMPT"
fi
```

### Homebrew

You need a homebrew installation for x86 compilation, preferably in ```/usr/local/```. This can be done via
```console
$ arch -x86_64 /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

{:.alert .alert-info}
You can have [seperate homebrew installations](https://docs.brew.sh/Installation#multiple-installations){:target="_blank" .alert-link} that use default paths for x86 and ARM, respectively. One valid configuration would be to have one homebrew installation for x86 compilation in ```/usr/local/``` and one for ARM compilation in ```/opt/homebrew/```.


We set an alias `brew86` to ensure that the x86 homebrew installation is invoked.
```console
$ alias brew86=arch -x86_64 /usr/local/bin/brew  # change path if necessary.
```

Confirm that homebrew recognizes CLT _or_ Xcode by querying
``` console
$ brew86 config | grep 'CLT\|Xcode'
```
If the output provides no information for _both_ entries, like so:
``` console
CLT: N/A
Xcode: N/A
```
you need to reinstall either suites. Downloading CLT [directly from Apples website](https://developer.apple.com/downloads/more/){:target="_blank"} fixes this issue for some users.

Finally, check that Rosetta 2 can be detected by Homebrew:
``` console
$ brew86 config | grep 'Rosetta 2'
```
If you have never run any application using Rosetta 2, a popup will appear asking you to install it. If the above command yields
``` console
Rosetta 2: true
```
you are ready to continue.

### General Dependencies
Install the x86 version of the [general dependencies](dependencies.html#general-dependencies) of Storm using homebrew. You need to enable x86 emulation (through Rosetta 2) by prefixing the `brew` command with `arch -x86_64`, e.g. by using the `brew86` alias that we set above.
 
{:.alert .alert-info}
If the wrong homebrew installation is specified, the installations are done to paths used for ARM binaries which will not work for x86 emulation.

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

### Building Storm from Source.

You can now obtain, configure, and build Storm as outlined [here](build.html#obtaining-the-source-code).
