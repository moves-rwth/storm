#include "storm-pars/modelchecker/region/SparseParameterLiftingModelChecker.h"

#include <queue>

#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/logic/FragmentSpecification.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/utility/vector.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/StandardRewardModel.h"

#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/NotSupportedException.h"

namespace storm {
    namespace modelchecker {
        
        template <typename SparseModelType, typename ConstantType>
        SparseParameterLiftingModelChecker<SparseModelType, ConstantType>::SparseParameterLiftingModelChecker() {
            //Intentionally left empty
        }
        
        template <typename SparseModelType, typename ConstantType>
        void SparseParameterLiftingModelChecker<SparseModelType, ConstantType>::specifyFormula(Environment const& env, storm::modelchecker::CheckTask<storm::logic::Formula, typename SparseModelType::ValueType> const& checkTask) {

            currentFormula = checkTask.getFormula().asSharedPointer();
            currentCheckTask = std::make_unique<storm::modelchecker::CheckTask<storm::logic::Formula, ConstantType>>(checkTask.substituteFormula(*currentFormula).template convertValueType<ConstantType>());
            
            if(currentCheckTask->getFormula().isProbabilityOperatorFormula()) {
                auto const& probOpFormula = currentCheckTask->getFormula().asProbabilityOperatorFormula();
                if(probOpFormula.getSubformula().isBoundedUntilFormula()) {
                    specifyBoundedUntilFormula(env, currentCheckTask->substituteFormula(probOpFormula.getSubformula().asBoundedUntilFormula()));
                } else if(probOpFormula.getSubformula().isUntilFormula()) {
                    specifyUntilFormula(env, currentCheckTask->substituteFormula(probOpFormula.getSubformula().asUntilFormula()));
                } else if (probOpFormula.getSubformula().isEventuallyFormula()) {
                    specifyReachabilityProbabilityFormula(env, currentCheckTask->substituteFormula(probOpFormula.getSubformula().asEventuallyFormula()));
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Parameter lifting is not supported for the given property.");
                }
            } else if (currentCheckTask->getFormula().isRewardOperatorFormula()) {
                auto const& rewOpFormula = currentCheckTask->getFormula().asRewardOperatorFormula();
                if(rewOpFormula.getSubformula().isEventuallyFormula()) {
                    specifyReachabilityRewardFormula(env, currentCheckTask->substituteFormula(rewOpFormula.getSubformula().asEventuallyFormula()));
                } else if (rewOpFormula.getSubformula().isCumulativeRewardFormula()) {
                    specifyCumulativeRewardFormula(env, currentCheckTask->substituteFormula(rewOpFormula.getSubformula().asCumulativeRewardFormula()));
                }
            }
        }
        
        template <typename SparseModelType, typename ConstantType>
        RegionResult SparseParameterLiftingModelChecker<SparseModelType, ConstantType>::analyzeRegion(Environment const& env, storm::storage::ParameterRegion<typename SparseModelType::ValueType> const& region, RegionResultHypothesis const& hypothesis, RegionResult const& initialResult, bool sampleVerticesOfRegion) {

            STORM_LOG_THROW(this->currentCheckTask->isOnlyInitialStatesRelevantSet(), storm::exceptions::NotSupportedException, "Analyzing regions with parameter lifting requires a property where only the value in the initial states is relevant.");
            STORM_LOG_THROW(this->currentCheckTask->isBoundSet(), storm::exceptions::NotSupportedException, "Analyzing regions with parameter lifting requires a bounded property.");
            STORM_LOG_THROW(this->parametricModel->getInitialStates().getNumberOfSetBits() == 1, storm::exceptions::NotSupportedException, "Analyzing regions with parameter lifting requires a model with a single initial state.");
            
            RegionResult result = initialResult;

            // Check if we need to check the formula on one point to decide whether to show AllSat or AllViolated
            if (hypothesis == RegionResultHypothesis::Unknown && result == RegionResult::Unknown) {
                 result = getInstantiationChecker().check(env, region.getCenterPoint())->asExplicitQualitativeCheckResult()[*this->parametricModel->getInitialStates().begin()] ? RegionResult::CenterSat : RegionResult::CenterViolated;
            }
            
            // try to prove AllSat or AllViolated, depending on the hypothesis or the current result
            if (hypothesis == RegionResultHypothesis::AllSat || result == RegionResult::ExistsSat || result == RegionResult::CenterSat) {
                // show AllSat:
                storm::solver::OptimizationDirection parameterOptimizationDirection = isLowerBound(this->currentCheckTask->getBound().comparisonType) ? storm::solver::OptimizationDirection::Minimize : storm::solver::OptimizationDirection::Maximize;
                if (this->check(env, region, parameterOptimizationDirection)->asExplicitQualitativeCheckResult()[*this->parametricModel->getInitialStates().begin()]) {
                    result = RegionResult::AllSat;
                } else if (sampleVerticesOfRegion) {
                    result = sampleVertices(env, region, result);
                }
            } else if (hypothesis == RegionResultHypothesis::AllViolated || result == RegionResult::ExistsViolated || result == RegionResult::CenterViolated) {
                // show AllViolated:
                storm::solver::OptimizationDirection parameterOptimizationDirection = isLowerBound(this->currentCheckTask->getBound().comparisonType) ? storm::solver::OptimizationDirection::Maximize : storm::solver::OptimizationDirection::Minimize;
                if (!this->check(env, region, parameterOptimizationDirection)->asExplicitQualitativeCheckResult()[*this->parametricModel->getInitialStates().begin()]) {
                    result = RegionResult::AllViolated;
                } else if (sampleVerticesOfRegion) {
                    result = sampleVertices(env, region, result);
                }
            } else {
                STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "When analyzing a region, an invalid initial result was given: " << initialResult);
            }
            return result;
        }
        
        template <typename SparseModelType, typename ConstantType>
        RegionResult SparseParameterLiftingModelChecker<SparseModelType, ConstantType>::sampleVertices(Environment const& env, storm::storage::ParameterRegion<typename SparseModelType::ValueType> const& region, RegionResult const& initialResult) {
            RegionResult result = initialResult;
            
            if (result == RegionResult::AllSat || result == RegionResult::AllViolated) {
                return result;
            }
            
            bool hasSatPoint = result == RegionResult::ExistsSat || result == RegionResult::CenterSat;
            bool hasViolatedPoint = result == RegionResult::ExistsViolated || result == RegionResult::CenterViolated;
            
            // Check if there is a point in the region for which the property is satisfied
            auto vertices = region.getVerticesOfRegion(region.getVariables());
            auto vertexIt = vertices.begin();
            while (vertexIt != vertices.end() && !(hasSatPoint && hasViolatedPoint)) {
                if (getInstantiationChecker().check(env, *vertexIt)->asExplicitQualitativeCheckResult()[*this->parametricModel->getInitialStates().begin()]) {
                    hasSatPoint = true;
                } else {
                    hasViolatedPoint = true;
                }
                ++vertexIt;
            }
            
            if (hasSatPoint) {
                if (hasViolatedPoint) {
                    result = RegionResult::ExistsBoth;
                } else if (result != RegionResult::CenterSat) {
                    result = RegionResult::ExistsSat;
                }
            } else if (hasViolatedPoint && result != RegionResult::CenterViolated) {
                result = RegionResult::ExistsViolated;
            }
            
            return result;
        }


        template <typename SparseModelType, typename ConstantType>
        std::unique_ptr<CheckResult> SparseParameterLiftingModelChecker<SparseModelType, ConstantType>::check(Environment const& env, storm::storage::ParameterRegion<typename SparseModelType::ValueType> const& region, storm::solver::OptimizationDirection const& dirForParameters) {
            auto quantitativeResult = computeQuantitativeValues(env, region, dirForParameters);
            if(currentCheckTask->getFormula().hasQuantitativeResult()) {
                return quantitativeResult;
            } else {
                return quantitativeResult->template asExplicitQuantitativeCheckResult<ConstantType>().compareAgainstBound(this->currentCheckTask->getFormula().asOperatorFormula().getComparisonType(), this->currentCheckTask->getFormula().asOperatorFormula().template getThresholdAs<ConstantType>());
            }
        }
        
        template <typename SparseModelType, typename ConstantType>
        std::unique_ptr<QuantitativeCheckResult<ConstantType>> SparseParameterLiftingModelChecker<SparseModelType, ConstantType>::getBound(Environment const& env, storm::storage::ParameterRegion<typename SparseModelType::ValueType> const& region, storm::solver::OptimizationDirection const& dirForParameters) {
            STORM_LOG_WARN_COND(this->currentCheckTask->getFormula().hasQuantitativeResult(), "Computing quantitative bounds for a qualitative formula...");
            return std::make_unique<ExplicitQuantitativeCheckResult<ConstantType>>(std::move(computeQuantitativeValues(env, region, dirForParameters)->template asExplicitQuantitativeCheckResult<ConstantType>()));
        }

        template <typename SparseModelType, typename ConstantType>
        typename SparseModelType::ValueType SparseParameterLiftingModelChecker<SparseModelType, ConstantType>::getBoundAtInitState(Environment const& env, storm::storage::ParameterRegion<typename SparseModelType::ValueType> const& region, storm::solver::OptimizationDirection const& dirForParameters) {
            STORM_LOG_THROW(this->parametricModel->getInitialStates().getNumberOfSetBits() == 1, storm::exceptions::NotSupportedException, "Getting a bound at the initial state requires a model with a single initial state.");
            return storm::utility::convertNumber<typename SparseModelType::ValueType>(getBound(env, region, dirForParameters)->template asExplicitQuantitativeCheckResult<ConstantType>()[*this->parametricModel->getInitialStates().begin()]);
        }
        
        template <typename SparseModelType, typename ConstantType>
        struct RegionBound {
            RegionBound(RegionBound<SparseModelType, ConstantType> const& other) = default;
            RegionBound(storm::storage::ParameterRegion<typename SparseModelType::ValueType> const& r, ConstantType const& b) : region(r), bound(b) {}
            storm::storage::ParameterRegion<typename SparseModelType::ValueType> region;
            ConstantType bound;
        };
        
        template <typename SparseModelType, typename ConstantType>
        std::pair<typename SparseModelType::ValueType, typename storm::storage::ParameterRegion<typename SparseModelType::ValueType>::Valuation> SparseParameterLiftingModelChecker<SparseModelType, ConstantType>::computeExtremalValue(Environment const& env, storm::storage::ParameterRegion<typename SparseModelType::ValueType> const& region, storm::solver::OptimizationDirection const& dir, typename SparseModelType::ValueType const& precision) {
            STORM_LOG_THROW(this->parametricModel->getInitialStates().getNumberOfSetBits() == 1, storm::exceptions::NotSupportedException, "Getting extremal values at the initial state requires a model with a single initial state.");
            boost::optional<ConstantType> value;
            typename storm::storage::ParameterRegion<typename SparseModelType::ValueType>::Valuation valuation;

            for (auto const& v : region.getVerticesOfRegion(region.getVariables())) {
                auto currValue = getInstantiationChecker().check(env, v)->template asExplicitQuantitativeCheckResult<ConstantType>()[*this->parametricModel->getInitialStates().begin()];
                if (!value.is_initialized() || (storm::solver::minimize(dir) ? currValue < value.get() : currValue > value.get())) {
                    value = currValue;
                    valuation = v;
                    STORM_LOG_INFO("Current value for extremum: " << value.get() << ".");
                }
            }
            
            auto cmp = storm::solver::minimize(dir) ?
                    [](RegionBound<SparseModelType, ConstantType> const& lhs, RegionBound<SparseModelType, ConstantType> const& rhs) { return lhs.bound > rhs.bound; } :
                    [](RegionBound<SparseModelType, ConstantType> const& lhs, RegionBound<SparseModelType, ConstantType> const& rhs) { return lhs.bound < rhs.bound; };
            std::priority_queue<RegionBound<SparseModelType, ConstantType>, std::vector<RegionBound<SparseModelType, ConstantType>>, decltype(cmp)> regionQueue(cmp);
            regionQueue.emplace(region, storm::utility::zero<ConstantType>());
            auto totalArea = storm::utility::convertNumber<ConstantType>(region.area());
            auto coveredArea = storm::utility::zero<ConstantType>();
            while (!regionQueue.empty()) {
                auto const& currRegion = regionQueue.top().region;
                
                // Check whether this region contains a new 'good' value
                auto currValue = getInstantiationChecker().check(env, currRegion.getCenterPoint())->template asExplicitQuantitativeCheckResult<ConstantType>()[*this->parametricModel->getInitialStates().begin()];
                if (storm::solver::minimize(dir) ? currValue < value.get() : currValue > value.get()) {
                    value = currValue;
                    valuation = currRegion.getCenterPoint();
                }

                // Check whether this region needs further investigation (i.e. splitting)
                auto currBound = getBound(env, currRegion, dir)->template asExplicitQuantitativeCheckResult<ConstantType>()[*this->parametricModel->getInitialStates().begin()];
                std::vector<storm::storage::ParameterRegion<typename SparseModelType::ValueType>> newRegions;
                if (storm::solver::minimize(dir)) {
                    if (currBound < value.get() - storm::utility::convertNumber<ConstantType>(precision)) {
                        currRegion.split(currRegion.getCenterPoint(), newRegions);
                    }
                } else {
                    if (currBound > value.get() + storm::utility::convertNumber<ConstantType>(precision)) {
                        currRegion.split(currRegion.getCenterPoint(), newRegions);
                    }
                }
                
                if (newRegions.empty()) {
                    coveredArea += storm::utility::convertNumber<ConstantType>(currRegion.area());
                }
                regionQueue.pop();
                for (auto const& r : newRegions) {
                    regionQueue.emplace(r, currBound);
                }
                STORM_LOG_INFO("Current value : " << value.get() << ", current bound: " << regionQueue.top().bound << ".");
                STORM_LOG_INFO("Covered " << (coveredArea * storm::utility::convertNumber<ConstantType>(100.0) / totalArea) << "% of the region.");
            }
            
            return std::make_pair(storm::utility::convertNumber<typename SparseModelType::ValueType>(value.get()), valuation);
        }

        
        template <typename SparseModelType, typename ConstantType>
        SparseModelType const& SparseParameterLiftingModelChecker<SparseModelType, ConstantType>::getConsideredParametricModel() const {
            return *parametricModel;
        }
        
        template <typename SparseModelType, typename ConstantType>
        CheckTask<storm::logic::Formula, ConstantType> const& SparseParameterLiftingModelChecker<SparseModelType, ConstantType>::getCurrentCheckTask() const {
            return *currentCheckTask;
        }

        template <typename SparseModelType, typename ConstantType>
        void SparseParameterLiftingModelChecker<SparseModelType, ConstantType>::specifyBoundedUntilFormula(Environment const& env, CheckTask<logic::BoundedUntilFormula, ConstantType> const& checkTask) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Parameter lifting is not supported for the given property.");
        }

        template <typename SparseModelType, typename ConstantType>
        void SparseParameterLiftingModelChecker<SparseModelType, ConstantType>::specifyUntilFormula(Environment const& env, CheckTask<logic::UntilFormula, ConstantType> const& checkTask) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Parameter lifting is not supported for the given property.");
        }

        template <typename SparseModelType, typename ConstantType>
        void SparseParameterLiftingModelChecker<SparseModelType, ConstantType>::specifyReachabilityProbabilityFormula(Environment const& env, CheckTask<logic::EventuallyFormula, ConstantType> const& checkTask) {
            // transform to until formula
            auto untilFormula = std::make_shared<storm::logic::UntilFormula const>(storm::logic::Formula::getTrueFormula(), checkTask.getFormula().getSubformula().asSharedPointer());
            specifyUntilFormula(env, currentCheckTask->substituteFormula(*untilFormula));
        }

        template <typename SparseModelType, typename ConstantType>
        void SparseParameterLiftingModelChecker<SparseModelType, ConstantType>::specifyReachabilityRewardFormula(Environment const& env, CheckTask<logic::EventuallyFormula, ConstantType> const& checkTask) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Parameter lifting is not supported for the given property.");
        }

        template <typename SparseModelType, typename ConstantType>
        void SparseParameterLiftingModelChecker<SparseModelType, ConstantType>::specifyCumulativeRewardFormula(Environment const& env, CheckTask<logic::CumulativeRewardFormula, ConstantType> const& checkTask) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Parameter lifting is not supported for the given property.");
        }

        template class SparseParameterLiftingModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double>;
        template class SparseParameterLiftingModelChecker<storm::models::sparse::Mdp<storm::RationalFunction>, double>;
        template class SparseParameterLiftingModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, storm::RationalNumber>;
        template class SparseParameterLiftingModelChecker<storm::models::sparse::Mdp<storm::RationalFunction>, storm::RationalNumber>;

    }
}
