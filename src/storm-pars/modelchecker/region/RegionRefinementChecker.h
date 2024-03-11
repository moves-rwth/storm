#pragma once

#include <memory>

#include "storm-pars/modelchecker/region/RegionModelChecker.h"
#include "storm-pars/modelchecker/region/RegionResult.h"
#include "storm-pars/modelchecker/region/RegionResultHypothesis.h"
#include "storm-pars/modelchecker/region/RegionSplittingStrategy.h"
#include "storm-pars/modelchecker/region/monotonicity/MonotonicityBackend.h"
#include "storm-pars/modelchecker/results/RegionCheckResult.h"
#include "storm-pars/modelchecker/results/RegionRefinementCheckResult.h"
#include "storm-pars/storage/ParameterRegion.h"

#include "storm/modelchecker/CheckTask.h"
#include "storm/models/ModelBase.h"

namespace storm {

class Environment;

namespace modelchecker {

template<typename ParametricType>
class RegionRefinementChecker {
   public:
    using CoefficientType = typename storm::storage::ParameterRegion<ParametricType>::CoefficientType;
    using VariableType = typename storm::storage::ParameterRegion<ParametricType>::VariableType;
    using Valuation = typename storm::storage::ParameterRegion<ParametricType>::Valuation;

    RegionRefinementChecker(RegionModelChecker<ParametricType>&& regionChecker);
    ~RegionRefinementChecker() = default;

    bool canHandle(std::shared_ptr<storm::models::ModelBase> parametricModel, CheckTask<storm::logic::Formula, ParametricType> const& checkTask) const;

    void specify(Environment const& env, std::shared_ptr<storm::models::ModelBase> parametricModel,
                 CheckTask<storm::logic::Formula, ParametricType> const& checkTask, std::shared_ptr<RegionSplittingStrategy> splittingStrategy = {},
                 std::shared_ptr<MonotonicityBackend<ParametricType>> monotonicityBackend = {}, bool allowModelSimplifications = true);

    /*!
     * Iteratively refines the region until the region analysis yields a conclusive result (AllSat or AllViolated).
     * @param region the considered region
     * @param coverageThreshold if given, the refinement stops as soon as the fraction of the area of the subregions with inconclusive result is less then this
     * threshold
     * @param depthThreshold if given, the refinement stops at the given depth. depth=0 means no refinement.
     * @param hypothesis if not 'unknown', it is only checked whether the hypothesis holds within the given region.
     * @param monThresh if given, determines at which depth to start using monotonicity
     */
    std::unique_ptr<storm::modelchecker::RegionRefinementCheckResult<ParametricType>> performRegionPartitioning(
        Environment const& env, storm::storage::ParameterRegion<ParametricType> const& region, std::optional<ParametricType> coverageThreshold,
        std::optional<uint64_t> depthThreshold = std::nullopt, RegionResultHypothesis const& hypothesis = RegionResultHypothesis::Unknown,
        uint64_t monThresh = 0);

    // TODO return type is not quite nice
    // TODO consider returning v' as well
    /*!
     * Finds the extremal value within the given region and with the given precision.
     * The returned value v corresponds to the value at the returned valuation.
     * The actual maximum (minimum) lies in the interval [v, v'] ([v', v])
     * where v' is based on the precision. (With absolute precision, v' = v +/- precision).
     * TODO: Check documentation, which was incomplete.
     *
     * @param env
     * @param region The region in which to optimize
     * @param dir The direction in which to optimize
     * @param precision The required precision (unless boundInvariant is set).
     * @param absolutePrecision true iff precision should be measured absolutely
     * @param boundInvariant if this invariant on v is violated, the algorithm may return v while violating the precision requirements.
     * @return
     */
    std::pair<CoefficientType, Valuation> computeExtremalValue(Environment const& env, storm::storage::ParameterRegion<ParametricType> const& region,
                                                               storm::solver::OptimizationDirection const& dir, CoefficientType const& precision,
                                                               bool absolutePrecision, std::optional<storm::logic::Bound> const& boundInvariant = std::nullopt);

    /*!
     * Checks whether the bound is satisfied on the complete region.
     * @return
     */
    bool verifyRegion(Environment const& env, storm::storage::ParameterRegion<ParametricType> const& region, storm::logic::Bound const& bound);

   private:
    enum class Context { Partitioning, ExtremalValue };
    std::set<VariableType> getSplittingVariables(detail::AnnotatedRegion<ParametricType> const& region, Context context) const;

    std::unique_ptr<RegionModelChecker<ParametricType>> regionChecker;
    std::shared_ptr<MonotonicityBackend<ParametricType>> monotonicityBackend;
    std::shared_ptr<RegionSplittingStrategy> regionSplittingStrategy;
};

}  // namespace modelchecker
}  // namespace storm
