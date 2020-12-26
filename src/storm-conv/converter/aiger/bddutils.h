#pragma once

#include <cstdint>

#include "storm/adapters/sylvan.h"

extern "C" {
#include "aiger.h"
}


inline unsigned var2lit(uint64_t var) {
    unsigned lit = (unsigned) ((var + 1) * 2);
    return lit;
}


unsigned bdd2lit(sylvan::Bdd const&, aiger*, unsigned&);
