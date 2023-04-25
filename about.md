---
title: About
navigation_weight: 1
layout: default
---

## Description

Storm is a tool for the analysis of systems involving random or probabilistic phenomena {% cite Kat16 %}. Such models arise, for example, in distributed algorithms (where randomization breaks symmetry between processes), security (randomized key generation), systems biology (species/molecules interact randomly depending on their concentration) or embedded systems (involving hardware failures). To ensure that such systems behave *correctly*, typically the quantitative aspects of the model have to be considered. For example, it may be inevitable that the system fails due to hardware failures under certain circumstances, but this may be tolerable as long as it only happens rarely, i.e. with a low probability.

Formal methods --- and in particular model checking --- has proved to discover intricate system errors. Probabilistic model checking has matured immensely in the past decade and has been identified as a key challenge by experts.

<blockquote class="blockquote">
  <p>A promising new direction in formal methods research these days is the development of probabilistic models, with associated tools for quantitative evaluation of system performance along with correctness.</p>
  <small>R. Alur, T. A. Henzinger, and M. Vardi in <cite>Theory and practice for system design and verification</cite> in <cite>ACM SIGLOG News 3, 2015</cite></small>
</blockquote>

<!--
<blockquote class="blockquote">
<p>I conclude with a list of challenges for the future. [...], Probabilistic Model Checking, [...].</p>
<small>E. M. Clarke in <cite>The birth of model checking</cite> in <cite>25 years of model checking, volume 5000 of LNCS, 2008</cite></small>
</blockquote>
-->

## Videos

We provide a three-part tutorial into Storm on YouTube.
The tutorial was originally presented at [DisCoTec 2020](https://www.discotec.org/2020/).

<div class="embed-responsive embed-responsive-16by9">
  <iframe class="embed-responsive-item" src="https://www.youtube.com/embed/TTfSZGiCQ3I?list=PLtEtbVzif4eg7c76bOzyreJvNVgmrj3Ub"></iframe>
</div>

You can try out all examples from the tutorial by yourself and even play around with modifications.
Use the links to follow along [part 2 about Storm](https://mybinder.org/v2/gh/moves-rwth/stormpyter/discotec2020?filepath=tutorial_discotec2020%2Fdiscotec_storm.ipynb){:target="_blank"} or [part 3 about stormpy](https://mybinder.org/v2/gh/moves-rwth/stormpyter/discotec2020?filepath=tutorial_discotec2020%2Fdiscotec_stormpy.ipynb){:target="_blank"}, the Python bindings of Storm.
For more details, also see the corresponding [webpage](https://github.com/moves-rwth/stormpyter/tree/master/tutorial_discotec2020) for the tutorial.

## Characteristics

There are several related tools addressing the analysis of probabilistic systems. However, these tools differ substantially regarding (among other things)

- the supported modeling formalisms,
- the input languages,
- the supported properties/queries,

and have been developed with different target objectives. We believe the characteristics and trade-offs Storm makes to be unique. In the following, we motivate and detail some of the development goals.

### <i class="fa fa-tachometer" aria-hidden="true"></i> Efficient Core

Model checking is both a data- and compute-intense task. In the worst case, the state space of the system has to be searched exhaustively. Probabilistic model checking is even harder in the sense that most techniques require an in-memory representation of the full system to be available and that it relies on solving gigantic equation systems. A crucial aspect of a probabilistic model checker therefore is it's efficiency in terms of time and memory. One of Storm's development goals is to provide a good space-time tradeoff and be faster than other competing tools. As the properties of the systems to analyze vary drastically, Storm has several [engines]({{ '/documentation/background/engines.html' | relative_url }}) that determine which data structures are used to build and analyze a model.

### <i class="fa fa-cogs" aria-hidden="true"></i> Modularity

Storm's infrastructure is built around the notion of a *solver*. They are small entities that implement one of a set of interfaces. For example, multiple solvers are available for

- linear equation systems
- Bellman equation systems
- stochastic games
- (mixed-integer) linear programming (MILP)
- satisfiability (modulo theories) (SMT)

Encapsulating solvers like this has several key advantages. First of all, it provides easy and coherent access to the tasks commonly involved in probabilistic model checking. New functionality can often be implemented by reusing existing solvers and combining them in a suitable way. Second, it enables Storm to offer multiple high-performance solvers by backing them with different dedicated state-of-the-art libraries to solve the given task. As the structure of input models heavily influences the performances of the solvers and there is no one-size-fits-all solution, this allows to pick opportune solvers based on their strengths. Besides, it may not be possible to include and ship a library because of licensing problems. For example, Storm offers an implementation of the MILP interface using the high-performance yet commercial [Gurobi](https://www.gurobi.com){:target="_blank"} solver. With the flexibility introduced by solvers, users can select Gurobi when it is available to them but can still pick another "fallback" solver otherwise. Furthermore, communities like the SMT community are very active and state-of-the-art solvers of today may be outdated tomorrow. The flexibility in adding new solvers ensures that Storm is easily kept up-to-date without destroying backward compatibility. Finally, it allows to easily develop new solvers with new strengths without knowledge about the global code base. Complying with the interface will yield a solver that can be used anywhere in the existing code base.

### <i class="fa fa-language" aria-hidden="true"></i> Various Input Languages

Let us assume a user is interested in verifying a particular system. In order for a probabilistic model checker to understand the behavior of the system, it needs to be modeled in some formal language the tool is capable of treating. However, different communities and different tools often favor or even demand different input languages. Besides, different modeling languages have different strengths and weaknesses and it depends on the system at hand which language is suited best. Sometimes, the model has already been created by another tool and it cannot be forwarded to another tool without transcription, because the target tool requires the input to be in a different language.

Storm tries to mitigate this problem by offering support for several major input [languages]({{ '/documentation/background/languages.html' | relative_url }}). While some of them are rewritten to one of the other input languages, others are supported *natively*. That is, Storm has specific model builders (and sometimes even analysis procedures) tailored towards different inputs. This allows Storm to make domain-specific optimizations as the structure of the original problem is preserved and is therefore accessible to Storm.

### <i class="icon-python"></i> Easy Interface

While Storm tries to make it easy to include new functionality, a developer still has to somewhat understand its architecture to make appropriate additions or changes. However, suppose a user just wants to combine functionality that is already there to create new algorithms. Optimally, he/she could abstract from some of C++'s intricacies and focus on the actual algorithm. This process is supported by [Stormpy](https://moves-rwth.github.io/stormpy/){:target="_blank"}, which provides a python API for an ever growing part of Storm's code base. These bindings not only allow to utilize the high-performance routines of Storm in combination with the ease of python, but also make it possible to intertwine calls to Storm with calls to other useful libraries available in python, for example (but not limited to):

- simple IO,
- plotting (e.g. [matplotlib](https://matplotlib.org/){:target="_blank"}),
- calling solvers such as [Z3](https://github.com/Z3Prover/z3){:target="_blank"} or [CPLEX](https://www.ibm.com/analytics/cplex-optimizer){:target="_blank"},
- libraries from the area of artificial intelligence and machine learning,
- rational function manipulation (via [pycarl](https://github.com/moves-rwth/pycarl){:target="_blank"}).

## Miscellaneous

Storm

- has roughly ~220k lines of C++ code (as of April 2023)
- is under development since 2012
- went open source in 2017
- has over 15 [contributors](https://github.com/moves-rwth/storm/#Authors)
- supported on most Unix platforms
- would have been impossible without all the [cool libraries around](documentation/dependencies)

## People

The developers can be reached via
- <i class="fa fa-envelope" aria-hidden="true"></i> support ```at``` stormchecker.org

If you have general feedback or questions on how to use Storm, please send us a mail.

For feature request or bug reports, please open a [new issue on GitHub](https://github.com/moves-rwth/storm/issues/new){:target="_blank"}.

Storm has initially been developed at the [Chair for Software Modeling and Verification](http://moves.rwth-aachen.de){:target="_blank"} at RWTH Aachen University.
The core developers are

- [Christian Hensel](https://moves.rwth-aachen.de/people/hensel/){:target="_blank"} RWTH Aachen University (until 2018)
- [Sebastian Junges](https://sjunges.github.io){:target="_blank"} Radboud University
- [Joost-Pieter Katoen](https://moves.rwth-aachen.de/people/katoen/){:target="_blank"} RWTH Aachen University
- [Tim Quatman](https://moves.rwth-aachen.de/people/quatmann/){:target="_blank"} RWTH Aachen University
- [Matthias Volk](https://people.utwente.nl/m.volk){:target="_blank"} University of Twente

## Website

This website is developed with
[Bootstrap](https://getbootstrap.com/){:target="_blank"},
[BootstrapCDN](https://www.bootstrapcdn.com/){:target="_blank"},
[DataTables](https://datatables.net){:target="_blank"},
[Flatly Theme](https://bootswatch.com/flatly/){:target="_blank"},
[Font awesome](https://fontawesome.com){:target="_blank"},
[Font mfizz](http://fizzed.com/oss/font-mfizz){:target="_blank"},
[Jekyll](https://jekyllrb.com){:target="_blank"},
[Sphinx Doc](http://www.sphinx-doc.org/en/stable/){:target="_blank"}.

Responsible for this website is

```text
Software Modeling and Verification Group (MOVES)
RWTH Aachen University, Aachen, Germany
E-mail: support [at] stormchecker [dot] org
Phone: +49 241 8021201
```
