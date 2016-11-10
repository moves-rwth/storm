#include "src/modelchecker/multiobjective/pcaa/SparsePcaaQuantitativeQuery.h"

#include "src/adapters/CarlAdapter.h"
#include "src/models/sparse/Mdp.h"
#include "src/models/sparse/MarkovAutomaton.h"
#include "src/models/sparse/StandardRewardModel.h"
#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "src/utility/constants.h"
#include "src/utility/vector.h"
#include "src/settings//SettingsManager.h"
#include "src/settings/modules/MultiObjectiveSettings.h"
#include "src/settings/modules/GeneralSettings.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            
            template <class SparseModelType, typename GeometryValueType>
            SparsePcaaQuantitativeQuery<SparseModelType, GeometryValueType>::SparsePcaaQuantitativeQuery(SparsePcaaPreprocessorReturnType<SparseModelType>& preprocessorResult) : SparsePcaaQuery<SparseModelType, GeometryValueType>(preprocessorResult) {
                STORM_LOG_ASSERT(preprocessorResult.queryType==SparsePcaaPreprocessorReturnType<SparseModelType>::QueryType::Quantitative, "Invalid query Type");
                STORM_LOG_ASSERT(preprocessorResult.indexOfOptimizingObjective, "Detected quantitative query but index of optimizing objective is not set.");
                indexOfOptimizingObjective = *preprocessorResult.indexOfOptimizingObjective;
                initializeThresholdData();
                
                // Set the maximum gap between lower and upper bound of the weightVectorChecker result.
                // This is the maximal edge length of the box we have to consider around each computed point
                // We pick the gap such that the maximal distance between two points within this box is less than the given precision divided by two.
                typename SparseModelType::ValueType gap = storm::utility::convertNumber<typename SparseModelType::ValueType>(storm::settings::getModule<storm::settings::modules::MultiObjectiveSettings>().getPrecision());
                gap /= (storm::utility::one<typename SparseModelType::ValueType>() + storm::utility::one<typename SparseModelType::ValueType>());
                gap /= storm::utility::sqrt(static_cast<typename SparseModelType::ValueType>(this->objectives.size()));
                this->weightVectorChecker->setMaximumLowerUpperBoundGap(gap);
                
            }
            
            template <class SparseModelType, typename GeometryValueType>
            void SparsePcaaQuantitativeQuery<SparseModelType, GeometryValueType>::initializeThresholdData() {
                thresholds.reserve(this->objectives.size());
                strictThresholds = storm::storage::BitVector(this->objectives.size(), false);
                std::vector<storm::storage::geometry::Halfspace<GeometryValueType>> thresholdConstraints;
                thresholdConstraints.reserve(this->objectives.size()-1);
                for(uint_fast64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                    if(this->objectives[objIndex].threshold) {
                        thresholds.push_back(storm::utility::convertNumber<GeometryValueType>(*this->objectives[objIndex].threshold));
                        WeightVector normalVector(this->objectives.size(), storm::utility::zero<GeometryValueType>());
                        normalVector[objIndex] = -storm::utility::one<GeometryValueType>();
                        thresholdConstraints.emplace_back(std::move(normalVector), -thresholds.back());
                        strictThresholds.set(objIndex, this->objectives[objIndex].thresholdIsStrict);
                    } else {
                        thresholds.push_back(storm::utility::zero<GeometryValueType>());
                    }
                }
                // Note: If we have a single objective (i.e., no objectives with thresholds), thresholdsAsPolytope gets no constraints
                thresholdsAsPolytope = storm::storage::geometry::Polytope<GeometryValueType>::create(thresholdConstraints);
            }
            
            template <class SparseModelType, typename GeometryValueType>
            std::unique_ptr<CheckResult> SparsePcaaQuantitativeQuery<SparseModelType, GeometryValueType>::check() {
               
                
                // First find one solution that achieves the given thresholds ...
                if(this->checkAchievability()) {
                    // ... then improve it
                    GeometryValueType result = this->improveSolution();
                    
                    // transform the obtained result for the preprocessed model to a result w.r.t. the original model and return the checkresult
                    typename SparseModelType::ValueType resultForOriginalModel =
                            storm::utility::convertNumber<typename SparseModelType::ValueType>(result) *
                            this->objectives[indexOfOptimizingObjective].toOriginalValueTransformationFactor +
                            this->objectives[indexOfOptimizingObjective].toOriginalValueTransformationOffset;
                    return std::unique_ptr<CheckResult>(new ExplicitQuantitativeCheckResult<typename SparseModelType::ValueType>(this->originalModel.getInitialStates().getNextSetIndex(0), resultForOriginalModel));
                } else {
                    return std::unique_ptr<CheckResult>(new ExplicitQualitativeCheckResult(this->originalModel.getInitialStates().getNextSetIndex(0), false));
                }
     
                
            }
            
            template <class SparseModelType, typename GeometryValueType>
            bool SparsePcaaQuantitativeQuery<SparseModelType, GeometryValueType>::checkAchievability() {
                // We don't care for the optimizing objective at this point
                this->diracWeightVectorsToBeChecked.set(indexOfOptimizingObjective, false);
                
                while(!this->maxStepsPerformed()){
                    WeightVector separatingVector = this->findSeparatingVector(thresholds);
                    this->performRefinementStep(std::move(separatingVector));
                    //Pick the threshold for the optimizing objective low enough so valid solutions are not excluded
                    thresholds[indexOfOptimizingObjective] = std::min(thresholds[indexOfOptimizingObjective], this->refinementSteps.back().lowerBoundPoint[indexOfOptimizingObjective]);
                    if(!checkIfThresholdsAreSatisfied(this->overApproximation)){
                        return false;
                    }
                    if(checkIfThresholdsAreSatisfied(this->underApproximation)){
                        return true;
                    }
                }
                STORM_LOG_ERROR("Could not check whether thresholds are achievable: Exceeded maximum number of refinement steps");
                return false;
            }

            template <class SparseModelType, typename GeometryValueType>
            GeometryValueType SparsePcaaQuantitativeQuery<SparseModelType, GeometryValueType>::improveSolution() {
                this->diracWeightVectorsToBeChecked.clear(); // Only check weight vectors that can actually improve the solution
            
                WeightVector directionOfOptimizingObjective(this->objectives.size(), storm::utility::zero<GeometryValueType>());
                directionOfOptimizingObjective[indexOfOptimizingObjective] = storm::utility::one<GeometryValueType>();
            
                // Improve the found solution.
                // Note that we do not care whether a threshold is strict anymore, because the resulting optimum should be
                // the supremum over all strategies. Hence, one could combine a scheduler inducing the optimum value (but possibly violating strict
                // thresholds) and (with very low probability) a scheduler that satisfies all (possibly strict) thresholds.
                GeometryValueType result = storm::utility::zero<GeometryValueType>();
                while(!this->maxStepsPerformed()) {
                    std::pair<Point, bool> optimizationRes = this->underApproximation->intersection(thresholdsAsPolytope)->optimize(directionOfOptimizingObjective);
                    STORM_LOG_THROW(optimizationRes.second, storm::exceptions::UnexpectedException, "The underapproximation is either unbounded or empty.");
                    result = optimizationRes.first[indexOfOptimizingObjective];
                    STORM_LOG_DEBUG("Best solution found so far is ~" << storm::utility::convertNumber<double>(result) << ".");
                    //Compute an upper bound for the optimum and check for convergence
                    optimizationRes = this->overApproximation->intersection(thresholdsAsPolytope)->optimize(directionOfOptimizingObjective);
                    if(optimizationRes.second) {
                        GeometryValueType precisionOfResult = optimizationRes.first[indexOfOptimizingObjective] - result;
                        if(precisionOfResult < storm::utility::convertNumber<GeometryValueType>(storm::settings::getModule<storm::settings::modules::MultiObjectiveSettings>().getPrecision())) {
                            // Goal precision reached!
                            return result;
                        } else {
                            STORM_LOG_DEBUG("Solution can be improved by at most " << storm::utility::convertNumber<double>(precisionOfResult));
                            thresholds[indexOfOptimizingObjective] = optimizationRes.first[indexOfOptimizingObjective];
                        }
                    } else {
                        thresholds[indexOfOptimizingObjective] = result + storm::utility::one<GeometryValueType>();
                    }
                    WeightVector separatingVector = this->findSeparatingVector(thresholds);
                    this->performRefinementStep(std::move(separatingVector));
                }
               STORM_LOG_ERROR("Could not reach the desired precision: Exceeded maximum number of refinement steps");
               return result;
            }
            
            template <class SparseModelType, typename GeometryValueType>
            bool SparsePcaaQuantitativeQuery<SparseModelType, GeometryValueType>::checkIfThresholdsAreSatisfied(std::shared_ptr<storm::storage::geometry::Polytope<GeometryValueType>> const& polytope) {
                std::vector<storm::storage::geometry::Halfspace<GeometryValueType>> halfspaces = polytope->getHalfspaces();
                for(auto const& h : halfspaces) {
                    GeometryValueType distance = h.distance(thresholds);
                    if(distance < storm::utility::zero<GeometryValueType>()) {
                        return false;
                    }
                    if(distance == storm::utility::zero<GeometryValueType>()) {
                        // In this case, the thresholds point is on the boundary of the polytope.
                        // Check if this is problematic for the strict thresholds
                        for(auto strictThreshold : strictThresholds) {
                            if(h.normalVector()[strictThreshold] > storm::utility::zero<GeometryValueType>()) {
                                return false;
                            }
                        }
                    }
                }
                return true;
            }
            

            
#ifdef STORM_HAVE_CARL
            template class SparsePcaaQuantitativeQuery<storm::models::sparse::Mdp<double>, storm::RationalNumber>;
            template class SparsePcaaQuantitativeQuery<storm::models::sparse::MarkovAutomaton<double>, storm::RationalNumber>;
            
            template class SparsePcaaQuantitativeQuery<storm::models::sparse::Mdp<storm::RationalNumber>, storm::RationalNumber>;
          //  template class SparsePcaaQuantitativeQuery<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>, storm::RationalNumber>;
#endif
        }
    }
}
