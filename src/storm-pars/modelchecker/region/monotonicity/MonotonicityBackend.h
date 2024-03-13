#pragma once

#include <set>
#include "storm-pars/analysis/MonotonicityResult.h"
#include "storm-pars/modelchecker/region/detail/AnnotatedRegion.h"
#include "storm-pars/storage/ParameterRegion.h"

namespace storm::modelchecker {

template<typename ParametricType>
class MonotonicityBackend {
   public:
    typedef typename storm::utility::parametric::VariableType<ParametricType>::type VariableType;
    typedef typename storm::analysis::MonotonicityResult<VariableType>::Monotonicity Monotonicity;

    virtual bool supportsLocalMonotonicity() const;

    /*!
     * Sets parameters that are assumed to be monotone throughout the analysis.
     * Previously specified parameters are overwritten.
     * @param monotoneIncreasingParameters
     * @param monotoneDecreasingParameters
     */
    void setMonotoneParameters(std::set<VariableType> const& monotoneIncreasingParameters, std::set<VariableType> const& monotoneDecreasingParameters);

    virtual std::map<VariableType, Monotonicity> computeGlobalMonotonicityInRegion(storm::storage::ParameterRegion<ParametricType> const& region);

    virtual void initializeMonotonicity(detail::AnnotatedRegion<ParametricType> const& region);

    virtual void updateMonotonicity(detail::AnnotatedRegion<ParametricType> const& region);

    virtual bool interactsWithRegionModelChecker() const;

   private:
    std::set<VariableType> increasingParameters;
    std::set<VariableType> decreasingParameters;

    uint64_t numberOfRegionsKnownThroughMonotonicity;
};

}  // namespace storm::modelchecker