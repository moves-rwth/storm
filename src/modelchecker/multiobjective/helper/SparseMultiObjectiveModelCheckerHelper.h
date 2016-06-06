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
                
                SparseMultiObjectiveModelCheckerHelper(Information& info);
                
                ~SparseMultiObjectiveModelCheckerHelper();
                
                void achievabilityQuery();
                void numericalQuery();
                void paretoQuery();


            private:
                
                /*
                 * Refines the solution w.r.t. the given direction vector
                 */
                void refineSolution(WeightVector const& direction);
                
                /*
                 * Returns a halfspace h that separates the underapproximation from the given point p, i.e.,
                 * - p lies on the border of h and
                 * - for each x in the underApproximation, it holds that h contains x
                 *
                 * @param pointToBeSeparated the point that is to be seperated
                 * @param objectivesToBeCheckedIndividually stores for each objective whether it still makes sense to check for this objective individually (i.e., with weight vector given by w_{objIndex} = 1 )
                 */
                storm::storage::geometry::Halfspace<RationalNumberType>  findSeparatingHalfspace(Point const& pointToBeSeparated, storm::storage::BitVector& individualObjectivesToBeChecked);
                void updateOverApproximation(Point const& newPoint, WeightVector const& newWeightVector);
                void updateUnderApproximation();
                
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
