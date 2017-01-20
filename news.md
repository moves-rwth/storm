---
title: News
navigation_weight: 1
layout: default
---

<div class="news-overview">

<ul class="list-group">
  
  {% for post in site.posts %}
  <a href="{{ post.url | prepend site.github.url }}" class="list-group-item list-group-item-action flex-column align-items-start">
    <div class="d-flex w-100 justify-content-between">
      <h5 class="mb-1">{{ post.title }}</h5>
      <small class="news-date">{{ post.date | date: "%-d %B %Y"}}</small>
    </div>
   {% for tag in post.tags %}
	<span class="badge badge-primary">{{ tag }}</span>
   {% endfor %}
   <br/>
   {{post.excerpt}}
    <small>more...</small>
  </a>
  
  {% endfor %}
</ul>
</div>
