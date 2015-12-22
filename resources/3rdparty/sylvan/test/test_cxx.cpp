/**
 * Just a small test file to ensure that Sylvan can compile in C++
 * Suggested by Shota Soga <shota.soga@gmail.com> for testing C++ compatibility
 */

#include <assert.h>
#include <sylvan.h>
#include <sylvan_obj.hpp>

using namespace sylvan;

void test()
{
    Bdd one = Bdd::bddOne();
    Bdd zero = Bdd::bddZero();

    Bdd v1 = Bdd::bddVar(1);
    Bdd v2 = Bdd::bddVar(2);

    Bdd t = v1 + v2;

    BddMap map;
    map.put(2, t);

    assert(v2.Compose(map) == v1+v2);

    t *= v2;

    assert(t == v2);
}

int main()
{
    // Standard Lace initialization
	lace_init(0, 1000000);
	lace_startup(0, NULL, NULL);

    // Simple Sylvan initialization, also initialize BDD and LDD support
	sylvan_init_package(1LL<<16, 1LL<<16, 1LL<<16, 1LL<<16);
	sylvan_init_bdd(1);

    test();

    sylvan_quit();
    lace_exit();
}
