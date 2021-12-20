#include "storm/modelchecker/multiobjective/pcaa/SparsePcaaQuantitativeQuery.h"

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/environment/modelchecker/MultiObjectiveModelCheckerEnvironment.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
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
SparsePcaaQuantitativeQuery<SparseModelType, GeometryValueType>::SparsePcaaQuantitativeQuery(
    preprocessing::SparseMultiObjectivePreprocessorResult<SparseModelType>& preprocessorResult)
    : SparsePcaaQuery<SparseModelType, GeometryValueType>(preprocessorResult) {
    STORM_LOG_ASSERT(preprocessorResult.queryType == preprocessing::SparseMultiObjectivePreprocessorResult<SparseModelType>::QueryType::Quantitative,
                     "Invalid query Type");

    for (uint_fast64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
        if (!this->objectives[objIndex].formula->hasBound()) {
            indexOfOptimizingObjective = objIndex;
            break;
        }
    }
    initializeThresholdData();

    // Set the maximum distance between lower and upper bound of the weightVectorChecker result.
    this->weightVectorChecker->setWeightedPrecision(storm::utility::convertNumber<typename SparseModelType::ValueType>(0.1));
}

template<class SparseModelType, typename GeometryValueType>
void SparsePcaaQuantitativeQuery<SparseModelType, GeometryValueType>::initializeThresholdData() {
    thresholds.reserve(this->objectives.size());
    strictThresholds = storm::storage::BitVector(this->objectives.size(), false);
    std::vector<storm::storage::geometry::Halfspace<GeometryValueType>> thresholdConstraints;
    thresholdConstraints.reserve(this->objectives.size() - 1);
    for (uint_fast64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
        auto const& formula = *this->objectives[objIndex].formula;

        if (formula.hasBound()) {
            thresholds.push_back(formula.template getThresholdAs<GeometryValueType>());
            if (storm::solver::minimize(formula.getOptimalityType())) {
                STORM_LOG_ASSERT(!storm::logic::isLowerBound(formula.getBound().comparisonType), "Minimizing objective should not specify an upper bound.");
                // Values for minimizing objectives will be negated in order to convert them to maximizing objectives.
                // Hence, we also negate the threshold
                thresholds.back() *= -storm::utility::one<GeometryValueType>();
            }
            strictThresholds.set(objIndex, storm::logic::isStrict(formula.getBound().comparisonType));
            WeightVector normalVector(this->objectives.size(), storm::utility::zero<GeometryValueType>());
            normalVector[objIndex] = -storm::utility::one<GeometryValueType>();
            thresholdConstraints.emplace_back(std::move(normalVector), -thresholds.back());
        } else {
            thresholds.push_back(storm::utility::zero<GeometryValueType>());
        }
    }
    // Note: If we have a single objective (i.e., no objectives with thresholds), thresholdsAsPolytope gets no constraints
    thresholdsAsPolytope = storm::storage::geometry::Polytope<GeometryValueType>::create(thresholdConstraints);
}

template<class SparseModelType, typename GeometryValueType>
std::unique_ptr<CheckResult> SparsePcaaQuantitativeQuery<SparseModelType, GeometryValueType>::check(Environment const& env) {
    // First find one solution that achieves the given thresholds ...
    if (this->checkAchievability(env)) {
        // ... then improve it
        GeometryValueType result = this->improveSolution(env);

        // transform the obtained result for the preprocessed model to a result w.r.t. the original model and return the checkresult
        auto const& obj = this->objectives[indexOfOptimizingObjective];
        if (storm::solver::maximize(obj.formula->getOptimalityType())) {
            if (obj.considersComplementaryEvent) {
                result = storm::utility::one<GeometryValueType>() - result;
            }
        } else {
            if (obj.considersComplementaryEvent) {
                result = storm::utility::one<GeometryValueType>() + result;  // 1 - (-result)
            } else {
                result *= -storm::utility::one<GeometryValueType>();
            }
        }
        auto resultForOriginalModel = storm::utility::convertNumber<typename SparseModelType::ValueType>(result);

        return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<typename SparseModelType::ValueType>(
            this->originalModel.getInitialStates().getNextSetIndex(0), resultForOriginalModel));
    } else {
        return std::unique_ptr<CheckResult>(new ExplicitQualitativeCheckResult(this->originalModel.getInitialStates().getNextSetIndex(0), false));
    }
}

template<class SparseModelType, typename GeometryValueType>
bool SparsePcaaQuantitativeQuery<SparseModelType, GeometryValueType>::checkAchievability(Environment const& env) {
    if (this->objectives.size() > 1) {
        // We don't care for the optimizing objective at this point
        this->diracWeightVectorsToBeChecked.set(indexOfOptimizingObjective, false);

        while (!this->maxStepsPerformed(env) && !storm::utility::resources::isTerminate()) {
            WeightVector separatingVector = this->findSeparatingVector(thresholds);
            this->updateWeightedPrecisionInAchievabilityPhase(separatingVector);
            this->performRefinementStep(env, std::move(separatingVector));
            // Pick the threshold for the optimizing objective low enough so valid solutions are not excluded
            thresholds[indexOfOptimizingObjective] =
                std::min(thresholds[indexOfOptimizingObjective], this->refinementSteps.back().lowerBoundPoint[indexOfOptimizingObjective]);
            if (!checkIfThresholdsAreSatisfied(this->overApproximation)) {
                return false;
            }
            if (checkIfThresholdsAreSatisfied(this->underApproximation)) {
                return true;
            }
        }
    } else {
        // If there is only one objective than its the optimizing one. Thus the query has to be achievable.
        return true;
    }
    STORM_LOG_ERROR("Could not check whether thresholds are achievable: Termination requested or maximum number of refinement steps exceeded.");
    return false;
}

template<class SparseModelType, typename GeometryValueType>
void SparsePcaaQuantitativeQuery<SparseModelType, GeometryValueType>::updateWeightedPrecisionInAchievabilityPhase(WeightVector const& weights) {
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
GeometryValueType SparsePcaaQuantitativeQuery<SparseModelType, GeometryValueType>::improveSolution(Environment const& env) {
    STORM_LOG_THROW(env.modelchecker().multi().getPrecisionType() == MultiObjectiveModelCheckerEnvironment::PrecisionType::Absolute,
                    storm::exceptions::IllegalArgumentException, "Unhandled multiobjective precision type.");
    this->diracWeightVectorsToBeChecked.clear();  // Only check weight vectors that can actually improve the solution

    WeightVector directionOfOptimizingObjective(this->objectives.size(), storm::utility::zero<GeometryValueType>());
    directionOfOptimizingObjective[indexOfOptimizingObjective] = storm::utility::one<GeometryValueType>();

    // Improve the found solution.
    // Note that we do not care whether a threshold is strict anymore, because the resulting optimum should be
    // the supremum over all strategies. Hence, one could combine a scheduler inducing the optimum value (but possibly violating strict
    // thresholds) and (with very low probability) a scheduler that satisfies all (possibly strict) thresholds.
    GeometryValueType result = storm::utility::zero<GeometryValueType>();
    while (!this->maxStepsPerformed(env) && !storm::utility::resources::isTerminate()) {
        if (this->refinementSteps.empty()) {
            // We did not make any refinement steps during the checkAchievability phase (e.g., because there is only one objective).
            this->weightVectorChecker->setWeightedPrecision(
                storm::utility::convertNumber<typename SparseModelType::ValueType>(env.modelchecker().multi().getPrecision()));
            WeightVector separatingVector = directionOfOptimizingObjective;
            this->performRefinementStep(env, std::move(separatingVector));
        }
        std::pair<Point, bool> optimizationRes = this->underApproximation->intersection(thresholdsAsPolytope)->optimize(directionOfOptimizingObjective);
        STORM_LOG_THROW(optimizationRes.second, storm::exceptions::UnexpectedException, "The underapproximation is either unbounded or empty.");
        result = optimizationRes.first[indexOfOptimizingObjective];
        STORM_LOG_DEBUG("Best solution found so far is ~" << storm::utility::convertNumber<double>(result) << ".");
        // Compute an upper bound for the optimum and check for convergence
        optimizationRes = this->overApproximation->intersection(thresholdsAsPolytope)->optimize(directionOfOptimizingObjective);
        if (optimizationRes.second) {
            GeometryValueType precisionOfResult = optimizationRes.first[indexOfOptimizingObjective] - result;
            if (precisionOfResult < storm::utility::convertNumber<GeometryValueType>(env.modelchecker().multi().getPrecision())) {
                // Goal precision reached!
                return result;
            } else {
                STORM_LOG_DEBUG("Solution can be improved by at most " << storm::utility::convertNumber<double>(precisionOfResult));
                thresholds[indexOfOptimizingObjective] = optimizationRes.first[indexOfOptimizingObjective];
            }
        } else {
            thresholds[indexOfOptimizingObjective] = result + storm::utility::one<GeometryValueType>();
        }
        WeightVector separatingVector = this->findSeparatingVector(thresholds);
        this->updateWeightedPrecisionInImprovingPhase(env, separatingVector);
        this->performRefinementStep(env, std::move(separatingVector));
    }
    STORM_LOG_ERROR("Could not reach the desired precision: Termination requested or maximum number of refinement steps exceeded.");
    return result;
}

template<class SparseModelType, typename GeometryValueType>
void SparsePcaaQuantitativeQuery<SparseModelType, GeometryValueType>::updateWeightedPrecisionInImprovingPhase(Environment const& env,
                                                                                                              WeightVector const& weights) {
    STORM_LOG_THROW(!storm::utility::isZero(weights[this->indexOfOptimizingObjective]), exceptions::UnexpectedException,
                    "The chosen weight-vector gives zero weight for the objective that is to be optimized.");
    STORM_LOG_THROW(env.modelchecker().multi().getPrecisionType() == MultiObjectiveModelCheckerEnvironment::PrecisionType::Absolute,
                    storm::exceptions::IllegalArgumentException, "Unhandled multiobjective precision type.");
    // If weighs[indexOfOptimizingObjective] is low, the computation of the weightVectorChecker needs to be more precise.
    // Our heuristic ensures that if p is the new vertex of the under-approximation, then max{ eps | p' = p + (0..0 eps 0..0) is in the over-approximation } <=
    // multiobjective_precision/0.9
    GeometryValueType weightedPrecision =
        weights[this->indexOfOptimizingObjective] * storm::utility::convertNumber<GeometryValueType>(env.modelchecker().multi().getPrecision());
    // Normalize by division with the Euclidean Norm of the weight-vector
    weightedPrecision /= storm::utility::sqrt(storm::utility::vector::dotProduct(weights, weights));
    weightedPrecision *= storm::utility::convertNumber<GeometryValueType>(0.9);

    this->weightVectorChecker->setWeightedPrecision(storm::utility::convertNumber<typename SparseModelType::ValueType>(weightedPrecision));
}

template<class SparseModelType, typename GeometryValueType>
bool SparsePcaaQuantitativeQuery<SparseModelType, GeometryValueType>::checkIfThresholdsAreSatisfied(
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
template class SparsePcaaQuantitativeQuery<storm::models::sparse::Mdp<double>, storm::RationalNumber>;
template class SparsePcaaQuantitativeQuery<storm::models::sparse::MarkovAutomaton<double>, storm::RationalNumber>;

template class SparsePcaaQuantitativeQuery<storm::models::sparse::Mdp<storm::RationalNumber>, storm::RationalNumber>;
template class SparsePcaaQuantitativeQuery<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>, storm::RationalNumber>;
#endif
}  // namespace multiobjective
}  // namespace modelchecker
}  // namespace storm
