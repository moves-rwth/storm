#include "src/modelchecker/multiobjective/pcaa/SparsePcaaAchievabilityQuery.h"

#include "src/adapters/CarlAdapter.h"
#include "src/models/sparse/Mdp.h"
#include "src/models/sparse/MarkovAutomaton.h"
#include "src/models/sparse/StandardRewardModel.h"
#include "src/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "src/utility/constants.h"
#include "src/utility/vector.h"
#include "src/settings//SettingsManager.h"
#include "src/settings/modules/GeneralSettings.h"
#include "src/settings/modules/MultiObjectiveSettings.h"

namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            
            template <class SparseModelType, typename GeometryValueType>
            SparsePcaaAchievabilityQuery<SparseModelType, GeometryValueType>::SparsePcaaAchievabilityQuery(SparsePcaaPreprocessorReturnType<SparseModelType>& preprocessorResult) : SparsePcaaQuery<SparseModelType, GeometryValueType>(preprocessorResult) {
                STORM_LOG_ASSERT(preprocessorResult.queryType==SparsePcaaPreprocessorReturnType<SparseModelType>::QueryType::Achievability, "Invalid query Type");
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
            void SparsePcaaAchievabilityQuery<SparseModelType, GeometryValueType>::initializeThresholdData() {
                thresholds.reserve(this->objectives.size());
                strictThresholds = storm::storage::BitVector(this->objectives.size(), false);
                for(uint_fast64_t objIndex = 0; objIndex < this->objectives.size(); ++objIndex) {
                    thresholds.push_back(storm::utility::convertNumber<GeometryValueType>(*this->objectives[objIndex].threshold));
                    strictThresholds.set(objIndex, this->objectives[objIndex].thresholdIsStrict);
                }
            }
            
            template <class SparseModelType, typename GeometryValueType>
            std::unique_ptr<CheckResult> SparsePcaaAchievabilityQuery<SparseModelType, GeometryValueType>::check() {
               
                bool result = this->checkAchievability();
                
                return std::unique_ptr<CheckResult>(new ExplicitQualitativeCheckResult(this->originalModel.getInitialStates().getNextSetIndex(0), result));
                
            }
            
            template <class SparseModelType, typename GeometryValueType>
            bool SparsePcaaAchievabilityQuery<SparseModelType, GeometryValueType>::checkAchievability() {
                // repeatedly refine the over/ under approximation until the threshold point is either in the under approx. or not in the over approx.
                while(!this->maxStepsPerformed()){
                    WeightVector separatingVector = this->findSeparatingVector(thresholds);
                    this->performRefinementStep(std::move(separatingVector));
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
            bool SparsePcaaAchievabilityQuery<SparseModelType, GeometryValueType>::checkIfThresholdsAreSatisfied(std::shared_ptr<storm::storage::geometry::Polytope<GeometryValueType>> const& polytope) {
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
            template class SparsePcaaAchievabilityQuery<storm::models::sparse::Mdp<double>, storm::RationalNumber>;
            template class SparsePcaaAchievabilityQuery<storm::models::sparse::MarkovAutomaton<double>, storm::RationalNumber>;
            
            template class SparsePcaaAchievabilityQuery<storm::models::sparse::Mdp<storm::RationalNumber>, storm::RationalNumber>;
         //   template class SparsePcaaAchievabilityQuery<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>, storm::RationalNumber>;
#endif
        }
    }
}
