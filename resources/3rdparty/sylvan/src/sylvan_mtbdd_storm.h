/**
 * Compute a - b
 */
#define mtbdd_minus(a, b) mtbdd_plus(a, mtbdd_negate(b))

/**
 * Binary operation Divide (for MTBDDs of same type)
 * Only for MTBDDs where all leaves are Integer or Double.
 * If either operand is mtbdd_false (not defined),
 * then the result is mtbdd_false (i.e. not defined).
 */
TASK_DECL_2(MTBDD, mtbdd_op_divide, MTBDD*, MTBDD*);
#define mtbdd_divide(a, b) mtbdd_apply(a, b, TASK(mtbdd_op_divide))

/**
 * Binary operation equals (for MTBDDs of same type)
 * Only for MTBDDs where either all leaves are Boolean, or Integer, or Double.
 * For Integer/Double MTBDD, if either operand is mtbdd_false (not defined),
 * then the result is the other operand.
 */
TASK_DECL_2(MTBDD, mtbdd_op_equals, MTBDD*, MTBDD*);
#define mtbdd_equals(a, b) mtbdd_apply(a, b, TASK(mtbdd_op_equals))

/**
 * Binary operation Less (for MTBDDs of same type)
 * Only for MTBDDs where either all leaves are Boolean, or Integer, or Double.
 * For Integer/Double MTBDD, if either operand is mtbdd_false (not defined),
 * then the result is the other operand.
 */
TASK_DECL_2(MTBDD, mtbdd_op_less, MTBDD*, MTBDD*);
#define mtbdd_less_as_bdd(a, b) mtbdd_apply(a, b, TASK(mtbdd_op_less))

/**
 * Binary operation Less (for MTBDDs of same type)
 * Only for MTBDDs where either all leaves are Boolean, or Integer, or Double.
 * For Integer/Double MTBDD, if either operand is mtbdd_false (not defined),
 * then the result is the other operand.
 */
TASK_DECL_2(MTBDD, mtbdd_op_less_or_equal, MTBDD*, MTBDD*);
#define mtbdd_less_or_equal_as_bdd(a, b) mtbdd_apply(a, b, TASK(mtbdd_op_less_or_equal))

/**
 * Binary operation Pow (for MTBDDs of same type)
 * Only for MTBDDs where either all leaves are Integer, Double or a Fraction.
 * For Integer/Double MTBDD, if either operand is mtbdd_false (not defined),
 * then the result is the other operand.
 */
TASK_DECL_2(MTBDD, mtbdd_op_pow, MTBDD*, MTBDD*);
#define mtbdd_pow(a, b) mtbdd_apply(a, b, TASK(mtbdd_op_pow))

/**
 * Binary operation Mod (for MTBDDs of same type)
 * Only for MTBDDs where either all leaves are Integer, Double or a Fraction.
 * For Integer/Double MTBDD, if either operand is mtbdd_false (not defined),
 * then the result is the other operand.
 */
TASK_DECL_2(MTBDD, mtbdd_op_mod, MTBDD*, MTBDD*);
#define mtbdd_mod(a, b) mtbdd_apply(a, b, TASK(mtbdd_op_mod))

/**
 * Binary operation Log (for MTBDDs of same type)
 * Only for MTBDDs where either all leaves are Double or a Fraction.
 * For Integer/Double MTBDD, if either operand is mtbdd_false (not defined),
 * then the result is the other operand.
 */
TASK_DECL_2(MTBDD, mtbdd_op_logxy, MTBDD*, MTBDD*);
#define mtbdd_logxy(a, b) mtbdd_apply(a, b, TASK(mtbdd_op_logxy))

/**
 * Monad that converts double to a Boolean MTBDD, translate terminals != 0 to 1 and to 0 otherwise;
 */
TASK_DECL_2(MTBDD, mtbdd_op_not_zero, MTBDD, size_t)
TASK_DECL_1(MTBDD, mtbdd_not_zero, MTBDD)
#define mtbdd_not_zero(dd) CALL(mtbdd_not_zero, dd)

/**
 * Monad that floors all values Double and Fraction values.
 */
TASK_DECL_2(MTBDD, mtbdd_op_floor, MTBDD, size_t)
TASK_DECL_1(MTBDD, mtbdd_floor, MTBDD)
#define mtbdd_floor(dd) CALL(mtbdd_floor, dd)

/**
 * Monad that ceils all values Double and Fraction values.
 */
TASK_DECL_2(MTBDD, mtbdd_op_ceil, MTBDD, size_t)
TASK_DECL_1(MTBDD, mtbdd_ceil, MTBDD)
#define mtbdd_ceil(dd) CALL(mtbdd_ceil, dd)

/**
 * Monad that converts Boolean to a Double MTBDD, translate terminals true to 1 and to 0 otherwise;
 */
TASK_DECL_2(MTBDD, mtbdd_op_bool_to_double, MTBDD, size_t)
TASK_DECL_1(MTBDD, mtbdd_bool_to_double, MTBDD)
#define mtbdd_bool_to_double(dd) CALL(mtbdd_bool_to_double, dd)

/**
 * Monad that converts Boolean to a uint MTBDD, translate terminals true to 1 and to 0 otherwise;
 */
TASK_DECL_2(MTBDD, mtbdd_op_bool_to_uint64, MTBDD, size_t)
TASK_DECL_1(MTBDD, mtbdd_bool_to_uint64, MTBDD)
#define mtbdd_bool_to_uint64(dd) CALL(mtbdd_bool_to_uint64, dd)

/**
 * Count the number of assignments (minterms) leading to a non-zero
 */
TASK_DECL_2(double, mtbdd_non_zero_count, MTBDD, size_t);
#define mtbdd_non_zero_count(dd, nvars) CALL(mtbdd_non_zero_count, dd, nvars)

// Checks whether the given MTBDD represents a zero leaf.
int mtbdd_iszero(MTBDD);

#define mtbdd_regular(dd) (dd & ~mtbdd_complement)
