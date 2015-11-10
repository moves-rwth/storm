Sylvan
======
Sylvan is a parallel (multi-core) BDD library in C. Sylvan allows both sequential and parallel BDD-based algorithms to benefit from parallelism. Sylvan uses the work-stealing framework Lace and a scalable lockless hashtable to implement scalable multi-core BDD operations.

Sylvan is developed (&copy; 2011-2014) by the [Formal Methods and Tools](http://fmt.ewi.utwente.nl/) group at the University of Twente as part of the MaDriD project, which is funded by NWO. Sylvan is licensed with the Apache 2.0 license.

You can contact the main author of Sylvan at <t.vandijk@utwente.nl>. Please let us know if you use Sylvan in your projects.

Sylvan is available at: https://github.com/utwente-fmt/sylvan  
Java/JNI bindings: https://github.com/trolando/jsylvan  
Haskell bindings: https://github.com/adamwalker/sylvan-haskell

Publications
------------
Dijk, T. van (2012) [The parallelization of binary decision diagram operations for model checking](http://essay.utwente.nl/61650/). Master’s Thesis, University of Twente, 27 April 2012.

Dijk, T. van and Laarman, A.W. and Pol, J.C. van de (2012) [Multi-Core BDD Operations for Symbolic Reachability](http://eprints.eemcs.utwente.nl/22166/). In: 11th International Workshop on Parallel and Distributed Methods in verifiCation, PDMC 2012, 17 Sept. 2012, London, UK. Electronic Notes in Theoretical Computer Science. Elsevier.

Usage
-----
For a quick demo, you can find an example of a simple BDD-based reachability algorithm in `test/mc.c`. This demo features both a sequential (bfs) and a parallel (par) symbolic reachability algorithm, demonstrating how both sequential programs and parallel programs can benefit from parallized BDD operations.

To use Sylvan, include header file `sylvan.h`.

Sylvan depends on the [work-stealing framework Lace](http://fmt.ewi.utwente.nl/tools/lace) for its implementation. Currently, Lace is embedded in the Sylvan distribution, but this may change in the future. To use the BDD operations of Sylvan, you must first start Lace and run all workers.

Sylvan must be initialized after `lace_init` with a call to `sylvan_init`. This function takes three parameters: the log2 size of the BDD nodes table, the log2 size of the operation cache and the caching granularity. For example, `sylvan_init(16, 15, 4)` will initialize Sylvan with a BDD nodes table of size 65536, an operation cache of size 32768 and granularity 4. See below for detailed information about caching granularity.

Example C code for initialization:
```
#include <sylvan.h>

// initialize with queue size of 1000000
// autodetect number of workers
lace_init(0, 1000000);
// startup with defaults (create N-1 workers)
lace_startup(0, NULL, NULL);
// initialize with unique table size = 2^25
//                 cache table size = 2^24
//                 cache granularity = 4
sylvan_init(25, 24, 4);
```

Sylvan may require a larger than normal program stack. You may need to increase the program stack size on your system using `ulimit -s`. Segmentation faults on large computations typically indicate a program stack overflow.

### Basic functionality

To create new BDDs, you can use:
- `sylvan_true`: representation of constant `true`.
- `sylvan_false`: representation of constant `false`.
- `sylvan_ithvar(var)`: representation of literal &lt;var&gt;.
- `sylvan_nithvar(var)`: representation of literal &lt;var&gt; negated.
- `sylvan_cube(variables, count, vector)`: create conjunction of variables in &lt;variables&gt; according to the corresponding value in &lt;vector&gt;.

To walk the BDD edges and obtain the variable at the root of a BDD, you can use:
- `sylvan_var(bdd)`: obtain variable of the root node of &lt;bdd&gt; - requires that &lt;bdd&gt; is not constant `true` or `false`.
- `sylvan_high(bdd)`: follow high edge of &lt;bdd&gt;.
- `sylvan_low(bdd)`: follow low edge of &lt;bdd&gt;.

You need to manually reference BDDs that you want to keep during garbage collection:
- `sylvan_ref(bdd)`: add reference to &lt;bdd&gt;.
- `sylvan_deref(bdd)`: remove reference to &lt;bdd&gt;.

See below for more detailed information about garbage collection. Note that garbage collection does not proceed outside BDD operations, e.g., you can safely call `sylvan_ref` on the result of a BDD operation.

The following 'primitives' are implemented:
- `sylvan_not(bdd)`: negation of &lt;bdd&gt;.
- `sylvan_ite(a,b,c)`: calculate 'if &lt;a&gt; then &lt;b&gt; else &lt;c&gt;'.
- `sylvan_exists(bdd, vars)`: existential quantification of &lt;bdd&gt; with respect to variables &lt;vars&gt;. Here, &lt;vars&gt; is a disjunction of literals.

Sylvan uses complement edges to implement negation, therefore negation is performed in constant time.

These primitives are used to implement the following operations:
- `sylvan_and(a, b)`
- `sylvan_or(a, b)`
- `sylvan_nand(a, b)`
- `sylvan_nor(a, b)`
- `sylvan_imp(a, b)` (a implies b)
- `sylvan_invimp(a, b)` (b implies a)
- `sylvan_xor(a, b)`
- `sylvan_equiv(a, b)` (alias: `sylvan_biimp`)
- `sylvan_diff(a, b)` (a and not b)
- `sylvan_less(a, b)` (b and not a)
- `sylvan_forall(bdd, vars)`

### Other BDD operations

We also implemented the following BDD operations:
- `sylvan_restrict(f, c)`: calculate the restrict algorithm on f,c.
- `sylvan_constrain(f, c)`: calculate the constrain f@c, also called generalized co-factor (gcf).
- `sylvan_relprod_paired(a, t, vars)`: calculate the relational product by applying transition relation &lt;t&gt; on &lt;a&gt;. Assumes variables in &lt;a&gt; are even and primed variables (only in &lt;t&gt;) are odd and paired (i.e., x' = x+1). Assumes &lt;t&gt; is defined on variables in &lt;vars&gt;, which is a disjunction of literals, including non-primed and primed variables.
- `sylvan_relprod_paired_prev(a, t, vars)`: calculate the relational product backwards, i.e., calculate previous states instead of next states.
- `sylvan_support(bdd)`: calculate the support of &lt;bdd&gt;, i.e., all variables that occur in the BDD; returns a disjunction of literals.
- `sylvan_satcount(bdd, vars)`: calculate the number of assignments that satisfy &lt;bdd&gt; with respect to the variables in &lt;vars&gt;, which is a disjunction of literals.
- `sylvan_pathcount(bdd)`: calculate the number of distinct paths from the root node of &lt;bdd&gt; to constant 'true'.
- `sylvan_nodecount(bdd)`: calculate the number of nodes in &lt;bdd&gt; (not thread-safe).
- `sylvan_sat_one_bdd(bdd)`: calculate a cube that satisfies &lt;bdd&gt;.
- `sylvan_sat_one(bdd, vars, count, vector)`: reverse of `sylvan_cube` on the result of `sylvan_sat_one_bdd`. Alias: `sylvan_pick_cube`.
 
### Garbage collection

Garbage collection is triggered when trying to insert a new node and no new bucket can be found within a reasonable upper bound. This upper bound is typically 8*(4+tablesize) buckets, where tablesize is the log2 size of the table. In practice, garbage collection occurs when the table is about 90% full. Garbage collection occurs by rehashing all BDD nodes that must stay in the table. It is designed such that all workers must cooperate on garbage collection. This condition is checked in `sylvan_gc_test()` which is called from every BDD operation and from a callback in the Lace framework that is called when there is no work.

- `sylvan_gc()`: manually trigger garbage collection.
- `sylvan_gc_enable()`: enable garbage collection.
- `sylvan_gc_disable()`: disable garbage collection.

### Caching granularity

Caching granularity works as follows: with granularity G, each variable x is in a variable group x/G, e.g. the first G variables are in variable group 0, the next G variables are in variable group 1, etc.
BDD operations only consult the operation cache when they change variable groups.

Higher granularity values result in less cache use. In practice, values between 4 and 8 tend to work well, but this depends on many factors, such as the structure and size of the BDDs and the characteristics of the computer architecture.

### Dynamic reordering

Dynamic reordening is currently not supported.
We are interested in examples where this is necessary for good performance and would like to perform research into parallel dynamic reordering in future work.

For now, we suggest users find a good static variable ordering.

### Resizing tables

Resizing of nodes table and operation cache is currently not supported. In theory stop-the-world resizing can be implemented to grow the nodes table, but not to shrink the nodes table, since shrinking requires recreating all BDDs. Operation cache resizing is trivial to implement.

Please let us know if you need this functionality.

