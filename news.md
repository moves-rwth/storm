---
title: News
navigation_weight: 0
layout: default
---

{% for post in site.posts %}
<div class="panel panel-default">
    <div class="panel-heading">{{ post.title }}
        <div class="pull-right">{{ post.date | date_to_long_string }}</div>
    </div>
    <div class="panel-body">
        {{post.excerpt}}
        <a href="{{ post.url }}">Read more</a>
    </div>
    {% if post.tags != empty %}
    <div class="panel-footer">
        {% for tag in post.tags %}
        <span class="badge badge-primary">{{ tag }}</span>
        {% endfor %}
    </div>
    {% endif %}
</div>
{% endfor %}
