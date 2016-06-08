#ifndef STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEMODELCHECKERHELPER_H_
#define STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEMODELCHECKERHELPER_H_

#include <map>
#include <unordered_map>
#include "src/modelchecker/multiobjective/helper/SparseMultiObjectiveModelCheckerInformation.h"
#include "src/modelchecker/multiObjective/helper/SparseWeightedObjectivesModelCheckerHelper.h"
#include "src/storage/geometry/Polytope.h"
#include "src/storage/TotalScheduler.h"


namespace storm {
    namespace modelchecker {
        namespace helper {
            
            template <class SparseModelType, typename RationalNumberType>
            class SparseMultiObjectiveModelCheckerHelper {
            public:
                typedef typename SparseModelType::ValueType ModelValueType;
                typedef typename SparseModelType::RewardModelType RewardModelType;
                typedef SparseMultiObjectiveModelCheckerInformation<SparseModelType> Information;
                
                
                typedef std::vector<RationalNumberType> Point;
                typedef std::vector<RationalNumberType> WeightVector;
                
                static void check(Information& info);
                
            private:
                SparseMultiObjectiveModelCheckerHelper(Information& info);
                
                ~SparseMultiObjectiveModelCheckerHelper();
                
                void achievabilityQuery();
                void numericalQuery();
                void paretoQuery();


                
                /*
                 * Refines the solution w.r.t. the given direction vector
                 */
                void refineSolution(WeightVector const& direction);
                
                /*
                 * Returns a weight vector w that separates the underapproximation from the given point p, i.e.,
                 * For each x in the underApproximation, it holds that w*x <= w*p
                 *
                 * @param pointToBeSeparated the point that is to be seperated
                 * @param objectivesToBeCheckedIndividually stores for each objective whether it still makes sense to check for this objective individually (i.e., with weight vector given by w_{objIndex} = 1 )
                 */
                WeightVector findSeparatingVector(Point const& pointToBeSeparated, storm::storage::BitVector& individualObjectivesToBeChecked) const;
                
                void updateOverApproximation(Point const& newPoint, WeightVector const& newWeightVector);
                void updateUnderApproximation();
                
                /*
                 * Returns true iff there is a point in the given polytope that satisfies the given thresholds.
                 * It is assumed that the given polytope contains the downward closure of its vertices.
                 */
                bool checkIfThresholdsAreSatisfied(std::shared_ptr<storm::storage::geometry::Polytope<RationalNumberType>> const& polytope, Point const& thresholds, storm::storage::BitVector const& strictThresholds) const;
                
                Information& info;
                SparseWeightedObjectivesModelCheckerHelper<SparseModelType> weightedObjectivesChecker;
                
                //TODO: sort points as needed
                std::map<Point, std::vector<WeightVector>> paretoOptimalPoints;
                std::unordered_map<storm::storage::TotalScheduler, typename std::map<Point, std::vector<WeightVector>>::iterator> schedulers;
                
                std::shared_ptr<storm::storage::geometry::Polytope<RationalNumberType>> overApproximation;
                std::shared_ptr<storm::storage::geometry::Polytope<RationalNumberType>> underApproximation;
            };
            
        }
    }
}

#endif /* STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMULTIOBJECTIVEMODELCHECKERHELPER_H_ */
