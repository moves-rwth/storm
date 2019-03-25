#include <sstream>
#include <queue>

#include "storm-pars/modelchecker/region/RegionModelChecker.h"

#include "storm/adapters/RationalFunctionAdapter.h"


#include "storm/utility/vector.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Mdp.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/CoreSettings.h"
#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/InvalidStateException.h"
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
            std::unique_ptr<storm::modelchecker::RegionRefinementCheckResult<ParametricType>> RegionModelChecker<ParametricType>::performRegionRefinement(Environment const& env, storm::storage::ParameterRegion<ParametricType> const& region, boost::optional<ParametricType> const& coverageThreshold, boost::optional<uint64_t> depthThreshold, RegionResultHypothesis const& hypothesis) {
                STORM_LOG_INFO("Applying refinement on region: " << region.toString(true) << " .");
                
                auto thresholdAsCoefficient = coverageThreshold ? storm::utility::convertNumber<CoefficientType>(coverageThreshold.get()) : storm::utility::zero<CoefficientType>();
                auto areaOfParameterSpace = region.area();
                auto fractionOfUndiscoveredArea = storm::utility::one<CoefficientType>();
                auto fractionOfAllSatArea = storm::utility::zero<CoefficientType>();
                auto fractionOfAllViolatedArea = storm::utility::zero<CoefficientType>();
                
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

                while (fractionOfUndiscoveredArea > thresholdAsCoefficient && !unprocessedRegions.empty()) {
                    assert(unprocessedRegions.size() == refinementDepths.size());
                    uint64_t currentDepth = refinementDepths.front();
                    STORM_LOG_INFO("Analyzing region #" << numOfAnalyzedRegions << " (Refinement depth " << currentDepth << "; " << storm::utility::convertNumber<double>(fractionOfUndiscoveredArea) * 100 << "% still unknown)");
                    auto& currentRegion = unprocessedRegions.front().first;
                    auto& res = unprocessedRegions.front().second;
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
                                currentRegion.split(currentRegion.getCenterPoint(), newRegions);
                                RegionResult initResForNewRegions = (res == RegionResult::CenterSat) ? RegionResult::ExistsSat :
                                                                         ((res == RegionResult::CenterViolated) ? RegionResult::ExistsViolated :
                                                                          RegionResult::Unknown);
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
                }
                
                auto regionCopyForResult = region;
                return std::make_unique<storm::modelchecker::RegionRefinementCheckResult<ParametricType>>(std::move(result), std::move(regionCopyForResult));
            }

        template <typename ParametricType>
        std::pair<ParametricType, typename storm::storage::ParameterRegion<ParametricType>::Valuation> RegionModelChecker<ParametricType>::computeExtremalValue(Environment const& env, storm::storage::ParameterRegion<ParametricType> const& region, storm::solver::OptimizationDirection const& dir, ParametricType const& precision) {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Computing extremal values is not supported for this region model checker.");
            return std::pair<ParametricType, typename storm::storage::ParameterRegion<ParametricType>::Valuation>();
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

    
#ifdef STORM_HAVE_CARL
            template class RegionModelChecker<storm::RationalFunction>;
#endif
    } //namespace modelchecker
} //namespace storm

