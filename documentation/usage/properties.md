---
title: Properties
layout: default
documentation: true
categories: [Usage]
---

Storm takes properties very similar to the prism property language, or alternatively as part of the jani-specification language. 


{:.alert .alert-info}
For DFTs, GSPNs and probabilistic programs, domain specific properties can be given. These are typically mapped to the properties discussed here. Their meaning and syntax can be found together with the documentation of these models. 


# Identifying states

Storm features two ways to identify sets of states, via labels or via the symbolic variables describing the Markov chain. 

- To refer to a label, the name of the label should be put in quotes, e.g. `"label_1"`.
- Integer-valued state-variables can compared to integers, e.g. `s < 5` describes all states in which the integer-variable `s` has a value smaller than `5`. 
- Boolean-valued state-variables can be listed directly, e.g. `b` describes all states in which the variable `b` is `true`. 


# PCTL / CSL

## Idea
The (probability) measure on Markov models is typically defined on paths. 
These paths are defined as in temporal logic, or more specifically CTL. That means that the formula alternates over descriptions of paths and descriptions of states.

The simplest representation of states are the atomic propositions described above. 
The simplest operators on paths are `F a` for finally `a` and `a U b` for `a` holds until at some point `b` holds. 

The probability-mass of the paths can be compared to a constant, and yields a property for states: `P>0.4 [F "target"]` describes the set of states in which the outgoing paths which finally reach a state labelled `target` together have a probability  greater than `0.4`. 

## Path formulae


## State formulae

- `|` or
- `&` and


## Obtaining probabilities

Although formally not allowed in PCTL, one can also request probability of of fulfilling a path formula from each state. Instead of comparing to a given value, the probability formula becomes `P=?`.
On models featuring non-determinism, the probability does not exist. Instead, one can refer here to minimum or maximum probability, respectively. 

{.alert .alert-danger}
Describe the meaning on non-deterministic models


# Reward extensions



# Conditional properties



# Multi-objective Model Checking

{:.alert .alert-danger} 
coming soon




