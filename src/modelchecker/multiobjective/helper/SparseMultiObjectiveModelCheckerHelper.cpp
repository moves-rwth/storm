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
            void SparseMultiObjectiveModelCheckerHelper<SparseModelType, RationalNumberType>::check(Information & info) {
                SparseMultiObjectiveModelCheckerHelper<SparseModelType, RationalNumberType> helper(info);
                uint_fast64_t numOfObjectivesWithoutThreshold = 0;
                for(auto const& obj : info.objectives) {
                    if(!obj.threshold) {
                        ++numOfObjectivesWithoutThreshold;
                    }
                }
                
                if(numOfObjectivesWithoutThreshold == 0) {
                    helper.achievabilityQuery();
                } else if (numOfObjectivesWithoutThreshold == 1) {
                    helper.numericalQuery();
                } else if (numOfObjectivesWithoutThreshold == info.objectives.size()) {
                    helper.paretoQuery();
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "The number of objecties without threshold is not valid. It should be either 0 (achievabilityQuery), 1 (numericalQuery), or " << info.objectives.size() << " (paretoQuery). Got " << numOfObjectivesWithoutThreshold << " instead.");
                }
            }
            
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
                Point thresholds;
                thresholds.reserve(info.objectives.size());
                storm::storage::BitVector strictThresholds(info.objectives.size());
                for(uint_fast64_t objIndex = 0; objIndex < info.objectives.size(); ++objIndex) {
                    STORM_LOG_THROW(info.objectives[objIndex].threshold, storm::exceptions::UnexpectedException, "Can not perform achievabilityQuery: No threshold given for at least one objective.");
                    thresholds.push_back(storm::utility::convertNumber<RationalNumberType>(*info.objectives[objIndex].threshold));
                    strictThresholds.set(objIndex, info.objectives[objIndex].thresholdIsStrict);
                }
                
                storm::storage::BitVector individualObjectivesToBeChecked(info.objectives.size(), true);
                while(true) { //TODO introduce convergence criterion? (like max num of iterations)
                    WeightVector separatingVector = findSeparatingVector(thresholds, individualObjectivesToBeChecked);
                    refineSolution(separatingVector);
                    if(!checkIfThresholdsAreSatisfied(overApproximation, thresholds, strictThresholds)){
                        std::cout << "PROPERTY VIOLATED" << std::endl;
                        return;
                    }
                    if(checkIfThresholdsAreSatisfied(underApproximation, thresholds, strictThresholds)){
                        std::cout << "PROPERTY SATISFIED" << std::endl;
                        return;
                    }
                }
            }
            
            template <class SparseModelType, typename RationalNumberType>
            void SparseMultiObjectiveModelCheckerHelper<SparseModelType, RationalNumberType>::numericalQuery() {
                Point thresholds;
                thresholds.reserve(info.objectives.size());
                storm::storage::BitVector strictThresholds(info.objectives.size());
                std::vector<storm::storage::geometry::Halfspace<RationalNumberType>> thresholdConstraints;
                thresholdConstraints.reserve(info.objectives.size()-1);
                uint_fast64_t optimizingObjIndex = info.objectives.size();
                for(uint_fast64_t objIndex = 0; objIndex < info.objectives.size(); ++objIndex) {
                    if(info.objectives[objIndex].threshold) {
                        thresholds.push_back(storm::utility::convertNumber<RationalNumberType>(*info.objectives[objIndex].threshold));
                        WeightVector normalVector(info.objectives.size(), storm::utility::zero<RationalNumberType>());
                        normalVector[objIndex] = -storm::utility::one<RationalNumberType>();
                        thresholdConstraints.emplace_back(std::move(normalVector), -thresholds.back());
                        std::cout << "adding threshold constraint" << thresholdConstraints.back().toString(true) << std::endl;
                        strictThresholds.set(objIndex, info.objectives[objIndex].thresholdIsStrict);
                    } else {
                        STORM_LOG_ASSERT(optimizingObjIndex == info.objectives.size(), "Numerical Query invoked but there are multiple objectives without threshold");
                        optimizingObjIndex = objIndex;
                        thresholds.push_back(storm::utility::zero<RationalNumberType>());
                    }
                }
                STORM_LOG_ASSERT(optimizingObjIndex < info.objectives.size(), "Numerical Query invoked but there are no objectives without threshold");
                auto thresholdsAsPolytope = storm::storage::geometry::Polytope<RationalNumberType>::create(thresholdConstraints);
                WeightVector directionOfOptimizingObjective(info.objectives.size(), storm::utility::zero<RationalNumberType>());
                directionOfOptimizingObjective[optimizingObjIndex] = storm::utility::one<RationalNumberType>();
                
                // Try to find one valid solution
                storm::storage::BitVector individualObjectivesToBeChecked(info.objectives.size(), true);
                individualObjectivesToBeChecked.set(optimizingObjIndex, false);
                do { //TODO introduce convergence criterion? (like max num of iterations)
                    WeightVector separatingVector = findSeparatingVector(thresholds, individualObjectivesToBeChecked);
                    refineSolution(separatingVector);
                    //Pick the threshold for the optimizing objective low enough so valid solutions are not excluded
                    for(auto const& paretoPoint : paretoOptimalPoints) {
                        thresholds[optimizingObjIndex] = std::min(thresholds[optimizingObjIndex], paretoPoint.first[optimizingObjIndex]);
                    }
                    if(!checkIfThresholdsAreSatisfied(overApproximation, thresholds, strictThresholds)){
                        std::cout << "INFEASIBLE OBJECTIVE THRESHOLDS" << std::endl;
                        return;
                    }
                } while(!checkIfThresholdsAreSatisfied(underApproximation, thresholds, strictThresholds));
                STORM_LOG_DEBUG("Found a solution that satisfies the objective thresholds.");
                individualObjectivesToBeChecked.clear();
                
                // Improve the found solution.
                // Note that we do not have to care whether a threshold is strict anymore, because the resulting optimum should be
                // the supremum over all strategies. Hence, one could combine a scheduler inducing the optimum value (but possibly violating strict
                // thresholds) and (with very low probability) a scheduler that satisfies all (possibly strict) thresholds.
                while (true) {
                    std::pair<Point, bool> optimizationRes = underApproximation->intersection(thresholdsAsPolytope)->optimize(directionOfOptimizingObjective);
                    STORM_LOG_THROW(optimizationRes.second, storm::exceptions::UnexpectedException, "The underapproximation is either unbounded or empty.");
                    thresholds[optimizingObjIndex] = optimizationRes.first[optimizingObjIndex];
                    STORM_LOG_DEBUG("Best solution found so far is " << storm::utility::convertNumber<double>(thresholds[optimizingObjIndex]) << ".");
                    //Compute an upper bound for the optimum and check for convergence
                    optimizationRes = overApproximation->intersection(thresholdsAsPolytope)->optimize(directionOfOptimizingObjective);
                    if(optimizationRes.second) {
                        RationalNumberType upperBoundOnOptimum = optimizationRes.first[optimizingObjIndex];
                        STORM_LOG_DEBUG("Solution can be improved by at most " << storm::utility::convertNumber<double>(upperBoundOnOptimum - thresholds[optimizingObjIndex]));
                        if(storm::utility::convertNumber<double>(upperBoundOnOptimum - thresholds[optimizingObjIndex]) < 0.0001) { //TODO get this value from settings
                            std::cout << "Found Optimum: " << storm::utility::convertNumber<double>(thresholds[optimizingObjIndex]) << "." << std::endl;
                            if(!checkIfThresholdsAreSatisfied(underApproximation, thresholds, strictThresholds)) {
                                std::cout << "Optimum is only the supremum" << std::endl;
                            }
                            return;
                        }
                    }
                    WeightVector separatingVector = findSeparatingVector(thresholds, individualObjectivesToBeChecked);
                    refineSolution(separatingVector);
                }
            }
            
            template <class SparseModelType, typename RationalNumberType>
            void SparseMultiObjectiveModelCheckerHelper<SparseModelType, RationalNumberType>::paretoQuery() {
                //First consider the objectives individually
                for(uint_fast64_t objIndex = 0; objIndex < info.objectives.size(); ++objIndex) {
                    WeightVector direction(info.objectives.size(), storm::utility::zero<RationalNumberType>());
                    direction[objIndex] = storm::utility::one<RationalNumberType>();
                    refineSolution(direction);
                }
                
                while(true) { //todo maxNumOfIterations ?
                    
                    // Get the halfspace of the underApproximation with maximal distance to a vertex of the overApproximation
                    std::vector<storm::storage::geometry::Halfspace<RationalNumberType>> underApproxHalfspaces = underApproximation->getHalfspaces();
                    std::vector<Point> overApproxVertices = overApproximation->getVertices();
                    uint_fast64_t farestHalfspaceIndex = underApproxHalfspaces.size();
                    RationalNumberType farestDistance = storm::utility::zero<RationalNumberType>();
                    for(uint_fast64_t halfspaceIndex = 0; halfspaceIndex < underApproxHalfspaces.size(); ++halfspaceIndex) {
                        for(auto const& vertex : overApproxVertices) {
                            RationalNumberType distance = underApproxHalfspaces[halfspaceIndex].euclideanDistance(vertex);
                            // Note that the distances are negative, i.e., we have to consider the minimum distance
                            if(distance < farestDistance) {
                                farestHalfspaceIndex = halfspaceIndex;
                                farestDistance = distance;
                            }
                        }
                    }
                    STORM_LOG_DEBUG("Farest distance between under- and overApproximation is " << storm::utility::convertNumber<double>(farestDistance));
                    if(storm::utility::convertNumber<double>(farestDistance) > -0.0001) { //todo take this value from settings
                        std::cout << "DONE computing the pareto curve" << std::endl;
                        return;
                    }
                    WeightVector direction = underApproxHalfspaces[farestHalfspaceIndex].normalVector();
                    // normalize the direction vector so the entries sum up to one
                    storm::utility::vector::scaleVectorInPlace(direction, storm::utility::one<RationalNumberType>() / std::accumulate(direction.begin(), direction.end(), storm::utility::zero<RationalNumberType>()));
                    refineSolution(direction);
                }
            }
            
            
            template <class SparseModelType, typename RationalNumberType>
            void SparseMultiObjectiveModelCheckerHelper<SparseModelType, RationalNumberType>::refineSolution(WeightVector const& direction) {
                weightedObjectivesChecker.check(storm::utility::vector::convertNumericVector<ModelValueType>(direction));
            
                storm::storage::TotalScheduler scheduler = weightedObjectivesChecker.getScheduler();
                Point weightedObjectivesResult = storm::utility::vector::convertNumericVector<RationalNumberType>(weightedObjectivesChecker.getInitialStateResultOfObjectives());
                STORM_LOG_DEBUG("Result with weighted objectives is " << storm::utility::vector::toString(weightedObjectivesResult));
                
                // Insert the computed scheduler and check whether we have already seen it before
                auto schedulerInsertRes = schedulers.insert(std::make_pair(std::move(scheduler), paretoOptimalPoints.end()));
                if(schedulerInsertRes.second){
                    // The scheduler is new, so insert the newly computed pareto optimal point.
                    // For the corresponding scheduler, we safe the entry in the map.
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
            typename SparseMultiObjectiveModelCheckerHelper<SparseModelType, RationalNumberType>::WeightVector SparseMultiObjectiveModelCheckerHelper<SparseModelType, RationalNumberType>::findSeparatingVector(Point const& pointToBeSeparated, storm::storage::BitVector& individualObjectivesToBeChecked) const {
                STORM_LOG_DEBUG("Searching a weight vector to seperate the point given by " << storm::utility::vector::convertNumericVector<double>(pointToBeSeparated) << ".");
                // First, we check 'simple' weight vectors that correspond to checking a single objective
                while (!individualObjectivesToBeChecked.empty()){
                    uint_fast64_t objectiveIndex = individualObjectivesToBeChecked.getNextSetIndex(0);
                    individualObjectivesToBeChecked.set(objectiveIndex, false);
                    
                    WeightVector normalVector(info.objectives.size(), storm::utility::zero<RationalNumberType>());
                    normalVector[objectiveIndex] = storm::utility::one<RationalNumberType>();
                    
                    storm::storage::geometry::Halfspace<RationalNumberType> h(normalVector, storm::utility::vector::dotProduct(normalVector, pointToBeSeparated));
                    bool hIsSeparating = true;
                    for(auto const& paretoPoint : paretoOptimalPoints){
                        hIsSeparating &= h.contains(paretoPoint.first);
                    }
                    if(hIsSeparating) {
                        return h.normalVector();
                    }
                }
                
                if(underApproximation->isEmpty()) {
                    // In this case [1 0..0] is always separating
                    WeightVector result(info.objectives.size(), storm::utility::zero<RationalNumberType>());
                    result.front() = storm::utility::one<RationalNumberType>();
                    return result;
                }
                
                // Get the halfspace of the underApproximation with maximal distance to the given point
                // Note that we are looking for a halfspace that does not contain the point. Thus the distances are negative and
                // we are actually searching for the one with minimal distance.
                std::vector<storm::storage::geometry::Halfspace<RationalNumberType>> halfspaces = underApproximation->getHalfspaces();
                uint_fast64_t farestHalfspaceIndex = 0;
                RationalNumberType farestDistance = halfspaces[farestHalfspaceIndex].euclideanDistance(pointToBeSeparated);
                for(uint_fast64_t halfspaceIndex = 1; halfspaceIndex < halfspaces.size(); ++halfspaceIndex) {
                    RationalNumberType distance = halfspaces[halfspaceIndex].euclideanDistance(pointToBeSeparated);
                    if(distance < farestDistance) {
                        farestHalfspaceIndex = halfspaceIndex;
                        farestDistance = distance;
                    }
                }
                STORM_LOG_THROW(farestDistance<=storm::utility::zero<RationalNumberType>(), storm::exceptions::UnexpectedException, "There is no seperating halfspace.");
                // normalize the result so the entries sum up to one
                WeightVector result = halfspaces[farestHalfspaceIndex].normalVector();
                storm::utility::vector::scaleVectorInPlace(result, storm::utility::one<RationalNumberType>() / std::accumulate(result.begin(), result.end(), storm::utility::zero<RationalNumberType>()));
                return result;
                    
                
                /* old version using LP solving
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
                 */
            }
            
            template <class SparseModelType, typename RationalNumberType>
            void SparseMultiObjectiveModelCheckerHelper<SparseModelType, RationalNumberType>::updateOverApproximation(Point const& newPoint, WeightVector const& newWeightVector) {
                storm::storage::geometry::Halfspace<RationalNumberType> h(newWeightVector, storm::utility::vector::dotProduct(newWeightVector, newPoint));
                
                // Due to numerical issues, it might be the case that the updated overapproximation does not contain the underapproximation,
                // e.g., when the new point is strictly contained in the underapproximation. Check if this is the case.
                RationalNumberType maximumOffset = h.offset();
                for(auto const& paretoPoint : paretoOptimalPoints){
                    maximumOffset = std::max(maximumOffset, storm::utility::vector::dotProduct(h.normalVector(), paretoPoint.first));
                }
                if(maximumOffset > h.offset()){
                    // We correct the issue by shifting the halfspace such that it contains the underapproximation
                    h.offset() = maximumOffset;
                    STORM_LOG_WARN("Numerical issues: The overapproximation would not contain the underapproximation. Hence, a halfspace is shifted by " << storm::utility::convertNumber<double>(h.euclideanDistance(newPoint)) << ".");
                }
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
                Point p1 = {-1000, 0};
                Point p2 = {0, -1000};
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
            
            template <class SparseModelType, typename RationalNumberType>
            bool SparseMultiObjectiveModelCheckerHelper<SparseModelType, RationalNumberType>::checkIfThresholdsAreSatisfied(std::shared_ptr<storm::storage::geometry::Polytope<RationalNumberType>> const& polytope, Point const& thresholds, storm::storage::BitVector const& strictThresholds) const {
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
            
                
                
                /*
                
                bool existsObjectiveWithoutThreshold = false;
                bool existsObjectiveWithStrictThreshold = false;
                Point thresholdPoint;
                thresholdPoint.reserve(info.objectives.size());
                for(auto const& obj : info.objectives) {
                    if(obj.threshold) {
                        thresholdPoint.push_back(storm::utility::convertNumber<RationalNumber>(*obj.threshold));
                        if(obj.thresholdIsStrict) {
                            existsObjectiveWithStrictThreshold = true;
                        }
                    } else {
                        existsObjectiveWithoutThreshold = true;
                        thresholdPoint.push_back(storm::utility::one<RationalNumberType>());
                    }
                }
                
                if(existsObjectiveWithoutThreshold) {
                    // We need to find values for the objectives without thresholds in a way that the result is not affected by them.
                    // We build a polytope that contains valid solutions according to the thresholds and then analyze the intersection with the given polytope.
                    std::vector<storm::storage::geometry::Halfspace<RationalNumberType>> thresholdConstraints;
                    WeightVector directionForOptimization(info.objectives.size(), storm::utility::zero<RationalNumberType>());
                    for(uint_fast64_t objIndex = 0; objIndex < info.objectives.size(); ++objIndex) {
                        if(info.objectives[objIndex].threshold) {
                            WeightVector normalVector(info.objectives.size(), storm::utility::zero<RationalNumberType>());
                            normalVector[objIndex] = -storm::utility::one<RationalNumberType>();
                            thresholdConstraints.emplace_back(std::move(normalVector), -thresholdPoint[objIndex]);
                            directionForOptimization[objIndex] = -storm::utility::one<RationalNumberType>();
                        }
                    }
                    auto validSolutions = polytope->intersection(storm::storage::geometry::Polytope<RationalNumberType>::create(std::move(thresholdConstraints)));
                    if(validSolutions->isEmpty()) {
                        return false;
                    }
                    auto optimizationRes = validSolutions->optimize(directionForOptimization);
                    STORM_LOG_THROW(optimizationRes.second, storm::exceptions::UnexpectedException, "Couldn't check whether thresholds are satisfied for a given polytope: Didn't expect an infeasible or unbounded solution.");
                    
                    for(uint_fast64_t objIndex = 0; objIndex < info.objectives.size(); ++objIndex) {
                        if(!info.objectives[objIndex].threshold) {
                            thresholdPoint[objIndex] = optimizationRes.first[objIndex] - storm::utility::one<RationalNumberType>();
                        }
                    }
                }
                
                
                
                    
                    
                
                // We then check whether the intersection of the thresholdPolytope and the given polytope contains any point (that also satisfies the strict thresholds);
                
                std::cout << "Valid solutions are " << validSolutions->toString(true) << std::endl;
                std::cout << "not empty" << std::endl;
                if(existsObjectiveWithStrictThreshold) {
                    std::cout << "hasObjWithStrictThreshold" << std::endl;
                    std::cout << "optRes is " << storm::utility::vector::convertNumericVector<double>(optimizationRes.first) << std::endl;
                    for(uint_fast64_t objIndex = 0; objIndex < info.objectives.size(); ++objIndex) {
                        if(info.objectives[objIndex].threshold && info.objectives[objIndex].thresholdIsStrict) {
                            RationalNumberType threshold = storm::utility::convertNumber<RationalNumberType>( *info.objectives[objIndex].threshold);
                            if (optimizationRes.first[objIndex] <= threshold) {
                                return false;
                            }
                        }
                    }
                }
                return true;
                
                
                
                Point thresholds;
                thresholds.reserve(info.objectives.size());
                for(auto const& obj : info.objectives) {
                    if(obj.threshold) {
                        thresholds.push_back(storm::utility::convertNumber<RationalNumber>(*obj.threshold));
                    } else {
                        STORM_LOG_ASSERT(numericalQueryResult, "Checking whether thresholds are satisfied for a numerical query but no threshold for the numerical objective was given.");
                        thresholds.push_back(*numericalQueryResult);
                    }
                }
                std::vector<storm::storage::geometry::Halfspace<RationalNumberType>> halfspaces = polytope->getHalfspaces();
                for(auto const& h : halfspaces) {
                    RationalNumberType distance = h.distance(thresholds);
                    if(distance < storm::utility::zero<RationalNumberType>()) {
                        return false;
                    }
                    if(distance == storm::utility::zero<RationalNumberType>()) {
                        // In this case, the thresholds point is on the boundary of the polytope.
                        // Check if this is problematic for the strict thresholds
                        for(uint_fast64_t objIndex = 0; objIndex < info.objectives.size(); ++objIndex) {
                            if(info.objectives[objIndex].thresholdIsStrict && h.normalVector()[objIndex] > storm::utility::zero<RationalNumberType>()) {
                                return false;
                            }
                        }
                    }
                }
                return true;

                
                
            }
            */
            
#ifdef STORM_HAVE_CARL
            template class SparseMultiObjectiveModelCheckerHelper<storm::models::sparse::Mdp<double>, storm::RationalNumber>;
#endif
        }
    }
}
