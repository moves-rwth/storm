cpptempl
=================
This is a template engine for C++.

Copyright
==================
Author: Ryan Ginstrom
MIT License

Syntax
=================

Variables::

	{$variable_name}

Loops::

	{% for person in people %}Name: {$person.name}{% endfor %}

If::

	{% if person.name == "Bob" %}Full name: Robert{% endif %}


Usage
=======================

Define a template::

	string text = "{% if item %}{$item}{% endif %}\n"
		"{% if thing %}{$thing}{% endif %}" ;

Set up data::

	cpptempl::data_map data ;
	data["item"] = "aaa" ;
	data["thing"] = "bbb" ;

Parse the template and data::

	string result = cpptempl::parse(text, data) ;

Lists, nested maps
========================

Example::

	cpptempl::data_map person_data ;
	person_data["name"] = "Bob" ;
	person_data["occupation"] = "Plumber" ;

	cpptempl::data_map content ;
	content["person"] = person_data ;
	content["friends"].push_back("Alice") ;
	content["friends"].push_back("Bob") ;
