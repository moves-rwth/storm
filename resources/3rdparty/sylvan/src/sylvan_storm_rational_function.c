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
#include <sylvan_cache.h>
#include <sylvan_int.h>
#include <sylvan_mtbdd_int.h>
#include <sylvan_storm_rational_function.h>

uint32_t srf_type;

static uint64_t sylvan_storm_rational_function_hash(const uint64_t v, const uint64_t seed) {
    storm_rational_function_ptr x = (storm_rational_function_ptr)v;
    return storm_rational_function_hash(x, seed);
}

static int sylvan_storm_rational_function_equals(const uint64_t left, const uint64_t right) {
    return storm_rational_function_equals((storm_rational_function_ptr)(size_t)left, (storm_rational_function_ptr)(size_t)right);
}

static void sylvan_storm_rational_function_create(uint64_t *val) {
    // This function is called by the unique table when a leaf does not yet exist. We make a copy, which will be stored in the hash table.
    storm_rational_function_ptr* x = (storm_rational_function_ptr*)(size_t)val;
    storm_rational_function_init(x);
}

static void sylvan_storm_rational_function_destroy(uint64_t val) {
    // This function is called by the unique table when a leaf is removed during garbage collection.
    storm_rational_function_ptr x = (storm_rational_function_ptr)(size_t)val;
    storm_rational_function_destroy(x);
}

static char* sylvan_storm_rational_function_to_str(int comp, uint64_t val, char* buf, size_t buflen) {
    return storm_rational_function_to_str((storm_rational_function_ptr)(size_t)val, buf, buflen);
    (void)comp;
}

void sylvan_storm_rational_function_init() {
    // Register custom leaf type storing rational functions.
    srf_type = sylvan_mt_create_type();
    sylvan_mt_set_hash(srf_type, sylvan_storm_rational_function_hash);
    sylvan_mt_set_equals(srf_type, sylvan_storm_rational_function_equals);
    sylvan_mt_set_create(srf_type, sylvan_storm_rational_function_create);
    sylvan_mt_set_destroy(srf_type, sylvan_storm_rational_function_destroy);
    sylvan_mt_set_to_str(srf_type, sylvan_storm_rational_function_to_str);
    // sylvan_mt_set_write_binary(srf_type, sylvan_storm_rational_function_write_binary);
    // sylvan_mt_set_read_binary(srf_type, sylvan_storm_rational_function_read_binary);
}

uint32_t sylvan_storm_rational_function_get_type() {
    return srf_type;
}

MTBDD mtbdd_storm_rational_function(storm_rational_function_ptr val) {
    uint64_t terminalValue = (uint64_t)val;
    return mtbdd_makeleaf(srf_type, terminalValue);
}

storm_rational_function_ptr mtbdd_getstorm_rational_function_ptr(MTBDD terminal) {
    uint64_t value = mtbdd_getvalue(terminal);
    return (storm_rational_function_ptr)value;
}

TASK_IMPL_2(MTBDD, mtbdd_op_bool_to_storm_rational_function, MTBDD, a, size_t, v) {
	if (a == mtbdd_false) {
		return mtbdd_storm_rational_function(storm_rational_function_get_zero());
	}
	if (a == mtbdd_true) {
		return mtbdd_storm_rational_function(storm_rational_function_get_one());
	}

    // Ugly hack to get rid of the error "unused variable v" (because there is no version of uapply without a parameter).
    (void)v;

    return mtbdd_invalid;
}

TASK_IMPL_1(MTBDD, mtbdd_bool_to_storm_rational_function, MTBDD, dd) {
    return mtbdd_uapply(dd, TASK(mtbdd_op_bool_to_storm_rational_function), 0);
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_function_op_equals, MTBDD*, pa, MTBDD*, pb) {
    MTBDD a = *pa, b = *pb;

    /* Check for partial functions */
    if (a == mtbdd_false || b == mtbdd_false) return mtbdd_false;

    if (mtbdd_isleaf(a) && mtbdd_isleaf(b)) {
        storm_rational_function_ptr ma = mtbdd_getstorm_rational_function_ptr(a);
        storm_rational_function_ptr mb = mtbdd_getstorm_rational_function_ptr(b);
        return storm_rational_function_equals(ma, mb) ? mtbdd_true : mtbdd_false;
    }

    /* Commutative, so swap a,b for better cache performance */
    if (a < b) {
        *pa = b;
        *pb = a;
    }

    return mtbdd_invalid;
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_function_op_plus, MTBDD*, pa, MTBDD*, pb) {
    MTBDD a = *pa, b = *pb;

    /* Check for partial functions */
    if (a == mtbdd_false) return b;
    if (b == mtbdd_false) return a;

    if (a == mtbdd_true || b == mtbdd_true) return mtbdd_true;

    /* If both leaves, compute plus */
    if (mtbdd_isleaf(a) && mtbdd_isleaf(b)) {
		storm_rational_function_ptr ma = mtbdd_getstorm_rational_function_ptr(a);
		storm_rational_function_ptr mb = mtbdd_getstorm_rational_function_ptr(b);

		storm_rational_function_ptr mres = storm_rational_function_plus(ma, mb);
        MTBDD res = mtbdd_storm_rational_function(mres);
		storm_rational_function_destroy(mres);

        return res;
    }

    /* Commutative, so swap a,b for better cache performance */
    if (a < b) {
        *pa = b;
        *pb = a;
    }

    return mtbdd_invalid;
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_function_op_minus, MTBDD*, pa, MTBDD*, pb) {
	MTBDD a = *pa, b = *pb;

    /* Check for partial functions */
    if (a == mtbdd_false) return sylvan_storm_rational_function_neg(b);
    if (b == mtbdd_false) return a;

    /* If both leaves, compute minus */
    if (mtbdd_isleaf(a) && mtbdd_isleaf(b)) {
		storm_rational_function_ptr ma = mtbdd_getstorm_rational_function_ptr(a);
		storm_rational_function_ptr mb = mtbdd_getstorm_rational_function_ptr(b);

		storm_rational_function_ptr mres = storm_rational_function_minus(ma, mb);
		MTBDD res = mtbdd_storm_rational_function(mres);
		storm_rational_function_destroy(mres);

        return res;
    }

    return mtbdd_invalid;
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_function_op_times, MTBDD*, pa, MTBDD*, pb) {
    MTBDD a = *pa, b = *pb;

    /* Check for partial functions */
    if (a == mtbdd_false || b == mtbdd_false) return mtbdd_false;
    if (a == mtbdd_true) return b;
    if (b == mtbdd_true) return a;

    /* If both leaves, compute multiplication */
    if (mtbdd_isleaf(a) && mtbdd_isleaf(b)) {
		storm_rational_function_ptr ma = mtbdd_getstorm_rational_function_ptr(a);
		storm_rational_function_ptr mb = mtbdd_getstorm_rational_function_ptr(b);

		storm_rational_function_ptr mres = storm_rational_function_times(ma, mb);
		MTBDD res = mtbdd_storm_rational_function(mres);
		storm_rational_function_destroy(mres);

        return res;
    }

    /* Commutative, so make "a" the lowest for better cache performance */
    if (a < b) {
        *pa = b;
        *pb = a;
    }

    return mtbdd_invalid;
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_function_op_divide, MTBDD*, pa, MTBDD*, pb) {
    MTBDD a = *pa, b = *pb;

    /* Check for partial functions */
    if (a == mtbdd_false || b == mtbdd_false) return mtbdd_false;

    /* If both leaves, compute division */
    if (mtbdd_isleaf(a) && mtbdd_isleaf(b)) {
		storm_rational_function_ptr ma = mtbdd_getstorm_rational_function_ptr(a);
        storm_rational_function_ptr mb = mtbdd_getstorm_rational_function_ptr(b);
        storm_rational_function_ptr mres;
        if (storm_rational_function_is_zero(ma)) {
            mres = storm_rational_function_get_zero();
        } else if (storm_rational_function_is_zero(mb)) {
            mres = storm_rational_function_get_infinity();
        } else {
            storm_rational_function_ptr mb = mtbdd_getstorm_rational_function_ptr(b);
            mres = storm_rational_function_divide(ma, mb);
        }
		MTBDD res = mtbdd_storm_rational_function(mres);
		storm_rational_function_destroy(mres);

        return res;
    }

    return mtbdd_invalid;
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_function_op_less, MTBDD*, pa, MTBDD*, pb) {
    MTBDD a = *pa, b = *pb;

    /* Check for partial functions and for Boolean (filter) */
    if (a == mtbdd_false || b == mtbdd_false) return mtbdd_false;

    /* If both leaves, compute less */
    if (mtbdd_isleaf(a) && mtbdd_isleaf(b)) {
        storm_rational_function_ptr ma = mtbdd_getstorm_rational_function_ptr(a);
        storm_rational_function_ptr mb = mtbdd_getstorm_rational_function_ptr(b);

        return storm_rational_function_less(ma, mb) ? mtbdd_true : mtbdd_false;
    }

    return mtbdd_invalid;
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_function_op_less_or_equal, MTBDD*, pa, MTBDD*, pb) {
    MTBDD a = *pa, b = *pb;

    /* Check for partial functions and for Boolean (filter) */
    if (a == mtbdd_false || b == mtbdd_false) return mtbdd_false;

    /* If both leaves, compute less or equal */
    if (mtbdd_isleaf(a) && mtbdd_isleaf(b)) {
        storm_rational_function_ptr ma = mtbdd_getstorm_rational_function_ptr(a);
        storm_rational_function_ptr mb = mtbdd_getstorm_rational_function_ptr(b);

        return storm_rational_function_less_or_equal(ma, mb) ? mtbdd_true : mtbdd_false;
    }

    return mtbdd_invalid;
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_function_op_mod, MTBDD*, pa, MTBDD*, pb) {
    MTBDD a = *pa, b = *pb;

    /* Check for partial functions and for Boolean (filter) */
    if (a == mtbdd_false || b == mtbdd_false) return mtbdd_false;

    /* If both leaves, compute modulo */
    if (mtbdd_isleaf(a) && mtbdd_isleaf(b)) {
        storm_rational_function_ptr ma = mtbdd_getstorm_rational_function_ptr(a);
        storm_rational_function_ptr mb = mtbdd_getstorm_rational_function_ptr(b);

        storm_rational_function_ptr mres = storm_rational_function_mod(ma, mb);
        MTBDD res = mtbdd_storm_rational_function(mres);
        storm_rational_function_destroy(mres);

        return res;
    }

    return mtbdd_invalid;
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_function_op_min, MTBDD*, pa, MTBDD*, pb) {
    MTBDD a = *pa, b = *pb;

    /* Check for partial functions and for Boolean (filter) */
    if (a == mtbdd_false || b == mtbdd_false) return mtbdd_false;

    /* If both leaves, compute min */
    if (mtbdd_isleaf(a) && mtbdd_isleaf(b)) {
        storm_rational_function_ptr ma = mtbdd_getstorm_rational_function_ptr(a);
        storm_rational_function_ptr mb = mtbdd_getstorm_rational_function_ptr(b);

        storm_rational_function_ptr mres = storm_rational_function_min(ma, mb);
        MTBDD res = mtbdd_storm_rational_function(mres);
        storm_rational_function_destroy(mres);

        return res;
    }

    /* Commutative, so make "a" the lowest for better cache performance */
    if (a < b) {
        *pa = b;
        *pb = a;
    }

    return mtbdd_invalid;
}

TASK_IMPL_3(MTBDD, sylvan_storm_rational_function_abstract_op_min, MTBDD, a, MTBDD, b, int, k) {
    if (k==0) {
        return mtbdd_apply(a, b, TASK(sylvan_storm_rational_function_op_min));
    } else {
        MTBDD res = a;
        for (int i=0; i<k; i++) {
            mtbdd_refs_push(res);
            res = mtbdd_apply(res, res, TASK(sylvan_storm_rational_function_op_min));
            mtbdd_refs_pop(1);
        }
        return res;
    }
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_function_op_max, MTBDD*, pa, MTBDD*, pb) {
    MTBDD a = *pa, b = *pb;

    /* Check for partial functions and for Boolean (filter) */
    if (a == mtbdd_false || b == mtbdd_false) return mtbdd_false;

    /* If both leaves, compute max */
    if (mtbdd_isleaf(a) && mtbdd_isleaf(b)) {
        storm_rational_function_ptr ma = mtbdd_getstorm_rational_function_ptr(a);
        storm_rational_function_ptr mb = mtbdd_getstorm_rational_function_ptr(b);

        storm_rational_function_ptr mres = storm_rational_function_max(ma, mb);
        MTBDD res = mtbdd_storm_rational_function(mres);
        storm_rational_function_destroy(mres);

        return res;
    }

    /* Commutative, so make "a" the lowest for better cache performance */
    if (a < b) {
        *pa = b;
        *pb = a;
    }

    return mtbdd_invalid;
}

TASK_IMPL_3(MTBDD, sylvan_storm_rational_function_abstract_op_max, MTBDD, a, MTBDD, b, int, k) {
    if (k==0) {
        return mtbdd_apply(a, b, TASK(sylvan_storm_rational_function_op_max));
    } else {
        MTBDD res = a;
        for (int i=0; i<k; i++) {
            mtbdd_refs_push(res);
            res = mtbdd_apply(res, res, TASK(sylvan_storm_rational_function_op_max));
            mtbdd_refs_pop(1);
        }
        return res;
    }
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_function_op_pow, MTBDD*, pa, MTBDD*, pb) {
    MTBDD a = *pa, b = *pb;

    /* Check for partial functions and for Boolean (filter) */
    if (a == mtbdd_false || b == mtbdd_false) return mtbdd_false;

    /* Handle multiplication of leaves */
    if (mtbdd_isleaf(a) && mtbdd_isleaf(b)) {
        storm_rational_function_ptr ma = mtbdd_getstorm_rational_function_ptr(a);
        storm_rational_function_ptr mb = mtbdd_getstorm_rational_function_ptr(b);

        storm_rational_function_ptr mres = storm_rational_function_pow(ma, mb);
        MTBDD res = mtbdd_storm_rational_function(mres);
        storm_rational_function_destroy(mres);

        return res;
    }

    return mtbdd_invalid;
}

TASK_IMPL_3(MTBDD, sylvan_storm_rational_function_abstract_op_plus, MTBDD, a, MTBDD, b, int, k) {
    if (k==0) {
        return mtbdd_apply(a, b, TASK(sylvan_storm_rational_function_op_plus));
    } else {
        MTBDD res = a;
        for (int i=0; i<k; i++) {
            mtbdd_refs_push(res);
            res = mtbdd_apply(res, res, TASK(sylvan_storm_rational_function_op_plus));
            mtbdd_refs_pop(1);
        }
        return res;
    }
}

TASK_IMPL_3(MTBDD, sylvan_storm_rational_function_abstract_op_times, MTBDD, a, MTBDD, b, int, k) {
    if (k==0) {
        return mtbdd_apply(a, b, TASK(sylvan_storm_rational_function_op_times));
    } else {
        MTBDD res = a;
        for (int i=0; i<k; i++) {
            mtbdd_refs_push(res);
            res = mtbdd_apply(res, res, TASK(sylvan_storm_rational_function_op_times));
            mtbdd_refs_pop(1);
        }
        return res;
    }
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_function_op_neg, MTBDD, dd, size_t, p) {
    /* Handle partial functions */
    if (dd == mtbdd_false) return mtbdd_false;

    /* Compute result for leaf */
    if (mtbdd_isleaf(dd)) {
		storm_rational_function_ptr mdd = mtbdd_getstorm_rational_function_ptr(dd);

		storm_rational_function_ptr mres = storm_rational_function_negate(mdd);
		MTBDD res = mtbdd_storm_rational_function(mres);
		storm_rational_function_destroy(mres);

        return res;
    }

    return mtbdd_invalid;
    (void)p;
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_function_op_floor, MTBDD, dd, size_t, p) {
    /* Handle partial functions */
    if (dd == mtbdd_false) return mtbdd_false;

    /* Compute result for leaf */
    if (mtbdd_isleaf(dd)) {
        storm_rational_function_ptr mdd = mtbdd_getstorm_rational_function_ptr(dd);

        storm_rational_function_ptr mres = storm_rational_function_floor(mdd);
        MTBDD res = mtbdd_storm_rational_function(mres);
        storm_rational_function_destroy(mres);

        return res;
    }

    return mtbdd_invalid;
    (void)p;
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_function_op_ceil, MTBDD, dd, size_t, p) {
    /* Handle partial functions */
    if (dd == mtbdd_false) return mtbdd_false;

    /* Compute result for leaf */
    if (mtbdd_isleaf(dd)) {
        storm_rational_function_ptr mdd = mtbdd_getstorm_rational_function_ptr(dd);

        storm_rational_function_ptr mres = storm_rational_function_ceil(mdd);
        MTBDD res = mtbdd_storm_rational_function(mres);
        storm_rational_function_destroy(mres);

        return res;
    }

    return mtbdd_invalid;
    (void)p;
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_function_op_to_double, MTBDD, dd, size_t, p) {
    /* Handle partial functions */
    if (dd == mtbdd_false) return mtbdd_false;

    /* Compute result for leaf */
    if (mtbdd_isleaf(dd)) {
		storm_rational_function_ptr mdd = mtbdd_getstorm_rational_function_ptr(dd);
		MTBDD result = mtbdd_double(storm_rational_function_get_value_double(mdd));
		return result;
    }

    return mtbdd_invalid;
	(void)p;
}

TASK_IMPL_3(MTBDD, sylvan_storm_rational_function_and_exists, MTBDD, a, MTBDD, b, MTBDD, v) {
    /* Check terminal cases */

    /* If v == true, then <vars> is an empty set */
    if (v == mtbdd_true) return mtbdd_apply(a, b, TASK(sylvan_storm_rational_function_op_times));

    /* Try the times operator on a and b */
    MTBDD result = CALL(sylvan_storm_rational_function_op_times, &a, &b);
    if (result != mtbdd_invalid) {
        /* Times operator successful, store reference (for garbage collection) */
        mtbdd_refs_push(result);
        /* ... and perform abstraction */
        result = mtbdd_abstract(result, v, TASK(sylvan_storm_rational_function_abstract_op_plus));
        mtbdd_refs_pop(1);
        /* Note that the operation cache is used in mtbdd_abstract */
        return result;
    }

    /* Maybe perform garbage collection */
    sylvan_gc_test();

    /* Check cache. Note that we do this now, since the times operator might swap a and b (commutative) */
    if (cache_get3(CACHE_MTBDD_AND_EXISTS_RF, a, b, v, &result)) return result;

    /* Now, v is not a constant, and either a or b is not a constant */

    /* Get top variable */
    int la = mtbdd_isleaf(a);
    int lb = mtbdd_isleaf(b);
    mtbddnode_t na = la ? 0 : MTBDD_GETNODE(a);
    mtbddnode_t nb = lb ? 0 : MTBDD_GETNODE(b);
    uint32_t va = la ? 0xffffffff : mtbddnode_getvariable(na);
    uint32_t vb = lb ? 0xffffffff : mtbddnode_getvariable(nb);
    uint32_t var = va < vb ? va : vb;

    mtbddnode_t nv = MTBDD_GETNODE(v);
    uint32_t vv = mtbddnode_getvariable(nv);

    if (vv < var) {
        /* Recursive, then abstract result */
        result = CALL(sylvan_storm_rational_function_and_exists, a, b, node_gethigh(v, nv));
        mtbdd_refs_push(result);
        result = mtbdd_apply(result, result, TASK(sylvan_storm_rational_function_op_plus));
        mtbdd_refs_pop(1);
    } else {
        /* Get cofactors */
        MTBDD alow, ahigh, blow, bhigh;
        alow  = (!la && va == var) ? node_getlow(a, na)  : a;
        ahigh = (!la && va == var) ? node_gethigh(a, na) : a;
        blow  = (!lb && vb == var) ? node_getlow(b, nb)  : b;
        bhigh = (!lb && vb == var) ? node_gethigh(b, nb) : b;

        if (vv == var) {
            /* Recursive, then abstract result */
            mtbdd_refs_spawn(SPAWN(sylvan_storm_rational_function_and_exists, ahigh, bhigh, node_gethigh(v, nv)));
            MTBDD low = mtbdd_refs_push(CALL(sylvan_storm_rational_function_and_exists, alow, blow, node_gethigh(v, nv)));
            MTBDD high = mtbdd_refs_push(mtbdd_refs_sync(SYNC(sylvan_storm_rational_function_and_exists)));
            result = CALL(mtbdd_apply, low, high, TASK(sylvan_storm_rational_function_op_plus));
            mtbdd_refs_pop(2);
        } else /* vv > v */ {
            /* Recursive, then create node */
            mtbdd_refs_spawn(SPAWN(sylvan_storm_rational_function_and_exists, ahigh, bhigh, v));
            MTBDD low = mtbdd_refs_push(CALL(sylvan_storm_rational_function_and_exists, alow, blow, v));
            MTBDD high = mtbdd_refs_sync(SYNC(sylvan_storm_rational_function_and_exists));
            mtbdd_refs_pop(1);
            result = mtbdd_makenode(var, low, high);
        }
    }

    /* Store in cache */
    cache_put3(CACHE_MTBDD_AND_EXISTS_RF, a, b, v, result);
    return result;
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_function_op_threshold, MTBDD, a, size_t, svalue) {
    // Ok, this will not work on all platforms and is UB in general
    // but sylvan uses the same trick as far as I can see
    // so on these platforms sylvan should not work either...
    // casting a size_t to a pointer
    storm_rational_function_ptr value = (storm_rational_function_ptr)svalue;

    if (mtbdd_isleaf(a)) {
        storm_rational_function_ptr ma = mtbdd_getstorm_rational_function_ptr(a);
        return storm_rational_function_less(ma, value) ? mtbdd_false : mtbdd_true;
    }

    return mtbdd_invalid;
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_function_op_strict_threshold, MTBDD, a, size_t, svalue) {
    // Ok, this will not work on all platforms and is UB in general
    // but sylvan uses the same trick as far as I can see
    // so on these platforms sylvan should not work either...
    // casting a size_t to a pointer
    storm_rational_function_ptr value = (storm_rational_function_ptr)svalue;

    if (mtbdd_isleaf(a)) {
        storm_rational_function_ptr ma = mtbdd_getstorm_rational_function_ptr(a);

        return storm_rational_function_less_or_equal(ma, value) ? mtbdd_false : mtbdd_true;
    }

    return mtbdd_invalid;
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_function_threshold, MTBDD, dd, storm_rational_function_ptr, value)
{
    // Ok, this will not work on all platforms and is UB in general
    // but sylvan uses the same trick as far as I can see
    // so on these platforms sylvan should not work either...
    // casting a pointer to a size_t
    return mtbdd_uapply(dd, TASK(sylvan_storm_rational_function_op_threshold), (size_t)value);
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_function_strict_threshold, MTBDD, dd, storm_rational_function_ptr, value)
{
    // Ok, this will not work on all platforms and is UB in general
    // but sylvan uses the same trick as far as I can see
    // so on these platforms sylvan should not work either...
    // casting a pointer to a size_t
    return mtbdd_uapply(dd, TASK(sylvan_storm_rational_function_op_strict_threshold), (size_t)value);
}

TASK_IMPL_1(MTBDD, sylvan_storm_rational_function_minimum, MTBDD, a) {
    /* Check terminal case */
    if (a == mtbdd_false) return mtbdd_false;
    mtbddnode_t na = MTBDD_GETNODE(a);
    if (mtbddnode_isleaf(na)) return a;

    /* Maybe perform garbage collection */
    sylvan_gc_test();

    /* Check cache */
    MTBDD result;
    if (cache_get3(CACHE_MTBDD_MINIMUM_RF, a, 0, 0, &result)) return result;

    /* Call recursive */
    SPAWN(mtbdd_minimum, node_getlow(a, na));
    MTBDD high = CALL(sylvan_storm_rational_function_minimum, node_gethigh(a, na));
    MTBDD low = SYNC(sylvan_storm_rational_function_minimum);

    storm_rational_function_ptr fl = mtbdd_getstorm_rational_function_ptr(low);
    storm_rational_function_ptr fh = mtbdd_getstorm_rational_function_ptr(high);

    if (storm_rational_function_less_or_equal(fl, fh)) {
        return low;
    } else {
        return high;
    }

    /* Store in cache */
    cache_put3(CACHE_MTBDD_MINIMUM_RF, a, 0, 0, result);
    return result;
}

TASK_IMPL_1(MTBDD, sylvan_storm_rational_function_maximum, MTBDD, a)
{
    /* Check terminal case */
    if (a == mtbdd_false) return mtbdd_false;
    mtbddnode_t na = MTBDD_GETNODE(a);
    if (mtbddnode_isleaf(na)) return a;

    /* Maybe perform garbage collection */
    sylvan_gc_test();

    /* Check cache */
    MTBDD result;
    if (cache_get3(CACHE_MTBDD_MAXIMUM_RF, a, 0, 0, &result)) return result;

    /* Call recursive */
    SPAWN(mtbdd_minimum, node_getlow(a, na));
    MTBDD high = CALL(sylvan_storm_rational_function_maximum, node_gethigh(a, na));
    MTBDD low = SYNC(sylvan_storm_rational_function_maximum);

    storm_rational_function_ptr fl = mtbdd_getstorm_rational_function_ptr(low);
    storm_rational_function_ptr fh = mtbdd_getstorm_rational_function_ptr(high);

    if (storm_rational_function_less(fl, fh)) {
        return high;
    } else {
        return low;
    }

    /* Store in cache */
    cache_put3(CACHE_MTBDD_MAXIMUM_RF, a, 0, 0, result);
    return result;
}

TASK_4(MTBDD, sylvan_storm_rational_function_equal_norm_d2, MTBDD, a, MTBDD, b, storm_rational_function_ptr, svalue, int*, shortcircuit)
{
    /* Check short circuit */
    if (*shortcircuit) return mtbdd_false;

    /* Check terminal case */
    if (a == b) return mtbdd_true;
    if (a == mtbdd_false) return mtbdd_false;
    if (b == mtbdd_false) return mtbdd_false;

    mtbddnode_t na = MTBDD_GETNODE(a);
    mtbddnode_t nb = MTBDD_GETNODE(b);
    int la = mtbddnode_isleaf(na);
    int lb = mtbddnode_isleaf(nb);

    if (la && lb) {
        storm_rational_function_ptr fa = mtbdd_getstorm_rational_function_ptr(a);
        storm_rational_function_ptr fb = mtbdd_getstorm_rational_function_ptr(b);

        return storm_rational_function_equal_modulo_precision(0, fa, fb, svalue) ? mtbdd_true : mtbdd_false;
    }

    if (b < a) {
        MTBDD t = a;
        a = b;
        b = t;
    }

    /* Maybe perform garbage collection */
    sylvan_gc_test();

    /* Count operation */
    sylvan_stats_count(MTBDD_EQUAL_NORM);

    /* Check cache */
    MTBDD result;
    if (cache_get3(CACHE_MTBDD_EQUAL_NORM_RF, a, b, (uint64_t)svalue, &result)) {
        sylvan_stats_count(MTBDD_EQUAL_NORM_CACHED);
        return result;
    }

    /* Get top variable */
    uint32_t va = la ? 0xffffffff : mtbddnode_getvariable(na);
    uint32_t vb = lb ? 0xffffffff : mtbddnode_getvariable(nb);
    uint32_t var = va < vb ? va : vb;

    /* Get cofactors */
    MTBDD alow, ahigh, blow, bhigh;
    alow  = va == var ? node_getlow(a, na)  : a;
    ahigh = va == var ? node_gethigh(a, na) : a;
    blow  = vb == var ? node_getlow(b, nb)  : b;
    bhigh = vb == var ? node_gethigh(b, nb) : b;

    SPAWN(sylvan_storm_rational_function_equal_norm_d2, ahigh, bhigh, svalue, shortcircuit);
    result = CALL(sylvan_storm_rational_function_equal_norm_d2, alow, blow, svalue, shortcircuit);
    if (result == mtbdd_false) *shortcircuit = 1;
    if (result != SYNC(sylvan_storm_rational_function_equal_norm_d2)) result = mtbdd_false;
    if (result == mtbdd_false) *shortcircuit = 1;

    /* Store in cache */
    if (cache_put3(CACHE_MTBDD_EQUAL_NORM_RF, a, b, (uint64_t)svalue, result)) {
        sylvan_stats_count(MTBDD_EQUAL_NORM_CACHEDPUT);
    }

    return result;
}

TASK_IMPL_3(MTBDD, sylvan_storm_rational_function_equal_norm_d, MTBDD, a, MTBDD, b, storm_rational_function_ptr, d)
{
    /* the implementation checks shortcircuit in every task and if the two
     MTBDDs are not equal module epsilon, then the computation tree quickly aborts */
    int shortcircuit = 0;
    return CALL(sylvan_storm_rational_function_equal_norm_d2, a, b, d, &shortcircuit);
}

/**
 * Compare two Double MTBDDs, returns Boolean True if they are equal within some value epsilon
 * This version computes the relative difference vs the value in a.
 */
TASK_4(MTBDD, sylvan_storm_rational_function_equal_norm_rel_d2, MTBDD, a, MTBDD, b, storm_rational_function_ptr, svalue, int*, shortcircuit)
{
    /* Check short circuit */
    if (*shortcircuit) return mtbdd_false;

    /* Check terminal case */
    if (a == b) return mtbdd_true;
    if (a == mtbdd_false) return mtbdd_false;
    if (b == mtbdd_false) return mtbdd_false;

    mtbddnode_t na = MTBDD_GETNODE(a);
    mtbddnode_t nb = MTBDD_GETNODE(b);
    int la = mtbddnode_isleaf(na);
    int lb = mtbddnode_isleaf(nb);

    if (la && lb) {
        storm_rational_function_ptr fa = mtbdd_getstorm_rational_function_ptr(a);
        storm_rational_function_ptr fb = mtbdd_getstorm_rational_function_ptr(b);

        return storm_rational_function_equal_modulo_precision(1, fa, fb, svalue) ? mtbdd_true : mtbdd_false;
    }

    /* Maybe perform garbage collection */
    sylvan_gc_test();

    /* Count operation */
    sylvan_stats_count(MTBDD_EQUAL_NORM_REL);

    /* Check cache */
    MTBDD result;
    if (cache_get3(CACHE_MTBDD_EQUAL_NORM_REL_RF, a, b, (uint64_t)svalue, &result)) {
        sylvan_stats_count(MTBDD_EQUAL_NORM_REL_CACHED);
        return result;
    }

    /* Get top variable */
    uint32_t va = la ? 0xffffffff : mtbddnode_getvariable(na);
    uint32_t vb = lb ? 0xffffffff : mtbddnode_getvariable(nb);
    uint32_t var = va < vb ? va : vb;

    /* Get cofactors */
    MTBDD alow, ahigh, blow, bhigh;
    alow  = va == var ? node_getlow(a, na)  : a;
    ahigh = va == var ? node_gethigh(a, na) : a;
    blow  = vb == var ? node_getlow(b, nb)  : b;
    bhigh = vb == var ? node_gethigh(b, nb) : b;

    SPAWN(sylvan_storm_rational_function_equal_norm_rel_d2, ahigh, bhigh, svalue, shortcircuit);
    result = CALL(sylvan_storm_rational_function_equal_norm_rel_d2, alow, blow, svalue, shortcircuit);
    if (result == mtbdd_false) *shortcircuit = 1;
    if (result != SYNC(sylvan_storm_rational_function_equal_norm_rel_d2)) result = mtbdd_false;
    if (result == mtbdd_false) *shortcircuit = 1;

    /* Store in cache */
    if (cache_put3(CACHE_MTBDD_EQUAL_NORM_REL_RF, a, b, (uint64_t)svalue, result)) {
        sylvan_stats_count(MTBDD_EQUAL_NORM_REL_CACHEDPUT);
    }

    return result;
}

TASK_IMPL_3(MTBDD, sylvan_storm_rational_function_equal_norm_rel_d, MTBDD, a, MTBDD, b, storm_rational_function_ptr, d)
{
    /* the implementation checks shortcircuit in every task and if the two
     MTBDDs are not equal module epsilon, then the computation tree quickly aborts */
    int shortcircuit = 0;
    return CALL(sylvan_storm_rational_function_equal_norm_rel_d2, a, b, d, &shortcircuit);
}
