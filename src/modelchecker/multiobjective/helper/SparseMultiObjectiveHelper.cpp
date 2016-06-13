#include "src/modelchecker/multiobjective/helper/SparseMultiObjectiveHelper.h"

#include "src/adapters/CarlAdapter.h"
#include "src/models/sparse/Mdp.h"
#include "src/models/sparse/StandardRewardModel.h"
#include "src/utility/constants.h"
#include "src/utility/vector.h"
#include "src/settings//SettingsManager.h"
#include "src/settings/modules/MultiObjectiveSettings.h"

#include "src/exceptions/UnexpectedException.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            
            
            template <class SparseModelType, typename RationalNumberType>
            typename SparseMultiObjectiveHelper<SparseModelType, RationalNumberType>::ReturnType SparseMultiObjectiveHelper<SparseModelType, RationalNumberType>::check(PreprocessorData const& data) {
                ReturnType result;
                result.overApproximation() = storm::storage::geometry::Polytope<RationalNumberType>::createUniversalPolytope();
                result.underApproximation() = storm::storage::geometry::Polytope<RationalNumberType>::createEmptyPolytope();
                
                uint_fast64_t numOfObjectivesWithoutThreshold = 0;
                for(auto const& obj : data.objectives) {
                    if(!obj.threshold) {
                        ++numOfObjectivesWithoutThreshold;
                    }
                }
                if(numOfObjectivesWithoutThreshold == 0) {
                    achievabilityQuery(data, result);
                } else if (numOfObjectivesWithoutThreshold == 1) {
                    numericalQuery(data, result);
                } else if (numOfObjectivesWithoutThreshold == data.objectives.size()) {
                    paretoQuery(data, result);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "The number of objecties without threshold is not valid. It should be either 0 (achievabilityQuery), 1 (numericalQuery), or " << data.objectives.size() << " (paretoQuery). Got " << numOfObjectivesWithoutThreshold << " instead.");
                }
                return result;
            }
            
            template <class SparseModelType, typename RationalNumberType>
            void SparseMultiObjectiveHelper<SparseModelType, RationalNumberType>::achievabilityQuery(PreprocessorData const& data, ReturnType& result) {
                Point thresholds;
                thresholds.reserve(data.objectives.size());
                storm::storage::BitVector strictThresholds(data.objectives.size());
                for(uint_fast64_t objIndex = 0; objIndex < data.objectives.size(); ++objIndex) {
                    thresholds.push_back(storm::utility::convertNumber<RationalNumberType>(*data.objectives[objIndex].threshold));
                    strictThresholds.set(objIndex, data.objectives[objIndex].thresholdIsStrict);
                }
                
                storm::storage::BitVector individualObjectivesToBeChecked(data.objectives.size(), true);
                SparseMultiObjectiveWeightVectorChecker<SparseModelType> weightVectorChecker(data);
                do {
                    WeightVector separatingVector = findSeparatingVector(thresholds, result.underApproximation(), individualObjectivesToBeChecked);
                    performRefinementStep(separatingVector, data.produceSchedulers, weightVectorChecker, result);
                    if(!checkIfThresholdsAreSatisfied(result.overApproximation(), thresholds, strictThresholds)){
                        result.setThresholdsAreAchievable(false);
                    }
                    if(checkIfThresholdsAreSatisfied(result.underApproximation(), thresholds, strictThresholds)){
                        result.setThresholdsAreAchievable(true);
                    }
                } while(!result.isThresholdsAreAchievableSet() && !maxStepsPerformed(result));
            }
            
            template <class SparseModelType, typename RationalNumberType>
            void SparseMultiObjectiveHelper<SparseModelType, RationalNumberType>::numericalQuery(PreprocessorData const& data, ReturnType& result) {
                Point thresholds;
                thresholds.reserve(data.objectives.size());
                storm::storage::BitVector strictThresholds(data.objectives.size());
                std::vector<storm::storage::geometry::Halfspace<RationalNumberType>> thresholdConstraints;
                thresholdConstraints.reserve(data.objectives.size()-1);
                uint_fast64_t optimizingObjIndex = data.objectives.size(); // init with invalid value...
                for(uint_fast64_t objIndex = 0; objIndex < data.objectives.size(); ++objIndex) {
                    if(data.objectives[objIndex].threshold) {
                        thresholds.push_back(storm::utility::convertNumber<RationalNumberType>(*data.objectives[objIndex].threshold));
                        WeightVector normalVector(data.objectives.size(), storm::utility::zero<RationalNumberType>());
                        normalVector[objIndex] = -storm::utility::one<RationalNumberType>();
                        thresholdConstraints.emplace_back(std::move(normalVector), -thresholds.back());
                        strictThresholds.set(objIndex, data.objectives[objIndex].thresholdIsStrict);
                    } else {
                        optimizingObjIndex = objIndex;
                        thresholds.push_back(storm::utility::zero<RationalNumberType>());
                    }
                }
                auto thresholdsAsPolytope = storm::storage::geometry::Polytope<RationalNumberType>::create(thresholdConstraints);
                WeightVector directionOfOptimizingObjective(data.objectives.size(), storm::utility::zero<RationalNumberType>());
                directionOfOptimizingObjective[optimizingObjIndex] = storm::utility::one<RationalNumberType>();
                
                // Try to find one valid solution
                storm::storage::BitVector individualObjectivesToBeChecked(data.objectives.size(), true);
                individualObjectivesToBeChecked.set(optimizingObjIndex, false);
                SparseMultiObjectiveWeightVectorChecker<SparseModelType> weightVectorChecker(data);
                do {
                    WeightVector separatingVector = findSeparatingVector(thresholds, result.underApproximation(), individualObjectivesToBeChecked);
                    performRefinementStep(separatingVector, data.produceSchedulers, weightVectorChecker, result);
                    //Pick the threshold for the optimizing objective low enough so valid solutions are not excluded
                    thresholds[optimizingObjIndex] = std::min(thresholds[optimizingObjIndex], result.refinementSteps().back().getPoint()[optimizingObjIndex]);
                    if(!checkIfThresholdsAreSatisfied(result.overApproximation(), thresholds, strictThresholds)){
                        result.setThresholdsAreAchievable(false);
                    }
                    if(checkIfThresholdsAreSatisfied(result.underApproximation(), thresholds, strictThresholds)){
                        result.setThresholdsAreAchievable(true);
                    }
                } while(!result.isThresholdsAreAchievableSet() && !maxStepsPerformed(result));
                if(result.getThresholdsAreAchievable()) {
                    STORM_LOG_DEBUG("Found a solution that satisfies the objective thresholds.");
                    individualObjectivesToBeChecked.clear();
                    // Improve the found solution.
                    // Note that we do not have to care whether a threshold is strict anymore, because the resulting optimum should be
                    // the supremum over all strategies. Hence, one could combine a scheduler inducing the optimum value (but possibly violating strict
                    // thresholds) and (with very low probability) a scheduler that satisfies all (possibly strict) thresholds.
                    while(true) {
                        std::pair<Point, bool> optimizationRes = result.underApproximation()->intersection(thresholdsAsPolytope)->optimize(directionOfOptimizingObjective);
                        STORM_LOG_THROW(optimizationRes.second, storm::exceptions::UnexpectedException, "The underapproximation is either unbounded or empty.");
                        thresholds[optimizingObjIndex] = optimizationRes.first[optimizingObjIndex];
                        result.setNumericalResult(thresholds[optimizingObjIndex]);
                        STORM_LOG_DEBUG("Best solution found so far is " << result.template getNumericalResult<double>() << ".");
                        //Compute an upper bound for the optimum and check for convergence
                        optimizationRes = result.overApproximation()->intersection(thresholdsAsPolytope)->optimize(directionOfOptimizingObjective);
                        if(optimizationRes.second) {
                            result.setPrecisionOfResult(optimizationRes.first[optimizingObjIndex] - thresholds[optimizingObjIndex]);
                            STORM_LOG_DEBUG("Solution can be improved by at most " << result.template getPrecisionOfResult<double>());
                        }
                        if(targetPrecisionReached(result) || maxStepsPerformed(result)) {
                            result.setOptimumIsAchievable(checkIfThresholdsAreSatisfied(result.underApproximation(), thresholds, strictThresholds));
                            break;
                        }
                        WeightVector separatingVector = findSeparatingVector(thresholds, result.underApproximation(), individualObjectivesToBeChecked);
                        performRefinementStep(separatingVector, data.produceSchedulers, weightVectorChecker, result);
                    }
                }
            }
            
            template <class SparseModelType, typename RationalNumberType>
            void SparseMultiObjectiveHelper<SparseModelType, RationalNumberType>::paretoQuery(PreprocessorData const& data, ReturnType& result) {
                SparseMultiObjectiveWeightVectorChecker<SparseModelType> weightVectorChecker(data);
                //First consider the objectives individually
                for(uint_fast64_t objIndex = 0; objIndex<data.objectives.size() && !maxStepsPerformed(result); ++objIndex) {
                    WeightVector direction(data.objectives.size(), storm::utility::zero<RationalNumberType>());
                    direction[objIndex] = storm::utility::one<RationalNumberType>();
                    performRefinementStep(direction, data.produceSchedulers, weightVectorChecker, result);
                }
                
                while(true) {
                    // Get the halfspace of the underApproximation with maximal distance to a vertex of the overApproximation
                    std::vector<storm::storage::geometry::Halfspace<RationalNumberType>> underApproxHalfspaces = result.underApproximation()->getHalfspaces();
                    std::vector<Point> overApproxVertices = result.overApproximation()->getVertices();
                    uint_fast64_t farestHalfspaceIndex = underApproxHalfspaces.size();
                    RationalNumberType farestDistance = storm::utility::zero<RationalNumberType>();
                    for(uint_fast64_t halfspaceIndex = 0; halfspaceIndex < underApproxHalfspaces.size(); ++halfspaceIndex) {
                        for(auto const& vertex : overApproxVertices) {
                            RationalNumberType distance = -underApproxHalfspaces[halfspaceIndex].euclideanDistance(vertex);
                            if(distance > farestDistance) {
                                farestHalfspaceIndex = halfspaceIndex;
                                farestDistance = distance;
                            }
                        }
                    }
                    result.setPrecisionOfResult(farestDistance);
                    STORM_LOG_DEBUG("Current precision of the approximation of the pareto curve is " << result.template getPrecisionOfResult<double>());
                    if(targetPrecisionReached(result) || maxStepsPerformed(result)) {
                        break;
                    }
                    WeightVector direction = underApproxHalfspaces[farestHalfspaceIndex].normalVector();
                    performRefinementStep(direction, data.produceSchedulers, weightVectorChecker, result);
                }
            }
            
            template <class SparseModelType, typename RationalNumberType>
            typename SparseMultiObjectiveHelper<SparseModelType, RationalNumberType>::WeightVector SparseMultiObjectiveHelper<SparseModelType, RationalNumberType>::findSeparatingVector(Point const& pointToBeSeparated, std::shared_ptr<storm::storage::geometry::Polytope<RationalNumberType>> const& underApproximation, storm::storage::BitVector& individualObjectivesToBeChecked) {
                STORM_LOG_DEBUG("Searching a weight vector to seperate the point given by " << storm::utility::vector::convertNumericVector<double>(pointToBeSeparated) << ".");
                
                if(underApproximation->isEmpty()) {
                    // In this case, every weight vector is  separating
                    uint_fast64_t objIndex = individualObjectivesToBeChecked.getNextSetIndex(0) % pointToBeSeparated.size();
                    WeightVector result(pointToBeSeparated.size(), storm::utility::zero<RationalNumberType>());
                    result[objIndex] = storm::utility::one<RationalNumberType>();
                    individualObjectivesToBeChecked.set(objIndex, false);
                    return result;
                }
                
                // Reaching this point means that the underApproximation contains halfspaces. The seperating vector has to be the normal vector of one of these halfspaces.
                // We pick one with maximal distance to the given point. However, weight vectors that correspond to checking individual objectives take precedence.
                std::vector<storm::storage::geometry::Halfspace<RationalNumberType>> halfspaces = underApproximation->getHalfspaces();
                uint_fast64_t farestHalfspaceIndex = halfspaces.size();
                RationalNumberType farestDistance = -storm::utility::one<RationalNumberType>();
                bool foundSeparatingSingleObjectiveVector = false;
                for(uint_fast64_t halfspaceIndex = 0; halfspaceIndex < halfspaces.size(); ++halfspaceIndex) {
                    // Note that we are looking for a halfspace that does not contain the point. Thus, the returned distances are negated.
                    RationalNumberType distance = -halfspaces[halfspaceIndex].euclideanDistance(pointToBeSeparated);
                    if(distance >= storm::utility::zero<RationalNumberType>()) {
                        storm::storage::BitVector nonZeroVectorEntries = ~storm::utility::vector::filterZero<RationalNumberType>(halfspaces[halfspaceIndex].normalVector());
                        bool isSingleObjectiveVector = nonZeroVectorEntries.getNumberOfSetBits() == 1 && individualObjectivesToBeChecked.get(nonZeroVectorEntries.getNextSetIndex(0));
                        // Check if this halfspace is better than the current one
                        if((!foundSeparatingSingleObjectiveVector && isSingleObjectiveVector ) || (foundSeparatingSingleObjectiveVector==isSingleObjectiveVector && distance>farestDistance)) {
                            foundSeparatingSingleObjectiveVector = foundSeparatingSingleObjectiveVector || isSingleObjectiveVector;
                            farestHalfspaceIndex = halfspaceIndex;
                            farestDistance = distance;
                        }
                    }
                }
                if(foundSeparatingSingleObjectiveVector) {
                    storm::storage::BitVector nonZeroVectorEntries = ~storm::utility::vector::filterZero<RationalNumberType>(halfspaces[farestHalfspaceIndex].normalVector());
                    individualObjectivesToBeChecked.set(nonZeroVectorEntries.getNextSetIndex(0), false);
                }
                
                STORM_LOG_THROW(farestHalfspaceIndex<halfspaces.size(), storm::exceptions::UnexpectedException, "There is no seperating vector.");
                STORM_LOG_DEBUG("Found separating  weight vector: " << storm::utility::vector::convertNumericVector<double>(halfspaces[farestHalfspaceIndex].normalVector()) << ".");
                return halfspaces[farestHalfspaceIndex].normalVector();
            }
            
            template <class SparseModelType, typename RationalNumberType>
            void SparseMultiObjectiveHelper<SparseModelType, RationalNumberType>::performRefinementStep(WeightVector const& direction, bool saveScheduler, SparseMultiObjectiveWeightVectorChecker<SparseModelType>& weightVectorChecker, ReturnType& result) {
                weightVectorChecker.check(storm::utility::vector::convertNumericVector<typename SparseModelType::ValueType>(direction));
                STORM_LOG_DEBUG("weighted objectives checker result is " << weightVectorChecker.getInitialStateResultOfObjectives());
                if(saveScheduler) {
                    result.refinementSteps().emplace_back(direction, weightVectorChecker.template getInitialStateResultOfObjectives<RationalNumberType>(), weightVectorChecker.getScheduler());
                } else {
                    result.refinementSteps().emplace_back(direction, weightVectorChecker.template getInitialStateResultOfObjectives<RationalNumberType>());
                }
                updateOverApproximation(result.refinementSteps(), result.overApproximation());
                updateUnderApproximation(result.refinementSteps(), result.underApproximation());
            }
            
            template <class SparseModelType, typename RationalNumberType>
            void SparseMultiObjectiveHelper<SparseModelType, RationalNumberType>::updateOverApproximation(std::vector<RefinementStep> const& refinementSteps, std::shared_ptr<storm::storage::geometry::Polytope<RationalNumberType>>& overApproximation) {
                storm::storage::geometry::Halfspace<RationalNumberType> h(refinementSteps.back().getWeightVector(), storm::utility::vector::dotProduct(refinementSteps.back().getWeightVector(), refinementSteps.back().getPoint()));
                
                // Due to numerical issues, it might be the case that the updated overapproximation does not contain the underapproximation,
                // e.g., when the new point is strictly contained in the underapproximation. Check if this is the case.
                RationalNumberType maximumOffset = h.offset();
                for(auto const& step : refinementSteps){
                    maximumOffset = std::max(maximumOffset, storm::utility::vector::dotProduct(h.normalVector(), step.getPoint()));
                }
                if(maximumOffset > h.offset()){
                    // We correct the issue by shifting the halfspace such that it contains the underapproximation
                    h.offset() = maximumOffset;
                    STORM_LOG_WARN("Numerical issues: The overapproximation would not contain the underapproximation. Hence, a halfspace is shifted by " << storm::utility::convertNumber<double>(h.euclideanDistance(refinementSteps.back().getPoint())) << ".");
                }
                overApproximation = overApproximation->intersection(h);
                STORM_LOG_DEBUG("Updated OverApproximation to " << overApproximation->toString(true));
            }
            
            template <class SparseModelType, typename RationalNumberType>
            void SparseMultiObjectiveHelper<SparseModelType, RationalNumberType>::updateUnderApproximation(std::vector<RefinementStep> const& refinementSteps, std::shared_ptr<storm::storage::geometry::Polytope<RationalNumberType>>& underApproximation) {
                std::vector<Point> paretoPointsVec;
                paretoPointsVec.reserve(refinementSteps.size());
                for(auto const& step : refinementSteps) {
                    paretoPointsVec.push_back(step.getPoint());
                }
                
                STORM_LOG_WARN("REMOVE ADDING ADDITIONAL VERTICES AS SOON AS HYPRO WORKS FOR DEGENERATED POLYTOPES");
                if(paretoPointsVec.front().size()==2) {
                    Point p1 = {-10000, -9999};
                    Point p2 = {-9999, -10000};
                    paretoPointsVec.push_back(p1);
                    paretoPointsVec.push_back(p2);
                } else {
                    Point p1 = {-10000, -9999, -9999};
                    Point p2 = {-9999, -10000, -9999};
                    Point p3 = {-9999, -9999, -10000};
                    paretoPointsVec.push_back(p1);
                    paretoPointsVec.push_back(p2);
                    paretoPointsVec.push_back(p3);
                }
                
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
            
            template <class SparseModelType, typename RationalNumberType>
            bool SparseMultiObjectiveHelper<SparseModelType, RationalNumberType>::checkIfThresholdsAreSatisfied(std::shared_ptr<storm::storage::geometry::Polytope<RationalNumberType>> const& polytope, Point const& thresholds, storm::storage::BitVector const& strictThresholds) {
                std::vector<storm::storage::geometry::Halfspace<RationalNumberType>> halfspaces = polytope->getHalfspaces();
                for(auto const& h : halfspaces) {
                    RationalNumberType distance = h.distance(thresholds);
                    if(distance < storm::utility::zero<RationalNumberType>()) {
                        return false;
                    }
                    if(distance == storm::utility::zero<RationalNumberType>()) {
                        // In this case, the thresholds point is on the boundary of the polytope.
                        // Check if this is problematic for the strict thresholds
                        for(auto strictThreshold : strictThresholds) {
                            if(h.normalVector()[strictThreshold] > storm::utility::zero<RationalNumberType>()) {
                                return false;
                            }
                        }
                    }
                }
                return true;
            }
            
            template <class SparseModelType, typename RationalNumberType>
            bool SparseMultiObjectiveHelper<SparseModelType, RationalNumberType>::targetPrecisionReached(ReturnType& result) {
                result.setTargetPrecisionReached(result.isPrecisionOfResultSet() &&
                                                 result.getPrecisionOfResult() < storm::utility::convertNumber<RationalNumberType>(storm::settings::getModule<storm::settings::modules::MultiObjectiveSettings>().getPrecision()));
                return result.getTargetPrecisionReached();
            }
            
            template <class SparseModelType, typename RationalNumberType>
            bool SparseMultiObjectiveHelper<SparseModelType, RationalNumberType>::maxStepsPerformed(ReturnType& result) {
                result.setMaxStepsPerformed(storm::settings::getModule<storm::settings::modules::MultiObjectiveSettings>().isMaxStepsSet() &&
                                            result.refinementSteps().size() >= storm::settings::getModule<storm::settings::modules::MultiObjectiveSettings>().getMaxSteps());
                return result.getMaxStepsPerformed();
            }
            
#ifdef STORM_HAVE_CARL
            template class SparseMultiObjectiveHelper<storm::models::sparse::Mdp<double>, storm::RationalNumber>;
#endif
        }
    }
}
