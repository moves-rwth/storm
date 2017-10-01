#ifdef __cplusplus
extern "C" {
#endif
    
#define bdd_isnegated(dd) ((dd & sylvan_complement) ? 1 : 0)
#define bdd_regular(dd) (dd & ~sylvan_complement)
#define bdd_isterminal(dd) (dd == sylvan_false || dd == sylvan_true)

TASK_DECL_3(BDD, sylvan_existsRepresentative, BDD, BDD, BDDVAR);
#define sylvan_existsRepresentative(a, vars) (CALL(sylvan_existsRepresentative, a, vars, 0))

#ifdef __cplusplus
}
#endif
