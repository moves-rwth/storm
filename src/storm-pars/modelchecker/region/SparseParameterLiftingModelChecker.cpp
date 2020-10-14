#include "storm-pars/modelchecker/region/SparseParameterLiftingModelChecker.h"

#include <queue>
#include <boost/container/flat_set.hpp>
#include <storm-pars/analysis/MonotonicityChecker.h>

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
            
            if (currentCheckTask->getFormula().isProbabilityOperatorFormula()) {
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
        RegionResult SparseParameterLiftingModelChecker<SparseModelType, ConstantType>::analyzeRegion(Environment const& env, storm::storage::ParameterRegion<typename SparseModelType::ValueType> const& region, RegionResultHypothesis const& hypothesis, RegionResult const& initialResult, bool sampleVerticesOfRegion, std::shared_ptr<storm::analysis::Order> reachabilityOrder, std::shared_ptr<storm::analysis::LocalMonotonicityResult<typename RegionModelChecker<typename SparseModelType::ValueType>::VariableType>> localMonotonicityResult) {
            typedef typename RegionModelChecker<typename SparseModelType::ValueType>::VariableType VariableType;
            typedef typename storm::analysis::MonotonicityResult<VariableType>::Monotonicity Monotonicity;
            typedef typename storm::utility::parametric::Valuation<typename SparseModelType::ValueType> Valuation;
            typedef typename storm::storage::ParameterRegion<typename SparseModelType::ValueType>::CoefficientType CoefficientType;
            STORM_LOG_THROW(this->currentCheckTask->isOnlyInitialStatesRelevantSet(), storm::exceptions::NotSupportedException, "Analyzing regions with parameter lifting requires a property where only the value in the initial states is relevant.");
            STORM_LOG_THROW(this->currentCheckTask->isBoundSet(), storm::exceptions::NotSupportedException, "Analyzing regions with parameter lifting requires a bounded property.");
            STORM_LOG_THROW(this->parametricModel->getInitialStates().getNumberOfSetBits() == 1, storm::exceptions::NotSupportedException, "Analyzing regions with parameter lifting requires a model with a single initial state.");
            
            RegionResult result = initialResult;

            // Check if we need to check the formula on one point to decide whether to show AllSat or AllViolated
            if (hypothesis == RegionResultHypothesis::Unknown && result == RegionResult::Unknown) {
                result = getInstantiationChecker().check(env, region.getCenterPoint())->asExplicitQualitativeCheckResult()[*this->parametricModel->getInitialStates().begin()] ? RegionResult::CenterSat : RegionResult::CenterViolated;
            }

            bool existsSat = (hypothesis == RegionResultHypothesis::AllSat || result == RegionResult::ExistsSat || result == RegionResult::CenterSat);
            bool existsViolated = (hypothesis == RegionResultHypothesis::AllViolated || result == RegionResult::ExistsViolated || result == RegionResult::CenterViolated);

            // Here we check on global monotonicity
            if (localMonotonicityResult != nullptr && localMonotonicityResult->isDone()) {
                // Try to check it with a global monotonicity result
                auto monRes = localMonotonicityResult->getGlobalMonotonicityResult();
                bool lowerBound = isLowerBound(this->currentCheckTask->getBound().comparisonType);

                if (monRes->isDone() && monRes->isAllMonotonicity()) {
                    // Build valuations
                    auto monMap = monRes->getMonotonicityResult();
                    Valuation valuationToCheckSat;
                    Valuation valuationToCheckViolated;
                    for (auto var : region.getVariables()) {
                        auto monVar = monMap[var];
                        if (monVar == Monotonicity::Constant) {
                            valuationToCheckSat.insert(std::pair<VariableType, CoefficientType>(var, region.getLowerBoundary(var)));
                            valuationToCheckViolated.insert(std::pair<VariableType, CoefficientType>(var, region.getLowerBoundary(var)));
                        } else if (monVar == Monotonicity::Decr) {
                            if (lowerBound) {
                                valuationToCheckSat.insert(std::pair<VariableType, CoefficientType>(var, region.getUpperBoundary(var)));
                                valuationToCheckViolated.insert(std::pair<VariableType, CoefficientType>(var, region.getLowerBoundary(var)));
                            } else {
                                valuationToCheckSat.insert(std::pair<VariableType, CoefficientType>(var, region.getLowerBoundary(var)));
                                valuationToCheckViolated.insert(std::pair<VariableType, CoefficientType>(var, region.getUpperBoundary(var)));
                            }
                        } else if (monVar == Monotonicity::Incr) {
                            if (lowerBound) {
                                valuationToCheckSat.insert(std::pair<VariableType, CoefficientType>(var, region.getLowerBoundary(var)));
                                valuationToCheckViolated.insert(std::pair<VariableType, CoefficientType>(var, region.getUpperBoundary(var)));
                            } else {
                                valuationToCheckSat.insert(std::pair<VariableType, CoefficientType>(var, region.getUpperBoundary(var)));
                                valuationToCheckViolated.insert(std::pair<VariableType, CoefficientType>(var, region.getLowerBoundary(var)));
                            }
                        }
                    }

                    // Check for result
                    if (existsSat && getInstantiationCheckerSAT().check(env, valuationToCheckSat)->asExplicitQualitativeCheckResult()[*this->parametricModel->getInitialStates().begin()]) {
                        STORM_LOG_INFO("Region " << region << " is AllSat, discovered with instantiation checker on " << valuationToCheckSat << " and help of monotonicity" << std::endl);
                        RegionModelChecker<typename SparseModelType::ValueType>::numberOfRegionsKnownThroughMonotonicity++;
                        return RegionResult::AllSat;
                    }

                    if (existsViolated && !getInstantiationCheckerVIO().check(env, valuationToCheckViolated)->asExplicitQualitativeCheckResult()[*this->parametricModel->getInitialStates().begin()]) {
                        STORM_LOG_INFO("Region " << region << " is AllViolated, discovered with instantiation checker on " << valuationToCheckViolated << " and help of monotonicity" << std::endl);
                        RegionModelChecker<typename SparseModelType::ValueType>::numberOfRegionsKnownThroughMonotonicity++;
                        return RegionResult::AllViolated;
                    }

                    return RegionResult::ExistsBoth;
                }
            }

            // Try to prove AllSat or AllViolated, depending on the hypothesis or the current result
            if (existsSat) {
                // show AllSat:
                storm::solver::OptimizationDirection parameterOptimizationDirection = isLowerBound(this->currentCheckTask->getBound().comparisonType) ? storm::solver::OptimizationDirection::Minimize : storm::solver::OptimizationDirection::Maximize;
                auto checkResult = this->check(env, region, parameterOptimizationDirection, reachabilityOrder, localMonotonicityResult);
                if (checkResult->asExplicitQualitativeCheckResult()[*this->parametricModel->getInitialStates().begin()]) {
                    result = RegionResult::AllSat;
                } else if (sampleVerticesOfRegion) {
                    result = sampleVertices(env, region, result);
                }
            } else if (existsViolated) {
                // show AllViolated:
                storm::solver::OptimizationDirection parameterOptimizationDirection = isLowerBound(this->currentCheckTask->getBound().comparisonType) ? storm::solver::OptimizationDirection::Maximize : storm::solver::OptimizationDirection::Minimize;
                auto checkResult = this->check(env, region, parameterOptimizationDirection, reachabilityOrder, localMonotonicityResult);
                if (!checkResult->asExplicitQualitativeCheckResult()[*this->parametricModel->getInitialStates().begin()]) {
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
        std::unique_ptr<CheckResult> SparseParameterLiftingModelChecker<SparseModelType, ConstantType>::check(Environment const& env, storm::storage::ParameterRegion<typename SparseModelType::ValueType> const& region, storm::solver::OptimizationDirection const& dirForParameters, std::shared_ptr<storm::analysis::Order> reachabilityOrder, std::shared_ptr<storm::analysis::LocalMonotonicityResult<typename RegionModelChecker<typename SparseModelType::ValueType>::VariableType>> localMonotonicityResult) {
            auto quantitativeResult = computeQuantitativeValues(env, region, dirForParameters, localMonotonicityResult);
            lastValue = quantitativeResult->template asExplicitQuantitativeCheckResult<ConstantType>()[*this->parametricModel->getInitialStates().begin()];
            if(currentCheckTask->getFormula().hasQuantitativeResult()) {
                return quantitativeResult;
            } else {
                return quantitativeResult->template asExplicitQuantitativeCheckResult<ConstantType>().compareAgainstBound(this->currentCheckTask->getFormula().asOperatorFormula().getComparisonType(), this->currentCheckTask->getFormula().asOperatorFormula().template getThresholdAs<ConstantType>());
            }
        }
        
        template <typename SparseModelType, typename ConstantType>
        std::unique_ptr<QuantitativeCheckResult<ConstantType>> SparseParameterLiftingModelChecker<SparseModelType, ConstantType>::getBound(Environment const& env, storm::storage::ParameterRegion<typename SparseModelType::ValueType> const& region, storm::solver::OptimizationDirection const& dirForParameters, std::shared_ptr<storm::analysis::LocalMonotonicityResult<typename RegionModelChecker<typename SparseModelType::ValueType>::VariableType>> localMonotonicityResult) {
            STORM_LOG_WARN_COND(this->currentCheckTask->getFormula().hasQuantitativeResult(), "Computing quantitative bounds for a qualitative formula...");
            return std::make_unique<ExplicitQuantitativeCheckResult<ConstantType>>(std::move(computeQuantitativeValues(env, region, dirForParameters, localMonotonicityResult)->template asExplicitQuantitativeCheckResult<ConstantType>()));
        }

        template <typename SparseModelType, typename ConstantType>
        typename SparseModelType::ValueType SparseParameterLiftingModelChecker<SparseModelType, ConstantType>::getBoundAtInitState(Environment const& env, storm::storage::ParameterRegion<typename SparseModelType::ValueType> const& region, storm::solver::OptimizationDirection const& dirForParameters) {
            STORM_LOG_THROW(this->parametricModel->getInitialStates().getNumberOfSetBits() == 1, storm::exceptions::NotSupportedException, "Getting a bound at the initial state requires a model with a single initial state.");
            return storm::utility::convertNumber<typename SparseModelType::ValueType>(getBound(env, region, dirForParameters)->template asExplicitQuantitativeCheckResult<ConstantType>()[*this->parametricModel->getInitialStates().begin()]);
        }

        template <typename SparseModelType, typename ConstantType>
        storm::modelchecker::SparseInstantiationModelChecker<SparseModelType, ConstantType>& SparseParameterLiftingModelChecker<SparseModelType, ConstantType>::getInstantiationCheckerSAT() {
            return getInstantiationChecker();
        }

        template <typename SparseModelType, typename ConstantType>
        storm::modelchecker::SparseInstantiationModelChecker<SparseModelType, ConstantType>& SparseParameterLiftingModelChecker<SparseModelType, ConstantType>::getInstantiationCheckerVIO() {
            return getInstantiationChecker();
        }

        template <typename SparseModelType, typename ConstantType>
        struct RegionBound {
            typedef typename storm::storage::ParameterRegion<typename SparseModelType::ValueType>::VariableType VariableType;

            RegionBound(RegionBound<SparseModelType, ConstantType> const& other) = default;
            RegionBound(storm::storage::ParameterRegion<typename SparseModelType::ValueType> const& r, std::shared_ptr<storm::analysis::Order> o, std::shared_ptr<storm::analysis::LocalMonotonicityResult<VariableType>> l, ConstantType const& b) : region(r), order(o), localMonRes(l), bound(b) {}
            storm::storage::ParameterRegion<typename SparseModelType::ValueType> region;
            std::shared_ptr<storm::analysis::Order> order;
            std::shared_ptr<storm::analysis::LocalMonotonicityResult<VariableType>> localMonRes;
            ConstantType bound;
        };
        
        template <typename SparseModelType, typename ConstantType>
        std::pair<typename SparseModelType::ValueType, typename storm::storage::ParameterRegion<typename SparseModelType::ValueType>::Valuation> SparseParameterLiftingModelChecker<SparseModelType, ConstantType>::computeExtremalValue(Environment const& env, storm::storage::ParameterRegion<typename SparseModelType::ValueType> const& region, storm::solver::OptimizationDirection const& dir, typename SparseModelType::ValueType const& precision) {
            typedef typename storm::storage::ParameterRegion<typename SparseModelType::ValueType>::CoefficientType CoefficientType;

            STORM_LOG_THROW(this->parametricModel->getInitialStates().getNumberOfSetBits() == 1, storm::exceptions::NotSupportedException, "Getting extremal values at the initial state requires a model with a single initial state.");
            boost::optional<ConstantType> value;
            typename storm::storage::ParameterRegion<typename SparseModelType::ValueType>::Valuation valuation;

            auto cmp = storm::solver::minimize(dir) ?
                    [](RegionBound<SparseModelType, ConstantType> const& lhs, RegionBound<SparseModelType, ConstantType> const& rhs) { return lhs.bound > rhs.bound ; } :
                    [](RegionBound<SparseModelType, ConstantType> const& lhs, RegionBound<SparseModelType, ConstantType> const& rhs) { return lhs.bound < rhs.bound ; };
            std::priority_queue<RegionBound<SparseModelType, ConstantType>, std::vector<RegionBound<SparseModelType, ConstantType>>, decltype(cmp)> regionQueue(cmp);
            storm::utility::Stopwatch initialWatch(true);

            if (region.getOptionalSplitThreshold() && region.getVariables().size() > region.getSplitThreshold()) {
                STORM_LOG_INFO("Not checking initial vertices as there are too many parameters");
                if (this->isUseMonotonicitySet()) {
                    auto o = this->extendOrder(env, nullptr, region);

                    auto monRes = std::shared_ptr<storm::analysis::LocalMonotonicityResult<VariableType>>(
                            new storm::analysis::LocalMonotonicityResult<VariableType>(o->getNumberOfStates()));
                    this->extendLocalMonotonicityResult(region, o, monRes);

                    if (storm::solver::minimize(dir)) {
                        regionQueue.emplace(region, o, monRes, storm::utility::zero<ConstantType>());
                    } else {
                        regionQueue.emplace(region, o, monRes, storm::utility::one<ConstantType>());
                    }
                } else {
                    if (storm::solver::minimize(dir)) {
                        regionQueue.emplace(region, nullptr, nullptr, storm::utility::zero<ConstantType>());
                    } else {
                        regionQueue.emplace(region, nullptr, nullptr, storm::utility::one<ConstantType>());
                    }
                }
            } else {
                if (this->isUseMonotonicitySet()) {
                    auto o = this->extendOrder(env, nullptr, region);

                    auto monRes = std::shared_ptr<storm::analysis::LocalMonotonicityResult<VariableType>>(
                            new storm::analysis::LocalMonotonicityResult<VariableType>(o->getNumberOfStates()));
                    this->extendLocalMonotonicityResult(region, o, monRes);

                    if (storm::solver::minimize(dir)) {
                        regionQueue.emplace(region, o, monRes, storm::utility::zero<ConstantType>());
                    } else {
                        regionQueue.emplace(region, o, monRes, storm::utility::one<ConstantType>());
                    }
                    // Only split vertices which are not monotone and when fullfilling bound
                    if (monRes->getGlobalMonotonicityResult()->isDone() &&
                        monRes->getGlobalMonotonicityResult()->isAllMonotonicity()) {
                        auto point = region.getPoint(dir, *(monRes->getGlobalMonotonicityResult()));
                        valuation = point;
                        value = getInstantiationChecker().check(env,
                                                                valuation)->template asExplicitQuantitativeCheckResult<ConstantType>()[*this->parametricModel->getInitialStates().begin()];
                        STORM_LOG_INFO("Extremum found with global monotonicity: " << value.get() << ".");
                        return std::make_pair(
                                storm::utility::convertNumber<typename SparseModelType::ValueType>(value.get()),
                                valuation);
                    } else {
                        std::set<VariableType> monIncr, monDecr, notMon;
                        monRes->getGlobalMonotonicityResult()->splitBasedOnMonotonicity(region.getVariables(), monIncr, monDecr, notMon);

                        auto point = region.getPoint(dir, monIncr, monDecr);
                        for (auto &v : region.getVerticesOfRegion(notMon)) {
                            for (auto &var : region.getVariables()) {
                                if (v.find(var) == v.end()) {
                                    v[var] = point[var];
                                }
                            }
                            auto currValue = getInstantiationChecker().check(env,
                                                                             v)->template asExplicitQuantitativeCheckResult<ConstantType>()[*this->parametricModel->getInitialStates().begin()];
                            if (!value.is_initialized() ||
                                (storm::solver::minimize(dir) ? currValue < value.get() : currValue > value.get())) {
                                value = currValue;
                                valuation = v;
                                STORM_LOG_INFO("Current value for extremum: " << value.get() << ".");
                            }
                        }
                        if (notMon.size() == 0) {
                            valuation = point;
                            value = getInstantiationChecker().check(env, valuation)->template asExplicitQuantitativeCheckResult<ConstantType>()[*this->parametricModel->getInitialStates().begin()];
                            STORM_LOG_INFO("Current value for extremum: " << value.get() << ".");
                        }
                    }
                } else {
                    for (auto &v : region.getVerticesOfRegion(region.getVariables())) {
                        auto currValue = getInstantiationChecker().check(env,
                                                                         v)->template asExplicitQuantitativeCheckResult<ConstantType>()[*this->parametricModel->getInitialStates().begin()];
                        if (!value.is_initialized() ||
                            (storm::solver::minimize(dir) ? currValue < value.get() : currValue > value.get())) {
                            value = currValue;
                            valuation = v;
                            STORM_LOG_INFO("Current value for extremum: " << value.get() << ".");
                        }
                    }

                    if (storm::solver::minimize(dir)) {
                        regionQueue.emplace(region, nullptr, nullptr, storm::utility::zero<ConstantType>());
                    } else {
                        regionQueue.emplace(region, nullptr, nullptr, storm::utility::one<ConstantType>());
                    }
                }
            }

            initialWatch.stop();
            STORM_PRINT(std::endl << "Total time for initial points: " << initialWatch << "." << std::endl << std::endl);


            storm::utility::Stopwatch loopWatch(true);
            auto numberOfSplits = 0;
            auto numberOfOrderCopies = 0;
            auto numberOfMonResCopies = 0;
            auto totalArea = storm::utility::convertNumber<ConstantType>(region.area());
            auto coveredArea = storm::utility::zero<ConstantType>();
            boost::container::flat_set<std::shared_ptr<storm::analysis::Order>> changedOrders;
            while (!regionQueue.empty()) {
                auto currRegion = regionQueue.top().region;
                STORM_LOG_INFO("Currently looking at region: " << currRegion);
                std::shared_ptr<storm::analysis::Order> order = regionQueue.top().order;
                std::shared_ptr<storm::analysis::LocalMonotonicityResult<VariableType>> localMonotonicityResult = regionQueue.top().localMonRes;
                std::vector<storm::storage::ParameterRegion<typename SparseModelType::ValueType>> newRegions;
                auto currBound = regionQueue.top().bound;

                // Check whether this region needs further investigation (i.e. splitting
                bool investigateBounds = !value || ((storm::solver::minimize(dir) && currBound < value.get() - storm::utility::convertNumber<ConstantType>(precision)) ||
                                                    ((!storm::solver::minimize(dir) && currBound > value.get() + storm::utility::convertNumber<ConstantType>(precision))));
                if (investigateBounds) {
                    auto bounds = getBound(env, currRegion, dir,
                                           localMonotonicityResult)->template asExplicitQuantitativeCheckResult<ConstantType>().getValueVector();
                    currBound = bounds[*this->parametricModel->getInitialStates().begin()];
                    bool lookAtRegion = !value || ((storm::solver::minimize(dir) && currBound < value.get() - storm::utility::convertNumber<ConstantType>(precision)) ||
                                                   ((!storm::solver::minimize(dir) && currBound > value.get() + storm::utility::convertNumber<ConstantType>(precision))));
                    investigateBounds &= lookAtRegion;
                    if (lookAtRegion) {
                        bool changedOrder = false;
                        if (this->isUseMonotonicitySet() && !order->getDoneBuilding()) {
                            changedOrder = changedOrders.contains(order);
                            assert (orderExtender);
                            if (!changedOrder || orderExtender->isHope(order, currRegion)) {
                                if (numberOfCopiesOrder[order] != 1) {
                                    order = copyOrder(order);
                                    numberOfOrderCopies++;
                                    numberOfCopiesOrder[order]--;
                                } else {
                                    assert (numberOfCopiesOrder[order] == 1);
                                    changedOrders.erase(order);
                                }
                                this->extendOrder(env, order, currRegion);
                                changedOrder = true;
                            }
                        }
                        if (changedOrder && this->isUseMonotonicitySet() && !localMonotonicityResult->isDone()) {
                            localMonotonicityResult = localMonotonicityResult->copy();
                            numberOfMonResCopies++;
                            this->extendLocalMonotonicityResult(currRegion, order, localMonotonicityResult);
                        }

                        // Check whether this region contains a new 'good' value
                        auto point = this->isUseMonotonicitySet() ? currRegion.getPoint(dir,
                                                                                        *(localMonotonicityResult->getGlobalMonotonicityResult()))
                                                                  : currRegion.getCenterPoint();
                        auto currValue = getInstantiationChecker().check(env,
                                                                         point)->template asExplicitQuantitativeCheckResult<ConstantType>()[*this->parametricModel->getInitialStates().begin()];
                        if (!value || (value && (storm::solver::minimize(dir) ? currValue < value.get() : currValue >
                                                                                                          value.get()))) {
                            value = currValue;
                            valuation = point;
                        }
                        if ((storm::solver::minimize(dir) &&
                             currBound < value.get() - storm::utility::convertNumber<ConstantType>(precision))
                            || (!storm::solver::minimize(dir) &&
                                currBound > value.get() + storm::utility::convertNumber<ConstantType>(precision))) {
                            if (this->isUseBoundsSet() && !order->getDoneBuilding()) {
                                if (storm::solver::minimize(dir)) {
                                    orderExtender->setMinValues(order, bounds);
                                    orderExtender->setMaxValues(order, getBound(env, currRegion,
                                                                                storm::solver::OptimizationDirection::Maximize,
                                                                                localMonotonicityResult)->template asExplicitQuantitativeCheckResult<ConstantType>().getValueVector());
                                } else {
                                    orderExtender->setMaxValues(order, bounds);
                                    orderExtender->setMinValues(order, getBound(env, currRegion,
                                                                                storm::solver::OptimizationDirection::Minimize,
                                                                                localMonotonicityResult)->template asExplicitQuantitativeCheckResult<ConstantType>().getValueVector());
                                }
                                // We try to extend the order again based on minMaxValues
                                auto i = order->getNumberOfDoneStates();
                                this->extendOrder(env, order, currRegion);
                                if (i < order->getNumberOfDoneStates()) {
                                    changedOrders.insert(order);
                                    this->extendLocalMonotonicityResult(currRegion, order, localMonotonicityResult);
                                }
                            }
                            if (this->isUseMonotonicitySet()) {
                                this->splitSmart(currRegion, newRegions, order,
                                                 *(localMonotonicityResult->getGlobalMonotonicityResult()));
                            } else if (this->isRegionSplitEstimateSupported()) {
                                auto empty = storm::analysis::MonotonicityResult<VariableType>();
                                this->splitSmart(currRegion, newRegions, order, empty);
                            } else {
                                currRegion.split(currRegion.getCenterPoint(), newRegions);
                            }
                        }
                    }
                }
                if (!investigateBounds) {
                    numberOfCopiesOrder[order]--;
                    numberOfCopiesMonRes[localMonotonicityResult]--;
                }

                if (newRegions.empty()) {
                    coveredArea += storm::utility::convertNumber<ConstantType>(currRegion.area());
                } else {
                    STORM_LOG_INFO("Splitting region " << currRegion << " into " << newRegions.size());
                    numberOfSplits++;
                }
                regionQueue.pop();

                if (this->isUseMonotonicitySet()) {
                    for (auto & r : newRegions ) {
                        regionQueue.emplace(r, order, localMonotonicityResult, currBound);
                    }
                    if (numberOfCopiesOrder.find(order) != numberOfCopiesOrder.end()) {
                        numberOfCopiesOrder[order] += newRegions.size();
                        numberOfCopiesMonRes[localMonotonicityResult] += newRegions.size();
                    } else {
                        numberOfCopiesOrder[order] = newRegions.size();
                        numberOfCopiesMonRes[localMonotonicityResult] = newRegions.size();
                    }
                } else {
                    for (auto const& r : newRegions) {
                        regionQueue.emplace(r, nullptr, nullptr, currBound);
                    }
                }


                STORM_LOG_INFO("Current value : " << value.get() << ", current bound: " << regionQueue.top().bound << ".");
                STORM_LOG_INFO("Covered " << (coveredArea * storm::utility::convertNumber<ConstantType>(100.0) / totalArea) << "% of the region." << std::endl);
                if (this->isUseMonotonicitySet() && regionQueue.empty()) {
                    loopWatch.stop();
                    auto& variables = currRegion.getVariables();
                    auto count = 0;
                    for (auto i = 0; i < order->getNumberOfStates(); ++i) {
                        for (auto& var : variables) {
                            if (localMonotonicityResult->getMonotonicity(i, var) != storm::analysis::LocalMonotonicityResult<VariableType>::Monotonicity::Unknown && localMonotonicityResult->getMonotonicity(i, var) != storm::analysis::LocalMonotonicityResult<VariableType>::Monotonicity::Not) {
                                count++;
                            }
                        }

                    }

                    STORM_PRINT("Total number of local monotonic states in last result (including =) and =( ): " << (count) << std::endl);
                }
            }
            if (!this->isUseMonotonicitySet()) {
                loopWatch.stop();
            }

            STORM_PRINT("Total number of splits: " << numberOfSplits << std::endl);
            if (this->isUseMonotonicitySet()) {
                STORM_PRINT("Total number of copies of the order: " << numberOfOrderCopies << std::endl);
                STORM_PRINT("Total number of copies of the local monotonicity result: " << numberOfMonResCopies
                                                                                        << std::endl);
            }
            STORM_PRINT(std::endl << "Total time for region refinement: " << loopWatch << "." << std::endl << std::endl);

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

        template<typename SparseModelType, typename ConstantType>
        std::shared_ptr<storm::analysis::Order> SparseParameterLiftingModelChecker<SparseModelType, ConstantType>::copyOrder(std::shared_ptr<storm::analysis::Order> order) {
            auto res = order->copy();
            if (orderExtender) {
                orderExtender->setUnknownStates(order, res);
                orderExtender->copyMinMax(order, res);
            }
            return res;
        }

        template<typename SparseModelType, typename ConstantType>
        void SparseParameterLiftingModelChecker<SparseModelType, ConstantType>::checkForPossibleMonotonicity(Environment const& env,
                                                                                                             const storage::ParameterRegion<typename SparseModelType::ValueType> &region,
                                                                                                             std::set<VariableType>& possibleMonotoneIncrParameters,
                                                                                                             std::set<VariableType>& possibleMonotoneDecrParameters,
                                                                                                             std::set<VariableType>& possibleNotMonotoneParameters,
                                                                                                             std::set<VariableType>const& consideredVariables,
                                                                                                             storm::solver::OptimizationDirection const& dir) {
            bool minimize = storm::solver::minimize(dir);
            for (auto& var : consideredVariables) {
                ConstantType previousCenter = -1;
                ConstantType previousUpper = -1;
                ConstantType previousLower = -1;
                bool monDecr = true;
                bool monIncr = true;
                auto valuationCenter = region.getCenterPoint();
                auto valuationLower = region.getLowerBoundaries();
                auto valuationUpper = region.getUpperBoundaries();

                valuationCenter[var] = region.getLowerBoundary(var);
                valuationLower[var] = region.getLowerBoundary(var);
                valuationUpper[var] = region.getLowerBoundary(var);
                // TODO: make cmdline argument
                int numberOfSamples = 10;
                auto stepSize = (region.getUpperBoundary(var) - region.getLowerBoundary(var)) / numberOfSamples;

                while (valuationCenter[var] <= region.getUpperBoundary(var)) {
                    // Create valuation
                    assert (valuationLower[var] <= region.getUpperBoundary(var));
                    assert (valuationUpper[var] <= region.getUpperBoundary(var));

                    ConstantType valueLower = getInstantiationChecker().check(env, valuationLower)->template asExplicitQuantitativeCheckResult<ConstantType>()[*this->parametricModel->getInitialStates().begin()];
                    ConstantType valueCenter = getInstantiationChecker().check(env, valuationCenter)->template asExplicitQuantitativeCheckResult<ConstantType>()[*this->parametricModel->getInitialStates().begin()];
                    ConstantType valueUpper = getInstantiationChecker().check(env, valuationUpper)->template asExplicitQuantitativeCheckResult<ConstantType>()[*this->parametricModel->getInitialStates().begin()];

                    // Calculate difference with result for previous valuation
                    ConstantType diffCenter = previousCenter - valueCenter;
                    ConstantType diffLower = previousLower - valueLower;
                    ConstantType diffUpper = previousUpper - valueUpper;
                    assert (previousCenter == -1 || (diffCenter >= -1 && diffCenter <= 1));
                    assert (previousUpper == -1 || (diffUpper >= -1 && diffUpper <= 1));
                    assert (previousLower == -1 || (diffLower >= -1 && diffLower <= 1));

                    if (previousLower != -1) {
                        assert (previousUpper != -1 && previousCenter != -1);
                        monDecr &= diffCenter > 0 && diffLower > 0 && diffUpper > 0; // then previous value is larger than the current value from the initial states
                        monIncr &= diffCenter < 0 && diffLower < 0 && diffUpper < 0;
                    } else {
                        assert (previousUpper == -1 && previousCenter == -1);
                    }
                    previousCenter = valueCenter;
                    previousLower = valueLower;
                    previousUpper = valueUpper;
                    if (!monDecr && ! monIncr) {
                        break;
                    }
                    valuationCenter[var] += stepSize;
                    valuationLower[var] += stepSize;
                    valuationUpper[var] += stepSize;
                }
                if (monIncr) {
                    possibleMonotoneIncrParameters.insert(var);
                } else if (monDecr) {
                    possibleMonotoneDecrParameters.insert(var);
                } else {
                    possibleNotMonotoneParameters.insert(var);
                }
            }
        }

        template class SparseParameterLiftingModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double>;
        template class SparseParameterLiftingModelChecker<storm::models::sparse::Mdp<storm::RationalFunction>, double>;
        template class SparseParameterLiftingModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, storm::RationalNumber>;
        template class SparseParameterLiftingModelChecker<storm::models::sparse::Mdp<storm::RationalFunction>, storm::RationalNumber>;

    }
}
