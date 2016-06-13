#ifndef STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEHELPER_H_
#define STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEHELPER_H_

#include "src/modelchecker/multiobjective/helper/SparseMultiObjectivePreprocessorReturnType.h"
#include "src/modelchecker/multiObjective/helper/SparseMultiObjectiveWeightVectorChecker.h"
#include "src/storage/geometry/Polytope.h"
#include "src/storage/TotalScheduler.h"


namespace storm {
    namespace modelchecker {
        namespace helper {
            
            template <class SparseModelType, typename RationalNumberType>
            class SparseMultiObjectiveHelper {
            public:
                typedef SpaseMultiObjectivePreprocessorReturnType<SparseModelType> PreprocessorData;
                typedef SpaseMultiObjectiveHelperReturnType<RationalNumberType> ReturnType;
                
                typedef std::vector<RationalNumberType> Point;
                typedef std::vector<RationalNumberType> WeightVector;
                
                
                static ReturnType check(PreprocessorData const& data);
                
            private:
                
                static void achievabilityQuery(PreprocessorData const& data, ReturnType& result);
                static void numericalQuery(PreprocessorData const& data, ReturnType& result);
                static void paretoQuery(PreprocessorData const& data, ReturnType& result);

                /*
                 * Refines the current result w.r.t. the given direction vector
                 */
                static void refineResult(WeightVector const& direction, SparseWeightedObjectivesModelCheckerHelper<SparseModelType> const& weightedObjectivesChecker, ReturnType& result);
                
                /*
                 * Returns a weight vector w that separates the under approximation from the given point p, i.e.,
                 * For each x in the under approximation, it holds that w*x <= w*p
                 *
                 * @param pointToBeSeparated the point that is to be seperated
                 * @param underapproximation the current under approximation
                 * @param objectivesToBeCheckedIndividually stores for each objective whether it still makes sense to check for this objective individually (i.e., with weight vector given by w_{objIndex} = 1 )
                 */
                static WeightVector findSeparatingVector(Point const& pointToBeSeparated, std::shared_ptr<storm::storage::geometry::Polytope<RationalNumberType>> const& underApproximation, storm::storage::BitVector& individualObjectivesToBeChecked) const;
                
                /*
                 * Updates the over- and under approximation, respectively
                 *
                 * @param iterations the iterations performed so far. iterations.back() should be the newest iteration, whose information is not yet included in the approximation.
                 * @param over/underApproximation the current over/underApproximation that will be updated
                 */
                static void updateOverApproximation(std::vector<typename ReturnType::Iteration> const& iterations, std::shared_ptr<storm::storage::geometry::Polytope<RationalNumberType>>& overApproximation);
                static void updateUnderApproximation(std::vector<typename ReturnType::Iteration> const& iterations, std::shared_ptr<storm::storage::geometry::Polytope<RationalNumberType>>& underApproximation);
                
                /*
                 * Returns true iff there is a point in the given polytope that satisfies the given thresholds.
                 * It is assumed that the given polytope contains the downward closure of its vertices.
                 */
                static bool checkIfThresholdsAreSatisfied(std::shared_ptr<storm::storage::geometry::Polytope<RationalNumberType>> const& polytope, Point const& thresholds, storm::storage::BitVector const& strictThresholds) const;
            };
            
        }
    }
}

#endif /* STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEHELPER_H_ */
