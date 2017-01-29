---
title: Command Line
layout: default
documentation: true
categories: [Usage]
---

storm provides many command-line arguments. Since these are subject to change and provide access to settings that are typically only meaningful to experts, we only cover the most important ones. There are [several binaries](../../getting-started.html#other-binaries) that provide access to specific functionality (DFT analysis, etc.). In the following, we will restrict the discussion to storm's main executable.

# Help Message

To get an overview of the available options, you can provide the `--help <hint>` flag. If the `<hint>` is absent or "all", the full help message is shown. Otherwise, only options matching the hint are displayed.

# Loading Models

There are various [input formats](languages.html) that storm can handle. Some of them need to be transformed to other formats or need to be fed into a specialized binary. The main executable `storm` can treat PRISM, JANI and explicit [input formats](languages.html). Depending on the latter, the model can be specified via one of the following switches.

- `--prism <prism-file>`
- `--jani <jani-file>`
- `--explicit <tra-file> <lab-file>`

# Specifying Properties

storm supports various [properties](properties.html). They can be passed to storm by providing the `--prop <properties> <selection>` switch. The `<properties>` argument can be either a property as a string or the path to a file containing the properties. The `<selection>` argument is optional. If set, it can be used to indicate that only certain properties of the provided ones are to be checked. More specifically, this argument is either "all" or a comma-separated list of [names of properties](properties.html#naming-properties) and/or property indices. Note that named properties cannot be indexed by name, but need to be referred to by their name.

# Engines

Generally, it depends on the characteristics of the input model, which of the model checking approaches works best (for example in terms of time or memory requirements). To select a specific engine, use one of the following switches.

- `--engine sparse`
- `--engine hybrid`
- `--engine dd`
- `--engine exploration`
- `--engine abs`

Note that not all engines support all model types and/or properties.


