/**
 * Just a small test file to ensure that Sylvan can compile in C++
 */

#include <assert.h>
#include <sylvan.h>
#include <sylvan_obj.hpp>

#include "test_assert.h"

using namespace sylvan;

TASK_0(int, runtest)
{
    Bdd one = Bdd::bddOne();
    Bdd zero = Bdd::bddZero();

    test_assert(one != zero);
    test_assert(one == !zero);

    Bdd v1 = Bdd::bddVar(1);
    Bdd v2 = Bdd::bddVar(2);

    Bdd t = v1 + v2;
    Bdd u = v1;
    u += v2;

    test_assert(t == u);

    BddMap map;
    map.put(2, t);

    test_assert(v2.Compose(map) == (v1 + v2));
    test_assert((t * v2) == v2);

    return 0;
}

void test6()
{
    BddMap m1;
    BddMap m2(m1);  // this triggers an assertion
}

int main()
{
    // Standard Lace initialization with 1 worker
    lace_start(1, 0);

    // Simple Sylvan initialization, also initialize BDD support
    sylvan_set_sizes(1LL<<16, 1LL<<16, 1LL<<16, 1LL<<16);
    sylvan_init_package();
    sylvan_init_bdd();

    test6();

    int res = RUN(runtest);

    sylvan_quit();
    lace_stop();

    return res;
}
