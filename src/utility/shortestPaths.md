# k-shortest Path Generator
[This is a collection of random notes for now, to help me remember
the design decisions and the rationale behind them.]

## Differences from REA algorithm
This class closely follows the Jimenez-Marzal REA algorithm.
However, there are some notable deviations:

### Target groups
Firstly, instead of a single target state, a group of target states is
considered. It is clear that this can achieved by removing all outgoing
transitions (other than self-loops, but this is moot due to not allowing
non-minimal paths -- see below) and instead introducing edges to a new sink
state that serves as a meta-target.

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

### Minimality of paths
Secondly, we define shortest paths as "minimal" shortest paths in the
following sense: The path may only visit the target state once (at the
end). In other words, no KSP may be a prefix of another.
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
