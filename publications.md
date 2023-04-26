---
title: Publications
navigation_weight: 5
layout: default
---


## Storm tool papers

{:.alert .alert-info}
If you want to cite Storm, please use the most recent paper in this category.

{% bibliography --group_by year --query @*[category=tool] %}

## Competition reports

{% bibliography --group_by year --query @*[category=competition] %}

## Papers about features in Storm

{:.alert .alert-info}
The publications in this category present functionality from which a substantial part is available in Storm's release.

{% bibliography --group_by year --query @*[category=feature] %}

## Tools using Storm
- [COOL-MC](https://github.com/LAVA-LAB/COOL-MC): Combining single-agent and multi-agent reinforcement learning with model checking
- [jajapy](https://github.com/Rapfff/jajapy): Baum-Welch algorithm on various kinds of Markov models
- [Momba](https://momba.dev/): Python framework for dealing with quantitative models centered around the JANI-model interchange format
- [Prophesy](https://github.com/moves-rwth/prophesy): Parameter synthesis in Markov models
- [PAYNT](https://github.com/randriu/synthesis): Automated synthesis of probabilistic programs
- [QMaude](https://maude.ucm.es/qmaude/): Quantitative specification and verification in Maude
- [SAFEST](https://www.safest.dgbtek.com/): Dynamic fault-tree analysis tool
- [STAMINA](https://staminachecker.org/): State-space truncation tool that can analyze infinite-sized models
- [TEMPEST](https://tempest-synthesis.org/): Synthesis tool for reactive systems and shields in probabilistic environments

## Papers using Storm as a backend

{:.alert .alert-info}
The publications in this category present tools, algorithms and case studies that use Storm as a backend. 

{% bibliography --group_by year --query @*[category=using] %}

{:.alert .alert-danger}
If there is a publication missing in some of the lists above, feel free to [contact us]({{ '/about.html#people' | relative_url }}){:.alert-link}.
