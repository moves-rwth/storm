# k-shortest Path Generator
[This is a collection of random notes for now, to help me remember
the design decisions and the rationale behind them.]

## Differences from REA algorithm
This class closely follows the Jimenez-Marzal REA algorithm.
However, there are some notable deviations in the way targets and shortest
paths are defined.

### Target groups
Firstly, instead of a single target state, a group of target states is
considered. It is clear that this can achieved by removing all outgoing
transitions (other than self-loops, but this is moot due to not allowing
non-minimal paths -- see below) and instead introducing edges to a new sink
state that serves as a meta-target.

<!--
In terms of implementation, there are two possible routes (that I can think
of):

 - Simply (but destructively) modifying the precomputation results (in
   particular the predecessor list). This is straightforward and has no
   performance penalty, but implies that each instance of the SP-Generator
   is restricted to a single group of targets.
   (Whereas the original algorithm can compute the KSPs to several targets,
   reusing all partial results.)
 - Keeping the computation "clean" so that all results remain universally
   valid (and thus reusable for other targets) by means of reversibly
   "overlaying" the graph modifications.

It is not clear if there will ever be a need for changing targets. While
the overlay option is alluring, in the spirit of YAGNI, I choose the
destructive, simple route.
-->

I chose to implement this by modifying the precomputation results, meaning
that they are only valid for a fixed group of target states. Thus, the
target group is required in the constructor. (It would have been possible to
allow for interchangeable target groups, but I don't anticipate that use
case.)

#### Special case: Using Matrix/Vector from SamplingModel

The class has been updated to support the matrix/vector that `SamplingModel`
generates (as an instance of a PDTMC) as input. This is in fact closely
related to the target groups, since it works as follows:

The input is a (sub-stochastic) transition matrix of the maybe-states (only!)
and a vector (again over the maybe-states) with the probabilities to an
implied target state.

This naturally corresponds to having a meta-target, except the probability
of its incoming edges range over $(0,1]$ rather than being $1$.
Thus, applying the term "target group" to the set of states with non-zero
transitions to the meta-target is now misleading (I suppose the correct term
would now be "meta-target predecessors"), but nevertheless it should work
exactly the same. [Right?]

In terms of implementation, in `getEdgeDistance` as well as in the loop of
the Dijkstra, the "virtual" edges to the meta-target were checked for and
set to probability $1$; this must now be changed to use the probability as
indicated in the `targetProbVector` if this input format is used.

### Minimality of paths
Secondly, we define shortest paths as "minimal" shortest paths in the
following sense: The path may not visit any target state except at the
end. As a corollary, no KSP (to a target node) may be a prefix of another.
This in particular forbids shortest path progressions such as these:

    1-SP: 1 2 3
    2-SP: 1 2 3 3
    3-SP: 1 2 3 3 3
    ...

This is a common feature if the target state is a sink; but we are not
interested in such paths.

(In fact, ideally we'd like to see paths whose node-intersection with all
shorter paths is non-empty (which is an even stronger statement than
loop-free-ness of paths), because we want to take a union of those node
sets. But that's a different matter.)


## Data structures, in particular: Path representation

The implicit shortest path representation that J&M describe in the paper
is used, except that indices rather than back-pointers are stored to
refer to the tail of the path.
[Maybe pointers would be faster? STL vector access via index should be
pretty fast too, though, and less error-prone.]

A bit more detail (recap of the paper):
All shortest paths (from `s` to `t`) can be described as some k-shortest
path to some node `u` plus an edge to `t`:

    s ~~k-shortest path~~> u --> t

Further, the shortest paths to some node are always computed in order and
without gaps, e.g., the 1, 2, 3-shortest paths to `t` will be computed
before the 4-SP. Thus, we store the SPs in a linked list for each node,
with the k-th entry[^1] being the k-th SP to that node.

Thus for an SP as shown above we simply store the predecessor node (`u`)
and the `k`, which allows us to look up the tail of the SP.
By recursively looking up the tail (until it's empty), we reconstruct
the entire path back-to-front.

[^1]: Which due to 0-based indexing has index `k-1`, of course! Damn it.
