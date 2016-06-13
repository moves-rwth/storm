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
                result.overApproximation = storm::storage::geometry::Polytope<RationalNumberType>::createUniversalPolytope();
                result.underApproximation = storm::storage::geometry::Polytope<RationalNumberType>::createEmptyPolytope();
                
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
                bool converged=false;
                SparseMultiObjectiveWeightVectorChecker<SparseModelType> weightVectorChecker(data);
                do {
                    WeightVector separatingVector = findSeparatingVector(thresholds, result.underApproximation, individualObjectivesToBeChecked);
                    refineResult(separatingVector, weightVectorChecker, result);
                    // Check for convergence
                    if(!checkIfThresholdsAreSatisfied(overApproximation, thresholds, strictThresholds)){
                        result.thresholdsAreAchievable = false;
                        converged=true;
                    }
                    if(checkIfThresholdsAreSatisfied(underApproximation, thresholds, strictThresholds)){
                        result.thresholdsAreAchievable = true;
                        converged=true;
                    }
                    converged |= (storm::settings::getModule<storm::settings::modules::MultiObjectiveSettings>().isMaxIterationsSet() && result.iterations.size() >= storm::settings::getModule<storm::settings::modules::MultiObjectiveSettings>().getMaxIterations());
                } while(!converged);
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
                bool converged=false;
                do {
                    WeightVector separatingVector = findSeparatingVector(thresholds, result.underApproximation, individualObjectivesToBeChecked);
                    refineResult(separatingVector, weightVectorChecker, result);
                    //Pick the threshold for the optimizing objective low enough so valid solutions are not excluded
                    thresholds[optimizingObjIndex] = std::min(thresholds[optimizingObjIndex], iterations.back().point[optimizingObjIndex]);
                    if(!checkIfThresholdsAreSatisfied(overApproximation, thresholds, strictThresholds)){
                        result.thresholdsAreAchievable = false;
                        converged=true;
                    }
                    if(checkIfThresholdsAreSatisfied(underApproximation, thresholds, strictThresholds)){
                        result.thresholdsAreAchievable = true;
                        converged=true;
                    }
                } while(!converged);
                if(*result.thresholdsAreAchievable) {
                    STORM_LOG_DEBUG("Found a solution that satisfies the objective thresholds.");
                    individualObjectivesToBeChecked.clear();
                    // Improve the found solution.
                    // Note that we do not have to care whether a threshold is strict anymore, because the resulting optimum should be
                    // the supremum over all strategies. Hence, one could combine a scheduler inducing the optimum value (but possibly violating strict
                    // thresholds) and (with very low probability) a scheduler that satisfies all (possibly strict) thresholds.
                    converged = false;
                    do {
                        std::pair<Point, bool> optimizationRes = underApproximation->intersection(thresholdsAsPolytope)->optimize(directionOfOptimizingObjective);
                        STORM_LOG_THROW(optimizationRes.second, storm::exceptions::UnexpectedException, "The underapproximation is either unbounded or empty.");
                        thresholds[optimizingObjIndex] = optimizationRes.first[optimizingObjIndex];
                        STORM_LOG_DEBUG("Best solution found so far is " << storm::utility::convertNumber<double>(thresholds[optimizingObjIndex]) << ".");
                        //Compute an upper bound for the optimum and check for convergence
                        optimizationRes = overApproximation->intersection(thresholdsAsPolytope)->optimize(directionOfOptimizingObjective);
                        if(optimizationRes.second) {
                            result.precisionOfResult = optimizationRes.first[optimizingObjIndex]; - thresholds[optimizingObjIndex];
                            STORM_LOG_DEBUG("Solution can be improved by at most " << storm::utility::convertNumber<double>(*result.precisionOfResult));
                            if(storm::utility::convertNumber<double>(*result.precisionOfResult) < storm::settings::getModule<storm::settings::modules::MultiObjectiveSettings>().getPrecision()) {
                                result.numericalResult = thresholds[optimizingObjIndex];
                                result.optimumIsAchievable = checkIfThresholdsAreSatisfied(underApproximation, thresholds, strictThresholds);
                                converged=true;
                            }
                        }
                        converged |= (storm::settings::getModule<storm::settings::modules::MultiObjectiveSettings>().isMaxIterationsSet() && result.iterations.size() >= storm::settings::getModule<storm::settings::modules::MultiObjectiveSettings>().getMaxIterations());
                        if(!converged) {
                            WeightVector separatingVector = findSeparatingVector(thresholds, result.underApproximation, individualObjectivesToBeChecked);
                            refineResult(separatingVector, weightVectorChecker, result);
                        }
                    } while(!converged);
                }
            }
            
            template <class SparseModelType, typename RationalNumberType>
            void SparseMultiObjectiveHelper<SparseModelType, RationalNumberType>::paretoQuery(PreprocessorData const& data, ReturnType& result) {
                //First consider the objectives individually
                for(uint_fast64_t objIndex = 0; objIndex < data.objectives.size(); ++objIndex) {
                    WeightVector direction(data.objectives.size(), storm::utility::zero<RationalNumberType>());
                    direction[objIndex] = storm::utility::one<RationalNumberType>();
                    refineResult(direction, weightVectorChecker, result);
                }
                
                bool converged=false;
                do {
                    // Get the halfspace of the underApproximation with maximal distance to a vertex of the overApproximation
                    std::vector<storm::storage::geometry::Halfspace<RationalNumberType>> underApproxHalfspaces = result.underApproximation->getHalfspaces();
                    std::vector<Point> overApproxVertices = result.overApproximation->getVertices();
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
                    STORM_LOG_DEBUG("Farest distance between under- and overApproximation is " << storm::utility::convertNumber<double>(farestDistance));
                    if(storm::utility::convertNumber<double>(farestDistance) < storm::settings::getModule<storm::settings::modules::MultiObjectiveSettings>().getPrecision()) {
                        result.precisionOfResult = farestDistance;
                        converged = true;
                    }
                    converged |= (storm::settings::getModule<storm::settings::modules::MultiObjectiveSettings>().isMaxIterationsSet() && result.iterations.size() >= storm::settings::getModule<storm::settings::modules::MultiObjectiveSettings>().getMaxIterations());
                    if(!converged) {
                        WeightVector direction = underApproxHalfspaces[farestHalfspaceIndex].normalVector();
                        refineResult(direction, weightVectorChecker, result);
                    }
                } while(!converged);
            }
            
            
            template <class SparseModelType, typename RationalNumberType>
            void SparseMultiObjectiveHelper<SparseModelType, RationalNumberType>::refineResult(WeightVector const& direction, SparseWeightedObjectivesModelCheckerHelper<SparseModelType> const& weightedObjectivesChecker, ReturnType& result) {
                weightedObjectivesChecker.check(storm::utility::vector::convertNumericVector<typename SparseModelType::ValueType>(direction));
                STORM_LOG_DEBUG("Result with weighted objectives is " << weightedObjectivesChecker.getInitialStateResultOfObjectives());
                result.iterations.emplace_back(direction, storm::utility::vector::convertNumericVector<RationalNumberType>(weightedObjectivesChecker.getInitialStateResultOfObjectives()), weightedObjectivesChecker.getScheduler());
                
                updateOverApproximation(result.iterations.back().point, result.iterations.back().weightVector);
                updateUnderApproximation();
            }
            
            template <class SparseModelType, typename RationalNumberType>
            typename SparseMultiObjectiveHelper<SparseModelType, RationalNumberType>::WeightVector SparseMultiObjectiveHelper<SparseModelType, RationalNumberType>::findSeparatingVector(Point const& pointToBeSeparated, std::shared_ptr<storm::storage::geometry::Polytope<RationalNumberType>> const& underApproximation, storm::storage::BitVector& individualObjectivesToBeChecked) const {
                STORM_LOG_DEBUG("Searching a weight vector to seperate the point given by " << storm::utility::vector::convertNumericVector<double>(pointToBeSeparated) << ".");
                
                if(underApproximation->isEmpty()) {
                    // In this case, every weight vector is  separating
                    uint_fast64_t objIndex = individualObjectivesToBeChecked.getNextSetIndex(0) % pointToBeSeparated.size();
                    WeightVector result(pointToBeSeparated.size(), storm::utility::zero<RationalNumberType>());
                    result[objIndex] = storm::utility::one<RationalNumberType>();
                    individualObjectivesToBeChecked.set(objIndex, false);
                    return result;
                }
                
                // Reaching this point means that the underApproximation contains halfspaces.
                // The seperating vector has to be the normal vector of one of these halfspaces.
                // We pick one with maximal distance to the given point. However, weight vectors that correspond to checking individual objectives take precedence.
                std::vector<storm::storage::geometry::Halfspace<RationalNumberType>> halfspaces = underApproximation->getHalfspaces();
                uint_fast64_t farestHalfspaceIndex = 0;
                // Note that we are looking for a halfspace that does not contain the point. Thus, the returned distances are negative.
                RationalNumberType farestDistance = -halfspaces[farestHalfspaceIndex].euclideanDistance(pointToBeSeparated);
                storm::storage::BitVector nonZeroVectorEntries = ~storm::utility::vector::filterZero<RationalNumberType>(halfspaces[farestHalfspaceIndex].normalVector());
                bool foundSeparatingSingleObjectiveVector = nonZeroVectorEntries.getNumberOfSetBits()==1 && individualObjectivesToBeChecked.get(nonZeroVectorEntries.getNextSetIndex(0)) && farestDistance>storm::utility::zero<RationalNumberType>();
                
                for(uint_fast64_t halfspaceIndex = 1; halfspaceIndex < halfspaces.size(); ++halfspaceIndex) {
                    RationalNumberType distance = -halfspaces[halfspaceIndex].euclideanDistance(pointToBeSeparated);
                    nonZeroVectorEntries = ~storm::utility::vector::filterZero<RationalNumberType>(halfspaces[farestHalfspaceIndex].normalVector());
                    bool isSeparatingSingleObjectiveVector = nonZeroVectorEntries.getNumberOfSetBits() == 1 && individualObjectivesToBeChecked.get(nonZeroVectorEntries.getNextSetIndex(0)) && distance>storm::utility::zero<RationalNumberType>();
                    // Check if this halfspace is 'better' than the current one
                    if((!foundSeparatingSingleObjectiveVector && isSeparatingSingleObjectiveVector ) || (foundSeparatingSingleObjectiveVector==isSeparatingSingleObjectiveVector && distance>farestDistance)) {
                        foundSeparatingSingleObjectiveVector |= isSeparatingSingleObjectiveVector;
                        farestHalfspaceIndex = halfspaceIndex;
                        farestDistance = distance;
                    }
                }
                if(foundSeparatingSingleObjectiveVector) {
                    nonZeroVectorEntries = ~storm::utility::vector::filterZero<RationalNumberType>(halfspaces[farestHalfspaceIndex].normalVector());
                    individualObjectivesToBeChecked.set(nonZeroVectorEntries.getNextSetIndex(0), false);
                }
                
                STORM_LOG_THROW(farestDistance>=storm::utility::zero<RationalNumberType>(), storm::exceptions::UnexpectedException, "There is no seperating vector.");
                STORM_LOG_DEBUG("Found separating  weight vector: " << storm::utility::vector::convertNumericVector<double>(halfspaces[farestHalfspaceIndex].normalVector()) << ".");
                return halfspaces[farestHalfspaceIndex].normalVector();
            }
            
            template <class SparseModelType, typename RationalNumberType>
            void SparseMultiObjectiveHelper<SparseModelType, RationalNumberType>::updateOverApproximation(std::vector<typename ReturnType::Iteration> const& iterations, std::shared_ptr<storm::storage::geometry::Polytope<RationalNumberType>>& overApproximation) {
                storm::storage::geometry::Halfspace<RationalNumberType> h(iterations.back().weightVector, storm::utility::vector::dotProduct(iterations.back().weightVector, iterations.back().point));
                
                // Due to numerical issues, it might be the case that the updated overapproximation does not contain the underapproximation,
                // e.g., when the new point is strictly contained in the underapproximation. Check if this is the case.
                RationalNumberType maximumOffset = h.offset();
                for(auto const& iteration : iterations){
                    maximumOffset = std::max(maximumOffset, storm::utility::vector::dotProduct(h.normalVector(), iteration.point));
                }
                if(maximumOffset > h.offset()){
                    // We correct the issue by shifting the halfspace such that it contains the underapproximation
                    h.offset() = maximumOffset;
                    STORM_LOG_WARN("Numerical issues: The overapproximation would not contain the underapproximation. Hence, a halfspace is shifted by " << storm::utility::convertNumber<double>(h.euclideanDistance(iterations.back().point)) << ".");
                }
                overApproximation = overApproximation->intersection(h);
                STORM_LOG_DEBUG("Updated OverApproximation to " << overApproximation->toString(true));
            }
            
            template <class SparseModelType, typename RationalNumberType>
            void SparseMultiObjectiveHelper<SparseModelType, RationalNumberType>::updateUnderApproximation(std::vector<typename ReturnType::Iteration> const& iterations, std::shared_ptr<storm::storage::geometry::Polytope<RationalNumberType>>& underApproximation) {
                std::vector<Point> paretoPointsVec;
                paretoPointsVec.reserve(iterations.size());
                for(auto const& iteration : iterations) {
                    paretoPointsVec.push_back(iterations.point);
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
            bool SparseMultiObjectiveHelper<SparseModelType, RationalNumberType>::checkIfThresholdsAreSatisfied(std::shared_ptr<storm::storage::geometry::Polytope<RationalNumberType>> const& polytope, Point const& thresholds, storm::storage::BitVector const& strictThresholds) const {
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
            
#ifdef STORM_HAVE_CARL
            template class SparseMultiObjectiveHelper<storm::models::sparse::Mdp<double>, storm::RationalNumber>;
#endif
        }
    }
}
