---
title: Languages
layout: default
documentation: true
category_weight: 2
categories: [Background]
---

<h1>Languages</h1>

{% include includes/toc.html %}

## PRISM

The PRISM language can be used to specify [DTMCs, CTMCs and MDPs]({{ '/documentation/background/models.html' | relative_url }}). Conceptually, it is a guarded command language using reactive modules. Storm supports (almost) the full PRISM language and extends it (in a straightforward way) to [Markov Automata]({{ '/documentation/background/models.html#markov-automata-mas' | relative_url }}).

For more information, please read the [PRISM manual](http://www.prismmodelchecker.org/manual/ThePRISMLanguage/Introduction){:target="_blank"}. A rich collection of examples is available at the [PRISM benchmark suite](http://www.prismmodelchecker.org/benchmarks/){:target="_blank"}.

## JANI

JANI is a recently introduced modeling language for the landscape of probabilistic models. To allow easy parsing, JANI is based on [JSON](http://www.json.org/){:target="_blank"}. In general, JANI models can encode a variety of model types. Storm's support for JANI models covers DTMCs, CTMCs and MDPs.

For more information, please visit the [Jani specification](http://www.jani-spec.org){:target="_blank"}, where you can also find some [examples](https://github.com/ahartmanns/jani-models/){:target="_blank"}.

## GSPNs

[Generalized Stochastic Petri Nets](#) can be given in either of two formats. For both formats [examples](#) are available.

### PNML

The [Petri Net Markup Language](http://www.pnml.org/){:target="_blank"} is an ISO-standardized XML format to specify Petri nets.

### GreatSPN editor projects

The [GreatSPN editor](http://www.di.unito.it/~amparore/mc4cslta/editor.html){:target="_blank"} is a GUI capable of specifying and verifying GSPNs. Project files (XML) specifying a single GSPN can be parsed directly by Storm.

## DFTs

Dynamic Fault Trees are given in the so-called [Galileo Format](https://www.cse.msu.edu/~cse870/Materials/FaultTolerant/manual-galileo.htm#Editing%20in%20the%20Textual%20View){:target="_blank"}.
The format is a simple textual format naming the root of the tree and then lists all nodes of the tree, together with the children of each node. A [rich collection](https://github.com/moves-rwth/dft-examples){:target="_blank"} of examples is available.

## Explicit

In the spirit of [MRMC](http://www.mrmc-tool.org/){:target="_blank"}, Storm also supports input models specified in an explicit format. While the format closely resembles that of MRMC, it does not match exactly. Likewise, at the moment the model export of PRISM generates files that can be easily modified to be handled by Storm, but they still need to be adapted by hand.

### Transition File

The main ingredient of an explicit model is a tra-file containing the transitions. For DTMCs, a tra file might look like this

```bash
dtmc
0 1 0.3
0 4 0.7
1 0 0.5
1 1 0.5
...
```

This specifies four transitions: from state 0 to 1 with probability 0.3, state 0 to 4 with probability 0.7 and so on. For CTMCs, only the model hint needs to be set to ```ctmc```. For MDPs, the tra file needs to have the following shape:

```bash
mdp
0 0 1 0.3
0 0 4 0.7
0 1 0 0.5
0 1 1 0.5
1 0 1 1.0
...
```

This specifies the behaviors of two states. State 0 has two nondeterministic choices (called 0 and 1). With the first one, choice 0, state 0 can probabilistically go to state 1 with probability 0.3 and to state 4 with probability 0.7. The second choice (1), is associated with a uniform distribution over the states 0 and 1.

### Labeling File

The labeling file format matches that of MRMC. That is, it looks similar to this:

```bash
#DECLARATION
one two goal
#END
0 one two
1 goal
```

This creates three distinct labels, called ```one```, ```two``` and ```goal```. State 0 is labeled with both ```one``` and ```two```, while state 1 only carries the ```goal``` label.
