---
title: Properties
layout: default
documentation: true
category_weight: 3
categories: [Background]
---

<h1>Properties</h1>

{% include includes/toc.html %}

Storm takes properties in a format that can be described as an "extended subset" of the [PRISM property language](http://www.prismmodelchecker.org/manual/PropertySpecification/Introduction){:target="_blank"}. Alternatively, if the input is given in terms of a [JANI](languages.html#jani) model, the properties are embedded in the model in the appropriate format.

{:.alert .alert-info}
For DFTs, GSPNs and probabilistic programs, domain-specific properties can be given. For this, we refer to the guide on how to [run Storm]({{ '/documentation/usage/running-storm.html' | relative_url }}){:.alert-link} on those inputs.

## Identifying States

Storm allows to identify sets of states by either labels or via the symbolic variables describing the model. Note that it depends on the [input language](languages.html) which of these two ways are enabled. For example, for explicit models, there are no symbolic variables and therefore only labels can be used. For PRISM and JANI input, both labels and expressions over model variables are valid. Finally, the expression `true` can be used to describe the set of all states.

### Labels

To refer to a label a model exports, put the name of the label in quotes. For example, if a model has the label `done` to express that in these states the model is done with something, write `"done"` to refer to these states.

### Propositional Expressions

For symbolic input (PRISM and JANI), one may also use expressions over the model variables to identify states. For example, if the model has an integer-valued state variable `s`, then `s < 5` refers to all states in which `s` is smaller than five. Of course, Boolean combinations of such expressions are allowed, i.e. `s < 5 & (b | s < y)` is valid for a model with integer variables `s` and `y` and Boolean variable `b`.

## Probabilistic Computation Tree Logic (PCTL) / Continuous Stochastic Logic (CSL)

### Idea
The (probability) measure on Markov models is typically defined on paths. These paths are defined as in temporal logic, or more specifically computation tree logic (CTL), a branching-time logic. That means that the formula alternates over descriptions of paths and descriptions of states.

### Path Formulae

For this, we assume that `a` and `b` are [state formulae](#state-formulae) and `{op}` is any one of `<, <=, >, >=`. The available path formulae are:

- `a U b` to describe paths on which at some point `b` holds and in all prior steps `a` holds.
- `F b` as a shortcut for `true U b`.
- `a U{op}k b` (where `k` is an expression evaluating to a number) to describe the paths on which `b` holds within `k` time (where time in discrete models means steps) and `a` holds before.
- `F{op}k b` as a shortcut for `true U{op}k b`.
- `G a` to describe paths on which `a` holds in every step.

### State Formulae

Here, we assume that `a` and `b` are state formulae, `phi` is a [path formula](#path-formulae) and `{op}` is any one of `<, <=, >, >=`. The available state formulae are:

- `c` where `c` is either a [label](#labels) or an [expression](#propositional-expressions) over the model variables.
- `a | b`, `a & b` to describe all states that satisfy `a` or `b`, `a` and `b`, respectively.
- `!a` to describe the states *not* satisfying `a`.
- `P{op}t [ phi ]` (where `t` is a threshold value) to describe the states in which the probability to satisfy `phi` conforms to the comparison `{op} t`.
- `LRA{op}t [a]` to describe states in which the long-run average probability to be in a state satisfying `a` conforms to `{op} t`.

### Obtaining Probabilities

Although formally not allowed in PCTL/CSL, one can also request the probability of fulfilling a path formula from each state. Instead of comparing to a given value `P{op}b [ phi ]`, one can write `P=? [ phi ]` to obtain the actual values rather then obtaining a truth value.

### Nondeterministic Models

For nondeterministic models, the formula can (and sometimes needs to) specify whether it refers to minimal or maximal probabilities. Since there is no information on how the nondeterminism in the models is to be resolved, Storm needs information on whether it should resolve the choices to *minimize* or *maximize* the values. That is, you cannot write `P=? [F a]`, but have to either write `Pmin=? [F a]` or `Pmax=? [F a]`. While you can also specify `min` and `max` when comparing to a probability threshold, it's not necessary to do it. By default, if the comparison operator `{op}` is `<` or `<=`, then the probability is maximized and otherwise minimized. The reasoning is the following: if the property holds in a state, then no matter which resolution of nondeterminism is taken, the probability will always be below (or equal) to the threshold value.

## Reward extensions

To measure rewards (or costs) in models, Storm supports extensions of the aforementioned logics. More specifically, there are the following reward formulae, where `a` is a [state formula](#state-formulae):

- `I=k` is the expected reward obtained in time point `k`.
- `C<=k` is the expected reward obtained up until time point `k`.
- `F a` is the expected reward obtained up until reaching the set of states characterized by `a`.
- `LRA` is the long-run average reward.

Just like path formulae, these reward formulae can be embedded in an `R` operator to allow for value comparison or value computation:

- `R{op}t [ r ]` (where `r` is a reward formulae) describes the states in which the reward of `r` conforms to the comparison `{op} t`.
- `R=? [ r ]`, `Rmin=? [ r ]` and `Rmax=? [ r ]` have the straightforward interpretation.

In models with several reward structures, a particular reward structure has to be chosen:

- `R{"name"}{op}t [ r ]`
- `R{"name"}=?`, `R{"name"}min=?`, `R{"name"}max=?`


In continuous-time models, rewards in states are based on the sojourn time in that state.
In particular, a constant reward of `1` in every state reflects the sojourn time. This time-reward does not need to be added manually.
Instead, the following immediately reflects the time operator, analogous to above.

- `T<=t [ r ]`
- `T=? [ r ]`, `Tmin=? [r]` and `Tmax=? [ r ]`.


## Conditional Properties

The extension to conditional properties let's you query the probabilities of an event conditioned on the fact that something else holds. The formula

- `P=? [ F a || F b ]`

can be used to query the probability that a state satisfying `a` is reached provided that a state satisfying `b` is also reached.

## Examples

To illustrate how the formulae can be used, we give some examples of syntactically valid properties:

- `P<0.5 [ F s=5 ]`
- `Pmax=? [ Pmin>0.2 [ F<=10 "elected" ] U x+y=10 ]`
- `LRA>=0.3 [ "up" ]`
- `R=? [ I=10 ]`
- `Rmax=? [F "elected" ]`
- `R=? [ LRA ]`
- `P=? [ F s<10 || F s<7 ]`
- `Tmin=? [ F s=3 ]`

## Multi-objective Model Checking

Storm supports multi-objective model checking: In nondeterministic models, different objectives might require different choices of actions in order to satisfy the property.
This induces trade-offs between different strategies. Multi-objective model checking reveals such trade-offs by computing the Pareto curve, as explained in our [example]({{ '/documentation/usage/running-storm.html#multi-objective-model-checking-of-a-markov-automaton' | relative_url }}).

The simplest way of asking about these trade-offs are achievability queries: They ask whether there exists a strategy which fulfils all thresholds.

- `multi(P>=0.3 [ F "one" ], P>=0.5 [ G "two" ])`

Instead of fixing all values, we can leave one threshold open and ask for the optimal value for the threshold:

-  `multi(Pmax=? [ F "one" ], P>=0.5 [ G "two" ])`

If the other properties together are not achievable, the query returns `False`.
One can also leave multiple thresholds open: Instead of a single value, the model checker returns a Pareto curve.

-  `multi(Pmax=? [ F "one" ], Pmax=? [ G "two" ])`


## Naming Properties

To allow referring to specific properties, they can be equipped with names. To give the name `name` to a property `P`, you can write `"name" : P`.


