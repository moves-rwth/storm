#include "src/modelchecker/multiobjective/helper/SparseMultiObjectiveHelper.h"

#include "src/adapters/CarlAdapter.h"
#include "src/models/sparse/Mdp.h"
#include "src/models/sparse/MarkovAutomaton.h"
#include "src/models/sparse/StandardRewardModel.h"
#include "src/modelchecker/multiobjective/helper/SparseMultiObjectiveObjectiveInformation.h"
#include "src/utility/constants.h"
#include "src/utility/vector.h"
#include "src/settings//SettingsManager.h"
#include "src/settings/modules/MultiObjectiveSettings.h"

#include "src/exceptions/UnexpectedException.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            
            
            template <class SparseModelType, typename RationalNumberType>
            typename SparseMultiObjectiveHelper<SparseModelType, RationalNumberType>::ResultData SparseMultiObjectiveHelper<SparseModelType, RationalNumberType>::check(PreprocessorData const& preprocessorData) {
                ResultData resultData;
                resultData.overApproximation() = storm::storage::geometry::Polytope<RationalNumberType>::createUniversalPolytope();
                resultData.underApproximation() = storm::storage::geometry::Polytope<RationalNumberType>::createEmptyPolytope();
                
                if(!checkIfPreprocessingWasConclusive(preprocessorData)) {
                    switch(preprocessorData.queryType) {
                        case PreprocessorData::QueryType::Achievability:
                            achievabilityQuery(preprocessorData, resultData);
                            break;
                        case PreprocessorData::QueryType::Numerical:
                            numericalQuery(preprocessorData, resultData);
                            break;
                        case PreprocessorData::QueryType::Pareto:
                            paretoQuery(preprocessorData, resultData);
                            break;
                        default:
                            STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unknown Query Type");
                    }
                }
                return resultData;
            }
            
            template <class SparseModelType, typename RationalNumberType>
            bool SparseMultiObjectiveHelper<SparseModelType, RationalNumberType>::checkIfPreprocessingWasConclusive(PreprocessorData const& preprocessorData) {
                if(preprocessorData.objectives.empty()) {
                    return true;
                }
                for(auto const& preprocessorResult : preprocessorData.solutionsFromPreprocessing) {
                    if(preprocessorResult == PreprocessorData::PreprocessorObjectiveSolution::False ||
                       preprocessorResult == PreprocessorData::PreprocessorObjectiveSolution::Undefined) {
                        return true;
                    }
                }
                return false;
            }
            
            template <class SparseModelType, typename RationalNumberType>
            void SparseMultiObjectiveHelper<SparseModelType, RationalNumberType>::achievabilityQuery(PreprocessorData const& preprocessorData, ResultData& resultData) {
                Point thresholds;
                thresholds.reserve(preprocessorData.objectives.size());
                storm::storage::BitVector strictThresholds(preprocessorData.objectives.size());
                for(uint_fast64_t objIndex = 0; objIndex < preprocessorData.objectives.size(); ++objIndex) {
                    thresholds.push_back(storm::utility::convertNumber<RationalNumberType>(*preprocessorData.objectives[objIndex].threshold));
                    strictThresholds.set(objIndex, preprocessorData.objectives[objIndex].thresholdIsStrict);
                }
                
                storm::storage::BitVector individualObjectivesToBeChecked(preprocessorData.objectives.size(), true);
                SparseMultiObjectiveWeightVectorChecker<SparseModelType> weightVectorChecker(preprocessorData);
                do {
                    WeightVector separatingVector = findSeparatingVector(thresholds, resultData.underApproximation(), individualObjectivesToBeChecked);
                    performRefinementStep(separatingVector, preprocessorData.produceSchedulers, weightVectorChecker, resultData);
                    if(!checkIfThresholdsAreSatisfied(resultData.overApproximation(), thresholds, strictThresholds)){
                        resultData.setThresholdsAreAchievable(false);
                    }
                    if(checkIfThresholdsAreSatisfied(resultData.underApproximation(), thresholds, strictThresholds)){
                        resultData.setThresholdsAreAchievable(true);
                    }
                } while(!resultData.isThresholdsAreAchievableSet() && !maxStepsPerformed(resultData));
            }
            
            template <class SparseModelType, typename RationalNumberType>
            void SparseMultiObjectiveHelper<SparseModelType, RationalNumberType>::numericalQuery(PreprocessorData const& preprocessorData, ResultData& resultData) {
                STORM_LOG_ASSERT(preprocessorData.indexOfOptimizingObjective, "Detected numerical query but index of optimizing objective is not set.");
                uint_fast64_t optimizingObjIndex = *preprocessorData.indexOfOptimizingObjective;
                Point thresholds;
                thresholds.reserve(preprocessorData.objectives.size());
                storm::storage::BitVector strictThresholds(preprocessorData.objectives.size(), false);
                std::vector<storm::storage::geometry::Halfspace<RationalNumberType>> thresholdConstraints;
                thresholdConstraints.reserve(preprocessorData.objectives.size()-1);
                for(uint_fast64_t objIndex = 0; objIndex < preprocessorData.objectives.size(); ++objIndex) {
                    if(preprocessorData.objectives[objIndex].threshold) {
                        thresholds.push_back(storm::utility::convertNumber<RationalNumberType>(*preprocessorData.objectives[objIndex].threshold));
                        WeightVector normalVector(preprocessorData.objectives.size(), storm::utility::zero<RationalNumberType>());
                        normalVector[objIndex] = -storm::utility::one<RationalNumberType>();
                        thresholdConstraints.emplace_back(std::move(normalVector), -thresholds.back());
                        strictThresholds.set(objIndex, preprocessorData.objectives[objIndex].thresholdIsStrict);
                    } else {
                        thresholds.push_back(storm::utility::zero<RationalNumberType>());
                    }
                }
                // Note: If we have a single objective (i.e., no objectives with thresholds), thresholdsAsPolytope gets no constraints
                auto thresholdsAsPolytope = storm::storage::geometry::Polytope<RationalNumberType>::create(thresholdConstraints);
                WeightVector directionOfOptimizingObjective(preprocessorData.objectives.size(), storm::utility::zero<RationalNumberType>());
                directionOfOptimizingObjective[optimizingObjIndex] = storm::utility::one<RationalNumberType>();
                
                // Try to find one valid solution
                storm::storage::BitVector individualObjectivesToBeChecked(preprocessorData.objectives.size(), true);
                individualObjectivesToBeChecked.set(optimizingObjIndex, false);
                SparseMultiObjectiveWeightVectorChecker<SparseModelType> weightVectorChecker(preprocessorData);
                do {
                    WeightVector separatingVector = findSeparatingVector(thresholds, resultData.underApproximation(), individualObjectivesToBeChecked);
                    performRefinementStep(separatingVector, preprocessorData.produceSchedulers, weightVectorChecker, resultData);
                    //Pick the threshold for the optimizing objective low enough so valid solutions are not excluded
                    thresholds[optimizingObjIndex] = std::min(thresholds[optimizingObjIndex], resultData.refinementSteps().back().getPoint()[optimizingObjIndex]);
                    if(!checkIfThresholdsAreSatisfied(resultData.overApproximation(), thresholds, strictThresholds)){
                        resultData.setThresholdsAreAchievable(false);
                    }
                    if(checkIfThresholdsAreSatisfied(resultData.underApproximation(), thresholds, strictThresholds)){
                        resultData.setThresholdsAreAchievable(true);
                    }
                } while(!resultData.isThresholdsAreAchievableSet() && !maxStepsPerformed(resultData));
                if(resultData.getThresholdsAreAchievable()) {
                    STORM_LOG_DEBUG("Found a solution that satisfies the objective thresholds.");
                    individualObjectivesToBeChecked.clear();
                    // Improve the found solution.
                    // Note that we do not have to care whether a threshold is strict anymore, because the resulting optimum should be
                    // the supremum over all strategies. Hence, one could combine a scheduler inducing the optimum value (but possibly violating strict
                    // thresholds) and (with very low probability) a scheduler that satisfies all (possibly strict) thresholds.
                    while(true) {
                        std::pair<Point, bool> optimizationRes = resultData.underApproximation()->intersection(thresholdsAsPolytope)->optimize(directionOfOptimizingObjective);
                        STORM_LOG_THROW(optimizationRes.second, storm::exceptions::UnexpectedException, "The underapproximation is either unbounded or empty.");
                        thresholds[optimizingObjIndex] = optimizationRes.first[optimizingObjIndex];
                        resultData.setNumericalResult(thresholds[optimizingObjIndex]);
                        STORM_LOG_DEBUG("Best solution found so far is " << resultData.template getNumericalResult<double>() << ".");
                        //Compute an upper bound for the optimum and check for convergence
                        optimizationRes = resultData.overApproximation()->intersection(thresholdsAsPolytope)->optimize(directionOfOptimizingObjective);
                        if(optimizationRes.second) {
                            resultData.setPrecisionOfResult(optimizationRes.first[optimizingObjIndex] - thresholds[optimizingObjIndex]);
                            STORM_LOG_DEBUG("Solution can be improved by at most " << resultData.template getPrecisionOfResult<double>());
                        }
                        if(targetPrecisionReached(resultData) || maxStepsPerformed(resultData)) {
                            resultData.setOptimumIsAchievable(checkIfThresholdsAreSatisfied(resultData.underApproximation(), thresholds, strictThresholds));
                            break;
                        }
                        WeightVector separatingVector = findSeparatingVector(thresholds, resultData.underApproximation(), individualObjectivesToBeChecked);
                        performRefinementStep(separatingVector, preprocessorData.produceSchedulers, weightVectorChecker, resultData);
                    }
                }
            }
            
            template <class SparseModelType, typename RationalNumberType>
            void SparseMultiObjectiveHelper<SparseModelType, RationalNumberType>::paretoQuery(PreprocessorData const& preprocessorData, ResultData& resultData) {
                SparseMultiObjectiveWeightVectorChecker<SparseModelType> weightVectorChecker(preprocessorData);
                //First consider the objectives individually
                for(uint_fast64_t objIndex = 0; objIndex<preprocessorData.objectives.size() && !maxStepsPerformed(resultData); ++objIndex) {
                    WeightVector direction(preprocessorData.objectives.size(), storm::utility::zero<RationalNumberType>());
                    direction[objIndex] = storm::utility::one<RationalNumberType>();
                    performRefinementStep(direction, preprocessorData.produceSchedulers, weightVectorChecker, resultData);
                }
                
                while(true) {
                    // Get the halfspace of the underApproximation with maximal distance to a vertex of the overApproximation
                    std::vector<storm::storage::geometry::Halfspace<RationalNumberType>> underApproxHalfspaces = resultData.underApproximation()->getHalfspaces();
                    std::vector<Point> overApproxVertices = resultData.overApproximation()->getVertices();
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
                    resultData.setPrecisionOfResult(farestDistance);
                    STORM_LOG_DEBUG("Current precision of the approximation of the pareto curve is " << resultData.template getPrecisionOfResult<double>());
                    if(targetPrecisionReached(resultData) || maxStepsPerformed(resultData)) {
                        break;
                    }
                    WeightVector direction = underApproxHalfspaces[farestHalfspaceIndex].normalVector();
                    performRefinementStep(direction, preprocessorData.produceSchedulers, weightVectorChecker, resultData);
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
            void SparseMultiObjectiveHelper<SparseModelType, RationalNumberType>::performRefinementStep(WeightVector const& direction, bool saveScheduler, SparseMultiObjectiveWeightVectorChecker<SparseModelType>& weightVectorChecker, ResultData& result) {
                weightVectorChecker.check(storm::utility::vector::convertNumericVector<typename SparseModelType::ValueType>(direction));
                STORM_LOG_DEBUG("weighted objectives checker result is " << storm::utility::vector::convertNumericVector<double>(weightVectorChecker.getInitialStateResultOfObjectives()));
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
                std::vector<Point> paretoPoints;
                paretoPoints.reserve(refinementSteps.size());
                for(auto const& step : refinementSteps) {
                    paretoPoints.push_back(step.getPoint());
                }
                underApproximation = storm::storage::geometry::Polytope<RationalNumberType>::createDownwardClosure(paretoPoints);
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
            bool SparseMultiObjectiveHelper<SparseModelType, RationalNumberType>::targetPrecisionReached(ResultData& resultData) {
                resultData.setTargetPrecisionReached(resultData.isPrecisionOfResultSet() &&
                                                 resultData.getPrecisionOfResult() < storm::utility::convertNumber<RationalNumberType>(storm::settings::getModule<storm::settings::modules::MultiObjectiveSettings>().getPrecision()));
                return resultData.getTargetPrecisionReached();
            }
            
            template <class SparseModelType, typename RationalNumberType>
            bool SparseMultiObjectiveHelper<SparseModelType, RationalNumberType>::maxStepsPerformed(ResultData& resultData) {
                resultData.setMaxStepsPerformed(storm::settings::getModule<storm::settings::modules::MultiObjectiveSettings>().isMaxStepsSet() &&
                                            resultData.refinementSteps().size() >= storm::settings::getModule<storm::settings::modules::MultiObjectiveSettings>().getMaxSteps());
                return resultData.getMaxStepsPerformed();
            }
            
#ifdef STORM_HAVE_CARL
            template class SparseMultiObjectiveHelper<storm::models::sparse::Mdp<double>, storm::RationalNumber>;
            template class SparseMultiObjectiveHelper<storm::models::sparse::MarkovAutomaton<double>, storm::RationalNumber>;
            
            template class SparseMultiObjectiveHelper<storm::models::sparse::Mdp<storm::RationalNumber>, storm::RationalNumber>;
            template class SparseMultiObjectiveHelper<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>, storm::RationalNumber>;
#endif
        }
    }
}
