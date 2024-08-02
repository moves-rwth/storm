#pragma once
#include "storm-config.h"

#ifdef STORM_HAVE_SPOT
#if defined(__clang__)
#pragma clang diagnostic push
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#endif

#include "spot/tl/formula.hh"
#include "spot/tl/parse.hh"
#include "spot/twaalgos/dot.hh"
#include "spot/twaalgos/hoa.hh"
#include "spot/twaalgos/totgba.hh"
#include "spot/twaalgos/translate.hh"

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif
#endif
