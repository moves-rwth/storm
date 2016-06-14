#ifndef STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEHELPER_H_
#define STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEHELPER_H_

#include "src/modelchecker/multiobjective/helper/SparseMultiObjectivePreprocessorData.h"
#include "src/modelchecker/multiobjective/helper/SparseMultiObjectiveResultData.h"
#include "src/modelchecker/multiobjective/helper/SparseMultiObjectiveRefinementStep.h"
#include "src/modelchecker/multiObjective/helper/SparseMultiObjectiveWeightVectorChecker.h"
#include "src/storage/geometry/Polytope.h"
#include "src/storage/TotalScheduler.h"


namespace storm {
    namespace modelchecker {
        namespace helper {
            
            template <class SparseModelType, typename RationalNumberType>
            class SparseMultiObjectiveHelper {
            public:
                typedef SparseMultiObjectivePreprocessorData<SparseModelType> PreprocessorData;
                typedef SparseMultiObjectiveResultData<RationalNumberType> ResultData;
                typedef SparseMultiObjectiveRefinementStep<RationalNumberType> RefinementStep;
                
                typedef std::vector<RationalNumberType> Point;
                typedef std::vector<RationalNumberType> WeightVector;
                
                static ResultData check(PreprocessorData const& preprocessorData);
                
            private:
                
                static void achievabilityQuery(PreprocessorData const& preprocessorData, ResultData& resultData);
                static void numericalQuery(PreprocessorData const& preprocessorData, ResultData& resultData);
                static void paretoQuery(PreprocessorData const& preprocessorData, ResultData& resultData);
                
                /*
                 * Returns a weight vector w that separates the under approximation from the given point p, i.e.,
                 * For each x in the under approximation, it holds that w*x <= w*p
                 *
                 * @param pointToBeSeparated the point that is to be seperated
                 * @param underapproximation the current under approximation
                 * @param objectivesToBeCheckedIndividually stores for each objective whether it still makes sense to check for this objective individually (i.e., with weight vector given by w_{objIndex} = 1 )
                 */
                static WeightVector findSeparatingVector(Point const& pointToBeSeparated, std::shared_ptr<storm::storage::geometry::Polytope<RationalNumberType>> const& underApproximation, storm::storage::BitVector& individualObjectivesToBeChecked);

                /*
                 * Refines the current result w.r.t. the given direction vector
                 */
                static void performRefinementStep(WeightVector const& direction, bool saveScheduler, SparseMultiObjectiveWeightVectorChecker<SparseModelType>& weightVectorChecker, ResultData& resultData);
                
                /*
                 * Updates the overapproximation after a refinement step has been performed
                 *
                 * @param refinementSteps the steps performed so far. The last entry should be the newest step, whose information is not yet included in the approximation.
                 * @param overApproximation the current overApproximation that will be updated
                 */
                static void updateOverApproximation(std::vector<RefinementStep> const& refinementSteps, std::shared_ptr<storm::storage::geometry::Polytope<RationalNumberType>>& overApproximation);
                
                /*
                 * Updates the underapproximation after a refinement step has been performed
                 *
                 * @param refinementSteps the steps performed so far. The last entry should be the newest step, whose information is not yet included in the approximation.
                 * @param underApproximation the current underApproximation that will be updated
                 */
                static void updateUnderApproximation(std::vector<RefinementStep> const& refinementSteps, std::shared_ptr<storm::storage::geometry::Polytope<RationalNumberType>>& underApproximation);
                
                /*
                 * Returns true iff there is a point in the given polytope that satisfies the given thresholds.
                 * It is assumed that the given polytope contains the downward closure of its vertices.
                 */
                static bool checkIfThresholdsAreSatisfied(std::shared_ptr<storm::storage::geometry::Polytope<RationalNumberType>> const& polytope, Point const& thresholds, storm::storage::BitVector const& strictThresholds);
                
                /*
                 * Returns whether the desired precision (as given in the settings) is reached by the current result.
                 * If the given resultData does not specify a precisionOfResult, false is returned.
                 * Also sets the resultData.targetPrecisionReached flag accordingly.
                 */
                static bool targetPrecisionReached(ResultData& resultData);
                
                /*
                 * Returns whether a maximum number of refinement steps is given in the settings and this threshold has been reached.
                 * Also sets the resultData.maxStepsPerformed flag accordingly.
                 */
                static bool maxStepsPerformed(ResultData& resultData);
            };
            
        }
    }
}

#endif /* STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEHELPER_H_ */
