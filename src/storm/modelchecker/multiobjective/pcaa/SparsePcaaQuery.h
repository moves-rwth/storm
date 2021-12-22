#ifndef STORM_MODELCHECKER_MULTIOBJECTIVE_PCAA_SPARSEPCAAQUERY_H_
#define STORM_MODELCHECKER_MULTIOBJECTIVE_PCAA_SPARSEPCAAQUERY_H_

#include "storm/modelchecker/multiobjective/pcaa/PcaaWeightVectorChecker.h"
#include "storm/modelchecker/multiobjective/preprocessing/SparseMultiObjectivePreprocessorResult.h"
#include "storm/modelchecker/results/CheckResult.h"
#include "storm/storage/geometry/Polytope.h"

namespace storm {

class Environment;

namespace modelchecker {
namespace multiobjective {

/*
 * This class represents a query for the Pareto curve approximation algorithm (Pcaa).
 * It implements the necessary computations for the different query types.
 */
template<class SparseModelType, typename GeometryValueType>
class SparsePcaaQuery {
   public:
    // Typedefs for simple geometric objects
    typedef std::vector<GeometryValueType> Point;
    typedef std::vector<GeometryValueType> WeightVector;

    virtual ~SparsePcaaQuery();

    /*
     * Invokes the computation and retrieves the result
     */
    virtual std::unique_ptr<CheckResult> check(Environment const& env) = 0;

    /*
     * Exports the current approximations and the currently processed points into respective .csv files located at the given directory.
     * The polytopes are represented as the set of vertices.
     * Note that the approximations will be intersected with a  (sufficiently large) hyperrectangle in order to ensure that the polytopes are bounded
     * This only works for 2 dimensional queries.
     */
    void exportPlotOfCurrentApproximation(Environment const& env) const;

   protected:
    /*
     * Represents the information obtained in a single iteration of the algorithm
     */
    struct RefinementStep {
        WeightVector weightVector;
        Point lowerBoundPoint;
        Point upperBoundPoint;
    };

    /*
     * Creates a new query for the Pareto curve approximation algorithm (Pcaa)
     * @param preprocessorResult the result from preprocessing
     */
    SparsePcaaQuery(preprocessing::SparseMultiObjectivePreprocessorResult<SparseModelType>& preprocessorResult);

    /*
     * Returns a weight vector w that separates the under approximation from the given point p, i.e.,
     * For each x in the under approximation, it holds that w*x <= w*p
     *
     * @param pointToBeSeparated the point that is to be seperated
     */
    WeightVector findSeparatingVector(Point const& pointToBeSeparated);

    /*
     * Refines the current result w.r.t. the given direction vector.
     */
    void performRefinementStep(Environment const& env, WeightVector&& direction);

    /*
     * Updates the overapproximation after a refinement step has been performed
     *
     * @note The last entry of this->refinementSteps should be the newest step whose information is not yet included in the approximation.
     */
    void updateOverApproximation();

    /*
     * Updates the underapproximation after a refinement step has been performed
     *
     * @note The last entry of this->refinementSteps should be the newest step whose information is not yet included in the approximation.
     */
    void updateUnderApproximation();

    /*
     * Returns true iff the maximum number of refinement steps (as possibly specified in the settings) has been reached
     */
    bool maxStepsPerformed(Environment const& env) const;

    SparseModelType const& originalModel;
    storm::logic::MultiObjectiveFormula const& originalFormula;

    std::vector<Objective<typename SparseModelType::ValueType>> objectives;

    // The corresponding weight vector checker
    std::unique_ptr<PcaaWeightVectorChecker<SparseModelType>> weightVectorChecker;

    // The results in each iteration of the algorithm
    std::vector<RefinementStep> refinementSteps;
    // Overapproximation of the set of achievable values
    std::shared_ptr<storm::storage::geometry::Polytope<GeometryValueType>> overApproximation;
    // Underapproximation of the set of achievable values
    std::shared_ptr<storm::storage::geometry::Polytope<GeometryValueType>> underApproximation;

    // stores for each objective whether it still makes sense to check for this objective individually (i.e., with weight vector given by w_{i}>0 iff i=objIndex
    // )
    storm::storage::BitVector diracWeightVectorsToBeChecked;
};

}  // namespace multiobjective
}  // namespace modelchecker
}  // namespace storm

#endif /* STORM_MODELCHECKER_MULTIOBJECTIVE_PCAA_SPARSEPCAAQUERY_H_ */
