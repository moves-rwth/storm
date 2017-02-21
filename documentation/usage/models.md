---
title: Models
layout: default
documentation: true
category_weight: 1
categories: [Usage]
---

# Models Without Nondeterminism

## Discrete-Time Markov Chains (DTMCs)

In a discrete-time Markov chain, every state is equipped with a probability distribution over successor states. That is, whenever the system is in a particular state, a random experiment is conducted to determine the state the system moves to in the next step. Here, time proceeds in a discrete manner: the steps the system takes are counted by the non-negative integers.

## Continuous-Time Markov Chains (CTMCs)

If the time in the model flows continuously and it is important to preserve this timing in the model, CTMCs can capture this. Just like in a CTMC, there is a probability distribution over successor states in each state. Additionally, every state is equipped with an *exit rate* lambda, that governs the sojourn time in that state. More specifically, the time spent in the state is negatively exponentially distributed with rate lambda.

# Models With Nondeterminism

Sometimes, it is necessary to model a nondeterministic choice between alternatives. Typical sources of nondeterminism include scheduling freedom (as found in distributed systems), abstraction and underspecification. In these cases, the modelling formalism needs to provide a means to specify nondeterministic behavior.

## Discrete-Time Markov Decision Processes (MDPs)

Markov Decision processes extend DTMCs with nondeterministic choice. If the system is in state s, first a nondeterministic choice over the actions available in s (the *enabled* actions) is taken. Then, the successor state is chosen probabilistically according to the probability distribution associated with the selected action.

## Markov Automata (MAs)

Markov automata essentially combine the features of CTMCs with MDPs. The states in an MA fall into one of three categories. *Probabilistic states* are just like the states of an MDP, i.e. there is a nondeterministic choice over probability distributions. *Markovian states* are like the states of a CTMC in that there is just one probability distribution over successor states, but the system will stay in these states for a negatively exponentially distributed delay. Finally, *hybrid states* have both, a nondeterministic choice over probabilistic choices as well as the possibility to stay in the state for some time and then pick the successor according to a certain probability distribution.