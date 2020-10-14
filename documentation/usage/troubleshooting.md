---
title: Troubleshooting
layout: default
documentation: true
category_weight: 3
categories: [Use Storm]
---

<h1>Troubleshooting</h1>

{% include includes/toc.html %}

## General help
- In general, Storm's development is an ongoing process.
  For example, bugs might be already fixed in the latest version.
  Before reporting a problem, make sure that you are using at least the latest release of Storm.
  You might also want to check out the current master branch of Storm which contains the current version with the latest improvements.

- Storm offers a lot of configurations parameters to tweak most aspects to your liking.
  Use the help functionality to get an overview of possible configuration options:
  ``` console
  $ storm --help
  ```
  As Storm offers over a hundred command line arguments, options are categorized in modules and not all options are displayed.
  You can display all options for a module by adding the module name to the help argument.
  An example call is:
  ``` console
  $ storm --help bisimulation
  ```
  The help command also offers a search functionality.
  Insert a keyword to show all configuration options containing this keyword:
  ``` console
  $ storm --help exact
  ```


## Input
- If problems with the input model occur, you might want to debug your model first.
  You can add the command-line flag `--explchecks` to perform additional consistency checks on the input model (for example whether the transition probabilities per state sum up to one).

## Numerical issues
Depending on the input file and property, the used numerical values (for transition probabilities, etc.) might be very small.
As a result, Storm might encounter numerical imprecisions due to using floating-point numbers.

- You can increase the precision requirement with the command-line flag `--precision 1e-6`.
- Even better, for supported models and properties you can use exact rational numbers and obtain exact results.
  You can activate this with the flag `--exact`.
  Note that the running time can significantly increase when using exact numbers.

  


# File an issue

If you encounter problems when using Storm, feel free to [contact us]({{ site.github.url }}/about.html#people) by writing a mail to
- <i class="fa fa-envelope" aria-hidden="true"></i> support ```at``` stormchecker.org.

You may also open an [issue on GitHub](https://github.com/moves-rwth/storm/issues){:target="_blank"}.
In any case, please provide as much information on your problem as you possibly can.
For example, provide the input model you want to check and the complete command-line arguments you used for running Storm.
It also helps us to know the exact version of Storm you are using.
You can output the version information with
``` console
$ storm --version
```
