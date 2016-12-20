/**
 * This is an implementation of storm::RationalFunction custom leaves of MTBDDs
 */

#ifndef SYLVAN_STORM_RATIONAL_FUNCTION_H
#define SYLVAN_STORM_RATIONAL_FUNCTION_H

#include <sylvan.h>
#include <storm_function_wrapper.h>

#define SYLVAN_HAVE_CARL 1

#if defined(SYLVAN_HAVE_CARL) || defined(STORM_HAVE_CARL)

#define SYLVAN_STORM_RATIONAL_FUNCTION_TYPE_ID (3)

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * Initialize storm::RationalFunction custom leaves
 */
void sylvan_storm_rational_function_init();

/** 
 * Returns the identifier necessary to use these custom leaves.
 */
uint32_t sylvan_storm_rational_function_get_type();

/**
 * Create storm::RationalFunction leaf
 */
MTBDD mtbdd_storm_rational_function(storm_rational_function_ptr val);

/**
 * Monad that converts Boolean to a storm::RationalFunction MTBDD, translate terminals true to 1 and to 0 otherwise;
 */
TASK_DECL_2(MTBDD, mtbdd_op_bool_to_storm_rational_function, MTBDD, size_t)
TASK_DECL_1(MTBDD, mtbdd_bool_to_storm_rational_function, MTBDD)
#define mtbdd_bool_to_storm_rational_function(dd) CALL(mtbdd_bool_to_storm_rational_function, dd)

/**
 * Operation "plus" for two storm::RationalFunction MTBDDs
 */
TASK_DECL_2(MTBDD, sylvan_storm_rational_function_op_plus, MTBDD*, MTBDD*)
TASK_DECL_3(MTBDD, sylvan_storm_rational_function_abstract_op_plus, MTBDD, MTBDD, int)

/**
 * Operation "minus" for two storm::RationalFunction MTBDDs
 */
TASK_DECL_2(MTBDD, sylvan_storm_rational_function_op_minus, MTBDD*, MTBDD*)

/**
 * Operation "times" for two storm::RationalFunction MTBDDs
 */
TASK_DECL_2(MTBDD, sylvan_storm_rational_function_op_times, MTBDD*, MTBDD*)
TASK_DECL_3(MTBDD, sylvan_storm_rational_function_abstract_op_times, MTBDD, MTBDD, int)

/**
 * Operation "divide" for two storm::RationalFunction MTBDDs
 */
TASK_DECL_2(MTBDD, sylvan_storm_rational_function_op_divide, MTBDD*, MTBDD*)

/**
 * Operation "negate" for one storm::RationalFunction MTBDD
 */
TASK_DECL_2(MTBDD, sylvan_storm_rational_function_op_neg, MTBDD, size_t)

/**
 * Compute a + b
 */
#define sylvan_storm_rational_function_plus(a, b) mtbdd_apply(a, b, TASK(sylvan_storm_rational_function_op_plus))

/**
 * Compute a - b
 */
#define sylvan_storm_rational_function_minus(a, b) mtbdd_apply(a, b, TASK(sylvan_storm_rational_function_op_minus))

/**
 * Compute a * b
 */
#define sylvan_storm_rational_function_times(a, b) mtbdd_apply(a, b, TASK(sylvan_storm_rational_function_op_times))

/**
 * Compute a / b
 */
#define sylvan_storm_rational_function_divide(a, b) mtbdd_apply(a, b, TASK(sylvan_storm_rational_function_op_divide))

/**
 * Compute -a
 */
#define sylvan_storm_rational_function_neg(a) mtbdd_uapply(a, TASK(sylvan_storm_rational_function_op_neg), 0)

/**
 * Multiply <a> and <b>, and abstract variables <vars> using summation.
 * This is similar to the "and_exists" operation in BDDs.
 */
TASK_DECL_3(MTBDD, sylvan_storm_rational_function_and_exists, MTBDD, MTBDD, MTBDD);
#define sylvan_storm_rational_function_and_exists(a, b, vars) CALL(sylvan_storm_rational_function_and_exists, a, b, vars)

/**
 * Abstract the variables in <v> from <a> by taking the sum of all values
 */
#define sylvan_storm_rational_function_abstract_plus(dd, v) mtbdd_abstract(dd, v, TASK(sylvan_storm_rational_function_abstract_op_plus))

/**
 * Apply a unary operation <op> to <dd>.
 * Callback <op> is consulted after the cache, thus the application to a terminal is cached.
 */
TASK_DECL_3(MTBDD, mtbdd_uapply_nocache, MTBDD, mtbdd_uapply_op, size_t);
#define mtbdd_uapply_nocache(dd, op, param) CALL(mtbdd_uapply_nocache, dd, op, param)

/**
 * Functionality regarding the replacement of leaves in MTBDDs.
 */

/**
 * Operation "replace" for one storm::RationalFunction MTBDD
 */
TASK_DECL_2(MTBDD, sylvan_storm_rational_function_op_replace_leaves, MTBDD, size_t)

/**
 * Compute the MTBDD that arises from a after calling the mtbddLeaveReplacementFunction on each leaf.
 */
#define sylvan_storm_rational_function_replace_leaves(a, ctx) mtbdd_uapply_nocache(a, TASK(sylvan_storm_rational_function_op_replace_leaves), ctx)

/**
 * Takes a storm::RationalFunction MTBDD and transforms it into a double MTBDD
 */
TASK_DECL_2(MTBDD, sylvan_storm_rational_function_op_to_double, MTBDD, size_t)

/**
 * Compute the MTBDD that arises from a after calling the mtbddLeaveReplacementFunction on each leaf.
 */
#define sylvan_storm_rational_function_to_double(a) mtbdd_uapply_nocache(a, TASK(sylvan_storm_rational_function_op_to_double), 0)

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // SYLVAN_HAVE_CARL

#endif // SYLVAN_STORM_RATIONAL_FUNCTION_H
