#include "storm/modelchecker/multiobjective/pcaa/SparsePcaaParetoQuery.h"

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/environment/modelchecker/MultiObjectiveModelCheckerEnvironment.h"
#include "storm/modelchecker/multiobjective/MultiObjectivePostprocessing.h"
#include "storm/modelchecker/results/ExplicitParetoCurveCheckResult.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/utility/SignalHandler.h"
#include "storm/utility/constants.h"
#include "storm/utility/vector.h"

namespace storm {
namespace modelchecker {
namespace multiobjective {

template<class SparseModelType, typename GeometryValueType>
SparsePcaaParetoQuery<SparseModelType, GeometryValueType>::SparsePcaaParetoQuery(
    preprocessing::SparseMultiObjectivePreprocessorResult<SparseModelType>& preprocessorResult)
    : SparsePcaaQuery<SparseModelType, GeometryValueType>(preprocessorResult) {
    STORM_LOG_ASSERT(preprocessorResult.queryType == preprocessing::SparseMultiObjectivePreprocessorResult<SparseModelType>::QueryType::Pareto,
                     "Invalid query Type");
}

template<class SparseModelType, typename GeometryValueType>
std::unique_ptr<CheckResult> SparsePcaaParetoQuery<SparseModelType, GeometryValueType>::check(Environment const& env) {
    STORM_LOG_THROW(env.modelchecker().multi().getPrecisionType() == MultiObjectiveModelCheckerEnvironment::PrecisionType::Absolute,
                    storm::exceptions::IllegalArgumentException, "Unhandled multiobjective precision type.");

    // Set the precision of the weight vector checker
    typename SparseModelType::ValueType weightedPrecision =
        storm::utility::convertNumber<typename SparseModelType::ValueType>(env.modelchecker().multi().getPrecision());
    weightedPrecision /= storm::utility::sqrt(storm::utility::convertNumber<typename SparseModelType::ValueType, uint_fast64_t>(this->objectives.size()));
    // multiobjPrecision / sqrt(numObjectives) is the largest possible value for which termination is guaranteed.
    // To see this, assume that for both weight vectors <1,0> and <0,1> we get the same point <a,b> for the under-approximation and <a+weightedPrecision,b>
    // resp. <a,b+weightedPrecision> for the over-approximation. Then, the over-approx. point <a+weightedPrecision,b+weightedPrecision> has a distance to the
    // under-approximation of weightedPrecision * sqrt(numObjectives). Lets be a little bit more precise to reduce the number of required iterations.
    weightedPrecision *= storm::utility::convertNumber<typename SparseModelType::ValueType>(0.9);
    this->weightVectorChecker->setWeightedPrecision(weightedPrecision);

    // refine the approximation
    exploreSetOfAchievablePoints(env);

    // obtain the data for the checkresult
    std::vector<std::vector<typename SparseModelType::ValueType>> paretoOptimalPoints;
    std::vector<Point> vertices = this->underApproximation->getVertices();
    paretoOptimalPoints.reserve(vertices.size());
    for (auto const& vertex : vertices) {
        paretoOptimalPoints.push_back(
            storm::utility::vector::convertNumericVector<typename SparseModelType::ValueType>(transformObjectiveValuesToOriginal(this->objectives, vertex)));
    }
    return std::unique_ptr<CheckResult>(new ExplicitParetoCurveCheckResult<typename SparseModelType::ValueType>(
        this->originalModel.getInitialStates().getNextSetIndex(0), std::move(paretoOptimalPoints),
        transformObjectivePolytopeToOriginal(this->objectives, this->underApproximation)
            ->template convertNumberRepresentation<typename SparseModelType::ValueType>(),
        transformObjectivePolytopeToOriginal(this->objectives, this->overApproximation)
            ->template convertNumberRepresentation<typename SparseModelType::ValueType>()));
}

template<class SparseModelType, typename GeometryValueType>
void SparsePcaaParetoQuery<SparseModelType, GeometryValueType>::exploreSetOfAchievablePoints(Environment const& env) {
    STORM_LOG_THROW(env.modelchecker().multi().getPrecisionType() == MultiObjectiveModelCheckerEnvironment::PrecisionType::Absolute,
                    storm::exceptions::IllegalArgumentException, "Unhandled multiobjective precision type.");

    // First consider the objectives individually
    for (uint_fast64_t objIndex = 0; objIndex < this->objectives.size() && !this->maxStepsPerformed(env); ++objIndex) {
        WeightVector direction(this->objectives.size(), storm::utility::zero<GeometryValueType>());
        direction[objIndex] = storm::utility::one<GeometryValueType>();
        this->performRefinementStep(env, std::move(direction));
        if (storm::utility::resources::isTerminate()) {
            break;
        }
    }

    while (!this->maxStepsPerformed(env) && !storm::utility::resources::isTerminate()) {
        // Get the halfspace of the underApproximation with maximal distance to a vertex of the overApproximation
        std::vector<storm::storage::geometry::Halfspace<GeometryValueType>> underApproxHalfspaces = this->underApproximation->getHalfspaces();
        std::vector<Point> overApproxVertices = this->overApproximation->getVertices();
        uint_fast64_t farestHalfspaceIndex = underApproxHalfspaces.size();
        GeometryValueType farestDistance = storm::utility::zero<GeometryValueType>();
        for (uint_fast64_t halfspaceIndex = 0; halfspaceIndex < underApproxHalfspaces.size(); ++halfspaceIndex) {
            for (auto const& vertex : overApproxVertices) {
                GeometryValueType distance = underApproxHalfspaces[halfspaceIndex].euclideanDistance(vertex);
                if (distance > farestDistance) {
                    farestHalfspaceIndex = halfspaceIndex;
                    farestDistance = distance;
                }
            }
        }
        if (farestDistance < storm::utility::convertNumber<GeometryValueType>(env.modelchecker().multi().getPrecision())) {
            // Goal precision reached!
            return;
        }
        STORM_LOG_INFO("Current precision of the approximation of the pareto curve is ~" << storm::utility::convertNumber<double>(farestDistance));
        WeightVector direction = underApproxHalfspaces[farestHalfspaceIndex].normalVector();
        this->performRefinementStep(env, std::move(direction));
    }
    STORM_LOG_ERROR("Could not reach the desired precision: Termination requested or maximum number of refinement steps exceeded.");
}

#ifdef STORM_HAVE_CARL
template class SparsePcaaParetoQuery<storm::models::sparse::Mdp<double>, storm::RationalNumber>;
template class SparsePcaaParetoQuery<storm::models::sparse::MarkovAutomaton<double>, storm::RationalNumber>;

template class SparsePcaaParetoQuery<storm::models::sparse::Mdp<storm::RationalNumber>, storm::RationalNumber>;
template class SparsePcaaParetoQuery<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>, storm::RationalNumber>;
#endif
}  // namespace multiobjective
}  // namespace modelchecker
}  // namespace storm
