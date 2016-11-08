#include "src/modelchecker/multiobjective/pcaa/SparsePcaaParetoQuery.h"

#include "src/adapters/CarlAdapter.h"
#include "src/models/sparse/Mdp.h"
#include "src/models/sparse/MarkovAutomaton.h"
#include "src/models/sparse/StandardRewardModel.h"
#include "src/modelchecker/results/ParetoCurveCheckResult.h"
#include "src/utility/constants.h"
#include "src/utility/vector.h"
#include "src/settings//SettingsManager.h"
#include "src/settings/modules/MultiObjectiveSettings.h"
#include "src/settings/modules/GeneralSettings.h"


namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            template <class SparseModelType, typename GeometryValueType>
            SparsePcaaParetoQuery<SparseModelType, GeometryValueType>::SparsePcaaParetoQuery(SparsePcaaPreprocessorReturnType<SparseModelType>& preprocessorResult) : SparsePcaaQuery<SparseModelType, GeometryValueType>(preprocessorResult) {
                STORM_LOG_ASSERT(preprocessorResult.queryType==SparsePcaaPreprocessorReturnType<SparseModelType>::QueryType::Pareto, "Invalid query Type");
                
                // Set the maximum gap between lower and upper bound of the weightVectorChecker result.
                // This is the maximal edge length of the box we have to consider around each computed point
                // We pick the gap such that the maximal distance between two points within this box is less than the given precision divided by two.
                typename SparseModelType::ValueType gap = storm::utility::convertNumber<typename SparseModelType::ValueType>(storm::settings::getModule<storm::settings::modules::MultiObjectiveSettings>().getPrecision());
                gap /= (storm::utility::one<typename SparseModelType::ValueType>() + storm::utility::one<typename SparseModelType::ValueType>());
                gap /= storm::utility::sqrt(static_cast<typename SparseModelType::ValueType>(this->objectives.size()));
                this->weightVectorChecker->setMaximumLowerUpperBoundGap(gap);
                
            }
            
            template <class SparseModelType, typename GeometryValueType>
            std::unique_ptr<CheckResult> SparsePcaaParetoQuery<SparseModelType, GeometryValueType>::check() {
                
                // refine the approximation
                exploreSetOfAchievablePoints();
                
                // obtain the data for the checkresult
                std::vector<std::vector<typename SparseModelType::ValueType>> paretoOptimalPoints;
                paretoOptimalPoints.reserve(this->refinementSteps.size());
                for(auto const& step : this->refinementSteps) {
                    paretoOptimalPoints.push_back(storm::utility::vector::convertNumericVector<typename SparseModelType::ValueType>(this->transformPointToOriginalModel(step.lowerBoundPoint)));
                }
                return std::unique_ptr<CheckResult>(new ParetoCurveCheckResult<typename SparseModelType::ValueType>(this->originalModel.getInitialStates().getNextSetIndex(0),
                                                                        std::move(paretoOptimalPoints),
                                                                        this->transformPolytopeToOriginalModel(this->underApproximation)->template convertNumberRepresentation<typename SparseModelType::ValueType>(),
                                                                        this->transformPolytopeToOriginalModel(this->overApproximation)->template convertNumberRepresentation<typename SparseModelType::ValueType>()));
                
                
            }
            
            template <class SparseModelType, typename GeometryValueType>
            void SparsePcaaParetoQuery<SparseModelType, GeometryValueType>::exploreSetOfAchievablePoints() {
            
                //First consider the objectives individually
                for(uint_fast64_t objIndex = 0; objIndex<this->objectives.size() && !this->maxStepsPerformed(); ++objIndex) {
                    WeightVector direction(this->objectives.size(), storm::utility::zero<GeometryValueType>());
                    direction[objIndex] = storm::utility::one<GeometryValueType>();
                    this->performRefinementStep(std::move(direction));
                }
                
                while(!this->maxStepsPerformed()) {
                    // Get the halfspace of the underApproximation with maximal distance to a vertex of the overApproximation
                    std::vector<storm::storage::geometry::Halfspace<GeometryValueType>> underApproxHalfspaces = this->underApproximation->getHalfspaces();
                    std::vector<Point> overApproxVertices = this->overApproximation->getVertices();
                    uint_fast64_t farestHalfspaceIndex = underApproxHalfspaces.size();
                    GeometryValueType farestDistance = storm::utility::zero<GeometryValueType>();
                    for(uint_fast64_t halfspaceIndex = 0; halfspaceIndex < underApproxHalfspaces.size(); ++halfspaceIndex) {
                        for(auto const& vertex : overApproxVertices) {
                            GeometryValueType distance = -underApproxHalfspaces[halfspaceIndex].euclideanDistance(vertex);
                            if(distance > farestDistance) {
                                farestHalfspaceIndex = halfspaceIndex;
                                farestDistance = distance;
                            }
                        }
                    }
                    if(farestDistance < storm::utility::convertNumber<GeometryValueType>(storm::settings::getModule<storm::settings::modules::MultiObjectiveSettings>().getPrecision())) {
                        // Goal precision reached!
                        return;
                    }
                    STORM_LOG_DEBUG("Current precision of the approximation of the pareto curve is ~" << storm::utility::convertNumber<double>(farestDistance));
                    WeightVector direction = underApproxHalfspaces[farestHalfspaceIndex].normalVector();
                    this->performRefinementStep(std::move(direction));
                }
                STORM_LOG_ERROR("Could not reach the desired precision: Exceeded maximum number of refinement steps");
            }
            
            

            
#ifdef STORM_HAVE_CARL
            template class SparsePcaaParetoQuery<storm::models::sparse::Mdp<double>, storm::RationalNumber>;
            template class SparsePcaaParetoQuery<storm::models::sparse::MarkovAutomaton<double>, storm::RationalNumber>;
            
            template class SparsePcaaParetoQuery<storm::models::sparse::Mdp<storm::RationalNumber>, storm::RationalNumber>;
          //  template class SparsePcaaParetoQuery<storm::models::sparse::MarkovAutomaton<storm::RationalNumber>, storm::RationalNumber>;
#endif
        }
    }
}
