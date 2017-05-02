#include "storm/modelchecker/multiobjective/pcaa/SparsePcaaParetoQuery.h"

#include "storm/adapters/CarlAdapter.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/modelchecker/results/ExplicitParetoCurveCheckResult.h"
#include "storm/utility/constants.h"
#include "storm/utility/vector.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/MultiObjectiveSettings.h"
#include "storm/settings/modules/GeneralSettings.h"


namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            template <class SparseModelType, typename GeometryValueType>
            SparsePcaaParetoQuery<SparseModelType, GeometryValueType>::SparsePcaaParetoQuery(SparseMultiObjectivePreprocessorReturnType<SparseModelType>& preprocessorResult) : SparsePcaaQuery<SparseModelType, GeometryValueType>(preprocessorResult) {
                STORM_LOG_ASSERT(preprocessorResult.queryType==SparseMultiObjectivePreprocessorReturnType<SparseModelType>::QueryType::Pareto, "Invalid query Type");
                
                // Set the precision of the weight vector checker
                typename SparseModelType::ValueType weightedPrecision = storm::utility::convertNumber<typename SparseModelType::ValueType>(storm::settings::getModule<storm::settings::modules::MultiObjectiveSettings>().getPrecision());
                weightedPrecision /= storm::utility::sqrt(storm::utility::convertNumber<typename SparseModelType::ValueType, uint_fast64_t>(this->objectives.size()));
                // multiobjPrecision / sqrt(numObjectives) is the largest possible value for which termination is guaranteed.
                // Lets be a little bit more precise to reduce the number of required iterations.
                weightedPrecision *= storm::utility::convertNumber<typename SparseModelType::ValueType>(0.9);
                this->weightVectorChecker->setWeightedPrecision(weightedPrecision);
                
            }
            
            template <class SparseModelType, typename GeometryValueType>
            std::unique_ptr<CheckResult> SparsePcaaParetoQuery<SparseModelType, GeometryValueType>::check() {
                
                // refine the approximation
                exploreSetOfAchievablePoints();
                
                // obtain the data for the checkresult
                std::vector<std::vector<typename SparseModelType::ValueType>> paretoOptimalPoints;
                std::vector<Point> vertices = this->underApproximation->getVertices();
                paretoOptimalPoints.reserve(vertices.size());
                for(auto const& vertex : vertices) {
                    paretoOptimalPoints.push_back(storm::utility::vector::convertNumericVector<typename SparseModelType::ValueType>(this->transformPointToOriginalModel(vertex)));
                }
                return std::unique_ptr<CheckResult>(new ExplicitParetoCurveCheckResult<typename SparseModelType::ValueType>(this->originalModel.getInitialStates().getNextSetIndex(0),
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
                            GeometryValueType distance = underApproxHalfspaces[halfspaceIndex].euclideanDistance(vertex);
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
                    STORM_LOG_INFO("Current precision of the approximation of the pareto curve is ~" << storm::utility::convertNumber<double>(farestDistance));
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
