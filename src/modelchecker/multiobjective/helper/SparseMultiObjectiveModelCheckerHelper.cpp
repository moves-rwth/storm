#include "src/modelchecker/multiobjective/helper/SparseMultiObjectiveModelCheckerHelper.h"

#include "src/adapters/CarlAdapter.h"
#include "src/models/sparse/Mdp.h"
#include "src/models/sparse/StandardRewardModel.h"
#include "src/utility/constants.h"
#include "src/utility/vector.h"

#include "src/exceptions/UnexpectedException.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            
            template <class SparseModelType, typename RationalNumberType>
            SparseMultiObjectiveModelCheckerHelper<SparseModelType, RationalNumberType>::SparseMultiObjectiveModelCheckerHelper(Information& info) : info(info), weightedObjectivesChecker(info) {
                overApproximation = storm::storage::geometry::Polytope<RationalNumberType>::createUniversalPolytope();
                underApproximation = storm::storage::geometry::Polytope<RationalNumberType>::createEmptyPolytope();
            }
            
            template <class SparseModelType, typename RationalNumberType>
            SparseMultiObjectiveModelCheckerHelper<SparseModelType, RationalNumberType>::~SparseMultiObjectiveModelCheckerHelper() {
                // Intentionally left empty
            }
            
            template <class SparseModelType, typename RationalNumberType>
            void SparseMultiObjectiveModelCheckerHelper<SparseModelType, RationalNumberType>::achievabilityQuery() {
                Point queryPoint;
                queryPoint.reserve(info.objectives.size());
                for(auto const& obj : info.objectives) {
                    STORM_LOG_THROW(obj.threshold, storm::exceptions::UnexpectedException, "Can not perform achievabilityQuery: No threshold given for at least one objective.");
                    queryPoint.push_back(storm::utility::convertNumber<RationalNumberType>(*obj.threshold));
                }
                storm::storage::BitVector individualObjectivesToBeChecked(info.objectives.size(), true);
                while(true) { //TODO introduce convergence criterion? (like max num of iterations)
                    storm::storage::geometry::Halfspace<RationalNumberType> separatingHalfspace = findSeparatingHalfspace(queryPoint, individualObjectivesToBeChecked);
                    refineSolution(separatingHalfspace.normalVector());
                    if(!overApproximation->contains(queryPoint)){
                        std::cout << "PROPERTY VIOLATED" << std::endl;
                        return;
                    }
                    if(underApproximation->contains(queryPoint)){
                        std::cout << "PROPERTY SATISFIED" << std::endl;
                        return;
                    }
                }
            }
            
            template <class SparseModelType, typename RationalNumberType>
            void SparseMultiObjectiveModelCheckerHelper<SparseModelType, RationalNumberType>::refineSolution(WeightVector const& direction) {
                weightedObjectivesChecker.check(storm::utility::vector::convertNumericVector<ModelValueType>(direction));
            
                storm::storage::TotalScheduler scheduler = weightedObjectivesChecker.getScheduler();
                Point weightedObjectivesResult = storm::utility::vector::convertNumericVector<RationalNumberType>(weightedObjectivesChecker.getInitialStateResultOfObjectives());
                
                // Insert the computed scheduler and check whether we have already seen it before
                auto schedulerInsertRes = schedulers.insert(std::make_pair(std::move(scheduler), paretoOptimalPoints.end()));
                if(schedulerInsertRes.second){
                    // The scheduler is new, so insert the newly computed pareto optimal point.
                    // Due to numerical issues, however, it might be the case that the updated overapproximation will not contain the underapproximation,
                    // e.g., when the new point is strictly contained in the underapproximation. Check if this is the case.
                    RationalNumberType computedOffset = storm::utility::vector::dotProduct(weightedObjectivesResult, direction);
                    RationalNumberType minimumOffset = computedOffset;
                    for(auto const& paretoPoint : paretoOptimalPoints){
                        minimumOffset = std::max(minimumOffset, storm::utility::vector::dotProduct(paretoPoint.first, direction));
                    }
                    if(minimumOffset > computedOffset){
                        // We correct the issue by shifting the weightedObjectivesResult in the given direction such that
                        // it lies on the hyperplane given by direction and minimumOffset
                        WeightVector correction(direction);
                        storm::utility::vector::scaleVectorInPlace(correction, (minimumOffset - computedOffset) / storm::utility::vector::dotProduct(direction, direction));
                        storm::utility::vector::addVectors(weightedObjectivesResult, correction, weightedObjectivesResult);
                        STORM_LOG_WARN("Numerical issues: The weightedObjectivesResult has to be shifted by " << storm::utility::sqrt(storm::utility::vector::dotProduct(correction, correction)));
                    }
                    // Now insert the point. For the corresponding scheduler, we safe the entry in the map.
                    schedulerInsertRes.first->second = paretoOptimalPoints.insert(std::make_pair(std::move(weightedObjectivesResult), std::vector<WeightVector>())).first;
                }
                // In the case where the scheduler is not new, we assume that the corresponding pareto optimal points for the old and new scheduler are equal.
                // hence, no additional point will be inserted.
                // Note that the values might not be exactly equal due to numerical issues.
                    
                Point const& paretoPoint = schedulerInsertRes.first->second->first;
                std::vector<WeightVector>& weightVectors = schedulerInsertRes.first->second->second;
                weightVectors.push_back(direction);

                updateOverApproximation(paretoPoint, weightVectors.back());
                updateUnderApproximation();
            }
            
            template <class SparseModelType, typename RationalNumberType>
            storm::storage::geometry::Halfspace<RationalNumberType> SparseMultiObjectiveModelCheckerHelper<SparseModelType, RationalNumberType>::findSeparatingHalfspace(Point const& pointToBeSeparated, storm::storage::BitVector& individualObjectivesToBeChecked) {
                // First, we check 'simple' weight vectors that correspond to checking a single objective
                while (!individualObjectivesToBeChecked.empty()){
                    uint_fast64_t objectiveIndex = individualObjectivesToBeChecked.getNextSetIndex(0);
                    individualObjectivesToBeChecked.set(objectiveIndex, false);
                    
                    WeightVector normalVector; // = (0..0 1 0..0)
                    normalVector.reserve(info.objectives.size());
                    for(uint_fast64_t i = 0; i<info.objectives.size(); ++i){
                        normalVector.push_back( (i==objectiveIndex) ? storm::utility::one<RationalNumberType>() : storm::utility::zero<RationalNumberType>());
                    }
                    
                    storm::storage::geometry::Halfspace<RationalNumberType> h(normalVector, storm::utility::vector::dotProduct(normalVector, pointToBeSeparated));
                    bool hIsSeparating = true;
                    for(auto const& paretoPoint : paretoOptimalPoints){
                        hIsSeparating &= h.contains(paretoPoint.first);
                    }
                    if(hIsSeparating) {
                        return h;
                    }
                }
                
                // We now use LP solving to find a seperating halfspace.
                // Note that StoRM's LPSover does not support rational numbers. Hence, we optimize a polytope instead.
                // The variables of the LP are the normalVector entries w_0 ... w_{n-1} and the minimal distance d between the halfspace and the paretoOptimalPoints
                uint_fast64_t numVariables = pointToBeSeparated.size() + 1;
                std::vector<storm::storage::geometry::Halfspace<RationalNumberType>> constraints;
                constraints.reserve((numVariables-1) + 2 + paretoOptimalPoints.size());
                // w_i >= 0 and d>= 0   <==>  -w_i <= 0 and -d <= 0
                for(uint_fast64_t i = 0; i<numVariables; ++i) {
                    WeightVector constraintCoefficients(numVariables, storm::utility::zero<RationalNumberType>());
                    constraintCoefficients[i] = -storm::utility::one<RationalNumberType>();
                    RationalNumberType constraintOffset = storm::utility::zero<RationalNumberType>();
                    constraints.emplace_back(std::move(constraintCoefficients), std::move(constraintOffset));
                }
                // sum_i w_i == 1   <==>   sum_i w_i <=1 and sum_i -w_i <= -1
                {
                    WeightVector constraintCoefficients(numVariables, storm::utility::one<RationalNumberType>());
                    constraintCoefficients.back() = storm::utility::zero<RationalNumberType>();
                    RationalNumberType constraintOffset = storm::utility::one<RationalNumberType>();
                    constraints.emplace_back(constraintCoefficients, constraintOffset);
                    storm::utility::vector::scaleVectorInPlace(constraintCoefficients, -storm::utility::one<RationalNumberType>());
                    constraintOffset = -storm::utility::one<RationalNumberType>();
                    constraints.emplace_back(std::move(constraintCoefficients), std::move(constraintOffset));
                }
                // let q=pointToBeSeparated. For each paretoPoint x: q*w - x*w >= d  <==> (x-q) * w + d <= 0
                for(auto const& paretoPoint : paretoOptimalPoints){
                    WeightVector constraintCoefficients(numVariables-1);
                    storm::utility::vector::subtractVectors(paretoPoint.first, pointToBeSeparated, constraintCoefficients);
                    constraintCoefficients.push_back(storm::utility::one<RationalNumberType>());
                    RationalNumberType constraintOffset = storm::utility::zero<RationalNumberType>();
                    constraints.emplace_back(std::move(constraintCoefficients), std::move(constraintOffset));
                }
                
                WeightVector optimizationVector(numVariables);
                optimizationVector.back() = storm::utility::one<RationalNumberType>(); // maximize d
                std::pair<WeightVector, bool> optimizeResult = storm::storage::geometry::Polytope<RationalNumberType>::create(constraints)->optimize(optimizationVector);
                STORM_LOG_THROW(optimizeResult.second, storm::exceptions::UnexpectedException, "There seems to be no seperating halfspace.");
                optimizeResult.first.pop_back(); //drop the distance
                RationalNumberType offset = storm::utility::vector::dotProduct(optimizeResult.first, pointToBeSeparated);
                return storm::storage::geometry::Halfspace<RationalNumberType>(std::move(optimizeResult.first), std::move(offset));
            }
            
            template <class SparseModelType, typename RationalNumberType>
            void SparseMultiObjectiveModelCheckerHelper<SparseModelType, RationalNumberType>::updateOverApproximation(Point const& newPoint, WeightVector const& newWeightVector) {
                storm::storage::geometry::Halfspace<RationalNumberType> h(newWeightVector, storm::utility::vector::dotProduct(newWeightVector, newPoint));
                overApproximation = overApproximation->intersection(h);
                STORM_LOG_DEBUG("Updated OverApproximation to " << overApproximation->toString(true));
            }
            
            template <class SparseModelType, typename RationalNumberType>
            void SparseMultiObjectiveModelCheckerHelper<SparseModelType, RationalNumberType>::updateUnderApproximation() {
                std::vector<Point> paretoPointsVec;
                paretoPointsVec.reserve(paretoOptimalPoints.size());
                for(auto const& paretoPoint : paretoOptimalPoints) {
                    paretoPointsVec.push_back(paretoPoint.first);
                }
                
                STORM_LOG_WARN("REMOVE ADDING ADDITIONAL VERTICES AS SOON AS HYPRO WORKS FOR DEGENERATED POLYTOPES");
                Point p1 = {-1, 0};
                Point p2 = {0, -1};
                paretoPointsVec.push_back(p1);
                paretoPointsVec.push_back(p2);
                
                boost::optional<Point> upperBounds;
                if(!paretoPointsVec.empty()){
                    //Get the pointwise maximum of the pareto points
                    upperBounds = paretoPointsVec.front();
                    for(auto paretoPointIt = paretoPointsVec.begin()+1; paretoPointIt != paretoPointsVec.end(); ++paretoPointIt){
                        auto upperBoundIt = upperBounds->begin();
                        for(auto const& paretoPointCoordinate : *paretoPointIt){
                            if(paretoPointCoordinate>*upperBoundIt){
                                *upperBoundIt = paretoPointCoordinate;
                            }
                            ++upperBoundIt;
                        }
                    }
                }
                
                underApproximation = storm::storage::geometry::Polytope<RationalNumberType>::create(paretoPointsVec)->downwardClosure(upperBounds);
                STORM_LOG_DEBUG("Updated UnderApproximation to " << underApproximation->toString(true));
            }
            
#ifdef STORM_HAVE_CARL
            template class SparseMultiObjectiveModelCheckerHelper<storm::models::sparse::Mdp<double>, storm::RationalNumber>;
#endif
        }
    }
}
