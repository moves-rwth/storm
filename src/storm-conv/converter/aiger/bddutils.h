#pragma once

extern "C" {
#include "aiger.h"
}


static inline unsigned var2lit(uint64_t);

static unsigned bdd2lit(sylvan::Bdd const&, aiger*, unsigned&);
