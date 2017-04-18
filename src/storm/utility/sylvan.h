#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wextra-semi"
#pragma clang diagnostic ignored "-Wzero-length-array"
#pragma clang diagnostic ignored "-Wgnu-zero-variadic-macro-arguments"
#pragma clang diagnostic ignored "-Wdeprecated-register"
#pragma clang diagnostic ignored "-Wc99-extensions"
#pragma clang diagnostic ignored "-Wunknown-pragmas"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"

#pragma GCC system_header // Only way to suppress some warnings atm.

#include "sylvan_obj.hpp"
#include "sylvan_storm_rational_number.h"
#include "sylvan_storm_rational_function.h"

#pragma GCC diagnostic pop
#pragma clang diagnostic pop
