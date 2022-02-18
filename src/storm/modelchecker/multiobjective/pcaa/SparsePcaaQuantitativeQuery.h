#ifndef STORM_MODELCHECKER_MULTIOBJECTIVE_PCAA_SPARSEPCAAQUANTITATIVEQUERY_H_
#define STORM_MODELCHECKER_MULTIOBJECTIVE_PCAA_SPARSEPCAAQUANTITATIVEQUERY_H_

#include "storm/modelchecker/multiobjective/pcaa/SparsePcaaQuery.h"

namespace storm {
namespace modelchecker {
namespace multiobjective {

/*
 * This class represents a query for the Pareto curve approximation algorithm (Pcaa).
 * It implements the necessary computations for the different query types.
 */
template<class SparseModelType, typename GeometryValueType>
class SparsePcaaQuantitativeQuery : public SparsePcaaQuery<SparseModelType, GeometryValueType> {
   public:
    // Typedefs for simple geometric objects
    typedef std::vector<GeometryValueType> Point;
    typedef std::vector<GeometryValueType> WeightVector;

    /*
     * Creates a new query for the Pareto curve approximation algorithm (Pcaa)
     * @param preprocessorResult the result from preprocessing
     */
    SparsePcaaQuantitativeQuery(preprocessing::SparseMultiObjectivePreprocessorResult<SparseModelType>& preprocessorResult);

    virtual ~SparsePcaaQuantitativeQuery() = default;

    /*
     * Invokes the computation and retrieves the result
     */
    virtual std::unique_ptr<CheckResult> check(Environment const& env) override;

   private:
    void initializeThresholdData();

    /*
     * Returns whether the given thresholds are achievable.
     */
    bool checkAchievability(Environment const& env);

    /*
     * Updates the precision of the weightVectorChecker w.r.t. the provided weights
     */
    void updateWeightedPrecisionInAchievabilityPhase(WeightVector const& weights);
    void updateWeightedPrecisionInImprovingPhase(Environment const& env, WeightVector const& weights);

    /*
     * Given that the thresholds are achievable, this function further refines the approximations and returns the optimized value
     */
    GeometryValueType improveSolution(Environment const& env);

    /*
     * Returns true iff there is one point in the given polytope that satisfies the given thresholds.
     * It is assumed that the given polytope contains the downward closure of its vertices.
     */
    bool checkIfThresholdsAreSatisfied(std::shared_ptr<storm::storage::geometry::Polytope<GeometryValueType>> const& polytope);

    uint_fast64_t indexOfOptimizingObjective;

    Point thresholds;
    storm::storage::BitVector strictThresholds;
    std::shared_ptr<storm::storage::geometry::Polytope<GeometryValueType>> thresholdsAsPolytope;
};

}  // namespace multiobjective
}  // namespace modelchecker
}  // namespace storm

#endif /* STORM_MODELCHECKER_MULTIOBJECTIVE_PCAA_SPARSEPCAAQUANTITATIVEQUERY_H_ */
