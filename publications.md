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
The publications in this category present functionality from which a substantial part (but not necessarily everything) is available in Storm's release.

{% bibliography --group_by year --query @*[category=feature] %}

## Papers using Storm as a backend

{:.alert .alert-warning}
The publications in this category present tools, algorithms and case studies that use Storm as a backend. At least some parts of this functionality are currently not part of Storm itself. The source code and results may be outdated or even unavailable.

{% bibliography --group_by year --query @*[category=using] %}
