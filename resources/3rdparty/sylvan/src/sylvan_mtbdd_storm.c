/**
 * Binary operation Times (for MTBDDs of same type)
 * Only for MTBDDs where either all leaves are Integer or Double.
 * If either operand is mtbdd_false (not defined),
 * then the result is mtbdd_false (i.e. not defined).
 */
TASK_IMPL_2(MTBDD, mtbdd_op_divide, MTBDD*, pa, MTBDD*, pb)
{
    MTBDD a = *pa, b = *pb;
    if (a == mtbdd_false || b == mtbdd_false) return mtbdd_false;
    
    // Do not handle Boolean MTBDDs...
    
    mtbddnode_t na = GETNODE(a);
    mtbddnode_t nb = GETNODE(b);
    
    if (mtbddnode_isleaf(na) && mtbddnode_isleaf(nb)) {
        uint64_t val_a = mtbddnode_getvalue(na);
        uint64_t val_b = mtbddnode_getvalue(nb);
        if (mtbddnode_gettype(na) == 0 && mtbddnode_gettype(nb) == 0) {
            int64_t va = *(int64_t*)(&val_a);
            int64_t vb = *(int64_t*)(&val_b);

            if (va == 0) return a;
            else if (vb == 0) return b;
            else {
                MTBDD result;
                if (va == 1) result = b;
                else if (vb == 1) result = a;
                else result = mtbdd_int64(va*vb);
                return result;
            }
        } else if (mtbddnode_gettype(na) == 1 && mtbddnode_gettype(nb) == 1) {
            // both double
            double vval_a = *(double*)&val_a;
            double vval_b = *(double*)&val_b;
            if (vval_a == 0.0) return a;
            else if (vval_b == 0.0) return b;
            else {
                MTBDD result;
                if (vval_a == 0.0 || vval_b == 1.0) result = a;
                result = mtbdd_double(vval_a / vval_b);
                return result;
            }
        }
        else if (mtbddnode_gettype(na) == 2 && mtbddnode_gettype(nb) == 2) {
            // both fraction
            uint64_t nom_a = val_a>>32;
            uint64_t nom_b = val_b>>32;
            uint64_t denom_a = val_a&0xffffffff;
            uint64_t denom_b = val_b&0xffffffff;
            // multiply!
            uint32_t c = gcd(denom_b, denom_a);
            uint32_t d = gcd(nom_a, nom_b);
            nom_a /= d;
            denom_a /= c;
            nom_a *= (denom_b/c);
            denom_a *= (nom_b/d);
            // compute result
            MTBDD result = mtbdd_fraction(nom_a, denom_a);
            return result;
        }
    }
    
    return mtbdd_invalid;
}

/**
 * Binary operation Equals (for MTBDDs of same type)
 * Only for MTBDDs where either all leaves are Boolean, or Integer, or Double.
 * For Integer/Double MTBDD, if either operand is mtbdd_false (not defined),
 * then the result is mtbdd_false (i.e. not defined).
 */
TASK_IMPL_2(MTBDD, mtbdd_op_equals, MTBDD*, pa, MTBDD*, pb)
{
    MTBDD a = *pa, b = *pb;
    if (a == mtbdd_false && b == mtbdd_false) return mtbdd_true;
    if (a == mtbdd_true && b == mtbdd_true) return mtbdd_true;
    
    mtbddnode_t na = GETNODE(a);
    mtbddnode_t nb = GETNODE(b);
    
    if (mtbddnode_isleaf(na) && mtbddnode_isleaf(nb)) {
        uint64_t val_a = mtbddnode_getvalue(na);
        uint64_t val_b = mtbddnode_getvalue(nb);
        if (mtbddnode_gettype(na) == 0 && mtbddnode_gettype(nb) == 0) {
            int64_t va = *(int64_t*)(&val_a);
            int64_t vb = *(int64_t*)(&val_b);
            if (va == vb) return mtbdd_true;
            return mtbdd_false;
        } else if (mtbddnode_gettype(na) == 1 && mtbddnode_gettype(nb) == 1) {
            // both double
            double vval_a = *(double*)&val_a;
            double vval_b = *(double*)&val_b;
            if (vval_a == vval_b) return mtbdd_true;
            return mtbdd_false;
        } else if (mtbddnode_gettype(na) == 2 && mtbddnode_gettype(nb) == 2) {
            // both fraction
            uint64_t nom_a = val_a>>32;
            uint64_t nom_b = val_b>>32;
            uint64_t denom_a = val_a&0xffffffff;
            uint64_t denom_b = val_b&0xffffffff;
            if (nom_a == nom_b && denom_a == denom_b) return mtbdd_true;
            return mtbdd_false;
        }
    }
    
    if (a < b) {
        *pa = b;
        *pb = a;
    }
    
    return mtbdd_invalid;
}

/**
 * Binary operation Equals (for MTBDDs of same type)
 * Only for MTBDDs where either all leaves are Boolean, or Integer, or Double.
 * For Integer/Double MTBDD, if either operand is mtbdd_false (not defined),
 * then the result is mtbdd_false (i.e. not defined).
 */
TASK_IMPL_2(MTBDD, mtbdd_op_less, MTBDD*, pa, MTBDD*, pb)
{
    MTBDD a = *pa, b = *pb;
    if (a == mtbdd_false && b == mtbdd_false) return mtbdd_true;
    if (a == mtbdd_true && b == mtbdd_true) return mtbdd_true;
    
    mtbddnode_t na = GETNODE(a);
    mtbddnode_t nb = GETNODE(b);
    
    if (mtbddnode_isleaf(na) && mtbddnode_isleaf(nb)) {
        uint64_t val_a = mtbddnode_getvalue(na);
        uint64_t val_b = mtbddnode_getvalue(nb);
        if (mtbddnode_gettype(na) == 0 && mtbddnode_gettype(nb) == 0) {
            int64_t va = *(int64_t*)(&val_a);
            int64_t vb = *(int64_t*)(&val_b);
            if (va < vb) return mtbdd_true;
            return mtbdd_false;
        } else if (mtbddnode_gettype(na) == 1 && mtbddnode_gettype(nb) == 1) {
            // both double
            double vval_a = *(double*)&val_a;
            double vval_b = *(double*)&val_b;
            if (vval_a < vval_b) return mtbdd_true;
            return mtbdd_false;
        } else if (mtbddnode_gettype(na) == 2 && mtbddnode_gettype(nb) == 2) {
            // both fraction
            uint64_t nom_a = val_a>>32;
            uint64_t nom_b = val_b>>32;
            uint64_t denom_a = val_a&0xffffffff;
            uint64_t denom_b = val_b&0xffffffff;
            return nom_a * denom_b < nom_b * denom_a ? mtbdd_true : mtbdd_false;
        }
    }
    
    return mtbdd_invalid;
}

/**
 * Binary operation Equals (for MTBDDs of same type)
 * Only for MTBDDs where either all leaves are Boolean, or Integer, or Double.
 * For Integer/Double MTBDD, if either operand is mtbdd_false (not defined),
 * then the result is mtbdd_false (i.e. not defined).
 */
TASK_IMPL_2(MTBDD, mtbdd_op_less_or_equal, MTBDD*, pa, MTBDD*, pb)
{
    MTBDD a = *pa, b = *pb;
    if (a == mtbdd_false && b == mtbdd_false) return mtbdd_true;
    if (a == mtbdd_true && b == mtbdd_true) return mtbdd_true;
    
    mtbddnode_t na = GETNODE(a);
    mtbddnode_t nb = GETNODE(b);
    
    if (mtbddnode_isleaf(na) && mtbddnode_isleaf(nb)) {
        uint64_t val_a = mtbddnode_getvalue(na);
        uint64_t val_b = mtbddnode_getvalue(nb);
        if (mtbddnode_gettype(na) == 0 && mtbddnode_gettype(nb) == 0) {
            int64_t va = *(int64_t*)(&val_a);
            int64_t vb = *(int64_t*)(&val_b);
            return va <= vb ? mtbdd_true : mtbdd_false;
        } else if (mtbddnode_gettype(na) == 1 && mtbddnode_gettype(nb) == 1) {
            // both double
            double vval_a = *(double*)&val_a;
            double vval_b = *(double*)&val_b;
            if (vval_a <= vval_b) return mtbdd_true;
            return mtbdd_false;
        } else if (mtbddnode_gettype(na) == 2 && mtbddnode_gettype(nb) == 2) {
            // both fraction
            uint64_t nom_a = val_a>>32;
            uint64_t nom_b = val_b>>32;
            uint64_t denom_a = val_a&0xffffffff;
            uint64_t denom_b = val_b&0xffffffff;
            nom_a *= denom_b;
            nom_b *= denom_a;
            return nom_a <= nom_b ? mtbdd_true : mtbdd_false;
        }
    }
    
    return mtbdd_invalid;
}

/**
 * Binary operation Pow (for MTBDDs of same type)
 * Only for MTBDDs where either all leaves are Double.
 * For Integer/Double MTBDD, if either operand is mtbdd_false (not defined),
 * then the result is mtbdd_false (i.e. not defined).
 */
TASK_IMPL_2(MTBDD, mtbdd_op_pow, MTBDD*, pa, MTBDD*, pb)
{
    MTBDD a = *pa, b = *pb;
    
    mtbddnode_t na = GETNODE(a);
    mtbddnode_t nb = GETNODE(b);
    
    if (mtbddnode_isleaf(na) && mtbddnode_isleaf(nb)) {
        uint64_t val_a = mtbddnode_getvalue(na);
        uint64_t val_b = mtbddnode_getvalue(nb);
        if (mtbddnode_gettype(na) == 0 && mtbddnode_gettype(nb) == 0) {
            assert(0);
        } else if (mtbddnode_gettype(na) == 1 && mtbddnode_gettype(nb) == 1) {
            // both double
            double vval_a = *(double*)&val_a;
            double vval_b = *(double*)&val_b;
            return mtbdd_double(pow(vval_a, vval_b));
        } else if (mtbddnode_gettype(na) == 2 && mtbddnode_gettype(nb) == 2) {
            assert(0);
        }
    }
    
    return mtbdd_invalid;
}

/**
 * Binary operation Mod (for MTBDDs of same type)
 * Only for MTBDDs where either all leaves are Double.
 * For Integer/Double MTBDD, if either operand is mtbdd_false (not defined),
 * then the result is mtbdd_false (i.e. not defined).
 */
TASK_IMPL_2(MTBDD, mtbdd_op_mod, MTBDD*, pa, MTBDD*, pb)
{
    MTBDD a = *pa, b = *pb;
    
    mtbddnode_t na = GETNODE(a);
    mtbddnode_t nb = GETNODE(b);
    
    if (mtbddnode_isleaf(na) && mtbddnode_isleaf(nb)) {
        uint64_t val_a = mtbddnode_getvalue(na);
        uint64_t val_b = mtbddnode_getvalue(nb);
        if (mtbddnode_gettype(na) == 0 && mtbddnode_gettype(nb) == 0) {
            assert(0);
        } else if (mtbddnode_gettype(na) == 1 && mtbddnode_gettype(nb) == 1) {
            // both double
            double vval_a = *(double*)&val_a;
            double vval_b = *(double*)&val_b;
            return mtbdd_double(fmod(vval_a, vval_b));
        } else if (mtbddnode_gettype(na) == 2 && mtbddnode_gettype(nb) == 2) {
            assert(0);
        }
    }
    
    return mtbdd_invalid;
}

/**
 * Binary operation Log (for MTBDDs of same type)
 * Only for MTBDDs where either all leaves are Double.
 * For Integer/Double MTBDD, if either operand is mtbdd_false (not defined),
 * then the result is mtbdd_false (i.e. not defined).
 */
TASK_IMPL_2(MTBDD, mtbdd_op_logxy, MTBDD*, pa, MTBDD*, pb)
{
    MTBDD a = *pa, b = *pb;
    
    mtbddnode_t na = GETNODE(a);
    mtbddnode_t nb = GETNODE(b);
    
    if (mtbddnode_isleaf(na) && mtbddnode_isleaf(nb)) {
        uint64_t val_a = mtbddnode_getvalue(na);
        uint64_t val_b = mtbddnode_getvalue(nb);
        if (mtbddnode_gettype(na) == 0 && mtbddnode_gettype(nb) == 0) {
            assert(0);
        } else if (mtbddnode_gettype(na) == 1 && mtbddnode_gettype(nb) == 1) {
            // both double
            double vval_a = *(double*)&val_a;
            double vval_b = *(double*)&val_b;
            return mtbdd_double(log(vval_a) / log(vval_b));
        } else if (mtbddnode_gettype(na) == 2 && mtbddnode_gettype(nb) == 2) {
            assert(0);
        }
    }
    
    return mtbdd_invalid;
}

TASK_IMPL_2(MTBDD, mtbdd_op_not_zero, MTBDD, a, size_t, v)
{
    /* We only expect "double" terminals, or false */
    if (a == mtbdd_false) return mtbdd_false;
    if (a == mtbdd_true) return mtbdd_true;
    
    // a != constant
    mtbddnode_t na = GETNODE(a);
    
    if (mtbddnode_isleaf(na)) {
        if (mtbddnode_gettype(na) == 0) {
            return mtbdd_getint64(a) != 0 ? mtbdd_true : mtbdd_false;
        } else if (mtbddnode_gettype(na) == 1) {
            return mtbdd_getdouble(a) != 0.0 ? mtbdd_true : mtbdd_false;
        } else if (mtbddnode_gettype(na) == 2) {
            return mtbdd_getnumer(a) != 0 ? mtbdd_true : mtbdd_false;
        }
    }
    
    // Ugly hack to get rid of the error "unused variable v" (because there is no version of uapply without a parameter).
    (void)v;
    
    return mtbdd_invalid;
}

TASK_IMPL_1(MTBDD, mtbdd_not_zero, MTBDD, dd)
{
    return mtbdd_uapply(dd, TASK(mtbdd_op_not_zero), 0);
}

TASK_IMPL_2(MTBDD, mtbdd_op_floor, MTBDD, a, size_t, v)
{
    /* We only expect "double" terminals, or false */
    if (a == mtbdd_false) return mtbdd_false;
    if (a == mtbdd_true) return mtbdd_true;
    
    // a != constant
    mtbddnode_t na = GETNODE(a);
    
    if (mtbddnode_isleaf(na)) {
        if (mtbddnode_gettype(na) == 0) {
            return a;
        } else if (mtbddnode_gettype(na) == 1) {
            MTBDD result = mtbdd_double(floor(mtbdd_getdouble(a)));
            return result;
        } else if (mtbddnode_gettype(na) == 2) {
            MTBDD result = mtbdd_fraction(mtbdd_getnumer(a) / mtbdd_getdenom(a), 1);
            return result;
        }
    }
    
    // Ugly hack to get rid of the error "unused variable v" (because there is no version of uapply without a parameter).
    (void)v;
    
    return mtbdd_invalid;
}

TASK_IMPL_1(MTBDD, mtbdd_floor, MTBDD, dd)
{
    return mtbdd_uapply(dd, TASK(mtbdd_op_floor), 0);
}

TASK_IMPL_2(MTBDD, mtbdd_op_ceil, MTBDD, a, size_t, v)
{
    /* We only expect "double" terminals, or false */
    if (a == mtbdd_false) return mtbdd_false;
    if (a == mtbdd_true) return mtbdd_true;
    
    // a != constant
    mtbddnode_t na = GETNODE(a);
    
    if (mtbddnode_isleaf(na)) {
        if (mtbddnode_gettype(na) == 0) {
            return a;
        } else if (mtbddnode_gettype(na) == 1) {
            MTBDD result = mtbdd_double(ceil(mtbdd_getdouble(a)));
            return result;
        } else if (mtbddnode_gettype(na) == 2) {
            MTBDD result = mtbdd_fraction(mtbdd_getnumer(a) / mtbdd_getdenom(a) + 1, 1);
            return result;
        }
    }

    // Ugly hack to get rid of the error "unused variable v" (because there is no version of uapply without a parameter).
    (void)v;
    
    return mtbdd_invalid;
}

TASK_IMPL_1(MTBDD, mtbdd_ceil, MTBDD, dd)
{
    return mtbdd_uapply(dd, TASK(mtbdd_op_ceil), 0);
}

TASK_IMPL_2(MTBDD, mtbdd_op_bool_to_double, MTBDD, a, size_t, v)
{
    /* We only expect "double" terminals, or false */
    if (a == mtbdd_false) return mtbdd_double(0);
    if (a == mtbdd_true) return mtbdd_double(1.0);
    
    // Ugly hack to get rid of the error "unused variable v" (because there is no version of uapply without a parameter).
    (void)v;
    
    return mtbdd_invalid;
}

TASK_IMPL_1(MTBDD, mtbdd_bool_to_double, MTBDD, dd)
{
    return mtbdd_uapply(dd, TASK(mtbdd_op_bool_to_double), 0);
}

TASK_IMPL_2(MTBDD, mtbdd_op_bool_to_int64, MTBDD, a, size_t, v)
{
    /* We only expect "double" terminals, or false */
    if (a == mtbdd_false) return mtbdd_int64(0);
    if (a == mtbdd_true) return mtbdd_int64(1);
    
    // Ugly hack to get rid of the error "unused variable v" (because there is no version of uapply without a parameter).
    (void)v;
    
    return mtbdd_invalid;
}

TASK_IMPL_1(MTBDD, mtbdd_bool_to_int64, MTBDD, dd)
{
    return mtbdd_uapply(dd, TASK(mtbdd_op_bool_to_int64), 0);
}

/**
 * Calculate the number of satisfying variable assignments according to <variables>.
 */
TASK_IMPL_2(double, mtbdd_non_zero_count, MTBDD, dd, size_t, nvars)
{
    /* Trivial cases */
    if (dd == mtbdd_false) return 0.0;

    mtbddnode_t na = GETNODE(dd);
    
    if (mtbdd_isleaf(dd)) {
        if (mtbddnode_gettype(na) == 0) {
            return mtbdd_getint64(dd) != 0 ? powl(2.0L, nvars) : 0.0;
        } else if (mtbddnode_gettype(na) == 1) {
            return mtbdd_getdouble(dd) != 0 ? powl(2.0L, nvars) : 0.0;
        } else if (mtbddnode_gettype(na) == 2) {
            return mtbdd_getnumer(dd) != 0 ? powl(2.0L, nvars) : 0.0;
        }
    }
    
    /* Perhaps execute garbage collection */
    sylvan_gc_test();
    
    union {
        double d;
        uint64_t s;
    } hack;
    
    /* Consult cache */
    if (cache_get3(CACHE_MTBDD_NONZERO_COUNT, dd, 0, nvars, &hack.s)) {
        sylvan_stats_count(CACHE_MTBDD_NONZERO_COUNT);
        return hack.d;
    }
    
    SPAWN(mtbdd_non_zero_count, mtbdd_gethigh(dd), nvars-1);
    double low = CALL(mtbdd_non_zero_count, mtbdd_getlow(dd), nvars-1);
    hack.d = low + SYNC(mtbdd_non_zero_count);
    
    cache_put3(CACHE_MTBDD_NONZERO_COUNT, dd, 0, nvars, hack.s);
    return hack.d;
}

int mtbdd_iszero(MTBDD dd) {
    if (mtbdd_gettype(dd) == 0) {
        return mtbdd_getint64(dd) == 0;
    } else if (mtbdd_gettype(dd) == 1) {
        return mtbdd_getdouble(dd) == 0;
    } else if (mtbdd_gettype(dd) == 2) {
        return mtbdd_getnumer(dd) == 0;
    }
    return 0;
}

int mtbdd_isnonzero(MTBDD dd) {
    return mtbdd_iszero(dd) ? 0 : 1;
}