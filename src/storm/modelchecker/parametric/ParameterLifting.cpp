#include <sstream>

#include "storm/modelchecker/parametric/ParameterLifting.h"

#include "storm/adapters/CarlAdapter.h"

#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"

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
        namespace parametric {

            ParameterLiftingSettings::ParameterLiftingSettings() {
                // todo: get this form settings
                this->applyExactValidation = false;
            }
            
            template <typename SparseModelType, typename ConstantType>
            ParameterLifting<SparseModelType, ConstantType>::ParameterLifting(SparseModelType const& parametricModel) : parametricModel(parametricModel) {
                initializationStopwatch.start();
                STORM_LOG_THROW(parametricModel.getInitialStates().getNumberOfSetBits() == 1, storm::exceptions::NotSupportedException, "Parameter lifting requires models with only one initial state");
                initializationStopwatch.stop();
            }
        
            template <typename SparseModelType, typename ConstantType>
            ParameterLiftingSettings const& ParameterLifting<SparseModelType, ConstantType>::getSettings() const {
                return settings;
            }
            
            template <typename SparseModelType, typename ConstantType>
            void ParameterLifting<SparseModelType, ConstantType>::setSettings(ParameterLiftingSettings const& newSettings) {
                settings = newSettings;
            }
            
            template <typename SparseModelType, typename ConstantType>
            void ParameterLifting<SparseModelType, ConstantType>::specifyFormula(CheckTask<storm::logic::Formula, typename SparseModelType::ValueType> const& checkTask) {
                initializationStopwatch.start();
                STORM_LOG_THROW(checkTask.isOnlyInitialStatesRelevantSet(), storm::exceptions::NotSupportedException, "Parameter lifting requires a property where only the value in the initial states is relevant.");
                STORM_LOG_THROW(checkTask.isBoundSet(), storm::exceptions::NotSupportedException, "Parameter lifting requires a bounded property.");
                
                simplifyParametricModel(checkTask);
                initializeUnderlyingCheckers();
                currentCheckTask = std::make_unique<storm::modelchecker::CheckTask<storm::logic::Formula, typename SparseModelType::ValueType>>(checkTask.substituteFormula(*currentFormula));

                STORM_LOG_THROW(parameterLiftingChecker->canHandle(*currentCheckTask) &&
                                (!exactParameterLiftingChecker || exactParameterLiftingChecker->canHandle(*currentCheckTask)),
                                storm::exceptions::NotSupportedException, "Parameter lifting is not supported for this property.");
                if (exactParameterLiftingChecker) {
                    exactParameterLiftingChecker->specifyFormula(*currentCheckTask);
                }
                parameterLiftingChecker->specifyFormula(*currentCheckTask);
                instantiationChecker->specifyFormula(*currentCheckTask);
                initializationStopwatch.stop();
            }
    
            template <typename SparseModelType, typename ConstantType>
            RegionCheckResult ParameterLifting<SparseModelType, ConstantType>::analyzeRegion(storm::storage::ParameterRegion<typename SparseModelType::ValueType> const& region, RegionCheckResult const& initialResult, bool sampleVerticesOfRegion) {
                      RegionCheckResult result = initialResult;

                // Check if we need to check the formula on one point to decide whether to show AllSat or AllViolated
                instantiationCheckerStopwatch.start();
                if (result == RegionCheckResult::Unknown) {
                     result = instantiationChecker->check(region.getCenterPoint())->asExplicitQualitativeCheckResult()[*getConsideredParametricModel().getInitialStates().begin()] ? RegionCheckResult::CenterSat : RegionCheckResult::CenterViolated;
                }
                instantiationCheckerStopwatch.stop();
                
                // try to prove AllSat or AllViolated, depending on the obtained result
                parameterLiftingCheckerStopwatch.start();
                if(result == RegionCheckResult::ExistsSat || result == RegionCheckResult::CenterSat) {
                    // show AllSat:
                    if(parameterLiftingChecker->check(region, this->currentCheckTask->getOptimizationDirection())->asExplicitQualitativeCheckResult()[*getConsideredParametricModel().getInitialStates().begin()]) {
                        result = RegionCheckResult::AllSat;
                    } else if (sampleVerticesOfRegion) {
                        parameterLiftingCheckerStopwatch.stop(); instantiationCheckerStopwatch.start();
                        // Check if there is a point in the region for which the property is violated
                        auto vertices = region.getVerticesOfRegion(region.getVariables());
                        for (auto const& v : vertices) {
                            if (!instantiationChecker->check(v)->asExplicitQualitativeCheckResult()[*getConsideredParametricModel().getInitialStates().begin()]) {
                                result = RegionCheckResult::ExistsBoth;
                            }
                        }
                        instantiationCheckerStopwatch.stop(); parameterLiftingCheckerStopwatch.start();
                    }
                } else if (result == RegionCheckResult::ExistsViolated || result == RegionCheckResult::CenterViolated) {
                    // show AllViolated:
                    if(!parameterLiftingChecker->check(region, storm::solver::invert(this->currentCheckTask->getOptimizationDirection()))->asExplicitQualitativeCheckResult()[*getConsideredParametricModel().getInitialStates().begin()]) {
                        result = RegionCheckResult::AllViolated;
                    } else if (sampleVerticesOfRegion) {
                        parameterLiftingCheckerStopwatch.stop(); instantiationCheckerStopwatch.start();
                        // Check if there is a point in the region for which the property is satisfied
                        auto vertices = region.getVerticesOfRegion(region.getVariables());
                        for (auto const& v : vertices) {
                            if (instantiationChecker->check(v)->asExplicitQualitativeCheckResult()[*getConsideredParametricModel().getInitialStates().begin()]) {
                                result = RegionCheckResult::ExistsBoth;
                            }
                        }
                        instantiationCheckerStopwatch.stop(); parameterLiftingCheckerStopwatch.start();
                    }
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "When analyzing a region, an invalid initial result was given: " << initialResult);
                }
                parameterLiftingCheckerStopwatch.stop();
                return result;
            }
            
            template <typename SparseModelType, typename ConstantType>
            RegionCheckResult ParameterLifting<SparseModelType, ConstantType>::analyzeRegionExactValidation(storm::storage::ParameterRegion<typename SparseModelType::ValueType> const& region, RegionCheckResult const& initialResult) {
                RegionCheckResult numericResult = analyzeRegion(region, initialResult, false);
                parameterLiftingCheckerStopwatch.start();
                if (numericResult == RegionCheckResult::AllSat || numericResult == RegionCheckResult::AllViolated) {
                    applyHintsToExactChecker();
                }
                if (numericResult == RegionCheckResult::AllSat) {
                    if(!exactParameterLiftingChecker->check(region, this->currentCheckTask->getOptimizationDirection())->asExplicitQualitativeCheckResult()[*getConsideredParametricModel().getInitialStates().begin()]) {
                        // Numerical result is wrong; Check whether the region is AllViolated!
                        if(!exactParameterLiftingChecker->check(region, storm::solver::invert(this->currentCheckTask->getOptimizationDirection()))->asExplicitQualitativeCheckResult()[*getConsideredParametricModel().getInitialStates().begin()]) {
                            parameterLiftingCheckerStopwatch.stop();
                            return RegionCheckResult::AllViolated;
                        } else {
                            return RegionCheckResult::Unknown;
                        }
                    }
                } else if (numericResult == RegionCheckResult::AllViolated) {
                    if(exactParameterLiftingChecker->check(region, storm::solver::invert(this->currentCheckTask->getOptimizationDirection()))->asExplicitQualitativeCheckResult()[*getConsideredParametricModel().getInitialStates().begin()]) {
                        // Numerical result is wrong; Check whether the region is AllSat!
                        if(exactParameterLiftingChecker->check(region, this->currentCheckTask->getOptimizationDirection())->asExplicitQualitativeCheckResult()[*getConsideredParametricModel().getInitialStates().begin()]) {
                            parameterLiftingCheckerStopwatch.stop();
                            return RegionCheckResult::AllSat;
                        } else {
                            return RegionCheckResult::Unknown;
                        }
                    }
                }
                parameterLiftingCheckerStopwatch.stop();
                return numericResult;
            }

    
            template <typename SparseModelType, typename ConstantType>
            std::vector<std::pair<storm::storage::ParameterRegion<typename SparseModelType::ValueType>, RegionCheckResult>> ParameterLifting<SparseModelType, ConstantType>::performRegionRefinement(storm::storage::ParameterRegion<typename SparseModelType::ValueType> const& region, CoefficientType const& threshold) {
                STORM_LOG_INFO("Applying refinement on region: " << region.toString(true) << " .");
                
                auto areaOfParameterSpace = region.area();
                auto fractionOfUndiscoveredArea = storm::utility::one<CoefficientType>();
                auto fractionOfAllSatArea = storm::utility::zero<CoefficientType>();
                auto fractionOfAllViolatedArea = storm::utility::zero<CoefficientType>();
                
                std::vector<std::pair<storm::storage::ParameterRegion<typename SparseModelType::ValueType>, RegionCheckResult>> regions;
                regions.emplace_back(region, RegionCheckResult::Unknown);
                storm::storage::BitVector resultRegions(1, false);
                uint_fast64_t indexOfCurrentRegion = 0;
                
                while (fractionOfUndiscoveredArea > threshold) {
                    STORM_LOG_THROW(indexOfCurrentRegion < regions.size(), storm::exceptions::InvalidStateException, "Threshold for undiscovered area not reached but no unprocessed regions left.");
                    STORM_LOG_INFO("Analyzing region #" << indexOfCurrentRegion << " (" << storm::utility::convertNumber<double>(fractionOfUndiscoveredArea) * 100 << "% still unknown)");
                    auto const& currentRegion = regions[indexOfCurrentRegion].first;
                    auto& res = regions[indexOfCurrentRegion].second;
                    if (settings.applyExactValidation) {
                        res = analyzeRegionExactValidation(currentRegion, res);
                    } else {
                        res = analyzeRegion(currentRegion, res, false);
                    }
                    switch (res) {
                        case RegionCheckResult::AllSat:
                            fractionOfUndiscoveredArea -= currentRegion.area() / areaOfParameterSpace;
                            fractionOfAllSatArea += currentRegion.area() / areaOfParameterSpace;
                            resultRegions.set(indexOfCurrentRegion, true);
                            break;
                        case RegionCheckResult::AllViolated:
                            fractionOfUndiscoveredArea -= currentRegion.area() / areaOfParameterSpace;
                            fractionOfAllViolatedArea += currentRegion.area() / areaOfParameterSpace;
                            resultRegions.set(indexOfCurrentRegion, true);
                            break;
                        default:
                            std::vector<storm::storage::ParameterRegion<typename SparseModelType::ValueType>> newRegions;
                            currentRegion.split(currentRegion.getCenterPoint(), newRegions);
                            resultRegions.grow(regions.size() + newRegions.size(), false);
                            RegionCheckResult initResForNewRegions = (res == RegionCheckResult::CenterSat) ? RegionCheckResult::ExistsSat :
                                                                     ((res == RegionCheckResult::CenterViolated) ? RegionCheckResult::ExistsViolated :
                                                                      RegionCheckResult::Unknown);
                            for(auto& newRegion : newRegions) {
                                regions.emplace_back(std::move(newRegion), initResForNewRegions);
                            }
                            break;
                    }
                    ++indexOfCurrentRegion;
                }
                resultRegions.resize(regions.size());
                
                if (storm::settings::getModule<storm::settings::modules::CoreSettings>().isShowStatisticsSet()) {
                    STORM_PRINT_AND_LOG("Parameter Lifting Statistics:" << std::endl);
                    STORM_PRINT_AND_LOG("    Analyzed a total of " << indexOfCurrentRegion << " regions." << std::endl);
                    STORM_PRINT_AND_LOG("    Initialization took " << initializationStopwatch << " seconds." << std::endl);
                    STORM_PRINT_AND_LOG("    Checking sampled models took " << instantiationCheckerStopwatch << " seconds." << std::endl);
                    STORM_PRINT_AND_LOG("    Checking lifted models took " << parameterLiftingCheckerStopwatch << " seconds." << std::endl);
                }
                return storm::utility::vector::filterVector(regions, resultRegions);
            }
    
            template <typename SparseModelType, typename ConstantType>
            SparseModelType const& ParameterLifting<SparseModelType, ConstantType>::getConsideredParametricModel() const {
                if (simplifiedModel) {
                    return *simplifiedModel;
                } else {
                    return parametricModel;
                }
            }
    
            template <typename SparseModelType, typename ConstantType>
            std::string ParameterLifting<SparseModelType, ConstantType>::visualizeResult(std::vector<std::pair<storm::storage::ParameterRegion<typename SparseModelType::ValueType>, RegionCheckResult>> const& result, storm::storage::ParameterRegion<typename SparseModelType::ValueType> const& parameterSpace, typename storm::storage::ParameterRegion<typename SparseModelType::ValueType>::VariableType const& x, typename storm::storage::ParameterRegion<typename SparseModelType::ValueType>::VariableType const& y) {
                
                std::stringstream stream;
                
                uint_fast64_t const sizeX = 128;
                uint_fast64_t const sizeY = 64;
                
                stream << "Parameter lifting result (visualization):" << std::endl;
                stream << " \t x-axis: " << x << "  \t y-axis: " << y << "  \t S=safe, [ ]=unsafe, -=ambiguous " << std::endl;
                for (uint_fast64_t i = 0; i < sizeX+2; ++i) stream << "#"; stream << std::endl;
                
                CoefficientType deltaX = (parameterSpace.getUpperBoundary(x) - parameterSpace.getLowerBoundary(x)) / storm::utility::convertNumber<CoefficientType>(sizeX);
                CoefficientType deltaY = (parameterSpace.getUpperBoundary(y) - parameterSpace.getLowerBoundary(y)) / storm::utility::convertNumber<CoefficientType>(sizeY);
                CoefficientType printedRegionArea = deltaX * deltaY;
                for (CoefficientType yUpper = parameterSpace.getUpperBoundary(y); yUpper != parameterSpace.getLowerBoundary(y); yUpper -= deltaY) {
                    CoefficientType yLower = yUpper - deltaY;
                    stream << "#";
                    for (CoefficientType xLower = parameterSpace.getLowerBoundary(x); xLower != parameterSpace.getUpperBoundary(x); xLower += deltaX) {
                        CoefficientType xUpper = xLower + deltaX;
                        bool currRegionSafe = false;
                        bool currRegionUnSafe = false;
                        bool currRegionComplete = false;
                        CoefficientType coveredArea = storm::utility::zero<CoefficientType>();
                        for (auto const& r : result) {
                            CoefficientType instersectionArea = std::max(storm::utility::zero<CoefficientType>(), std::min(yUpper, r.first.getUpperBoundary(y)) - std::max(yLower, r.first.getLowerBoundary(y)));
                            instersectionArea *= std::max(storm::utility::zero<CoefficientType>(), std::min(xUpper, r.first.getUpperBoundary(x)) - std::max(xLower, r.first.getLowerBoundary(x)));
                            if(!storm::utility::isZero(instersectionArea)) {
                                currRegionSafe = currRegionSafe || r.second == RegionCheckResult::AllSat;
                                currRegionUnSafe = currRegionUnSafe || r.second == RegionCheckResult::AllViolated;
                                coveredArea += instersectionArea;
                                if(currRegionSafe && currRegionUnSafe) {
                                    break;
                                }
                                if(coveredArea == printedRegionArea) {
                                    currRegionComplete = true;
                                    break;
                                }
                            }
                        }
                        if (currRegionComplete && currRegionSafe && !currRegionUnSafe) {
                            stream << "S";
                        } else if (currRegionComplete && currRegionUnSafe && !currRegionSafe) {
                            stream << " ";
                        } else {
                            stream << "-";
                        }
                    }
                    stream << "#" << std::endl;
                }
                for (uint_fast64_t i = 0; i < sizeX+2; ++i) stream << "#"; stream << std::endl;
                return stream.str();
            }
        
#ifdef STORM_HAVE_CARL
            template class ParameterLifting<storm::models::sparse::Dtmc<storm::RationalFunction>, double>;
            template class ParameterLifting<storm::models::sparse::Mdp<storm::RationalFunction>, double>;
            template class ParameterLifting<storm::models::sparse::Dtmc<storm::RationalFunction>, storm::RationalNumber>;
            template class ParameterLifting<storm::models::sparse::Mdp<storm::RationalFunction>, storm::RationalNumber>;
#endif
        } // namespace parametric
    } //namespace modelchecker
} //namespace storm

