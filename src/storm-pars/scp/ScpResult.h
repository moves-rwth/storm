#pragma once

#include <cstdint>
#include <map>

#include "storm/adapters/RationalFunctionForward.h"
#include "storm/utility/ExtendedNumber.h"

namespace storm::pars::scp {

enum class ScpStatus { Feasible, Infeasible, MaxIterationsReached, Timeout };

/*!
 * The outcome of an SCP search at ConstantType precision (double or storm::RationalNumber).
 * `value` is held in storm::utility::ExtendedValueType<ConstantType> so that an infinite result (a
 * reward property whose initial state has infinite expected reward) is represented exactly.
 *
 * `valuation` and `value` describe the best point found, whatever the status. Only for
 * status == Feasible has `valuation` been verified by exactly model-checking the instantiated
 * model; for the other statuses it is a best-effort point.
 */
template<typename ConstantType>
struct ScpResult {
    ScpStatus status;
    std::map<storm::RationalFunctionVariable, storm::RationalFunctionCoefficient> valuation;
    storm::utility::ExtendedValueType<ConstantType> value;
    /// The iteration at which (`valuation`, `value`) was established; 0 for the initial point.
    uint64_t iterations;
};

}  // namespace storm::pars::scp
