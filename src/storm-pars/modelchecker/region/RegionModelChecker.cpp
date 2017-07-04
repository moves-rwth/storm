#include <sstream>
#include <queue>

#include "storm-pars/modelchecker/region/RegionModelChecker.h"

#include "storm/adapters/RationalFunctionAdapter.h"


#include "storm/utility/vector.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Mdp.h"

#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/InvalidArgumentException.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/CoreSettings.h"


namespace storm {
    namespace modelchecker {

            template <typename ParametricType>
            RegionModelChecker<ParametricType>::RegionModelChecker() {
                // Intentionally left empty
            }
        
            template <typename ParametricType>
            std::unique_ptr<storm::modelchecker::RegionCheckResult<ParametricType>> RegionModelChecker<ParametricType>::analyzeRegions(std::vector<storm::storage::ParameterRegion<ParametricType>> const& regions, bool sampleVerticesOfRegion) {
                
                std::vector<std::pair<storm::storage::ParameterRegion<ParametricType>, storm::modelchecker::RegionResult>> result;
                
                for (auto const& region : regions) {
                    storm::modelchecker::RegionResult regionRes = analyzeRegion(region, storm::modelchecker::RegionResult::Unknown, sampleVerticesOfRegion);
                    result.emplace_back(region, regionRes);
                }
                
                return std::make_unique<storm::modelchecker::RegionCheckResult<ParametricType>>(std::move(result));
            }

            template <typename ParametricType>
            std::unique_ptr<storm::modelchecker::RegionRefinementCheckResult<ParametricType>> RegionModelChecker<ParametricType>::performRegionRefinement(storm::storage::ParameterRegion<ParametricType> const& region, ParametricType const& threshold) {
                STORM_LOG_INFO("Applying refinement on region: " << region.toString(true) << " .");
                
                auto thresholdAsCoefficient = storm::utility::convertNumber<CoefficientType>(threshold);
                auto areaOfParameterSpace = region.area();
                auto fractionOfUndiscoveredArea = storm::utility::one<CoefficientType>();
                auto fractionOfAllSatArea = storm::utility::zero<CoefficientType>();
                auto fractionOfAllViolatedArea = storm::utility::zero<CoefficientType>();
                
                std::queue<std::pair<storm::storage::ParameterRegion<ParametricType>, RegionResult>> unprocessedRegions;
                std::vector<std::pair<storm::storage::ParameterRegion<ParametricType>, RegionResult>> result;
                unprocessedRegions.emplace(region, RegionResult::Unknown);
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

                while (fractionOfUndiscoveredArea > thresholdAsCoefficient) {
                    STORM_LOG_THROW(!unprocessedRegions.empty(), storm::exceptions::InvalidStateException, "Threshold for undiscovered area not reached but no unprocessed regions left.");
                    STORM_LOG_INFO("Analyzing region #" << numOfAnalyzedRegions << " (" << storm::utility::convertNumber<double>(fractionOfUndiscoveredArea) * 100 << "% still unknown)");
                    auto& currentRegion = unprocessedRegions.front().first;
                    auto& res = unprocessedRegions.front().second;
                    res = analyzeRegion(currentRegion, res, false);
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
                            std::vector<storm::storage::ParameterRegion<ParametricType>> newRegions;
                            currentRegion.split(currentRegion.getCenterPoint(), newRegions);
                            RegionResult initResForNewRegions = (res == RegionResult::CenterSat) ? RegionResult::ExistsSat :
                                                                     ((res == RegionResult::CenterViolated) ? RegionResult::ExistsViolated :
                                                                      RegionResult::Unknown);
                            for(auto& newRegion : newRegions) {
                                unprocessedRegions.emplace(std::move(newRegion), initResForNewRegions);
                            }
                            break;
                    }
                    ++numOfAnalyzedRegions;
                    unprocessedRegions.pop();
                    if (storm::settings::getModule<storm::settings::modules::CoreSettings>().isShowStatisticsSet()) {
                        while (displayedProgress < storm::utility::one<CoefficientType>() - fractionOfUndiscoveredArea) {
                            STORM_PRINT_AND_LOG("#");
                            displayedProgress += storm::utility::convertNumber<CoefficientType>(0.01);
                        }
                    }
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
    
#ifdef STORM_HAVE_CARL
            template class RegionModelChecker<storm::RationalFunction>;
#endif
    } //namespace modelchecker
} //namespace storm

