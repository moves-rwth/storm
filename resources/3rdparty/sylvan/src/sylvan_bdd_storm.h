#define bdd_isnegated(dd) ((dd & sylvan_complement) ? 1 : 0)
#define bdd_regular(dd) (dd & ~sylvan_complement)
#define bdd_isterminal(dd) (dd == sylvan_false || dd == sylvan_true)