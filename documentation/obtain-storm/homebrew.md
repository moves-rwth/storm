---
title: Install via Homebrew
layout: default
documentation: true
category_weight: 2
categories: [Obtain Storm]
---

# Install Storm via Homebrew

If you are running a version of macOS that is newer than Mavericks, you can use [homebrew](https://brew.sh/){:target="_blank"}, the "missing package manager for macOS". Once you have installed Homebrew, you need to make Homebrew aware of how to install Storm. In brew-speak, you need to *tap* the Storm Homebrew formulas

```console
$ brew tap moves-rwth/storm
```

Then, installing Storm is as easy as

```console
$ brew install stormchecker
```

This will install Storm and all necessary and some recommended dependencies. More options provided by the package can be seen by invoking

```console
$ brew info stormchecker
```

After installing the package, you should directly be able to invoke

```console
$ storm
```

and continue with the guide on how to [run Storm]({{ site.github.url }}/documentation/usage/running-storm.html).

### Additional steps for ARM-based Apple Silicon CPUs
For ARM-based Apple Silicon CPUs, installing Storm currently requires a homebrew installation that uses default x86 installation paths. One valid executable location would be ```/usr/local/bin/brew```. 
See the [homebrew section of this page](apple-silicon.html#homebrew) for further information.

You have to enable x86 emulation when invoking homebrew:
```console
$ export X86_BREW=/usr/local/bin/brew # change path if necessary.
$ $X86_BREW tap moves-rwth/storm
$ arch -x86_64 $X86_BREW install stormchecker
```
After the installation step, you should be able to invoke `storm` as mentioned above. It might be necessary to specify the path of the executable using
```console
$ $($X86_BREW --prefix stormchecker)/bin/storm
```
In this case we recommend adding the corresponding path to your `$PATH` environment variable using
```console
$ export PATH=$PATH:$($X86_BREW --prefix stormchecker)/bin
```

{:.alert .alert-info}
Native compilation on ARM-based systems is not yet supported. This is work in progress.