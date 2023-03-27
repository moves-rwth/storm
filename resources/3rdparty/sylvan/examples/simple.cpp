#include <assert.h>
#include <stdio.h>
#include <stdint.h>

#include <sylvan.h>
#include <sylvan_obj.hpp>

using namespace sylvan;

VOID_TASK_0(simple_cxx)
{
    Bdd one = Bdd::bddOne(); // the True terminal
    Bdd zero = Bdd::bddZero(); // the False terminal

    // check if they really are the True/False terminal
    assert(one.GetBDD() == sylvan_true);
    assert(zero.GetBDD() == sylvan_false);

    Bdd a = Bdd::bddVar(0); // create a BDD variable x_0
    Bdd b = Bdd::bddVar(1); // create a BDD variable x_1

    // check if a really is the Boolean formula "x_0"
    assert(!a.isConstant());
    assert(a.TopVar() == 0);
    assert(a.Then() == one);
    assert(a.Else() == zero);

    // check if b really is the Boolean formula "x_1"
    assert(!b.isConstant());
    assert(b.TopVar() == 1);
    assert(b.Then() == one);
    assert(b.Else() == zero);

    // compute !a
    Bdd not_a = !a;

    // check if !!a is really a
    assert((!not_a) == a);

    // compute a * b and !(!a + !b) and check if they are equivalent
    Bdd a_and_b = a * b;
    Bdd not_not_a_or_not_b = !(!a + !b);
    assert(a_and_b == not_not_a_or_not_b);

    // perform some simple quantification and check the results
    Bdd ex = a_and_b.ExistAbstract(a); // \exists a . a * b
    assert(ex == b);
    Bdd andabs = a.AndAbstract(b, a); // \exists a . a * b using AndAbstract
    assert(ex == andabs);
    Bdd univ = a_and_b.UnivAbstract(a); // \forall a . a * b
    assert(univ == zero);

    // alternative method to get the cube "ab" using bddCube
    BddSet variables = a * b;
    std::vector<unsigned char> vec = {1, 1};
    assert(a_and_b == Bdd::bddCube(variables, vec));

    // test the bddCube method for all combinations
    assert((!a * !b) == Bdd::bddCube(variables, std::vector<uint8_t>({0, 0})));
    assert((!a * b)  == Bdd::bddCube(variables, std::vector<uint8_t>({0, 1})));
    assert((!a)      == Bdd::bddCube(variables, std::vector<uint8_t>({0, 2})));
    assert((a * !b)  == Bdd::bddCube(variables, std::vector<uint8_t>({1, 0})));
    assert((a * b)   == Bdd::bddCube(variables, std::vector<uint8_t>({1, 1})));
    assert((a)       == Bdd::bddCube(variables, std::vector<uint8_t>({1, 2})));
    assert((!b)      == Bdd::bddCube(variables, std::vector<uint8_t>({2, 0})));
    assert((b)       == Bdd::bddCube(variables, std::vector<uint8_t>({2, 1})));
    assert(one       == Bdd::bddCube(variables, std::vector<uint8_t>({2, 2})));
}

VOID_TASK_1(_main, void*, arg)
{
    // Initialize Sylvan
    // With starting size of the nodes table 1 << 21, and maximum size 1 << 27.
    // With starting size of the cache table 1 << 20, and maximum size 1 << 20.
    // Memory usage: 24 bytes per node, and 36 bytes per cache bucket
    // - 1<<24 nodes: 384 MB
    // - 1<<25 nodes: 768 MB
    // - 1<<26 nodes: 1536 MB
    // - 1<<27 nodes: 3072 MB
    // - 1<<24 cache: 576 MB
    // - 1<<25 cache: 1152 MB
    // - 1<<26 cache: 2304 MB
    // - 1<<27 cache: 4608 MB
    sylvan_set_sizes(1LL<<22, 1LL<<26, 1LL<<22, 1LL<<26);
    sylvan_init_package();

    // Initialize the BDD module with granularity 1 (cache every operation)
    // A higher granularity (e.g. 6) often results in better performance in practice
    sylvan_init_bdd();

    // Now we can do some simple stuff using the C++ objects.
    CALL(simple_cxx);

    // Report statistics (if SYLVAN_STATS is 1 in the configuration)
    sylvan_stats_report(stdout);

    // And quit, freeing memory
    sylvan_quit();

    // We didn't use arg
    (void)arg;
}

int
main (int argc, char *argv[])
{
    int n_workers = 0; // automatically detect number of workers
    size_t deque_size = 0; // default value for the size of task deques for the workers

    // Initialize the Lace framework for <n_workers> workers.
    lace_start(n_workers, deque_size);

    RUN(_main, NULL);

    // The lace_startup command also exits Lace after _main is completed.

    return 0;
    (void)argc; // unused variable
    (void)argv; // unused variable
}
