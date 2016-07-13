#include <sylvan_config.h>

#include <assert.h>
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sylvan.h>
#include <sylvan_common.h>
/*#include <sylvan_mtbdd_int.h>*/
#include <sylvan_storm_rational_function.h>

#include <storm_function_wrapper.h>

/**
 * helper function for hash
 */
#ifndef rotl64
static inline uint64_t
rotl64(uint64_t x, int8_t r)
{
    return ((x<<r) | (x>>(64-r)));
}
#endif

static uint64_t
sylvan_storm_rational_function_hash(const uint64_t v, const uint64_t seed)
{
    /* Hash the storm::RationalFunction in pointer v */
    
	storm_rational_function_ptr x = (storm_rational_function_ptr)(size_t)v;

	return storm_rational_function_hash(x, seed);
}

static int
sylvan_storm_rational_function_equals(const uint64_t left, const uint64_t right)
{
    /* This function is called by the unique table when comparing a new
       leaf with an existing leaf */
	storm_rational_function_ptr a = (storm_rational_function_ptr)(size_t)left;
	storm_rational_function_ptr b = (storm_rational_function_ptr)(size_t)right;

    /* Just compare x and y */
    return (storm_rational_function_equals(a, b) == 0) ? 1 : 0;
}

static void
sylvan_storm_rational_function_create(uint64_t *val)
{
    /* This function is called by the unique table when a leaf does not yet exist.
       We make a copy, which will be stored in the hash table. */
	storm_rational_function_ptr* x = (storm_rational_function_ptr*)(size_t)val;
	storm_rational_function_init(x);
}

static void
sylvan_storm_rational_function_destroy(uint64_t val)
{
    /* This function is called by the unique table
       when a leaf is removed during garbage collection. */
	storm_rational_function_ptr x = (storm_rational_function_ptr)(size_t)val;
	storm_rational_function_destroy(x);
}

static uint32_t sylvan_storm_rational_function_type;
static uint64_t CACHE_STORM_RATIONAL_FUNCTION_AND_EXISTS;

/**
 * Initialize storm::RationalFunction custom leaves
 */
void
sylvan_storm_rational_function_init()
{
    /* Register custom leaf 3 */
    sylvan_storm_rational_function_type = mtbdd_register_custom_leaf(sylvan_storm_rational_function_hash, sylvan_storm_rational_function_equals, sylvan_storm_rational_function_create, sylvan_storm_rational_function_destroy);
	CACHE_STORM_RATIONAL_FUNCTION_AND_EXISTS = cache_next_opid();
}

/**
 * Create storm::RationalFunction leaf
 */
MTBDD
mtbdd_storm_rational_function(storm_rational_function_t val)
{
    return mtbdd_makeleaf(sylvan_storm_rational_function_type, (size_t)val);
}

/**
 * Operation "plus" for two storm::RationalFunction MTBDDs
 * Interpret partial function as "0"
 */
TASK_IMPL_2(MTBDD, sylvan_storm_rational_function_op_plus, MTBDD*, pa, MTBDD*, pb)
{
    MTBDD a = *pa, b = *pb;

    /* Check for partial functions */
    if (a == mtbdd_false) return b;
    if (b == mtbdd_false) return a;

    /* If both leaves, compute plus */
    if (mtbdd_isleaf(a) && mtbdd_isleaf(b)) {
		storm_rational_function_ptr ma = (storm_rational_function_ptr)mtbdd_getvalue(a);
		storm_rational_function_ptr mb = (storm_rational_function_ptr)mtbdd_getvalue(b);

		storm_rational_function_ptr mres = storm_rational_function_plus(ma, mb);
        MTBDD res = mtbdd_storm_rational_function(mres);
        
		// TODO: Delete mres?

        return res;
    }

    /* Commutative, so swap a,b for better cache performance */
    if (a < b) {
        *pa = b;
        *pb = a;
    }

    return mtbdd_invalid;
}

/**
 * Operation "minus" for two storm::RationalFunction MTBDDs
 * Interpret partial function as "0"
 */
TASK_IMPL_2(MTBDD, sylvan_storm_rational_function_op_minus, MTBDD*, pa, MTBDD*, pb)
{
    MTBDD a = *pa, b = *pb;

    /* Check for partial functions */
    if (a == mtbdd_false) return sylvan_storm_rational_function_neg(b);
    if (b == mtbdd_false) return a;

    /* If both leaves, compute plus */
    if (mtbdd_isleaf(a) && mtbdd_isleaf(b)) {
		storm_rational_function_ptr ma = (storm_rational_function_ptr)mtbdd_getvalue(a);
		storm_rational_function_ptr mb = (storm_rational_function_ptr)mtbdd_getvalue(b);

		storm_rational_function_ptr mres = storm_rational_function_minus(ma, mb);
		MTBDD res = mtbdd_storm_rational_function(mres);

		// TODO: Delete mres?

        return res;
    }

    return mtbdd_invalid;
}

/**
 * Operation "times" for two storm::RationalFunction MTBDDs.
 * One of the parameters can be a BDD, then it is interpreted as a filter.
 * For partial functions, domain is intersection
 */
TASK_IMPL_2(MTBDD, sylvan_storm_rational_function_op_times, MTBDD*, pa, MTBDD*, pb)
{
    MTBDD a = *pa, b = *pb;

    /* Check for partial functions and for Boolean (filter) */
    if (a == mtbdd_false || b == mtbdd_false) return mtbdd_false;

    /* If one of Boolean, interpret as filter */
    if (a == mtbdd_true) return b;
    if (b == mtbdd_true) return a;

    /* Handle multiplication of leaves */
    if (mtbdd_isleaf(a) && mtbdd_isleaf(b)) {
		storm_rational_function_ptr ma = (storm_rational_function_ptr)mtbdd_getvalue(a);
		storm_rational_function_ptr mb = (storm_rational_function_ptr)mtbdd_getvalue(b);

		storm_rational_function_ptr mres = storm_rational_function_times(ma, mb);
		MTBDD res = mtbdd_storm_rational_function(mres);

		// TODO: Delete mres?

        return res;
    }

    /* Commutative, so make "a" the lowest for better cache performance */
    if (a < b) {
        *pa = b;
        *pb = a;
    }

    return mtbdd_invalid;
}

/**
 * Operation "divide" for two storm::RationalFunction MTBDDs.
 * For partial functions, domain is intersection
 */
TASK_IMPL_2(MTBDD, sylvan_storm_rational_function_op_divide, MTBDD*, pa, MTBDD*, pb)
{
    MTBDD a = *pa, b = *pb;

    /* Check for partial functions */
    if (a == mtbdd_false || b == mtbdd_false) return mtbdd_false;

    /* Handle division of leaves */
    if (mtbdd_isleaf(a) && mtbdd_isleaf(b)) {
		storm_rational_function_ptr ma = (storm_rational_function_ptr)mtbdd_getvalue(a);
		storm_rational_function_ptr mb = (storm_rational_function_ptr)mtbdd_getvalue(b);

		storm_rational_function_ptr mres = storm_rational_function_divide(ma, mb);
		MTBDD res = mtbdd_storm_rational_function(mres);

		// TODO: Delete mres?

        return res;
    }

    return mtbdd_invalid;
}

/**
 * Operation "neg" for one storm::RationalFunction MTBDD
 */
TASK_IMPL_2(MTBDD, sylvan_storm_rational_function_op_neg, MTBDD, dd, size_t, p)
{
    /* Handle partial functions */
    if (dd == mtbdd_false) return mtbdd_false;

    /* Compute result for leaf */
    if (mtbdd_isleaf(dd)) {
		storm_rational_function_ptr mdd = (storm_rational_function_ptr)mtbdd_getvalue(dd);

		storm_rational_function_ptr mres = storm_rational_function_negate(mdd);
		MTBDD res = mtbdd_storm_rational_function(mres);

		// TODO: Delete mres?
        return res;
    }

    return mtbdd_invalid;
    (void)p;
}
