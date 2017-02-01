---
title: About
navigation_weight: 1
layout: default
---

storm is framework for probabilistic model checking[^1]. It can be used to verify that models including stochastic behavior satisfy properties.  

# Distinguishing features
storm distinguishes itself from other probabilistic model checkers by a couple of features:
<!-- Tabs & box are in html -->

<br/>
<ul class="nav nav-tabs" role="tablist">
<li class="nav-item">
<a class="nav-link active" data-toggle="tab" href="#perfcore" role="tab">
<i class="fa fa-cogs" aria-hidden="true"></i>
Performant Core 
</a>
</li>
<li class="nav-item">
<a class="nav-link" data-toggle="tab" href="#modsolvers" role="tab">
<i class="fa fa-wrench" aria-hidden="true"></i>
Modular Solvers
</a>
</li>
<li class="nav-item">
<a class="nav-link" data-toggle="tab" href="#inputs" role="tab">
<i class="fa fa-language" aria-hidden="true"></i>
Variety of Inputs
</a>
</li>
<li class="nav-item">
<a class="nav-link" data-toggle="tab" href="#python" role="tab">
<i class="fa fa-tachometer" aria-hidden="true"></i>
Rapid Prototyping
</a>
</li>
</ul>


{::options parse_block_html="true" /}
<!-- Tab panes -->
<div class="tab-content">
<div class="tab-pane active" id="perfcore" role="tabpanel">

</div>
<div class="tab-pane" id="modsolvers" role="tabpanel">
Solvers are used for a variety of tasks within storm. The form of the tasks, e.g., a linear equation system, an satisfiability problem or an optimization problem, is often similar. It is thus beneficial to provide a set of solvers which handle these problems efficiently. However, while the form may be similar, the structure of the problems themselve vary signicantly, and different implementations for these task each have their on strengths. Thus, instead of having only one equation system solver and one satisfiability solver, having a set of solvers increases storms performance.

Moreover, many state-of-the-art solvers have an academic license -- this makes it impossible to ship the solvers directly with storm. However, with the flexibility storm offers with regard to solver selection, academic users can use these state-of-the-art solvers. 

Finally, e.g. the SMT community is very active, and state-of-the-art solvers of today may be deprecated tomorrow. The flexibility in adding new solvers ensures that storm is easiliy kept up-to-date, without destroying backward compatibility.
</div>
<div class="tab-pane" id="inputs" role="tabpanel">
storm has support for a variety of inputs: This is not only convenient for the user, it also allows domain-specific optimizations as the structure of the original problem is preserved and thus accesible for storm.
</div>
<div class="tab-pane" id="python" role="tabpanel">
[Stormpy](https://moves-rwth.github.io/stormpy/) provides python bindings for an ever growing part of its code base. The bindings allow to utilize the high-performance routines of storm in combination with python. This has several advances, including but not limited to: 

- pythons simple IO capability
- plotting via matplotlib, 
- calling solver routines such as z3 or cplex,
- libraries from the area of arteficial intelligence and machine learning.
    
</div>
</div>

<script>
$('#myTab a').click(function (e) {
e.preventDefault()
$(this).tab('show')
})
</script>
<br/>

[^1]: [J.-P. Katoen, *The Probabilistic Model Checking Landscape*, 2016](http://www-i2.informatik.rwth-aachen.de/pub/index.php?type=download&pub_id=1296)

# Some key facts about storm:

- ~110k lines of C++ code
- Under development since 2012
- Available as open source since 2017
- Over 15 contributors
- Supported on most Unix platforms
- Impossible without all the cool libraries around: [Thank you](thanks.html)!

# People behind storm

The developers can be reached via storm-dev ```at``` i2.informatik.rwth-aachen.de

storm has been developed at the [Chair for Software Modeling and Verification](http://moves.rwth-aachen.de) at RWTH Aachen University by

- [Christian Dehnert](#)
- [Sebastian Junges](#)
- [Joost-Pieter Katoen](#)
- [Matthias Volk](#)

and received significant contributions from 

- [Philipp Berger](#)
- [Tim Quatman](#) 

