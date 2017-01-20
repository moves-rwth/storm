---
title: Languages
layout: default
documentation: true
categories: [Usage]
---

# PRISM

The PRISM language can be used to specify [DTMCs, CTMCs and MDPs](documentation/usage/models.html). Conceptually, it is a guarded command language using reactive modules. Storm supports (almost) the full PRISM language and extends it (in a straightforward way) to [Markov Automata](documentation/usage/models.html).

For more information, please read the [PRISM manual](http://www.prismmodelchecker.org/manual/ThePRISMLanguage/Introduction) and check out the [semantics document](). A rich collection of examples is available at the [prism benchmark suite]().

# Jani

Jani models define DTMCs, MDPs, CTMCs or MA.

For more information, please visit the [jani-specification](http://www.jani-spec.org), where you can also find some [examples](https://github.com/ahartmanns/jani-models/)

# GSPNs

[Generalized Stochastic Petri Nets](#) can be given in either of two formats. For both formats [examples](#) are available.

## PNML

The [Petri Net Markup Language]() is an ISO-standardized XML format to specify Petri nets.

## greatSPN editor projects

The [greatSPN editor](http://www.di.unito.it/~amparore/mc4cslta/editor.html) is a GUI capable of specifying and verifying GSPNs. Project files (XML) specifying a single GSPN can be parsed directly by storm. 



# DFTs

[Dynamic Fault Trees](#) are given in the so-called Galileo Format.
The format is a simple textual format naming the root of the tree and then lists all nodes of the tree, together with the children of each node.

```

```

A [rich collection](https://github.com/moves-rwth/dft-examples) of examples is available.

# cpGCL



# Explicit



