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
#include <sylvan_storm_rational_number.h>

uint32_t srn_type;

static uint64_t sylvan_storm_rational_number_hash(const uint64_t v, const uint64_t seed) {
    storm_rational_number_ptr x = (storm_rational_number_ptr)v;
    return storm_rational_number_hash(x, seed);
}

static int sylvan_storm_rational_number_equals(const uint64_t left, const uint64_t right) {
    return storm_rational_number_equals((storm_rational_number_ptr)(size_t)left, (storm_rational_number_ptr)(size_t)right);
}

static void sylvan_storm_rational_number_create(uint64_t *val) {
    // This function is called by the unique table when a leaf does not yet exist. We make a copy, which will be stored in the hash table.
    storm_rational_number_ptr* x = (storm_rational_number_ptr*)(size_t)val;
    storm_rational_number_init(x);
}

static void sylvan_storm_rational_number_destroy(uint64_t val) {
    // This function is called by the unique table when a leaf is removed during garbage collection.
    storm_rational_number_ptr x = (storm_rational_number_ptr)(size_t)val;
    storm_rational_number_destroy(x);
}

static char* sylvan_storm_rational_number_to_str(int comp, uint64_t val, char* buf, size_t buflen) {
    return storm_rational_number_to_str((storm_rational_number_ptr)(size_t)val, buf, buflen);
    (void)comp;
}

void sylvan_storm_rational_number_init() {
    // Register custom leaf type storing rational numbers.
    srn_type = sylvan_mt_create_type();
    sylvan_mt_set_hash(srn_type, sylvan_storm_rational_number_hash);
    sylvan_mt_set_equals(srn_type, sylvan_storm_rational_number_equals);
    sylvan_mt_set_create(srn_type, sylvan_storm_rational_number_create);
    sylvan_mt_set_destroy(srn_type, sylvan_storm_rational_number_destroy);
    sylvan_mt_set_to_str(srn_type, sylvan_storm_rational_number_to_str);
    // sylvan_mt_set_write_binary(srn_type, sylvan_storm_rational_number_write_binary);
    // sylvan_mt_set_read_binary(srn_type, sylvan_storm_rational_number_read_binary);
}

uint32_t sylvan_storm_rational_number_get_type() {
    return srn_type;
}

MTBDD mtbdd_storm_rational_number(storm_rational_number_ptr val) {
    uint64_t terminalValue = (uint64_t)val;
    return mtbdd_makeleaf(srn_type, terminalValue);
}

storm_rational_number_ptr mtbdd_getstorm_rational_number_ptr(MTBDD terminal) {
    uint64_t value = mtbdd_getvalue(terminal);
    return (storm_rational_number_ptr)value;
}

TASK_IMPL_2(MTBDD, mtbdd_op_bool_to_storm_rational_number, MTBDD, a, size_t, v) {
	if (a == mtbdd_false) {
		return mtbdd_storm_rational_number(storm_rational_number_get_zero());
	}
	if (a == mtbdd_true) {
		return mtbdd_storm_rational_number(storm_rational_number_get_one());
	}

    // Ugly hack to get rid of the error "unused variable v" (because there is no version of uapply without a parameter).
    (void)v;

    return mtbdd_invalid;
}

TASK_IMPL_1(MTBDD, mtbdd_bool_to_storm_rational_number, MTBDD, dd) {
    return mtbdd_uapply(dd, TASK(mtbdd_op_bool_to_storm_rational_number), 0);
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_number_op_equals, MTBDD*, pa, MTBDD*, pb) {
    MTBDD a = *pa, b = *pb;

    /* Check for partial functions */
    if (a == mtbdd_false || b == mtbdd_false) return mtbdd_false;

    if (mtbdd_isleaf(a) && mtbdd_isleaf(b)) {
        storm_rational_number_ptr ma = mtbdd_getstorm_rational_number_ptr(a);
        storm_rational_number_ptr mb = mtbdd_getstorm_rational_number_ptr(b);
        return storm_rational_number_equals(ma, mb) ? mtbdd_true : mtbdd_false;
    }

    /* Commutative, so swap a,b for better cache performance */
    if (a < b) {
        *pa = b;
        *pb = a;
    }

    return mtbdd_invalid;
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_number_op_plus, MTBDD*, pa, MTBDD*, pb) {
    MTBDD a = *pa, b = *pb;

    /* Check for partial functions */
    if (a == mtbdd_false) return b;
    if (b == mtbdd_false) return a;

    if (a == mtbdd_true || b == mtbdd_true) return mtbdd_true;

    /* If both leaves, compute plus */
    if (mtbdd_isleaf(a) && mtbdd_isleaf(b)) {
		storm_rational_number_ptr ma = mtbdd_getstorm_rational_number_ptr(a);
		storm_rational_number_ptr mb = mtbdd_getstorm_rational_number_ptr(b);

		storm_rational_number_ptr mres = storm_rational_number_plus(ma, mb);
        MTBDD res = mtbdd_storm_rational_number(mres);
		storm_rational_number_destroy(mres);

        return res;
    }

    /* Commutative, so swap a,b for better cache performance */
    if (a < b) {
        *pa = b;
        *pb = a;
    }

    return mtbdd_invalid;
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_number_op_minus, MTBDD*, pa, MTBDD*, pb) {
	MTBDD a = *pa, b = *pb;

    /* Check for partial functions */
    if (a == mtbdd_false) return sylvan_storm_rational_number_neg(b);
    if (b == mtbdd_false) return a;

    /* If both leaves, compute minus */
    if (mtbdd_isleaf(a) && mtbdd_isleaf(b)) {
		storm_rational_number_ptr ma = mtbdd_getstorm_rational_number_ptr(a);
		storm_rational_number_ptr mb = mtbdd_getstorm_rational_number_ptr(b);

		storm_rational_number_ptr mres = storm_rational_number_minus(ma, mb);
		MTBDD res = mtbdd_storm_rational_number(mres);
		storm_rational_number_destroy(mres);

        return res;
    }

    return mtbdd_invalid;
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_number_op_times, MTBDD*, pa, MTBDD*, pb) {
    MTBDD a = *pa, b = *pb;

    /* Check for partial functions */
    if (a == mtbdd_false || b == mtbdd_false) return mtbdd_false;
    if (a == mtbdd_true) return b;
    if (b == mtbdd_true) return a;

    /* If both leaves, compute multiplication */
    if (mtbdd_isleaf(a) && mtbdd_isleaf(b)) {
		storm_rational_number_ptr ma = mtbdd_getstorm_rational_number_ptr(a);
		storm_rational_number_ptr mb = mtbdd_getstorm_rational_number_ptr(b);

		storm_rational_number_ptr mres = storm_rational_number_times(ma, mb);
		MTBDD res = mtbdd_storm_rational_number(mres);
		storm_rational_number_destroy(mres);

        return res;
    }

    /* Commutative, so make "a" the lowest for better cache performance */
    if (a < b) {
        *pa = b;
        *pb = a;
    }

    return mtbdd_invalid;
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_number_op_divide, MTBDD*, pa, MTBDD*, pb) {
    MTBDD a = *pa, b = *pb;

    /* Check for partial functions */
    if (a == mtbdd_false || b == mtbdd_false) return mtbdd_false;

    /* If both leaves, compute division */
    if (mtbdd_isleaf(a) && mtbdd_isleaf(b)) {
		storm_rational_number_ptr ma = mtbdd_getstorm_rational_number_ptr(a);
        storm_rational_number_ptr mb = mtbdd_getstorm_rational_number_ptr(b);
        storm_rational_number_ptr mres;
        if (storm_rational_number_is_zero(ma)) {
            mres = storm_rational_number_get_zero();
        } else if (storm_rational_number_is_zero(mb)) {
            mres = storm_rational_number_get_infinity();
        } else {
            storm_rational_number_ptr mb = mtbdd_getstorm_rational_number_ptr(b);
            mres = storm_rational_number_divide(ma, mb);
        }
		MTBDD res = mtbdd_storm_rational_number(mres);
		storm_rational_number_destroy(mres);

        return res;
    }

    return mtbdd_invalid;
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_number_op_less, MTBDD*, pa, MTBDD*, pb) {
    MTBDD a = *pa, b = *pb;

    /* Check for partial functions and for Boolean (filter) */
    if (a == mtbdd_false || b == mtbdd_false) return mtbdd_false;

    /* If both leaves, compute less */
    if (mtbdd_isleaf(a) && mtbdd_isleaf(b)) {
        storm_rational_number_ptr ma = mtbdd_getstorm_rational_number_ptr(a);
        storm_rational_number_ptr mb = mtbdd_getstorm_rational_number_ptr(b);

        return storm_rational_number_less(ma, mb) ? mtbdd_true : mtbdd_false;
    }

    return mtbdd_invalid;
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_number_op_greater, MTBDD*, pa, MTBDD*, pb) {
    MTBDD a = *pa, b = *pb;

    /* Check for partial functions and for Boolean (filter) */
    if (a == mtbdd_false || b == mtbdd_false) return mtbdd_false;

    /* If both leaves, compute less */
    if (mtbdd_isleaf(a) && mtbdd_isleaf(b)) {
        storm_rational_number_ptr ma = mtbdd_getstorm_rational_number_ptr(a);
        storm_rational_number_ptr mb = mtbdd_getstorm_rational_number_ptr(b);

        return storm_rational_number_less_or_equal(ma, mb) ? mtbdd_false : mtbdd_true;
    }

    return mtbdd_invalid;
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_number_op_less_or_equal, MTBDD*, pa, MTBDD*, pb) {
    MTBDD a = *pa, b = *pb;

    /* Check for partial functions and for Boolean (filter) */
    if (a == mtbdd_false || b == mtbdd_false) return mtbdd_false;

    /* If both leaves, compute less or equal */
    if (mtbdd_isleaf(a) && mtbdd_isleaf(b)) {
        storm_rational_number_ptr ma = mtbdd_getstorm_rational_number_ptr(a);
        storm_rational_number_ptr mb = mtbdd_getstorm_rational_number_ptr(b);

        return storm_rational_number_less_or_equal(ma, mb) ? mtbdd_true : mtbdd_false;
    }

    return mtbdd_invalid;
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_number_op_greater_or_equal, MTBDD*, pa, MTBDD*, pb) {
    MTBDD a = *pa, b = *pb;

    /* Check for partial functions and for Boolean (filter) */
    if (a == mtbdd_false || b == mtbdd_false) return mtbdd_false;

    /* If both leaves, compute less or equal */
    if (mtbdd_isleaf(a) && mtbdd_isleaf(b)) {
        storm_rational_number_ptr ma = mtbdd_getstorm_rational_number_ptr(a);
        storm_rational_number_ptr mb = mtbdd_getstorm_rational_number_ptr(b);

        return storm_rational_number_less(ma, mb) ? mtbdd_false : mtbdd_true;
    }

    return mtbdd_invalid;
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_number_op_mod, MTBDD*, pa, MTBDD*, pb) {
    MTBDD a = *pa, b = *pb;

    /* Check for partial functions and for Boolean (filter) */
    if (a == mtbdd_false || b == mtbdd_false) return mtbdd_false;

    /* If both leaves, compute modulo */
    if (mtbdd_isleaf(a) && mtbdd_isleaf(b)) {
        storm_rational_number_ptr ma = mtbdd_getstorm_rational_number_ptr(a);
        storm_rational_number_ptr mb = mtbdd_getstorm_rational_number_ptr(b);

        storm_rational_number_ptr mres = storm_rational_number_mod(ma, mb);
        MTBDD res = mtbdd_storm_rational_number(mres);
        storm_rational_number_destroy(mres);

        return res;
    }

    return mtbdd_invalid;
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_number_op_min, MTBDD*, pa, MTBDD*, pb) {
    MTBDD a = *pa, b = *pb;

    /* Check for partial functions and for Boolean (filter) */
    if (a == mtbdd_false || b == mtbdd_false) return mtbdd_false;

    /* If both leaves, compute min */
    if (mtbdd_isleaf(a) && mtbdd_isleaf(b)) {
        storm_rational_number_ptr ma = mtbdd_getstorm_rational_number_ptr(a);
        storm_rational_number_ptr mb = mtbdd_getstorm_rational_number_ptr(b);

        storm_rational_number_ptr mres = storm_rational_number_min(ma, mb);
        MTBDD res = mtbdd_storm_rational_number(mres);
        storm_rational_number_destroy(mres);

        return res;
    }

    /* Commutative, so make "a" the lowest for better cache performance */
    if (a < b) {
        *pa = b;
        *pb = a;
    }

    return mtbdd_invalid;
}

TASK_IMPL_3(MTBDD, sylvan_storm_rational_number_abstract_op_min, MTBDD, a, MTBDD, b, int, k) {
    if (k==0) {
        return mtbdd_apply(a, b, TASK(sylvan_storm_rational_number_op_min));
    } else {
        MTBDD res = a;
        for (int i=0; i<k; i++) {
            mtbdd_refs_push(res);
            res = mtbdd_apply(res, res, TASK(sylvan_storm_rational_number_op_min));
            mtbdd_refs_pop(1);
        }
        return res;
    }
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_number_op_max, MTBDD*, pa, MTBDD*, pb) {
    MTBDD a = *pa, b = *pb;

    /* Check for partial functions and for Boolean (filter) */
    if (a == mtbdd_false || b == mtbdd_false) return mtbdd_false;

    /* If both leaves, compute max */
    if (mtbdd_isleaf(a) && mtbdd_isleaf(b)) {
        storm_rational_number_ptr ma = mtbdd_getstorm_rational_number_ptr(a);
        storm_rational_number_ptr mb = mtbdd_getstorm_rational_number_ptr(b);

        storm_rational_number_ptr mres = storm_rational_number_max(ma, mb);
        MTBDD res = mtbdd_storm_rational_number(mres);
        storm_rational_number_destroy(mres);

        return res;
    }

    /* Commutative, so make "a" the lowest for better cache performance */
    if (a < b) {
        *pa = b;
        *pb = a;
    }

    return mtbdd_invalid;
}

TASK_IMPL_3(MTBDD, sylvan_storm_rational_number_abstract_op_max, MTBDD, a, MTBDD, b, int, k) {
    if (k==0) {
        return mtbdd_apply(a, b, TASK(sylvan_storm_rational_number_op_max));
    } else {
        MTBDD res = a;
        for (int i=0; i<k; i++) {
            mtbdd_refs_push(res);
            res = mtbdd_apply(res, res, TASK(sylvan_storm_rational_number_op_max));
            mtbdd_refs_pop(1);
        }
        return res;
    }
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_number_op_pow, MTBDD*, pa, MTBDD*, pb) {
    MTBDD a = *pa, b = *pb;

    /* Check for partial functions and for Boolean (filter) */
    if (a == mtbdd_false || b == mtbdd_false) return mtbdd_false;

    /* Handle multiplication of leaves */
    if (mtbdd_isleaf(a) && mtbdd_isleaf(b)) {
        storm_rational_number_ptr ma = mtbdd_getstorm_rational_number_ptr(a);
        storm_rational_number_ptr mb = mtbdd_getstorm_rational_number_ptr(b);

        storm_rational_number_ptr mres = storm_rational_number_pow(ma, mb);
        MTBDD res = mtbdd_storm_rational_number(mres);
        storm_rational_number_destroy(mres);

        return res;
    }

    return mtbdd_invalid;
}

TASK_IMPL_3(MTBDD, sylvan_storm_rational_number_abstract_op_plus, MTBDD, a, MTBDD, b, int, k) {
    if (k==0) {
        return mtbdd_apply(a, b, TASK(sylvan_storm_rational_number_op_plus));
    } else {
        MTBDD res = a;
        for (int i=0; i<k; i++) {
            mtbdd_refs_push(res);
            res = mtbdd_apply(res, res, TASK(sylvan_storm_rational_number_op_plus));
            mtbdd_refs_pop(1);
        }
        return res;
    }
}

TASK_IMPL_3(MTBDD, sylvan_storm_rational_number_abstract_op_times, MTBDD, a, MTBDD, b, int, k) {
    if (k==0) {
        return mtbdd_apply(a, b, TASK(sylvan_storm_rational_number_op_times));
    } else {
        MTBDD res = a;
        for (int i=0; i<k; i++) {
            mtbdd_refs_push(res);
            res = mtbdd_apply(res, res, TASK(sylvan_storm_rational_number_op_times));
            mtbdd_refs_pop(1);
        }
        return res;
    }
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_number_op_neg, MTBDD, dd, size_t, p) {
    /* Handle partial functions */
    if (dd == mtbdd_false) return mtbdd_false;

    /* Compute result for leaf */
    if (mtbdd_isleaf(dd)) {
		storm_rational_number_ptr mdd = mtbdd_getstorm_rational_number_ptr(dd);

		storm_rational_number_ptr mres = storm_rational_number_negate(mdd);
		MTBDD res = mtbdd_storm_rational_number(mres);
		storm_rational_number_destroy(mres);

        return res;
    }

    return mtbdd_invalid;
    (void)p;
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_number_op_floor, MTBDD, dd, size_t, p) {
    /* Handle partial functions */
    if (dd == mtbdd_false) return mtbdd_false;

    /* Compute result for leaf */
    if (mtbdd_isleaf(dd)) {
        storm_rational_number_ptr mdd = mtbdd_getstorm_rational_number_ptr(dd);

        storm_rational_number_ptr mres = storm_rational_number_floor(mdd);
        MTBDD res = mtbdd_storm_rational_number(mres);
        storm_rational_number_destroy(mres);

        return res;
    }

    return mtbdd_invalid;
    (void)p;
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_number_op_ceil, MTBDD, dd, size_t, p) {
    /* Handle partial functions */
    if (dd == mtbdd_false) return mtbdd_false;

    /* Compute result for leaf */
    if (mtbdd_isleaf(dd)) {
        storm_rational_number_ptr mdd = mtbdd_getstorm_rational_number_ptr(dd);

        storm_rational_number_ptr mres = storm_rational_number_ceil(mdd);
        MTBDD res = mtbdd_storm_rational_number(mres);
        storm_rational_number_destroy(mres);

        return res;
    }

    return mtbdd_invalid;
    (void)p;
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_number_op_to_double, MTBDD, dd, size_t, p) {
    /* Handle partial functions */
    if (dd == mtbdd_false) return mtbdd_false;

    /* Compute result for leaf */
    if (mtbdd_isleaf(dd)) {
		storm_rational_number_ptr mdd = mtbdd_getstorm_rational_number_ptr(dd);
		MTBDD result = mtbdd_double(storm_rational_number_get_value_double(mdd));
		return result;
    }

    return mtbdd_invalid;
	(void)p;
}

TASK_IMPL_3(MTBDD, sylvan_storm_rational_number_and_exists, MTBDD, a, MTBDD, b, MTBDD, v) {
    /* Check terminal cases */

    /* If v == true, then <vars> is an empty set */
    if (v == mtbdd_true) return mtbdd_apply(a, b, TASK(sylvan_storm_rational_number_op_times));

    /* Try the times operator on a and b */
    MTBDD result = CALL(sylvan_storm_rational_number_op_times, &a, &b);
    if (result != mtbdd_invalid) {
        /* Times operator successful, store reference (for garbage collection) */
        mtbdd_refs_push(result);
        /* ... and perform abstraction */
        result = mtbdd_abstract(result, v, TASK(sylvan_storm_rational_number_abstract_op_plus));
        mtbdd_refs_pop(1);
        /* Note that the operation cache is used in mtbdd_abstract */
        return result;
    }

    /* Maybe perform garbage collection */
    sylvan_gc_test();

    /* Check cache. Note that we do this now, since the times operator might swap a and b (commutative) */
    if (cache_get3(CACHE_MTBDD_AND_EXISTS_RN, a, b, v, &result)) return result;

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
        result = CALL(sylvan_storm_rational_number_and_exists, a, b, node_gethigh(v, nv));
        mtbdd_refs_push(result);
        result = mtbdd_apply(result, result, TASK(sylvan_storm_rational_number_op_plus));
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
            mtbdd_refs_spawn(SPAWN(sylvan_storm_rational_number_and_exists, ahigh, bhigh, node_gethigh(v, nv)));
            MTBDD low = mtbdd_refs_push(CALL(sylvan_storm_rational_number_and_exists, alow, blow, node_gethigh(v, nv)));
            MTBDD high = mtbdd_refs_push(mtbdd_refs_sync(SYNC(sylvan_storm_rational_number_and_exists)));
            result = CALL(mtbdd_apply, low, high, TASK(sylvan_storm_rational_number_op_plus));
            mtbdd_refs_pop(2);
        } else /* vv > v */ {
            /* Recursive, then create node */
            mtbdd_refs_spawn(SPAWN(sylvan_storm_rational_number_and_exists, ahigh, bhigh, v));
            MTBDD low = mtbdd_refs_push(CALL(sylvan_storm_rational_number_and_exists, alow, blow, v));
            MTBDD high = mtbdd_refs_sync(SYNC(sylvan_storm_rational_number_and_exists));
            mtbdd_refs_pop(1);
            result = mtbdd_makenode(var, low, high);
        }
    }

    /* Store in cache */
    cache_put3(CACHE_MTBDD_AND_EXISTS_RN, a, b, v, result);
    return result;
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_number_op_threshold, MTBDD, a, size_t, svalue) {
    storm_rational_number_ptr value = (storm_rational_number_ptr)(void*)svalue;

    if (mtbdd_isleaf(a)) {
        storm_rational_number_ptr ma = mtbdd_getstorm_rational_number_ptr(a);
        return storm_rational_number_less(ma, value) ? mtbdd_false : mtbdd_true;
    }

    return mtbdd_invalid;
}

TASK_IMPL_2(MTBDD, sylvan_storm_rational_number_op_strict_threshold, MTBDD, a, size_t, svalue) {
    storm_rational_number_ptr value = (storm_rational_number_ptr)(void*)svalue;

    if (mtbdd_isleaf(a)) {
        storm_rational_number_ptr ma = mtbdd_getstorm_rational_number_ptr(a);

        return storm_rational_number_less_or_equal(ma, value) ? mtbdd_false : mtbdd_true;
    }

    return mtbdd_invalid;
}

TASK_IMPL_1(MTBDD, sylvan_storm_rational_number_minimum, MTBDD, a) {
    /* Check terminal case */
    if (a == mtbdd_false) return mtbdd_false;
    mtbddnode_t na = MTBDD_GETNODE(a);
    if (mtbddnode_isleaf(na)) return a;

    /* Maybe perform garbage collection */
    sylvan_gc_test();

    /* Check cache */
    MTBDD result;
    if (cache_get3(CACHE_MTBDD_MINIMUM_RN, a, 0, 0, &result)) return result;

    /* Call recursive */
    SPAWN(sylvan_storm_rational_number_minimum, node_getlow(a, na));
    MTBDD high = CALL(sylvan_storm_rational_number_minimum, node_gethigh(a, na));
    MTBDD low = SYNC(sylvan_storm_rational_number_minimum);

    storm_rational_number_ptr fl = mtbdd_getstorm_rational_number_ptr(low);
    storm_rational_number_ptr fh = mtbdd_getstorm_rational_number_ptr(high);

    if (storm_rational_number_less_or_equal(fl, fh)) {
        return low;
    } else {
        return high;
    }

    /* Store in cache */
    cache_put3(CACHE_MTBDD_MINIMUM_RN, a, 0, 0, result);
    return result;
}

TASK_IMPL_1(MTBDD, sylvan_storm_rational_number_maximum, MTBDD, a)
{
    /* Check terminal case */
    if (a == mtbdd_false) return mtbdd_false;
    mtbddnode_t na = MTBDD_GETNODE(a);
    if (mtbddnode_isleaf(na)) return a;

    /* Maybe perform garbage collection */
    sylvan_gc_test();

    /* Check cache */
    MTBDD result;
    if (cache_get3(CACHE_MTBDD_MAXIMUM_RN, a, 0, 0, &result)) return result;

    /* Call recursive */
    SPAWN(sylvan_storm_rational_number_maximum, node_getlow(a, na));
    MTBDD high = CALL(sylvan_storm_rational_number_maximum, node_gethigh(a, na));
    MTBDD low = SYNC(sylvan_storm_rational_number_maximum);

    storm_rational_number_ptr fl = mtbdd_getstorm_rational_number_ptr(low);
    storm_rational_number_ptr fh = mtbdd_getstorm_rational_number_ptr(high);

    if (storm_rational_number_less(fl, fh)) {
        return high;
    } else {
        return low;
    }

    /* Store in cache */
    cache_put3(CACHE_MTBDD_MAXIMUM_RN, a, 0, 0, result);
    return result;
}

TASK_4(MTBDD, sylvan_storm_rational_number_equal_norm_d2, MTBDD, a, MTBDD, b, storm_rational_number_ptr, svalue, int*, shortcircuit)
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
        storm_rational_number_ptr fa = mtbdd_getstorm_rational_number_ptr(a);
        storm_rational_number_ptr fb = mtbdd_getstorm_rational_number_ptr(b);

        return storm_rational_number_equal_modulo_precision(0, fa, fb, svalue) ? mtbdd_true : mtbdd_false;
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
    if (cache_get3(CACHE_MTBDD_EQUAL_NORM_RN, a, b, (uint64_t)svalue, &result)) {
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

    SPAWN(sylvan_storm_rational_number_equal_norm_d2, ahigh, bhigh, svalue, shortcircuit);
    result = CALL(sylvan_storm_rational_number_equal_norm_d2, alow, blow, svalue, shortcircuit);
    if (result == mtbdd_false) *shortcircuit = 1;
    if (result != SYNC(sylvan_storm_rational_number_equal_norm_d2)) result = mtbdd_false;
    if (result == mtbdd_false) *shortcircuit = 1;

    /* Store in cache */
    if (cache_put3(CACHE_MTBDD_EQUAL_NORM_RN, a, b, (uint64_t)svalue, result)) {
        sylvan_stats_count(MTBDD_EQUAL_NORM_CACHEDPUT);
    }

    return result;
}

TASK_IMPL_3(MTBDD, sylvan_storm_rational_number_equal_norm_d, MTBDD, a, MTBDD, b, storm_rational_number_ptr, d)
{
    /* the implementation checks shortcircuit in every task and if the two
     MTBDDs are not equal module epsilon, then the computation tree quickly aborts */
    int shortcircuit = 0;
    return CALL(sylvan_storm_rational_number_equal_norm_d2, a, b, d, &shortcircuit);
}

/**
 * Compare two Double MTBDDs, returns Boolean True if they are equal within some value epsilon
 * This version computes the relative difference vs the value in a.
 */
TASK_4(MTBDD, sylvan_storm_rational_number_equal_norm_rel_d2, MTBDD, a, MTBDD, b, storm_rational_number_ptr, svalue, int*, shortcircuit)
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
        storm_rational_number_ptr fa = mtbdd_getstorm_rational_number_ptr(a);
        storm_rational_number_ptr fb = mtbdd_getstorm_rational_number_ptr(b);

        return storm_rational_number_equal_modulo_precision(1, fa, fb, svalue) ? mtbdd_true : mtbdd_false;
    }

    /* Maybe perform garbage collection */
    sylvan_gc_test();

    /* Count operation */
    sylvan_stats_count(MTBDD_EQUAL_NORM_REL);

    /* Check cache */
    MTBDD result;
    if (cache_get3(CACHE_MTBDD_EQUAL_NORM_REL_RN, a, b, (uint64_t)svalue, &result)) {
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

    SPAWN(sylvan_storm_rational_number_equal_norm_rel_d2, ahigh, bhigh, svalue, shortcircuit);
    result = CALL(sylvan_storm_rational_number_equal_norm_rel_d2, alow, blow, svalue, shortcircuit);
    if (result == mtbdd_false) *shortcircuit = 1;
    if (result != SYNC(sylvan_storm_rational_number_equal_norm_rel_d2)) result = mtbdd_false;
    if (result == mtbdd_false) *shortcircuit = 1;

    /* Store in cache */
    if (cache_put3(CACHE_MTBDD_EQUAL_NORM_REL_RN, a, b, (uint64_t)svalue, result)) {
        sylvan_stats_count(MTBDD_EQUAL_NORM_REL_CACHEDPUT);
    }

    return result;
}

TASK_IMPL_3(MTBDD, sylvan_storm_rational_number_equal_norm_rel_d, MTBDD, a, MTBDD, b, storm_rational_number_ptr, d)
{
    /* the implementation checks shortcircuit in every task and if the two
     MTBDDs are not equal module epsilon, then the computation tree quickly aborts */
    int shortcircuit = 0;
    return CALL(sylvan_storm_rational_number_equal_norm_rel_d2, a, b, d, &shortcircuit);
}

TASK_IMPL_3(BDD, sylvan_storm_rational_number_min_abstract_representative, MTBDD, a, BDD, v, BDDVAR, prev_level) {
    /* Maybe perform garbage collection */
    sylvan_gc_test();

    if (sylvan_set_isempty(v)) {
        return sylvan_true;
    }

    /* Cube is guaranteed to be a cube at this point. */
    if (mtbdd_isleaf(a)) {
        BDD _v = sylvan_set_next(v);
        BDD res = CALL(sylvan_storm_rational_number_min_abstract_representative, a, _v, prev_level);
        if (res == sylvan_invalid) {
            return sylvan_invalid;
        }
        sylvan_ref(res);

        BDD res1 = sylvan_not(sylvan_ite(sylvan_ithvar(bddnode_getvariable(MTBDD_GETNODE(v))), sylvan_true, sylvan_not(res)));
        if (res1 == sylvan_invalid) {
            sylvan_deref(res);
            return sylvan_invalid;
        }
        sylvan_deref(res);
        return res1;
    }

    mtbddnode_t na = MTBDD_GETNODE(a);
    uint32_t va = mtbddnode_getvariable(na);
    bddnode_t nv = MTBDD_GETNODE(v);
    BDDVAR vv = bddnode_getvariable(nv);

    /* Abstract a variable that does not appear in a. */
    if (va > vv) {
        BDD _v = sylvan_set_next(v);
        BDD res = CALL(sylvan_storm_rational_number_min_abstract_representative, a, _v, va);
        if (res == sylvan_invalid) {
            return sylvan_invalid;
        }

        // Fill in the missing variables to make representative unique.
        sylvan_ref(res);
        BDD res1 = sylvan_ite(sylvan_ithvar(vv), sylvan_false, res);
        if (res1 == sylvan_invalid) {
            sylvan_deref(res);
            return sylvan_invalid;
        }
        sylvan_deref(res);
       	return res1;
    }

    /* Check cache */
    MTBDD result;
    if (cache_get3(CACHE_MTBDD_ABSTRACT_REPRESENTATIVE, a, v, (size_t)1, &result)) {
        sylvan_stats_count(MTBDD_ABSTRACT_CACHED);
        return result;
    }

    MTBDD E = mtbdd_getlow(a);
    MTBDD T = mtbdd_gethigh(a);

    /* If the two indices are the same, so are their levels. */
    if (va == vv) {
        BDD _v = sylvan_set_next(v);
        BDD res1 = CALL(sylvan_storm_rational_number_min_abstract_representative, E, _v, va);
        if (res1 == sylvan_invalid) {
            return sylvan_invalid;
        }
        sylvan_ref(res1);

        BDD res2 = CALL(sylvan_storm_rational_number_min_abstract_representative, T, _v, va);
        if (res2 == sylvan_invalid) {
            sylvan_deref(res1);
            return sylvan_invalid;
        }
        sylvan_ref(res2);

        MTBDD left = sylvan_storm_rational_number_abstract_min(E, _v);
        if (left == mtbdd_invalid) {
            sylvan_deref(res1);
            sylvan_deref(res2);
            return sylvan_invalid;
        }
        mtbdd_ref(left);

        MTBDD right = sylvan_storm_rational_number_abstract_min(T, _v);
        if (right == mtbdd_invalid) {
            sylvan_deref(res1);
            sylvan_deref(res2);
            mtbdd_deref(left);
            return sylvan_invalid;
        }
        mtbdd_ref(right);

        BDD tmp = sylvan_storm_rational_number_less_or_equal(left, right);
        if (tmp == sylvan_invalid) {
            sylvan_deref(res1);
            sylvan_deref(res2);
            mtbdd_deref(left);
            mtbdd_deref(right);
            return sylvan_invalid;
        }
        sylvan_ref(tmp);

        mtbdd_deref(left);
        mtbdd_deref(right);

        BDD res1Inf = sylvan_ite(tmp, res1, sylvan_false);
        if (res1Inf == sylvan_invalid) {
            sylvan_deref(res1);
            sylvan_deref(res2);
            sylvan_deref(tmp);
            return sylvan_invalid;
        }
        sylvan_ref(res1Inf);
        sylvan_deref(res1);

        BDD res2Inf = sylvan_ite(tmp, sylvan_false, res2);
        if (res2Inf == sylvan_invalid) {
            sylvan_deref(res2);
            sylvan_deref(res1Inf);
            sylvan_deref(tmp);
            return sylvan_invalid;
        }
        sylvan_ref(res2Inf);
        sylvan_deref(res2);
        sylvan_deref(tmp);

        BDD res = (res1Inf == res2Inf) ? sylvan_ite(sylvan_ithvar(va), sylvan_false, res1Inf) : sylvan_ite(sylvan_ithvar(va), res2Inf, res1Inf);

        if (res == sylvan_invalid) {
            sylvan_deref(res1Inf);
            sylvan_deref(res2Inf);
            return sylvan_invalid;
        }
        sylvan_ref(res);
        sylvan_deref(res1Inf);
        sylvan_deref(res2Inf);

        /* Store in cache */
        if (cache_put3(CACHE_MTBDD_ABSTRACT_REPRESENTATIVE, a, v, (size_t)1, res)) {
            sylvan_stats_count(MTBDD_ABSTRACT_CACHEDPUT);
        }

        sylvan_deref(res);
        return res;
    }
    else { /* if (va < vv) */
        BDD res1 = CALL(sylvan_storm_rational_number_min_abstract_representative, E, v, va);
        if (res1 == sylvan_invalid) {
            return sylvan_invalid;
        }
        sylvan_ref(res1);
        BDD res2 = CALL(sylvan_storm_rational_number_min_abstract_representative, T, v, va);
        if (res2 == sylvan_invalid) {
            sylvan_deref(res1);
            return sylvan_invalid;
        }
        sylvan_ref(res2);

        BDD res = (res1 == res2) ? res1 : sylvan_ite(sylvan_ithvar(va), res2, res1);
        if (res == sylvan_invalid) {
            sylvan_deref(res1);
            sylvan_deref(res2);
            return sylvan_invalid;
        }
        sylvan_deref(res1);
        sylvan_deref(res2);

        /* Store in cache */
        if (cache_put3(CACHE_MTBDD_ABSTRACT_REPRESENTATIVE, a, v, (size_t)1, res)) {
            sylvan_stats_count(MTBDD_ABSTRACT_CACHEDPUT);
        }
        return res;
    }
}

TASK_IMPL_3(BDD, sylvan_storm_rational_number_max_abstract_representative, MTBDD, a, MTBDD, v, uint32_t, prev_level) {
    /* Maybe perform garbage collection */
    sylvan_gc_test();

    if (sylvan_set_isempty(v)) {
        return sylvan_true;
    }

    /* Count operation */
    sylvan_stats_count(MTBDD_ABSTRACT);

    /* Cube is guaranteed to be a cube at this point. */
    if (mtbdd_isleaf(a)) {
        /* Compute result */
        BDD _v = sylvan_set_next(v);
        BDD res = CALL(sylvan_storm_rational_number_max_abstract_representative, a, _v, prev_level);
        if (res == sylvan_invalid) {
            return sylvan_invalid;
        }
        sylvan_ref(res);

        BDD res1 = sylvan_not(sylvan_ite(sylvan_ithvar(bddnode_getvariable(MTBDD_GETNODE(v))), sylvan_true, sylvan_not(res)));
        if (res1 == sylvan_invalid) {
            sylvan_deref(res);
            return sylvan_invalid;
        }
        sylvan_deref(res);

        return res1;
    }

    mtbddnode_t na = MTBDD_GETNODE(a);
    uint32_t va = mtbddnode_getvariable(na);
    bddnode_t nv = MTBDD_GETNODE(v);
    BDDVAR vv = bddnode_getvariable(nv);

    /* Abstract a variable that does not appear in a. */
    if (vv < va) {
        BDD _v = sylvan_set_next(v);
        BDD res = CALL(sylvan_storm_rational_number_max_abstract_representative, a, _v, va);
        if (res == sylvan_invalid) {
            return sylvan_invalid;
        }

        // Fill in the missing variables to make representative unique.
        sylvan_ref(res);
        BDD res1 = sylvan_ite(sylvan_ithvar(vv), sylvan_false, res);
        if (res1 == sylvan_invalid) {
            sylvan_deref(res);
            return sylvan_invalid;
        }
        sylvan_deref(res);
       	return res1;
    }

    /* Check cache */
    MTBDD result;
    if (cache_get3(CACHE_MTBDD_ABSTRACT_REPRESENTATIVE, a, v, (size_t)0, &result)) {
        sylvan_stats_count(MTBDD_ABSTRACT_CACHED);
        return result;
    }

    MTBDD E = mtbdd_getlow(a);
    MTBDD T = mtbdd_gethigh(a);

    /* If the two indices are the same, so are their levels. */
    if (va == vv) {
        BDD _v = sylvan_set_next(v);
        BDD res1 = CALL(sylvan_storm_rational_number_max_abstract_representative, E, _v, va);
        if (res1 == sylvan_invalid) {
            return sylvan_invalid;
        }
        sylvan_ref(res1);

        BDD res2 = CALL(sylvan_storm_rational_number_max_abstract_representative, T, _v, va);
        if (res2 == sylvan_invalid) {
            sylvan_deref(res1);
            return sylvan_invalid;
        }
        sylvan_ref(res2);

        MTBDD left = sylvan_storm_rational_number_abstract_max(E, _v);
        if (left == mtbdd_invalid) {
            sylvan_deref(res1);
            sylvan_deref(res2);
            return sylvan_invalid;
        }
        mtbdd_ref(left);

        MTBDD right = sylvan_storm_rational_number_abstract_max(T, _v);
        if (right == mtbdd_invalid) {
            sylvan_deref(res1);
            sylvan_deref(res2);
            mtbdd_deref(left);
            return sylvan_invalid;
        }
        mtbdd_ref(right);

        BDD tmp = sylvan_storm_rational_number_greater_or_equal(left, right);
        if (tmp == sylvan_invalid) {
            sylvan_deref(res1);
            sylvan_deref(res2);
            mtbdd_deref(left);
            mtbdd_deref(right);
            return sylvan_invalid;
        }
        sylvan_ref(tmp);

        mtbdd_deref(left);
        mtbdd_deref(right);

        BDD res1Inf = sylvan_ite(tmp, res1, sylvan_false);
        if (res1Inf == sylvan_invalid) {
            sylvan_deref(res1);
            sylvan_deref(res2);
            sylvan_deref(tmp);
            return sylvan_invalid;
        }
        sylvan_ref(res1Inf);
        sylvan_deref(res1);

        BDD res2Inf = sylvan_ite(tmp, sylvan_false, res2);
        if (res2Inf == sylvan_invalid) {
            sylvan_deref(res2);
            sylvan_deref(res1Inf);
            sylvan_deref(tmp);
            return sylvan_invalid;
        }
        sylvan_ref(res2Inf);
        sylvan_deref(res2);
        sylvan_deref(tmp);

        BDD res = (res1Inf == res2Inf) ? sylvan_ite(sylvan_ithvar(va), sylvan_false, res1Inf) : sylvan_ite(sylvan_ithvar(va), res2Inf, res1Inf);

        if (res == sylvan_invalid) {
            sylvan_deref(res1Inf);
            sylvan_deref(res2Inf);
            return sylvan_invalid;
        }
        sylvan_ref(res);
        sylvan_deref(res1Inf);
        sylvan_deref(res2Inf);

        /* Store in cache */
        if (cache_put3(CACHE_MTBDD_ABSTRACT_REPRESENTATIVE, a, v, (size_t)0, res)) {
            sylvan_stats_count(MTBDD_ABSTRACT_CACHEDPUT);
        }

        sylvan_deref(res);
        return res;
    }
    else { /* if (va < vv) */
        BDD res1 = CALL(sylvan_storm_rational_number_max_abstract_representative, E, v, va);
        if (res1 == sylvan_invalid) {
            return sylvan_invalid;
        }
        sylvan_ref(res1);
        BDD res2 = CALL(sylvan_storm_rational_number_max_abstract_representative, T, v, va);
        if (res2 == sylvan_invalid) {
            sylvan_deref(res1);
            return sylvan_invalid;
        }
        sylvan_ref(res2);

        BDD res = (res1 == res2) ? res1 : sylvan_ite(sylvan_ithvar(va), res2, res1);
        if (res == sylvan_invalid) {
            sylvan_deref(res1);
            sylvan_deref(res2);
            return sylvan_invalid;
        }
        sylvan_deref(res1);
        sylvan_deref(res2);

        /* Store in cache */
        if (cache_put3(CACHE_MTBDD_ABSTRACT_REPRESENTATIVE, a, v, (size_t)0, res)) {
            sylvan_stats_count(MTBDD_ABSTRACT_CACHEDPUT);
        }

        return res;
    }
}
