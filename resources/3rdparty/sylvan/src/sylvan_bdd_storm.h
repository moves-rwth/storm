/* Do not include this file directly. Instead, include sylvan.h */

#ifndef SYLVAN_BDD_STORM_H
#define SYLVAN_BDD_STORM_H

#ifdef __cplusplus
extern "C" {
#endif

#define bdd_isnegated(dd) ((dd & sylvan_complement) ? 1 : 0)
#define bdd_regular(dd) (dd & ~sylvan_complement)
#define bdd_isterminal(dd) (dd == sylvan_false || dd == sylvan_true)

TASK_DECL_3(BDD, sylvan_existsRepresentative, BDD, BDD, BDDVAR);
#define sylvan_existsRepresentative(a, vars) (RUN(sylvan_existsRepresentative, a, vars, 0))


/*
 * The without operator as defined by Rauzy93
 * https://doi.org/10.1016/0951-8320(93)90060-C
 *
 * \note
 * f and g must be monotonic
 *
 * \return
 * f without paths that are included in a path in g
 */
TASK_DECL_2(BDD, sylvan_without, BDD, BDD);
#define sylvan_without(f, g) (RUN(sylvan_without, f, g))

/*
 * The minsol algorithm as defined by Rauzy93
 * https://doi.org/10.1016/0951-8320(93)90060-C
 *
 * \note
 * f must be monotonic
 *
 * \return
 * A bdd encoding the minmal solutions of f
 */
TASK_DECL_1(BDD, sylvan_minsol, BDD);
#define sylvan_minsol(f) (RUN(sylvan_minsol, f))

#ifdef __cplusplus
}
#endif

#endif
