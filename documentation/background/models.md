---
title: Models
layout: default
documentation: true
category_weight: 1
categories: [Background]
---

The model types Storm supports can be categorized along two dimensions. First, the *notion of time* in a model may be either discrete (or "time-abstract") or continuous. While the former formalizes time as discrete steps, time in the latter flows continuously. Second, models may contain *nondeterminism*. That is, there are choices that are resolved nondeterministically in the sense that there is no information about a likelihood of the involved choices. Rather, any possible resolution has to be taken into account when reasoning about such systems. Typical sources of nondeterminism include scheduling freedom (as found in distributed systems), abstraction and underspecification. In the following, we briefly describe each model type.

## Models without nondeterminism

### Discrete-time Markov chains (DTMCs)

In a discrete-time Markov chain, every state is equipped with a probability distribution over successor states. That is, whenever the system is in a particular state, a random experiment is conducted to determine the state the system moves to in the next step. Here, time proceeds in a discrete manner: the steps the system takes are counted by the non-negative integers.

### Continuous-time Markov chains (CTMCs)

If the time in the model flows continuously and it is important to preserve this timing in the model, CTMCs can capture this. Just like in a DTMC, there is a probability distribution over successor states in each state. Additionally, every state is equipped with an *exit rate* lambda, that governs the sojourn time in that state. More specifically, the time spent in the state is negatively exponentially distributed with rate lambda.

## Models with nondeterminism

### Discrete-time Markov decision processes (MDPs)

Markov Decision processes extend DTMCs with nondeterministic choice. If the system is in state s, first a nondeterministic choice over the actions available in s (the *enabled* actions) is taken. Then, the successor state is chosen probabilistically according to the probability distribution associated with the selected action.

### Markov automata (MAs)

Markov automata essentially combine the features of CTMCs with MDPs. The states in an MA fall into one of three categories. *Probabilistic states* are just like the states of an MDP, i.e. there is a nondeterministic choice over probability distributions. *Markovian states* are like the states of a CTMC in that there is just one probability distribution over successor states, but the system will stay in these states for a negatively exponentially distributed delay. Finally, *hybrid states* have both, a nondeterministic choice over probabilistic choices as well as the possibility to stay in the state for some time and then pick the successor according to a certain probability distribution.
