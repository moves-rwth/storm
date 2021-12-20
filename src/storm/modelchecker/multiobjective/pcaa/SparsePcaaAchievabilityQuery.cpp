#include "storm/modelchecker/multiobjective/pcaa/SparsePcaaAchievabilityQuery.h"

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/environment/modelchecker/MultiObjectiveModelCheckerEnvironment.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/utility/SignalHandler.h"
#include "storm/utility/constants.h"
#include "storm/utility/vector.h"

#include "storm/exceptions/InvalidOperationException.h"

namespace storm {
namespace modelchecker {
namespace multiobjective {

template<class SparseModelType, typename GeometryValueType>
SparsePcaaAchievabilityQuery<SparseModelType, GeometryValueType>::SparsePcaaAchievabilityQuery(
    preprocessing::SparseMultiObjectivePreprocessorResult<SparseModelType>& preprocessorResult)
    : SparsePcaaQuery<SparseModelType, GeometryValueType>(preprocessorResult) {
    STORM_LOG_ASSERT(preprocessorResult.queryType == preprocessing::SparseMultiObjectivePreprocessorResult<SparseModelType>::QueryType::Achievability,
                     "Invalid query Type");
    initializeThresholdData();

    // Set the precision of the weight vector checker. Will be refined during the computation
    this->weightVectorChecker->setWeightedPrecision(storm::utility::convertNumber<typename SparseModelType::ValueType>(0.1));
}

template<class SparseModelType, typename GeometryValueType>
void SparsePcaaAchievabilityQuery<SparseModelType, GeometryValueType>::initializeThresholdData() {
    thresholds.reserve(this->objectives.size());
    strictThresholds = storm::storage::BitVector(this->objectives.size(), false);
    for (uint_fast64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
        auto const& formula = *this->objectives[objIndex].formula;
        STORM_LOG_ASSERT(formula.hasBound(), "Achievability query invoked but there is an objective without bound.");
        thresholds.push_back(formula.template getThresholdAs<GeometryValueType>());
        if (storm::solver::minimize(formula.getOptimalityType())) {
            STORM_LOG_ASSERT(!storm::logic::isLowerBound(formula.getBound().comparisonType), "Minimizing objective should not specify an upper bound.");
            // Values for minimizing objectives will be negated in order to convert them to maximizing objectives.
            // Hence, we also negate the threshold
            thresholds.back() *= -storm::utility::one<GeometryValueType>();
        }
        strictThresholds.set(objIndex, storm::logic::isStrict(formula.getBound().comparisonType));
    }
}

template<class SparseModelType, typename GeometryValueType>
std::unique_ptr<CheckResult> SparsePcaaAchievabilityQuery<SparseModelType, GeometryValueType>::check(Environment const& env) {
    bool result = this->checkAchievability(env);

    return std::unique_ptr<CheckResult>(new ExplicitQualitativeCheckResult(this->originalModel.getInitialStates().getNextSetIndex(0), result));
}

template<class SparseModelType, typename GeometryValueType>
bool SparsePcaaAchievabilityQuery<SparseModelType, GeometryValueType>::checkAchievability(Environment const& env) {
    // repeatedly refine the over/ under approximation until the threshold point is either in the under approx. or not in the over approx.
    while (!this->maxStepsPerformed(env) && !storm::utility::resources::isTerminate()) {
        WeightVector separatingVector = this->findSeparatingVector(thresholds);
        this->updateWeightedPrecision(separatingVector);
        this->performRefinementStep(env, std::move(separatingVector));
        if (!checkIfThresholdsAreSatisfied(this->overApproximation)) {
            return false;
        }
        if (checkIfThresholdsAreSatisfied(this->underApproximation)) {
            return true;
        }
    }
    STORM_LOG_ERROR("Could not check whether thresholds are achievable: Termination requested or maximum number of refinement steps exceeded.");
    return false;
}

template<class SparseModelType, typename GeometryValueType>
void SparsePcaaAchievabilityQuery<SparseModelType, GeometryValueType>::updateWeightedPrecision(WeightVector const& weights) {
    // Our heuristic considers the distance between the under- and the over approximation w.r.t. the given direction
    std::pair<Point, bool> optimizationResOverApprox = this->overApproximation->optimize(weights);
    if (optimizationResOverApprox.second) {
        std::pair<Point, bool> optimizationResUnderApprox = this->underApproximation->optimize(weights);
        if (optimizationResUnderApprox.second) {
            GeometryValueType distance = storm::utility::vector::dotProduct(optimizationResOverApprox.first, weights) -
                                         storm::utility::vector::dotProduct(optimizationResUnderApprox.first, weights);
            STORM_LOG_ASSERT(distance >= storm::utility::zero<GeometryValueType>(), "Negative distance between under- and over approximation was not expected");
            // Normalize the distance by dividing it with the Euclidean Norm of the weight-vector
            distance /= storm::utility::sqrt(storm::utility::vector::dotProduct(weights, weights));
            distance /= GeometryValueType(2);
            this->weightVectorChecker->setWeightedPrecision(storm::utility::convertNumber<typename SparseModelType::ValueType>(distance));
        }
    }
    // do not update the precision if one of the approximations is unbounded in the provided direction
}

template<class SparseModelType, typename GeometryValueType>
bool SparsePcaaAchievabilityQuery<SparseModelType, GeometryValueType>::checkIfThresholdsAreSatisfied(
    std::shared_ptr<storm::storage::geometry::Polytope<GeometryValueType>> const& polytope) {
    std::vector<storm::storage::geometry::Halfspace<GeometryValueType>> halfspaces = polytope->getHalfspaces();
    for (auto const& h : halfspaces) {
        if (storm::utility::isZero(h.distance(thresholds))) {
            // Check if the threshold point is on the boundary of the halfspace and whether this is violates strict thresholds
            if (h.isPointOnBoundary(thresholds)) {
                for (auto strictThreshold : strictThresholds) {
                    if (h.normalVector()[strictThreshold] > storm::utility::zero<GeometryValueType>()) {
                        return false;
                    }
                }
            }
        } else {
            return false;
        }
    }
    return true;
}

#ifdef STORM_HAVE_CARL
template class SparsePcaaAchievabilityQuery<storm::models::sparse::Mdp<double>, storm::RationalNumber>;
template class SparsePcaaAchievabilityQuery<storm::models::sparse::MarkovAutomaton<double>, storm::RationalNumber>;

template class SparsePcaaAchievabilityQuery<storm::models::sparse::Mdp<storm::RationalNumber>, storm::RationalNumber>;
template class SparsePcaaAchievabilityQuery<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>, storm::RationalNumber>;
#endif
}  // namespace multiobjective
}  // namespace modelchecker
}  // namespace storm
