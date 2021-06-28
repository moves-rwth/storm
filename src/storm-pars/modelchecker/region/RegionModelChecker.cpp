#include <sstream>
#include <queue>

#include "storm-pars/analysis/OrderExtender.cpp"
#include "storm-pars/modelchecker/region/RegionModelChecker.h"

#include "storm/adapters/RationalFunctionAdapter.h"

#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/models/sparse/Dtmc.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/CoreSettings.h"

#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/InvalidArgumentException.h"


namespace storm {
    namespace modelchecker {

            template <typename ParametricType>
            RegionModelChecker<ParametricType>::RegionModelChecker() {
                // Intentionally left empty
            }
        
            template <typename ParametricType>
            std::unique_ptr<storm::modelchecker::RegionCheckResult<ParametricType>> RegionModelChecker<ParametricType>::analyzeRegions(Environment const& env, std::vector<storm::storage::ParameterRegion<ParametricType>> const& regions, std::vector<RegionResultHypothesis> const& hypotheses, bool sampleVerticesOfRegion) {
                
                STORM_LOG_THROW(regions.size() == hypotheses.size(), storm::exceptions::InvalidArgumentException, "The number of regions and the number of hypotheses do not match");
                std::vector<std::pair<storm::storage::ParameterRegion<ParametricType>, storm::modelchecker::RegionResult>> result;
                
                auto hypothesisIt = hypotheses.begin();
                for (auto const& region : regions) {
                    storm::modelchecker::RegionResult regionRes = analyzeRegion(env, region, *hypothesisIt, storm::modelchecker::RegionResult::Unknown, sampleVerticesOfRegion);
                    result.emplace_back(region, regionRes);
                    ++hypothesisIt;
                }
                
                return std::make_unique<storm::modelchecker::RegionCheckResult<ParametricType>>(std::move(result));
            }

            template <typename ParametricType>
            ParametricType RegionModelChecker<ParametricType>::getBoundAtInitState(Environment const& env, storm::storage::ParameterRegion<ParametricType> const& region, storm::solver::OptimizationDirection const& dirForParameters) {
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "The selected region model checker does not support this functionality.");
                return storm::utility::zero<ParametricType>();
            }
        
            template <typename ParametricType>
            std::unique_ptr<storm::modelchecker::RegionRefinementCheckResult<ParametricType>> RegionModelChecker<ParametricType>::performRegionRefinement(Environment const& env, storm::storage::ParameterRegion<ParametricType> const& region, boost::optional<ParametricType> const& coverageThreshold, boost::optional<uint64_t> depthThreshold, RegionResultHypothesis const& hypothesis, uint64_t monThresh) {
                STORM_LOG_INFO("Applying refinement on region: " << region.toString(true) << " .");
                
                auto thresholdAsCoefficient = coverageThreshold ? storm::utility::convertNumber<CoefficientType>(coverageThreshold.get()) : storm::utility::zero<CoefficientType>();
                auto areaOfParameterSpace = region.area();
                auto fractionOfUndiscoveredArea = storm::utility::one<CoefficientType>();
                auto fractionOfAllSatArea = storm::utility::zero<CoefficientType>();
                auto fractionOfAllViolatedArea = storm::utility::zero<CoefficientType>();
                numberOfRegionsKnownThroughMonotonicity = 0;
                
                // The resulting (sub-)regions
                std::vector<std::pair<storm::storage::ParameterRegion<ParametricType>, RegionResult>> result;
                
                // FIFO queues storing the data for the regions that we still need to process.
                std::queue<std::pair<storm::storage::ParameterRegion<ParametricType>, RegionResult>> unprocessedRegions;

                std::queue<uint64_t> refinementDepths;
                unprocessedRegions.emplace(region, RegionResult::Unknown);
                refinementDepths.push(0);

                uint_fast64_t numOfAnalyzedRegions = 0;
                CoefficientType displayedProgress = storm::utility::zero<CoefficientType>();
                if (storm::settings::getModule<storm::settings::modules::CoreSettings>().isShowStatisticsSet()) {
                    STORM_PRINT_AND_LOG("Progress (solved fraction) :" << std::endl <<  "0% [");
                    while (displayedProgress < storm::utility::one<CoefficientType>() - thresholdAsCoefficient) {
                        STORM_PRINT_AND_LOG(" ");
                        displayedProgress += storm::utility::convertNumber<CoefficientType>(0.01);
                    }
                    while (displayedProgress < storm::utility::one<CoefficientType>()) {
                        STORM_PRINT_AND_LOG("-");
                        displayedProgress += storm::utility::convertNumber<CoefficientType>(0.01);
                    }
                    STORM_PRINT_AND_LOG("] 100%" << std::endl << "   [");
                    displayedProgress = storm::utility::zero<CoefficientType>();
                }

                // NORMAL WHILE LOOP
                uint64_t currentDepth = refinementDepths.front();
                while ((!useMonotonicity || currentDepth < monThresh) && fractionOfUndiscoveredArea > thresholdAsCoefficient && !unprocessedRegions.empty()) {
                    assert(unprocessedRegions.size() == refinementDepths.size());
                    STORM_LOG_INFO("Analyzing region #" << numOfAnalyzedRegions << " (Refinement depth " << currentDepth << "; " << storm::utility::convertNumber<double>(fractionOfUndiscoveredArea) * 100 << "% still unknown)");
                    auto& currentRegion = unprocessedRegions.front().first;
                    auto& res = unprocessedRegions.front().second;
                    std::shared_ptr<storm::analysis::Order> order;
                    std::shared_ptr<storm::analysis::LocalMonotonicityResult<VariableType>> localMonotonicityResult;
                    res = analyzeRegion(env, currentRegion, hypothesis, res, false);

                    switch (res) {
                        case RegionResult::AllSat:
                            fractionOfUndiscoveredArea -= currentRegion.area() / areaOfParameterSpace;
                            fractionOfAllSatArea += currentRegion.area() / areaOfParameterSpace;
                            result.push_back(std::move(unprocessedRegions.front()));
                            break;
                        case RegionResult::AllViolated:
                            fractionOfUndiscoveredArea -= currentRegion.area() / areaOfParameterSpace;
                            fractionOfAllViolatedArea += currentRegion.area() / areaOfParameterSpace;
                            result.push_back(std::move(unprocessedRegions.front()));
                            break;
                        default:
                            // Split the region as long as the desired refinement depth is not reached.
                            if (!depthThreshold || currentDepth < depthThreshold.get()) {
                                std::vector<storm::storage::ParameterRegion<ParametricType>> newRegions;
                                RegionResult initResForNewRegions = (res == RegionResult::CenterSat) ? RegionResult::ExistsSat :
                                                                    ((res == RegionResult::CenterViolated) ? RegionResult::ExistsViolated :
                                                                     RegionResult::Unknown);

                                currentRegion.split(currentRegion.getCenterPoint(), newRegions);
                                for (auto& newRegion : newRegions) {
                                    unprocessedRegions.emplace(std::move(newRegion), initResForNewRegions);
                                    refinementDepths.push(currentDepth + 1);
                                }

                            } else {
                                // If the region is not further refined, it is still added to the result
                                result.push_back(std::move(unprocessedRegions.front()));
                            }
                            break;
                    }
                    ++numOfAnalyzedRegions;
                    unprocessedRegions.pop();
                    refinementDepths.pop();
                    if (storm::settings::getModule<storm::settings::modules::CoreSettings>().isShowStatisticsSet()) {
                        while (displayedProgress < storm::utility::one<CoefficientType>() - fractionOfUndiscoveredArea) {
                            STORM_PRINT_AND_LOG("#");
                            displayedProgress += storm::utility::convertNumber<CoefficientType>(0.01);
                        }
                    }
                    currentDepth = refinementDepths.front();
                }

                // FIFO queues for the order and local monotonicity results
                std::queue<std::shared_ptr<storm::analysis::Order>> orders;
                std::queue<std::shared_ptr<storm::analysis::LocalMonotonicityResult<VariableType>>> localMonotonicityResults;
                std::shared_ptr<storm::analysis::Order> order;
                std::shared_ptr<storm::analysis::LocalMonotonicityResult<VariableType>> localMonotonicityResult;
                if (useMonotonicity && fractionOfUndiscoveredArea > thresholdAsCoefficient && !unprocessedRegions.empty()) {
                    storm::utility::Stopwatch monWatch(true);

                    orders.emplace(extendOrder(env, nullptr, region));
                    assert (orders.front() != nullptr);
                    auto monRes = std::shared_ptr< storm::analysis::LocalMonotonicityResult<VariableType>>(new storm::analysis::LocalMonotonicityResult<VariableType>(orders.front()->getNumberOfStates()));
                    extendLocalMonotonicityResult(region, orders.front(), monRes);
                    localMonotonicityResults.emplace(monRes);
                    order = orders.front();
                    localMonotonicityResult = localMonotonicityResults.front();

                    if (!order->getDoneBuilding()) {
                        // we need to use copies for both order and local mon res
                        while(unprocessedRegions.size() > orders.size()){
                            orders.emplace(order->copy());
                            localMonotonicityResults.emplace(localMonotonicityResult->copy());
                        }
                    } else if (!localMonotonicityResult->isDone()) {
                        // the order will not change anymore
                        while(unprocessedRegions.size() > orders.size()) {
                            orders.emplace(order);
                            localMonotonicityResults.emplace(localMonotonicityResult->copy());
                        }
                    } else {
                        // both will not change anymore
                        while (unprocessedRegions.size() > orders.size()) {
                            orders.emplace(order);
                            localMonotonicityResults.emplace(localMonotonicityResult);
                        }
                    }
                    monWatch.stop();
                    STORM_PRINT(std::endl << "Time for orderBuilding and monRes initialization: " << monWatch << "." << std::endl << std::endl);
                }
                bool useSameOrder = useMonotonicity && order->getDoneBuilding();
                bool useSameLocalMonotonicityResult = useSameOrder && localMonotonicityResult->isDone();

                // USEMON WHILE LOOP
                while (useMonotonicity && fractionOfUndiscoveredArea > thresholdAsCoefficient && !unprocessedRegions.empty()) {
                    assert ((useSameLocalMonotonicityResult && localMonotonicityResults.size() == 1)|| unprocessedRegions.size() == localMonotonicityResults.size());
                    assert ((useSameOrder && orders.size() == 1) || unprocessedRegions.size() == orders.size());
                    assert(unprocessedRegions.size() == refinementDepths.size());
                    currentDepth = refinementDepths.front();
                    STORM_LOG_INFO("Analyzing region #" << numOfAnalyzedRegions << " (Refinement depth " << currentDepth << "; " << storm::utility::convertNumber<double>(fractionOfUndiscoveredArea) * 100 << "% still unknown)");
                    auto& currentRegion = unprocessedRegions.front().first;
                    auto& res = unprocessedRegions.front().second;

                    assert(!orders.empty());
                    if (!useSameOrder) {
                        order = orders.front();
                        if (!order->getDoneBuilding()) {
                            extendOrder(env, order, currentRegion);
                        }
                    }
                    if (!useSameLocalMonotonicityResult) {
                        localMonotonicityResult = localMonotonicityResults.front();
                        if (!localMonotonicityResult->isDone()) {
                            extendLocalMonotonicityResult(currentRegion, order, localMonotonicityResult);
                        }
                    }

                    res = analyzeRegion(env, currentRegion, hypothesis, res, false, order, localMonotonicityResult);

                    switch (res) {
                        case RegionResult::AllSat:
                            fractionOfUndiscoveredArea -= currentRegion.area() / areaOfParameterSpace;
                            fractionOfAllSatArea += currentRegion.area() / areaOfParameterSpace;
                            STORM_LOG_INFO("Region " << unprocessedRegions.front() << " is AllSat");
                            result.push_back(std::move(unprocessedRegions.front()));
                            break;
                        case RegionResult::AllViolated:
                            fractionOfUndiscoveredArea -= currentRegion.area() / areaOfParameterSpace;
                            fractionOfAllViolatedArea += currentRegion.area() / areaOfParameterSpace;
                            STORM_LOG_INFO("Region " << unprocessedRegions.front() << " is AllViolated");

                            result.push_back(std::move(unprocessedRegions.front()));
                            break;
                        default:
                            // Split the region as long as the desired refinement depth is not reached.
                            if (!depthThreshold || currentDepth < depthThreshold.get()) {
                                std::vector<storm::storage::ParameterRegion<ParametricType>> newRegions;
                                RegionResult initResForNewRegions = (res == RegionResult::CenterSat) ? RegionResult::ExistsSat :
                                                                    ((res == RegionResult::CenterViolated) ? RegionResult::ExistsViolated :
                                                                     RegionResult::Unknown);

                                std::vector<storm::storage::ParameterRegion<ParametricType>> newKnownRegions;
                                // Only split in (non)monotone vars
                                splitSmart(currentRegion, newRegions, order, *(localMonotonicityResult->getGlobalMonotonicityResult()), false);
                                assert (newRegions.size() != 0);

                                initResForNewRegions = (res == RegionResult::CenterSat) ? RegionResult::ExistsSat :
                                                       ((res == RegionResult::CenterViolated) ? RegionResult::ExistsViolated :
                                                        RegionResult::Unknown);
                                bool first = true;
                                for (auto& newRegion : newRegions) {
                                    if (!useSameOrder) {
                                        if (first) {
                                            orders.emplace(order);
                                            localMonotonicityResults.emplace(localMonotonicityResult);
                                            first = false;
                                        } else {
                                            if (!order->getDoneBuilding()) {
                                                // we need to use copies for both order and local mon res
                                                orders.emplace(order->copy());
                                                localMonotonicityResults.emplace(localMonotonicityResult->copy());
                                            } else if (!localMonotonicityResult->isDone()) {
                                                // the order will not change anymore
                                                orders.emplace(order);
                                                localMonotonicityResults.emplace(localMonotonicityResult->copy());
                                            } else {
                                                // both will not change anymore
                                                orders.emplace(order);
                                                localMonotonicityResults.emplace(localMonotonicityResult);
                                            }
                                        }
                                    } else if (!useSameLocalMonotonicityResult) {
                                        if (first) {
                                            localMonotonicityResults.emplace(localMonotonicityResult);
                                            first = false;
                                        } else {
                                            if (!localMonotonicityResult->isDone()) {
                                                localMonotonicityResults.emplace(localMonotonicityResult->copy());
                                            } else {
                                                localMonotonicityResults.emplace(localMonotonicityResult);
                                            }
                                        }
                                    }
                                    unprocessedRegions.emplace(std::move(newRegion), initResForNewRegions);
                                    refinementDepths.push(currentDepth + 1);
                                }
                            } else {
                                // If the region is not further refined, it is still added to the result
                                result.push_back(std::move(unprocessedRegions.front()));
                            }
                            break;
                    }

                    ++numOfAnalyzedRegions;
                    unprocessedRegions.pop();
                    refinementDepths.pop();
                    if (!useSameOrder) {
                        orders.pop();
                    }
                    if (!useSameLocalMonotonicityResult) {
                        localMonotonicityResults.pop();
                    }

                    if (storm::settings::getModule<storm::settings::modules::CoreSettings>().isShowStatisticsSet()) {
                        while (displayedProgress < storm::utility::one<CoefficientType>() - fractionOfUndiscoveredArea) {
                            STORM_PRINT_AND_LOG("#");
                            displayedProgress += storm::utility::convertNumber<CoefficientType>(0.01);
                        }
                    }
                }
                
                // Add the still unprocessed regions to the result
                while (!unprocessedRegions.empty()) {
                    result.push_back(std::move(unprocessedRegions.front()));
                    unprocessedRegions.pop();
                }
                
                if (storm::settings::getModule<storm::settings::modules::CoreSettings>().isShowStatisticsSet()) {
                    while (displayedProgress < storm::utility::one<CoefficientType>()) {
                        STORM_PRINT_AND_LOG("-");
                        displayedProgress += storm::utility::convertNumber<CoefficientType>(0.01);
                    }
                    STORM_PRINT_AND_LOG("]" << std::endl);
                    
                    STORM_PRINT_AND_LOG("Region Refinement Statistics:" << std::endl);
                    STORM_PRINT_AND_LOG("    Analyzed a total of " << numOfAnalyzedRegions << " regions." << std::endl);

                    if (useMonotonicity) {
                        STORM_PRINT_AND_LOG("    " << numberOfRegionsKnownThroughMonotonicity << " regions where discovered with help of monotonicity." << std::endl);

                    }
                }
                
                auto regionCopyForResult = region;
                return std::make_unique<storm::modelchecker::RegionRefinementCheckResult<ParametricType>>(std::move(result), std::move(regionCopyForResult));
            }


        template <typename ParametricType>
        void RegionModelChecker<ParametricType>::extendLocalMonotonicityResult(storm::storage::ParameterRegion<ParametricType> const& region, std::shared_ptr<storm::analysis::Order> order, std::shared_ptr<storm::analysis::LocalMonotonicityResult<VariableType>> localMonotonicityResult){
            STORM_LOG_WARN("Initializing local Monotonicity Results not implemented for RegionModelChecker.");
        }

        template <typename ParametricType>
        std::pair<ParametricType, typename storm::storage::ParameterRegion<ParametricType>::Valuation> RegionModelChecker<ParametricType>::computeExtremalValue(Environment const& env, storm::storage::ParameterRegion<ParametricType> const& region, storm::solver::OptimizationDirection const& dir, ParametricType const& precision) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Computing extremal values is not supported for this region model checker.");
            return std::pair<ParametricType, typename storm::storage::ParameterRegion<ParametricType>::Valuation>();
        }

        template <typename ParametricType>
        bool RegionModelChecker<ParametricType>::checkExtremalValue(Environment const& env, storm::storage::ParameterRegion<ParametricType> const& region, storm::solver::OptimizationDirection const& dir, ParametricType const& precision, ParametricType const& valueToCheck) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Checking extremal values is not supported for this region model checker.");
            return false;
        }

        
        template <typename ParametricType>
        bool RegionModelChecker<ParametricType>::isRegionSplitEstimateSupported() const {
            return false;
        }
        
        template <typename ParametricType>
        std::map<typename RegionModelChecker<ParametricType>::VariableType, double> RegionModelChecker<ParametricType>::getRegionSplitEstimate() const {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Region split estimation is not supported by this region model checker.");
            return std::map<typename RegionModelChecker<ParametricType>::VariableType, double>();
        }

        template <typename ParametricType>
        std::shared_ptr<storm::analysis::Order> RegionModelChecker<ParametricType>::extendOrder(Environment const& env, std::shared_ptr<storm::analysis::Order> order, storm::storage::ParameterRegion<ParametricType> region) {
            STORM_LOG_WARN("Extending order for RegionModelChecker not implemented");
            // Does nothing
            return order;
        }

        template <typename ParametricType>
        void RegionModelChecker<ParametricType>::setConstantEntries(std::shared_ptr<storm::analysis::LocalMonotonicityResult<VariableType>> localMonotonicityResult) {
            STORM_LOG_WARN("Setting constant entries fo local monotonicity result not implemented");
            // Does nothing
        }

        template <typename ParametricType>
        bool RegionModelChecker<ParametricType>::isUseMonotonicitySet() const{
            return useMonotonicity;
        }

        template <typename ParametricType>
        bool RegionModelChecker<ParametricType>::isUseBoundsSet() {
            return useBounds;
        }

        template <typename ParametricType>
        bool RegionModelChecker<ParametricType>::isOnlyGlobalSet() {
            return useOnlyGlobal;
        }

        template <typename ParametricType>
        void RegionModelChecker<ParametricType>::setUseMonotonicity(bool monotonicity) {
            this->useMonotonicity = monotonicity;
        }

        template <typename ParametricType>
        void RegionModelChecker<ParametricType>::setUseBounds(bool bounds) {
            assert (!bounds || useMonotonicity);
            this->useBounds = bounds;
        }

        template <typename ParametricType>
        void RegionModelChecker<ParametricType>::setUseOnlyGlobal(bool global) {
            assert (!global || useMonotonicity);
            this->useOnlyGlobal = global;
        }

        template <typename ParametricType>
        void RegionModelChecker<ParametricType>::splitSmart(storm::storage::ParameterRegion<ParametricType> & currentRegion, std::vector<storm::storage::ParameterRegion<ParametricType>> &regionVector, std::shared_ptr<storm::analysis::Order> order, storm::analysis::MonotonicityResult<VariableType> & monRes, bool splitForExtremum) const {
            STORM_LOG_WARN("Smart splitting for this model checker not implemented");
            currentRegion.split(currentRegion.getCenterPoint(), regionVector);
        }

        template<typename ParametricType>
        void RegionModelChecker<ParametricType>::setMonotoneParameters(std::pair<std::set<typename storm::storage::ParameterRegion<ParametricType>::VariableType>, std::set<typename storm::storage::ParameterRegion<ParametricType>::VariableType>> monotoneParameters) {
            monotoneIncrParameters = std::move(monotoneParameters.first);
            monotoneDecrParameters = std::move(monotoneParameters.second);
        }

#ifdef STORM_HAVE_CARL
            template class RegionModelChecker<storm::RationalFunction>;
#endif
    } //namespace modelchecker
} //namespace storm

