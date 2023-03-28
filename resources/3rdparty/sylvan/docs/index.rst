Sylvan
=====================

Sylvan is a parallel (multi-core) MTBDD library written in C. Sylvan
implements parallelized operations on BDDs, MTBDDs and LDDs. Both
sequential and parallel BDD-based algorithms can benefit from
parallelism. Sylvan uses the work-stealing framework Lace and parallel
datastructures to implement scalable multi-core operations on decision
diagrams.

Sylvan is developed (© 2011-2016) by the `Formal Methods and
Tools <http://fmt.ewi.utwente.nl/>`__ group at the University of Twente
as part of the MaDriD project, which is funded by NWO, and (© 2016-2017)
by the `Formal Methods and Verification <http://fmv.jku.at/>`__ group at
the Johannes Kepler University Linz as part of the RiSE project. Sylvan
is licensed with the Apache 2.0 license.
The main author of the project is Tom van Dijk who can be reached via
tom@tvandijk.nl.  
Please let us know if you use Sylvan in your projects and if you need
decision diagram operations that are currently not implemented in Sylvan.

The main repository of Sylvan is https://github.com/trolando/sylvan. A
mirror is available at https://github.com/utwente-fmt/sylvan.

Bindings for other languages than C/C++ also exist:

-  Java/JNI bindings: https://github.com/utwente-fmt/jsylvan
-  Haskell bindings: https://github.com/adamwalker/sylvan-haskell
-  Python bindings: https://github.com/johnyf/dd

Dependencies
------------

Sylvan has the following dependencies:

- **CMake** for compiling.
- **gmp** (``libgmp-dev``) for the GMP leaves in MTBDDs.
- **Sphinx** if you want to build the documentation.

Sylvan depends on the `work-stealing framework
Lace <http://fmt.ewi.utwente.nl/tools/lace>`__ for its implementation.
Lace is embedded in the Sylvan distribution.
Lace requires one additional library:

- **hwloc** (``libhwloc-dev``) for pinning worker threads to processors.


Building
--------

It is recommended to build Sylvan in a separate build directory:

.. code:: bash

    mkdir build
    cd build
    cmake ..
    make && make test && make install

It is recommended to use ``ccmake`` to configure the build settings of Sylvan. For example,
you can choose whether you want shared/static libraries, whether you want to enable
statistics gathering and whether you want a ``Debug`` or a ``Release`` build.

Using Sylvan
------------

To use Sylvan, the library and its dependency Lace must be initialized:

.. code:: c

    #include <sylvan.h>

    main() {
        int n_workers = 0; // auto-detect
        lace_init(n_workers, 0);
        lace_startup(0, NULL, NULL);

        // use at most 512 MB, nodes:cache ratio 2:1, initial size 1/32 of maximum
        sylvan_set_limits(512*1024*1024, 1, 5);
        sylvan_init_package();
        sylvan_init_mtbdd();

        /* ... do stuff ... */

        sylvan_stats_report(stdout);
        sylvan_quit();
        lace_exit();
    }

The call to ``lace_init`` initializes the Lace framework, which sets up the data structures
for work-stealing. The parameter ``n_workers`` can be set to 0 for auto-detection. The
function ``lace_startup`` then creates all other worker threads. The worker threads run
until ``lace_exit`` is called. Lace must be started before Sylvan can be initialized.

Sylvan is initialized with a call to ``sylvan_init_package``. Before this call, Sylvan needs to know
how much memory to allocate for the nodes table and the operation cache. In this example, we use the
``sylvan_set_limits`` function to tell Sylvan that it may allocate at most 512 MB for these tables.
The second parameter indicates the ratio of the nodes table and the operation cache, with each
higher number doubling the size of the nodes table. Negative numbers double the size of the operation
cache instead. In the example, we want the nodes table to be twice as big as the operation cache.
The third parameter controls how often garbage collection doubles the table sizes before
their maximum size is reached. The value 5 means that the initial tables are 32x as small as the maximum size.
By default, every execution of garbage collection doubles the table sizes.

After ``sylvan_init_package``, subpackages like ``mtbdd`` and ``ldd`` can be initialized with
``sylvan_init_mtbdd`` and ``sylvan_init_ldd``. This allocates auxiliary datastructures.

If you enabled statistics generation (via CMake), then you can use ``sylvan_stats_report`` to report
the obtained statistics to a given ``FILE*``.

The Lace framework
~~~~~~~~~~~~~~~~~~

Sylvan uses the Lace framework to offer 'automatic' parallelization of decision diagram operations.
Many functions in Sylvan are Lace tasks. To call a Lace task, the variables 
``__lace_worker`` and ``__lace_dq_head`` must be initialized as **local** variables of the current function.
Use the macro ``LACE_ME`` to initialize the variables in every function that calls Sylvan functions
and is not itself a Lace task.

Garbage collection and referencing nodes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Like all decision diagram implementations, Sylvan performs garbage collection.
Garbage collection is triggered when trying to insert a new node and no
empty space can be found in the table within a reasonable upper bound.

Garbage collection can be disabled with ``sylvan_gc_disable`` and enabled again with ``sylvan_gc_enable``.
Call ``sylvan_gc`` to manually trigger garbage collection.

To ensure that no decision diagram nodes are overwritten, you must ensure that
Sylvan knows which decision diagrams you care about.
Each subpackage implements mechanisms to store references to decision diagrams that must be kept.
For example, the *mtbdd* subpackage implements ``mtbdd_protect`` and ``mtbdd_unprotect`` to store pointers to
MTBDD variables.

.. code:: c

    MTBDD* allocate_var() {
        MTBDD* my_var = (MTBDD*)calloc(sizeof(MTBDD), 1);
        mtbdd_protect(my_var);
        return my_var;
    }

    free_var(MTBDD* my_var) {
        mtbdd_unprotect(my_var);
        free(my_var);
    }

If you use ``mtbdd_protect`` you do not need to update the reference every time the value changes.

The *mtbdd* subpackage also implements thread-local stacks to temporarily store pointers and results of tasks:

.. code:: c

    MTBDD some_thing = ...;
    mtbdd_refs_pushptr(&some_thing);
    MTBDD result_param1 = mtbdd_false, result_param2 = mtbdd_false;
    mtbdd_refs_pushptr(&result_param1);
    mtbdd_refs_pushptr(&result_param2);
    while (some_condition) {
        mtbdd_refs_spawn(SPAWN(an_operation, some_thing, param1));
        result_param2 = CALL(an_operation, some_thing, param2);
        result_param1 = mtbdd_refs_sync(SYNC(an_operation));
        some_thing = CALL(another_operation, result1, result2);
    }
    mtbdd_refs_popptr(3);
    return some_thing;

It is recommended to use the thread-local stacks for local variables, and to use the ``protect`` and ``unprotect``
functions for other variables. Every SPAWN and SYNC of a Lace task that returns an MTBDD must be decorated with
``mtbdd_refs_stack`` and ``mtbdd_refs_sync`` as in the above example.

References to decision diagrams must be added before a worker may cooperate on garbage collection.
Workers can cooperate on garbage collection during ``SYNC`` and when functions create nodes or use ``sylvan_gc_test`` to test whether to assist in garbage collection.
Functions for adding or removing references never perform garbage collection.
Furthermore, only the ``mtbdd_makenode`` function (and other node making primitives) implicitly reference their parameters; all other functions do not reference their parameters.
Nesting Sylvan functions (including ``sylvan_ithvar``) is bad practice and should be avoided.

**Warning**: Sylvan is a multi-threaded library and all workers must cooperate for garbage collection. If you use locking mechanisms in your code, beware of deadlocks!
You can explicitly cooperate on garbage collection with ``sylvan_gc_test()``.

Basic BDD/MTBDD functionality
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In Sylvan, BDDs are special cases of MTBDDs.
Several functions are specific for BDDs and they start with ``sylvan_``, whereas generic MTBDD functions start
with ``mtbdd_``.

To create new BDDs, you can use:

- ``mtbdd_true``: representation of constant ``true``.
- ``mtbdd_false``: representation of constant ``false``.
- ``sylvan_ithvar(var)``: representation of literal <var> (negated: ``sylvan_nithvar(var)``)

To follow the BDD edges and obtain the variable at the root of a BDD,
you can use (only for internal nodes, not for leaves ``mtbdd_true`` and ``mtbdd_false``):

- ``mtbdd_getvar(bdd)``: obtain the variable of the root node of <bdd>.
- ``mtbdd_gethigh(bdd)``: follow the high edge of <bdd>.
- ``mtbdd_getlow(bdd)``: follow the low edge of <bdd>.

You need to manually reference BDDs that you want to keep during garbage
collection (see the above explanation):

- ``mtbdd_protect(bddptr)``: add a pointer reference to <bddptr>.
- ``mtbdd_unprotect(bddptr)``: remove a pointer reference to <bddptr>.
- ``mtbdd_refs_pushptr(bddptr)``: add a local pointer reference to <bddptr>.
- ``mtbdd_refs_popptr(amount)``: remove the last <amount> local pointer references.
- ``mtbdd_refs_spawn(SPAWN(...))``: spawn a task that returns a BDD/MTBDD.
- ``mtbdd_refs_sync(SYNC(...))``: sync a task that returns a BDD/MTBDD.

It is recommended to use ``mtbdd_protect`` and ``mtbdd_unprotect``.
The C++ objects (defined in ``sylvan_obj.hpp``) handle this automatically.
For local variables, we recommend ``mtbdd_refs_pushptr`` and ``mtbdd_refs_popptr``.

The following basic BDD operations are implemented:

- ``sylvan_not(bdd)``: compute the negation of <bdd>.
- ``sylvan_ite(a,b,c)``: compute 'if <a> then <b> else <c>'.
- ``sylvan_and(a, b)``: compute '<a> and <b>'.
- ``sylvan_or(a, b)``: compute '<a> or <b>'.
- ``sylvan_nand(a, b)``: compute 'not (<a> and <b>)'.
- ``sylvan_nor(a, b)``: compute 'not (<a> or <b>)'.
- ``sylvan_imp(a, b)``: compute '<a> then <b>'.
- ``sylvan_invimp(a, b)``: compute '<b> then <a>'.
- ``sylvan_xor(a, b)``: compute '<a> xor <b>'.
- ``sylvan_equiv(a, b)``: compute '<a> = <b>'.
- ``sylvan_diff(a, b)``: compute '<a> and not <b>'.
- ``sylvan_less(a, b)``: compute '<b> and not <a>'.
- ``sylvan_exists(bdd, vars)``: existential quantification of <bdd> with respect to variables <vars>.
- ``sylvan_forall(bdd, vars)``: universal quantification of <bdd> with respect to variables <vars>.
- ``sylvan_project(bdd, vars)``: the dual of ``sylvan_exists``, projects the <bdd> to the variable domain <vars>.

A set of variables (like <vars> above) is a BDD representing the conjunction of the variables.
A number of convencience functions are defined to manipulate sets of variables:

- ``mtbdd_set_empty()``: obtain an empty set.
- ``mtbdd_set_isempty(set)``: compute whether the set is empty.
- ``mtbdd_set_first(set)``: obtain the first variable of the set.
- ``mtbdd_set_next(set)``: obtain the subset without the first variable.
- ``mtbdd_set_from_array(arr, len)``: create a set from a given array.
- ``mtbdd_set_to_array(set, arr)``: write the set to the given array.
- ``mtbdd_set_add(set, var)``: compute the set plus the variable.
- ``mtbdd_set_union(set1, set2)``: compute the union of two sets.
- ``mtbdd_set_remove(set, var)``: compute the set minus the variable.
- ``mtbdd_set_minus(set1, set2)``: compute the set <set1> minus the variables in <set2>.
- ``mtbdd_set_count(set)``: compute the number of variables in the set.
- ``mtbdd_set_contains(set, var)``: compute whether the set contains the variable.

Sylvan also implements composition and substitution/variable renaming using a "MTBDD map". An MTBDD map is a special structure
implemented with special MTBDD nodes to store a mapping from variables (uint32_t) to MTBDDs. Like sets of variables and MTBDDs, MTBDD maps must
also be referenced for garbage collection. The following functions are related to MTBDD maps:

- ``mtbdd_compose(dd, map)``: apply the map to the given decision diagram, transforming every node with a variable that is associated with some function F in the map by ``if <F> then <high> else <low>``.
- ``sylvan_compose(dd, map)``: same as ``mtbdd_compose``, but assumes the decision diagram only has Boolean leaves.
- ``mtbdd_map_empty()``: obtain an empty map.
- ``mtbdd_map_isempty(map)``: compute whether the map is empty.
- ``mtbdd_map_key(map)``: obtain the key of the first pair of the map.
- ``mtbdd_map_value(map)``: obtain the value of the first pair of the map.
- ``mtbdd_map_next(map)``: obtain the submap without the first pair.
- ``mtbdd_map_add(map, key, value)``: compute the map plus the given key-value pair.
- ``mtbdd_map_update(map1, map2)``: compute the union of two maps, with priority to map2 if both maps share variables.
- ``mtbdd_map_remove(map, var)``: compute the map minus the variable.
- ``mtbdd_map_removeall(map, set)``: compute the map minus the given variables.
- ``mtbdd_map_count(set)``: compute the number of pairs in the map.
- ``mtbdd_map_contains(map, var)``: compute whether the map contains the variable.

Sylvan implements a number of counting operations:

- ``mtbdd_satcount(bdd, number_of_vars)``: compute the number of minterms (assignments that lead to True) for a function with <number_of_vars> variables; we don't need to know the exact variables that may be in the BDD, just how many there are.
- ``sylvan_pathcount(bdd)``: compute the number of distinct paths to True.
- ``mtbdd_nodecount(bdd)``: compute the number of nodes (and leaves) in the BDD.
- ``mtbdd_nodecount_more(array, length)``: compute the number of nodes (and leaves) in the array of BDDs.

Sylvan implements various advanced operations:

- ``sylvan_and_exists(bdd_a, bdd_b, vars)``: compute ``sylvan_exists(sylvan_and(bdd_a, bdd_b), vars)`` in one step.
- ``sylvan_and_project(bdd_a, bdd_b, vars)``: compute ``sylvan_project(sylvan_and(bdd_a, bdd_b), vars)`` in one step.
- ``sylvan_cube(vars, values)``: compute a cube (to leaf True) of the given variables, where the array values indicates for each variable whether to use it in negative form (value 0) or positive form (value 1) or to skip it (as dont-care, value 2).
- ``sylvan_union_cube(set, vars, values)``: compute ``sylvan_or(set, sylvan_cube(vars, values))`` in one step.
- ``sylvan_constrain(bdd_f, bdd_c)``: compute the generic cofactor of F constrained by C, i.e, set F to False for all assignments not in C.
- ``sylvan_restrict(bdd_f, bdd_c)``: compute Coudert and Madre's restrict algorithm, which tries to minimize bdd_f according to a care set C using sibling substitution; the invariant is ``restrict(f, c) \and c == f \and c``; the result of this algorithm is often but not always smaller than the original.
- ``sylvan_pick_cube(bdd)`` or ``sylvan_sat_one_bdd(bdd)``: extract a single path to True from the BDD (returns the BDD of this path)
- ``sylvan_pick_single_cube(bdd, vars)`` or ``sylvan_sat_single(bdd, vars)`` extracts a single minterm from the BDD (returns the BDD of this assignment)
- ``sylvan_sat_one(bdd, vars, array)``: extract a single minterm from the BDD given the set of variables and write the values of the variables in order to the given array, with 0 when it is negative, 1 when it is positive, and 2 when it is dontcare.

Sylvan implements several operations for transition systems. These operations assume an interleaved variable ordering, such that *source* or *unprimed* variables have even parity (0, 2, 4...) and matching *target* or *primed* variables have odd parity (1, 3, 5...).
The transition relations may be partial transition relations that only manipulate a subset of variables; hence, the operations also require the set of variables.

- ``sylvan_relnext(set, relation, vars)``: apply the (partial) relation on the given variables to the set.
- ``sylvan_relprev(relation, set, vars)``: apply the (partial) relation in reverse to the set; this computes predecessors but can also concatenate relations as follows: ``sylvan_relprev(rel1, rel2, rel1_vars)``.
- ``sylvan_closure(relation)``: compute the transitive closure of the given set recursively (see Matsunaga et al, DAC 1993)

See ``src/sylvan_bdd.h`` and ``src/mtbdd.h`` for other operations on BDDs and MTBDDs.

Custom leaves
~~~~~~~~~~~~~

See ``src/sylvan_mt.h`` and the example in ``src/sylvan_gmp.h`` and ``src/sylvan_gmp.c`` for custom leaves in MTBDDs.

Custom decision diagram operations
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Adding custom decision diagram operations is easy. Include ``sylvan_int.h`` for the internal functions. See ``sylvan_cache.h``
for how to use the operation cache.

List decision diagrams
~~~~~~~~~~~~~~~~~~~~~~

See ``src/sylvan_ldd.h`` for operations on list decision diagrams.

File I/O
~~~~~~~~

You can store and load BDDs using a number of methods, which are documented in the header files ``sylvan_mtbdd.h`` and ``sylvan_ldd.h``.

Support for C++
~~~~~~~~~~~~~~~

See ``src/sylvan_obj.hpp`` for the C++ interface.

Table resizing
~~~~~~~~~~~~~~

During garbage collection, it is possible to resize the nodes table and
the cache. By default, Sylvan doubles the table sizes during every garbage
collection until the maximum table size has been reached. There is also a
less aggressive version that only resizes when at least half the table is
full. This can be configured in ``src/sylvan_config.h``. It is not
possible to decrease the size of the nodes table and the cache.

Dynamic reordering
~~~~~~~~~~~~~~~~~~

Dynamic reordening is not yet supported. For now, we suggest users
find a good static variable ordering.

Examples
--------

Simple examples can be found in the ``examples`` subdirectory. The file
``simple.cpp`` contains a toy program that uses the C++ objects to
perform basic BDD manipulation. The ``mc.c`` and ``lddmc.c`` programs
are more advanced examples of symbolic model checking (with example
models in the ``models`` subdirectory).

Troubleshooting
---------------

Sylvan may require a larger than normal program stack. You may need to
increase the program stack size on your system using ``ulimit -s``.
Segmentation faults on large computations typically indicate a program
stack overflow.

I am getting the error "unable to allocate memory: ...!"
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Sylvan allocates virtual memory using mmap. If you specify a combined
size for the cache and node table larger than your actual available
memory you may need to set ``vm.overcommit_memory`` to ``1``. E.g.
``echo 1 > /proc/sys/vm/overcommit_memory``. You can make this setting
permanent with
``echo "vm.overcommit_memory = 1" > /etc/sysctl.d/99-sylvan.conf``. You
can verify the setting with ``cat /proc/sys/vm/overcommit_memory``. It
should report ``1``.

I get errors about ``__lace_worker`` and ``__lace_dq_head``
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Many Sylvan operations are implemented as Lace tasks. To call a Lace
task, the variables ``__lace_worker`` and ``__lace_dq_head`` must be
initialized. Use the macro ``LACE_ME`` to do this. Only use ``LACE_ME``
locally (in a function), never globally!

Publications
------------

T. van Dijk (2016) `Sylvan: Multi-core Decision
Diagrams <http://dx.doi.org/10.3990/1.9789036541602>`__. PhD Thesis.

T. van Dijk and J.C. van de Pol (2016) `Sylvan: Multi-core Framework
for Decision Diagrams <http://dx.doi.org/10.1007/s10009-016-0433-2>`__.
In: STTT (Special Issue), Springer.

T. van Dijk and J.C. van de Pol (2015) `Sylvan: Multi-core Decision
Diagrams <http://dx.doi.org/10.1007/978-3-662-46681-0_60>`__. In: TACAS
2015, LNCS 9035. Springer.

T. van Dijk and A.W. Laarman and J.C. van de Pol (2012) `Multi-Core BDD
Operations for Symbolic
Reachability <http://eprints.eemcs.utwente.nl/22166/>`__. In: PDMC 2012,
ENTCS. Elsevier.


