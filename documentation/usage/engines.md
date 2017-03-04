---
title: Engines
layout: default
documentation: true
category_weight: 5
categories: [Usage]
---

As with so many things, there is no one-size-fits-all in probabilistic model checking. Generally, it depends on the characteristics of the input model, which of the model checking approaches works best (for example in terms of time or memory requirements). Therefore, established model checkers provide a range of different *engines* that pursue different approaches to solve the same problem. In practice, the functionality of the engines is typically incomparable, that is there may be features that an engine can solve that another one cannot and vice versa. In the following, we give a brief intuitive overview on the engines provided by Storm.

# Sparse

- `--engine sparse`

# DD

- `--engine dd`

# Hybrid

- `--engine hybrid`

# Exploration

- `--engine expl`

# Abstraction-Refinement

- `--engine abs`

