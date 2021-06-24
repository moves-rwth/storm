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

## Papers using Storm as a backend

{:.alert .alert-info}
The publications in this category present tools, algorithms and case studies that use Storm as a backend. 

{% bibliography --group_by year --query @*[category=using] %}

{:.alert .alert-danger}
If there is a publication missing in some of the lists above, feel free to [contact us]({{ '/about.html#people' | relative_url }}).