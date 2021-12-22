#include "storm/utility/VectorHelper.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/CoreSettings.h"

#include "storm/adapters/IntelTbbAdapter.h"
#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/adapters/RationalNumberAdapter.h"

#include "storm/utility/vector.h"

#include "storm/exceptions/InvalidSettingsException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/utility/macros.h"

namespace storm {
namespace utility {

template<typename ValueType>
VectorHelper<ValueType>::VectorHelper() : doParallelize(storm::settings::getModule<storm::settings::modules::CoreSettings>().isUseIntelTbbSet()) {
#ifndef STORM_HAVE_INTELTBB
    STORM_LOG_THROW(!doParallelize, storm::exceptions::InvalidSettingsException, "Cannot parallelize without TBB.");
#endif
}

template<typename ValueType>
bool VectorHelper<ValueType>::parallelize() const {
    return doParallelize;
}

template<typename ValueType>
void VectorHelper<ValueType>::reduceVector(storm::solver::OptimizationDirection dir, std::vector<ValueType> const& source, std::vector<ValueType>& target,
                                           std::vector<uint_fast64_t> const& rowGrouping, std::vector<uint_fast64_t>* choices) const {
#ifdef STORM_HAVE_INTELTBB
    if (this->parallelize()) {
        storm::utility::vector::reduceVectorMinOrMaxParallel(dir, source, target, rowGrouping, choices);
    } else {
        storm::utility::vector::reduceVectorMinOrMax(dir, source, target, rowGrouping, choices);
    }
#else
    storm::utility::vector::reduceVectorMinOrMax(dir, source, target, rowGrouping, choices);
#endif
}

template<>
void VectorHelper<storm::RationalFunction>::reduceVector(storm::solver::OptimizationDirection, std::vector<storm::RationalFunction> const&,
                                                         std::vector<storm::RationalFunction>&, std::vector<uint_fast64_t> const&,
                                                         std::vector<uint_fast64_t>*) const {
    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "This operation is not supported.");
}

template class VectorHelper<double>;

#ifdef STORM_HAVE_CARL
template class VectorHelper<storm::RationalNumber>;
template class VectorHelper<storm::RationalFunction>;
#endif
}  // namespace utility
}  // namespace storm
