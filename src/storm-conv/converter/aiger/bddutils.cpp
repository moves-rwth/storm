extern "C" {
#include "aiger.h"
}


static inline unsigned var2lit(uint64_t var) {
    unsigned lit = (unsigned) ((var + 1) * 2);
    return lit;
}

static unsigned bdd2lit(sylvan::Bdd const& b, aiger* aig, unsigned& maxvar) {
    if (b.isOne())
        return 1;
    if (b.isZero())
        return 0;
    // otherwise we need to use Shannon expansion and build
    // subcircuits
    uint32_t idx = b.TopVar();
    sylvan::Bdd t = b.Then();
    sylvan::Bdd e = b.Else();
    unsigned tLit = bdd2lit(t, aig, maxvar);
    unsigned eLit = bdd2lit(e, aig, maxvar);
    // we have circuits for then, else, we need an and here
    unsigned thenCoFactor = var2lit(++maxvar);
    unsigned elseCoFactor = var2lit(++maxvar);
    aiger_add_and(aig, thenCoFactor, var2lit(idx), tLit);
    aiger_add_and(aig, elseCoFactor, aiger_not(var2lit(idx)), eLit);
    unsigned res = var2lit(++maxvar);
    aiger_add_and(aig, res, aiger_not(thenCoFactor), aiger_not(elseCoFactor));
    return aiger_not(res);
}
