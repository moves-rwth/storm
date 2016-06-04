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
                achievabilityQuery(queryPoint);
            }
            
            template <class SparseModelType, typename RationalNumberType>
            void SparseMultiObjectiveModelCheckerHelper<SparseModelType, RationalNumberType>::achievabilityQuery(Point const& queryPoint) {
                storm::storage::BitVector individualObjectivesToBeChecked(info.objectives.size(), true);
                while(true) { //TODO introduce convergence criterion? (like max num of iterations)
                    storm::storage::geometry::Halfspace<RationalNumberType> separatingHalfspace = findSeparatingHalfspace(queryPoint, individualObjectivesToBeChecked);
                    
                    weightedObjectivesChecker.check(storm::utility::vector::convertNumericVector<ModelValueType>(separatingHalfspace.normalVector()));
                    storm::storage::TotalScheduler scheduler = weightedObjectivesChecker.getScheduler();
                    Point weightedObjectivesResult = storm::utility::vector::convertNumericVector<RationalNumberType>(weightedObjectivesChecker.getInitialStateResultOfObjectives());
                    
                    // Insert the computed scheduler and check whether we have already seen it before
                    auto schedulerInsertRes = schedulers.insert(std::make_pair(std::move(scheduler), paretoOptimalPoints.end()));
                    if(schedulerInsertRes.second){
                        // The scheduler is new, so insert the newly computed pareto optimal point.
                        // To each scheduler, we assign the (unique) pareto optimal point it induces.
                        schedulerInsertRes.first->second = paretoOptimalPoints.insert(std::make_pair(std::move(weightedObjectivesResult), std::vector<WeightVector>())).first;
                    }
                    // In the case where the scheduler is not new, we assume that the corresponding pareto optimal points for the old and new scheduler are equal
                    // Note that the values might not be exactly equal due to numerical issues.
                    
                    Point const& paretoPoint = schedulerInsertRes.first->second->first;
                    std::vector<WeightVector>& weightVectors = schedulerInsertRes.first->second->second;
                    weightVectors.push_back(std::move(separatingHalfspace.normalVector()));
                    
                    updateOverApproximation(paretoPoint, weightVectors.back());
                    if(!overApproximation->contains(queryPoint)){
                        std::cout << "PROPERTY VIOLATED" << std::endl;
                        return;
                    }
                    updateUnderApproximation();
                    if(underApproximation->contains(queryPoint)){
                        std::cout << "PROPERTY SATISFIED" << std::endl;
                        return;
                    }
                }
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
                return underApproximation->findSeparatingHalfspace(pointToBeSeparated);
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
