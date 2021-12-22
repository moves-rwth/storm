#ifndef STORM_MODELCHECKER_MULTIOBJECTIVE_PCAA_SPARSEPCAAPARETOQUERY_H_
#define STORM_MODELCHECKER_MULTIOBJECTIVE_PCAA_SPARSEPCAAPARETOQUERY_H_

#include "storm/modelchecker/multiobjective/pcaa/SparsePcaaQuery.h"

namespace storm {
namespace modelchecker {
namespace multiobjective {

/*
 * This class represents a query for the Pareto curve approximation algorithm (Pcaa).
 * It implements the necessary computations for the different query types.
 */
template<class SparseModelType, typename GeometryValueType>
class SparsePcaaParetoQuery : public SparsePcaaQuery<SparseModelType, GeometryValueType> {
   public:
    // Typedefs for simple geometric objects
    typedef std::vector<GeometryValueType> Point;
    typedef std::vector<GeometryValueType> WeightVector;

    /*
     * Creates a new query for the Pareto curve approximation algorithm (Pcaa)
     * @param preprocessorResult the result from preprocessing
     */
    SparsePcaaParetoQuery(preprocessing::SparseMultiObjectivePreprocessorResult<SparseModelType>& preprocessorResult);

    virtual ~SparsePcaaParetoQuery() = default;

    /*
     * Invokes the computation and retrieves the result
     */
    virtual std::unique_ptr<CheckResult> check(Environment const& env) override;

   private:
    /*
     * Performs refinement steps until the approximation is sufficiently precise
     */
    void exploreSetOfAchievablePoints(Environment const& env);
};

}  // namespace multiobjective
}  // namespace modelchecker
}  // namespace storm

#endif /* STORM_MODELCHECKER_MULTIOBJECTIVE_PCAA_SPARSEPCAAPARETOQUERY_H_ */
