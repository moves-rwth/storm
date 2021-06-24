---
title: Documentation
navigation_weight: 3
layout: default
---

{% for cat in site.category-list %}
### {{ cat }}
<ul>
  {% assign sorted_pages = site.pages | sort: 'category_weight' %}
  {% for page in sorted_pages %}
    {% if page.documentation == true %}
      {% for pc in page.categories %}
        {% if pc == cat %}
          <li><a href="{{ page.url | relative_url }}">{{ page.title }}</a></li>
        {% endif %}   <!-- cat-match-p -->
      {% endfor %}  <!-- page-category -->
    {% endif %}   <!-- resource-p -->
  {% endfor %}  <!-- page -->
</ul>
{% endfor %}  <!-- cat -->
