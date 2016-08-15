/* */

/**
 * Calculates \exists variables . a
 */
TASK_IMPL_3(BDD, sylvan_existsRepresentative, BDD, a, BDD, variables, BDDVAR, prev_level)
{
	int aIsNegated = (a & sylvan_complement) == ((uint64_t)0) ? 0 : 1;
	
	BDD aRegular = (aIsNegated) ? sylvan_not(a) : a;
	
	if (aRegular == sylvan_false) {
		if (aIsNegated) {
			//printf("return in preprocessing...1\n");
			return a;			
		}
		
		if (sylvan_set_isempty(variables)) {
			//printf("return in preprocessing...2\n");
			return sylvan_true;			
		} else {
			//printf("return in preprocessing...3\n");
			return variables;			
		}
	} else if (sylvan_set_isempty(variables)) {
		//printf("return in preprocessing...4\n");
		return a;		
	}
	/* From now on, f and cube are non-constant. */	
	bddnode_t na = GETNODE(a);
    BDDVAR level = bddnode_getvariable(na);

    bddnode_t nv = GETNODE(variables);
    BDDVAR vv = bddnode_getvariable(nv);
	
	//printf("a level %i and cube level %i\n", level, vv);
	
	/* Abstract a variable that does not appear in f. */
    if (level > vv) {
		BDD _v = sylvan_set_next(variables);
        BDD res = CALL(sylvan_existsRepresentative, a, _v, level);
        if (res == sylvan_invalid) {
            return sylvan_invalid;
        }
        bdd_refs_push(res);
        
        BDD res1 = sylvan_makenode(vv, sylvan_false, res);

        if (res1 == sylvan_invalid) {
            bdd_refs_pop(1);
            return sylvan_invalid;
        }
        bdd_refs_pop(1);

		//printf("return after abstr. var that does not appear in f...\n");
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
            return sylvan_true;
        }
        bdd_refs_push(res1);
        
        BDD res2 = CALL(sylvan_existsRepresentative, aHigh, _v, level);
        if (res2 == sylvan_invalid) {
            bdd_refs_pop(1);
            return sylvan_invalid;
        }
        bdd_refs_push(res2);
        
        BDD left = CALL(sylvan_exists, aLow, _v, 0);
        if (left == sylvan_invalid) {
            bdd_refs_pop(2);
            return sylvan_invalid;
        }
		
        bdd_refs_push(left);

        BDD res1Inf = sylvan_ite(left, res1, sylvan_false);
        if (res1Inf == sylvan_invalid) {
            bdd_refs_pop(3);
            return sylvan_invalid;
        }
        bdd_refs_push(res1Inf);
        
        //Cudd_IterDerefBdd(manager,res1);

        BDD res2Inf = sylvan_ite(left, sylvan_false, res2);
        if (res2Inf == sylvan_invalid) {
            bdd_refs_pop(4);
            return sylvan_invalid;
        }
        bdd_refs_push(res2Inf);
        
        //Cudd_IterDerefBdd(manager,res2);
        //Cudd_IterDerefBdd(manager,left);
        
        assert(res1Inf != res2Inf);
        BDD res = sylvan_makenode(level, res2Inf, res1Inf);

        if (res == sylvan_invalid) {
            bdd_refs_pop(5);
            return sylvan_invalid;
        }

        // cuddCacheInsert2(manager, Cudd_bddExistAbstractRepresentative, f, cube, res);
		// TODO: CACHING HERE
		
		//printf("return properly computed result...\n");
		bdd_refs_pop(5);
        return res;
    } else { /* if (level == vv) */
        BDD res1 = CALL(sylvan_existsRepresentative, aLow, variables, level);
        if (res1 == sylvan_invalid){
            return sylvan_invalid;
        }
        bdd_refs_push(res1);
        
        BDD res2 = CALL(sylvan_existsRepresentative, aHigh, variables, level);
        if (res2 == sylvan_invalid) {
            bdd_refs_pop(1);
            return sylvan_invalid;
        }
        bdd_refs_push(res2);
        
        /* ITE takes care of possible complementation of res1 and of the
         ** case in which res1 == res2. */
		BDD res = sylvan_makenode(level, res2, res1);
        if (res == sylvan_invalid) {
            bdd_refs_pop(2);
            return sylvan_invalid;
        }
        
		bdd_refs_pop(2);
		//printf("return of last case...\n");
        return res;
    }
	
	// Prevent unused variable warning
	(void)prev_level;
}
