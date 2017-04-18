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

Sylvan has the following required dependencies:

- **CMake** for compiling.
- **gmp** (``libgmp-dev``) for the GMP leaves in MTBDDs.
- **hwloc** (``libhwloc-dev``) for pinning worker threads to processors.

Sylvan depends on the `work-stealing framework
Lace <http://fmt.ewi.utwente.nl/tools/lace>`__ for its implementation.
Lace is embedded in the Sylvan distribution.

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

        size_t nodes_minsize = 1LL<<22;
        size_t nodes_maxsize = 1LL<<26;
        size_t cache_minsize = 1LL<<23;
        size_t cache_maxsize = 1LL<<27;
        sylvan_init_package(nodes_minsize, nodes_maxsize, cache_minsize, cache_maxsize);
        sylvan_init_mtbdd();

        ...

        sylvan_stats_report(stdout);
        sylvan_quit();
        lace_exit();
    }

The call to ``lace_init`` initializes the Lace framework, which sets up the data structures
for work-stealing. The parameter ``n_workers`` can be set to 0 for auto-detection. The
function ``lace_startup`` then creates all other worker threads. The worker threads run
until ``lace_exit`` is called. Lace must be started before Sylvan can be initialized.

Sylvan is initialized with a call to ``sylvan_init_package``. Here we choose the initial
and maximum sizes of the nodes table and the operation cache. In the example, we choose a maximum
nodes table size of 2^26 and a maximum cache size of 2^27. The initial sizes are
set to 2^22 and 2^23, respectively. The sizes must be powers of 2.
Sylvan allocates memory for the maximum sizes *in virtual memory* but only uses the space
needed for the initial sizes. The sizes are doubled during garbage collection, until the maximum
size has been reached.

After ``sylvan_init_package``, the subpackages ``mtbdd`` and ``ldd`` can be initialized with
``sylvan_init_mtbdd`` and ``sylvan_init_ldd``. This mainly allocates auxiliary datastructures for
garbage collection.

If you enable statistics generation (via CMake) then you can use ``sylvan_stats_report`` to report
the obtained statistics to a given ``FILE*``.

The Lace framework
~~~~~~~~~~~~~~~~~~

Sylvan uses the Lace framework to offer 'automatic' parallelization of decision diagram operations.
Many functions in Sylvan are Lace tasks. To call a Lace task, the variables 
``__lace_worker`` and ``__lace_dq_head`` must be initialized **locally**.
Use the macro ``LACE_ME`` to initialize the variables in every function that calls Sylvan functions
and is not itself a Lace task.

Garbage collection and referencing nodes
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Like all decision diagram implementations, Sylvan performs garbage collection.
Garbage collection is triggered when trying to insert a new node and no
empty space can be found in the table within a reasonable upper bound.

To ensure that no decision diagram nodes are overwritten, you must ensure that
Sylvan knows which decision diagrams you care about.
The easiest way to do this is with ``sylvan_protect`` and ``sylvan_unprotect`` to protect
a given pointer.
These functions protect the decision diagram referenced to by that pointer at the time
that garbage collection is performed.
Unlike some other implementations of decision diagrams,
you can modify the variable between the calls to ``sylvan_protect`` and ``sylvan_unprotect``
without explicitly changing the reference.

To manually trigger garbage collection, call ``sylvan_gc``.
You can use ``sylvan_gc_disable`` and ``sylvan_gc_enable`` to disable garbage collection or
enable it again. If garbage collection is disabled, the program will abort when the nodes table
is full.
**Warning**: Sylvan is a multi-threaded library and all workers must cooperate for garbage collection. If you use locking mechanisms in your code, beware of deadlocks!

Basic BDD functionality
~~~~~~~~~~~~~~~~~~~~~~~

To create new BDDs, you can use:

- ``sylvan_true``: representation of constant ``true``.
- ``sylvan_false``: representation of constant ``false``.
- ``sylvan_ithvar(var)``: representation of literal <var> (negated: ``sylvan_nithvar(var)``)

To follow the BDD edges and obtain the variable at the root of a BDD,
you can use (only for internal nodes, not for leaves ``sylvan_true`` and ``sylvan_false``):

- ``sylvan_var(bdd)``: obtain the variable of the root node of <bdd>.
- ``sylvan_high(bdd)``: follow the high edge of <bdd>.
- ``sylvan_low(bdd)``: follow the low edge of <bdd>.

You need to manually reference BDDs that you want to keep during garbage
collection:

- ``sylvan_protect(bddptr)``: add a pointer reference to <bddptr>.
- ``sylvan_unprotect(bddptr)``: remove a pointer reference to <bddptr>.
- ``sylvan_ref(bdd)``: add a reference to <bdd>.
- ``sylvan_deref(bdd)``: remove a reference to <bdd>.

It is recommended to use ``sylvan_protect`` and ``sylvan_unprotect``.
The C++ objects (defined in ``sylvan_obj.hpp``) handle this automatically.

The following basic operations are implemented:

- ``sylvan_not(bdd)``: compute the negation of <bdd>.
- ``sylvan_ite(a,b,c)``: compute 'if <a> then <b> else <c>'.
- ``sylvan_and(a, b)``: compute '<a> and <b>'
- ``sylvan_or(a, b)``: compute '<a> or <b>'
- ``sylvan_nand(a, b)``: compute 'not (<a> and <b>)'
- ``sylvan_nor(a, b)``: compute 'not (<a> or <b>)'
- ``sylvan_imp(a, b)``: compute '<a> then <b>'
- ``sylvan_invimp(a, b)``: compute '<b> then <a>'
- ``sylvan_xor(a, b)``: compute '<a> xor <b>'
- ``sylvan_equiv(a, b)``: compute '<a> = <b>'
- ``sylvan_diff(a, b)``: compute '<a> and not <b>'
- ``sylvan_less(a, b)``: compute '<b> and not <a>'
- ``sylvan_exists(bdd, vars)``: existential quantification of <bdd> with respect to variables <vars>.
- ``sylvan_forall(bdd, vars)``: universal quantification of <bdd> with respect to variables <vars>.

A set of variables (like <vars> above) is a BDD representing the conjunction of the variables.

Other BDD operations
~~~~~~~~~~~~~~~~~~~~

See ``src/sylvan_bdd.h`` for other operations on BDDs, especially operations
that are relevant for model checking.

Basic MTBDD functionality
~~~~~~~~~~~~~~~~~~~~~~~~~

See ``src/sylvan_mtbdd.h`` for operations on multi-terminal BDDs.

Basic LDD functionality
~~~~~~~~~~~~~~~~~~~~~~~

See ``src/sylvan_ldd.h`` for operations on List DDs.

Support for C++
~~~~~~~~~~~~~~~

See ``src/sylvan_obj.hpp`` for the C++ interface.

.. Adding custom decision diagram operations
.. ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Table resizing
~~~~~~~~~~~~~~

During garbage collection, it is possible to resize the nodes table and
the cache. Sylvan provides two default implementations: an aggressive
version that resizes every time garbage collection is performed, and a
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


