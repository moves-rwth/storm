#include "sylvan_int.h"

/**
 * Calculates \exists variables . a
 */
TASK_IMPL_3(BDD, sylvan_existsRepresentative, BDD, a, BDD, variables, BDDVAR, prev_level)
{
	int aIsNegated = (a & sylvan_complement) == ((uint64_t)0) ? 0 : 1;

	BDD aRegular = (aIsNegated) ? sylvan_not(a) : a;

	if (aRegular == sylvan_false) {
		if (aIsNegated) {
			if (sylvan_set_isempty(variables)) {
				return sylvan_true;
			} else {
				//printf("return in preprocessing...3\n");
				BDD _v = sylvan_set_next(variables);
				BDD res = CALL(sylvan_existsRepresentative, a, _v, prev_level);
				if (res == sylvan_invalid) {
					return sylvan_invalid;
				}
				sylvan_ref(res);

				BDD res1 = sylvan_ite(sylvan_ithvar(bddnode_getvariable(MTBDD_GETNODE(variables))), sylvan_false, res);
				if (res1 == sylvan_invalid) {
					sylvan_deref(res);
					return sylvan_invalid;
				}
				sylvan_deref(res);
				return res1;
			}
		} else {
			return a;
		}
	} else if (sylvan_set_isempty(variables)) {
		return a;
	}

    BDD result;
    if (cache_get3(CACHE_MTBDD_ABSTRACT_REPRESENTATIVE, a, variables, (size_t)2, &result)) {
        sylvan_stats_count(MTBDD_ABSTRACT_CACHED);
        return result;
    }

	/* From now on, f and cube are non-constant. */
	bddnode_t na = MTBDD_GETNODE(a);
    BDDVAR level = bddnode_getvariable(na);

    bddnode_t nv = MTBDD_GETNODE(variables);
    BDDVAR vv = bddnode_getvariable(nv);

	/* Abstract a variable that does not appear in f. */
    if (level > vv) {
		BDD _v = sylvan_set_next(variables);
        BDD res = CALL(sylvan_existsRepresentative, a, _v, level);
        if (res == sylvan_invalid) {
            return sylvan_invalid;
        }
        sylvan_ref(res);

        BDD res1 = sylvan_ite(sylvan_ithvar(vv), sylvan_false, res);

        if (res1 == sylvan_invalid) {
            sylvan_deref(res);
            return sylvan_invalid;
        }
        sylvan_deref(res);

       	return res1;
    }

	/* Compute the cofactors of a. */
	BDD aLow = node_low(a, na); // ELSE
    BDD aHigh = node_high(a, na); // THEN

	/* If the two indices are the same, so are their levels. */
    if (level == vv) {
		BDD _v = sylvan_set_next(variables);
        BDD res1 = CALL(sylvan_existsRepresentative, aLow, _v, level);
        if (res1 == sylvan_invalid) {
            return sylvan_invalid;
        }
        if (res1 == sylvan_true) {
			return sylvan_not(variables);
        }
        sylvan_ref(res1);

        BDD res2 = CALL(sylvan_existsRepresentative, aHigh, _v, level);
        if (res2 == sylvan_invalid) {
            sylvan_deref(res1);
            return sylvan_invalid;
        }
        sylvan_ref(res2);

        BDD left = CALL(sylvan_exists, aLow, _v, 0);
        if (left == sylvan_invalid) {
            sylvan_deref(res1);
			sylvan_deref(res2);
            return sylvan_invalid;
        }
        sylvan_ref(left);

        BDD res1Inf = sylvan_ite(left, res1, sylvan_false);
        if (res1Inf == sylvan_invalid) {
            sylvan_deref(res1);
			sylvan_deref(res2);
			sylvan_deref(left);
            return sylvan_invalid;
        }
        sylvan_ref(res1Inf);
		sylvan_deref(res1);

        BDD res2Inf = sylvan_ite(left, sylvan_false, res2);
        if (res2Inf == sylvan_invalid) {
			sylvan_deref(res2);
			sylvan_deref(left);
			sylvan_deref(res1Inf);
            return sylvan_invalid;
        }
        sylvan_ref(res2Inf);
		sylvan_deref(res2);
		sylvan_deref(left);

        assert(res1Inf != res2Inf);
        BDD res = sylvan_ite(sylvan_ithvar(level), res2Inf, res1Inf);
        if (res == sylvan_invalid) {
            sylvan_deref(res1Inf);
			sylvan_deref(res2Inf);
            return sylvan_invalid;
        }

        /* Store in cache */
        if (cache_put3(CACHE_MTBDD_ABSTRACT_REPRESENTATIVE, a, variables, (size_t)2, res)) {
            sylvan_stats_count(MTBDD_ABSTRACT_CACHEDPUT);
        }

		sylvan_deref(res1Inf);
		sylvan_deref(res2Inf);

        return res;
    } else { /* if (level == vv) */
        BDD res1 = CALL(sylvan_existsRepresentative, aLow, variables, level);
        if (res1 == sylvan_invalid){
            return sylvan_invalid;
        }
        sylvan_ref(res1);

        BDD res2 = CALL(sylvan_existsRepresentative, aHigh, variables, level);
        if (res2 == sylvan_invalid) {
            sylvan_deref(res1);
            return sylvan_invalid;
        }
        sylvan_ref(res2);

        /* ITE takes care of possible complementation of res1 and of the
         ** case in which res1 == res2. */
		BDD res = sylvan_ite(sylvan_ithvar(level), res2, res1);
        if (res == sylvan_invalid) {
            sylvan_deref(res1);
			sylvan_deref(res2);
            return sylvan_invalid;
        }

		sylvan_deref(res1);
		sylvan_deref(res2);

        /* Store in cache */
        if (cache_put3(CACHE_MTBDD_ABSTRACT_REPRESENTATIVE, a, variables, (size_t)2, res)) {
            sylvan_stats_count(MTBDD_ABSTRACT_CACHEDPUT);
        }

        return res;
    }

	// Prevent unused variable warning
	(void)prev_level;
}

TASK_IMPL_2(BDD, sylvan_without, BDD, f, BDD, g)
{
    if (f == sylvan_false || g == sylvan_true) return sylvan_false;
    if (f == sylvan_true) return sylvan_true;
    if (g == sylvan_false) return f;

    BDD result;
    if (cache_get3(CACHE_BDD_WITHOUT, f, g, sylvan_false, &result)) {
        // TODO add statistics
        return result;
    }

    bddnode_t nf = MTBDD_GETNODE(f);
    bddnode_t ng = MTBDD_GETNODE(g);

    BDDVAR vf = bddnode_getvariable(nf);
    BDDVAR vg = bddnode_getvariable(ng);

    if (vf > vg) {
        BDD g2 = node_low(g, ng);
        return CALL(sylvan_without, f, g2);
    }

    BDD f1 = node_high(f,nf);
    BDD f2 = node_low(f,nf);
    BDD g1 = (vf < vg) ? g : node_high(g,ng);
    BDD g2 = (vf < vg) ? g : node_low(g,ng);

    bdd_refs_spawn(SPAWN(sylvan_without, f1, g1));
    BDD v = CALL(sylvan_without, f2, g2);
    bdd_refs_push(v);
    BDD u = bdd_refs_sync(SYNC(sylvan_without));
    bdd_refs_push(u);

    result = sylvan_ite(sylvan_ithvar(vf), u, v);
    bdd_refs_pop(2);

    if (cache_put3(CACHE_BDD_WITHOUT, f, g, sylvan_false, result)) {
        // TODO add statistics
    }

    return result;
}

TASK_IMPL_1(BDD, sylvan_minsol, BDD, f)
{
    if(f == sylvan_false || f == sylvan_true) return f;

    BDD result;
    if (cache_get3(CACHE_BDD_MINSOL, f, sylvan_false, sylvan_false, &result)) {
        // TODO add statistics
        return result;
    }

    // f = Ite(vf, g, h)
    bddnode_t nf = MTBDD_GETNODE(f);
    BDDVAR vf = bddnode_getvariable(nf);
    BDD g = node_high(f,nf);
    BDD h = node_low(f,nf);

    bdd_refs_spawn(SPAWN(sylvan_minsol, h));

    BDD k = CALL(sylvan_minsol, g);
    bdd_refs_push(k);

    BDD u = sylvan_without(k, h);
    bdd_refs_pop(1);
    bdd_refs_push(u);

    BDD v = bdd_refs_sync(SYNC(sylvan_minsol));
    bdd_refs_push(v);

    result = sylvan_ite(sylvan_ithvar(vf), u, v);
    bdd_refs_pop(2);

    if (cache_put3(CACHE_BDD_MINSOL, f, sylvan_false, sylvan_false, result)) {
        // TODO add statistics
    }

    return result;
}

