#include <sstream>

#include "storm/modelchecker/parametric/ParameterLifting.h"

#include "storm/adapters/CarlAdapter.h"

#include "storm/modelchecker/parametric/SparseDtmcParameterLiftingModelChecker.h"
#include "storm/modelchecker/parametric/SparseDtmcInstantiationModelChecker.h"
#include "storm/modelchecker/parametric/SparseMdpParameterLiftingModelChecker.h"
#include "storm/modelchecker/parametric/SparseMdpInstantiationModelChecker.h"
#include "storm/transformer/SparseParametricDtmcSimplifier.h"
#include "storm/transformer/SparseParametricMdpSimplifier.h"
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

       
            template <typename SparseModelType, typename ConstantType>
            ParameterLifting<SparseModelType, ConstantType>::ParameterLifting(SparseModelType const& parametricModel) : parametricModel(parametricModel){
                initializationStopwatch.start();
                STORM_LOG_THROW(parametricModel.getInitialStates().getNumberOfSetBits() == 1, storm::exceptions::NotSupportedException, "Parameter lifting requires models with only one initial state");
                initializationStopwatch.stop();
            }
    
            template <typename SparseModelType, typename ConstantType>
            void ParameterLifting<SparseModelType, ConstantType>::specifyFormula(CheckTask<storm::logic::Formula, typename SparseModelType::ValueType> const& checkTask) {
                initializationStopwatch.start();
                STORM_LOG_THROW(checkTask.isOnlyInitialStatesRelevantSet(), storm::exceptions::NotSupportedException, "Parameter lifting requires a property where only the value in the initial states is relevant.");
                STORM_LOG_THROW(checkTask.isBoundSet(), storm::exceptions::NotSupportedException, "Parameter lifting requires a bounded property.");
                
                simplifyParametricModel(checkTask);
                initializeUnderlyingCheckers();
                currentCheckTask = std::make_unique<storm::modelchecker::CheckTask<storm::logic::Formula, typename SparseModelType::ValueType>>(checkTask.substituteFormula(*currentFormula));

                STORM_LOG_THROW(parameterLiftingChecker->canHandle(*currentCheckTask), storm::exceptions::NotSupportedException, "Parameter lifting is not supported for this property.");
                instantiationChecker->specifyFormula(*currentCheckTask);
                parameterLiftingChecker->specifyFormula(*currentCheckTask);
                initializationStopwatch.stop();
            }
    
            template <typename SparseModelType, typename ConstantType>
            RegionCheckResult ParameterLifting<SparseModelType, ConstantType>::analyzeRegion(storm::storage::ParameterRegion<typename SparseModelType::ValueType> const& region, RegionCheckResult const& initialResult, bool sampleVerticesOfRegion) const {
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
            std::vector<std::pair<storm::storage::ParameterRegion<typename SparseModelType::ValueType>, RegionCheckResult>> ParameterLifting<SparseModelType, ConstantType>::performRegionRefinement(storm::storage::ParameterRegion<typename SparseModelType::ValueType> const& region, typename storm::storage::ParameterRegion<typename SparseModelType::ValueType>::CoefficientType const& threshold) const {
                STORM_LOG_INFO("Applying refinement on region: " << region.toString(true) << " .");
                
                auto areaOfParameterSpace = region.area();
                auto fractionOfUndiscoveredArea = storm::utility::one<typename storm::storage::ParameterRegion<typename SparseModelType::ValueType>::CoefficientType>();
                auto fractionOfAllSatArea = storm::utility::zero<typename storm::storage::ParameterRegion<typename SparseModelType::ValueType>::CoefficientType>();
                auto fractionOfAllViolatedArea = storm::utility::zero<typename storm::storage::ParameterRegion<typename SparseModelType::ValueType>::CoefficientType>();
                
                std::vector<std::pair<storm::storage::ParameterRegion<typename SparseModelType::ValueType>, RegionCheckResult>> regions;
                regions.emplace_back(region, RegionCheckResult::Unknown);
                storm::storage::BitVector resultRegions(1, false);
                uint_fast64_t indexOfCurrentRegion = 0;
                
                while (fractionOfUndiscoveredArea > threshold) {
                    STORM_LOG_THROW(indexOfCurrentRegion < regions.size(), storm::exceptions::InvalidStateException, "Threshold for undiscovered area not reached but no unprocessed regions left.");
                    STORM_LOG_INFO("Analyzing region #" << indexOfCurrentRegion << " (" << storm::utility::convertNumber<double>(fractionOfUndiscoveredArea) * 100 << "% still unknown)");
                    auto const& currentRegion = regions[indexOfCurrentRegion].first;
                    auto& res = regions[indexOfCurrentRegion].second;
                    res = analyzeRegion(currentRegion, res, false);
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
            SparseParameterLiftingModelChecker<SparseModelType, ConstantType> const& ParameterLifting<SparseModelType, ConstantType>::getParameterLiftingChecker() const {
                return *parameterLiftingChecker;
            }
    
            template <typename SparseModelType, typename ConstantType>
            SparseInstantiationModelChecker<SparseModelType, ConstantType> const& ParameterLifting<SparseModelType, ConstantType>::getInstantiationChecker() const {
                return *instantiationChecker;
            }
    
            template <typename SparseModelType, typename ConstantType>
            SparseModelType const& ParameterLifting<SparseModelType, ConstantType>::getConsideredParametricModel() const {
                if (simplifiedModel) {
                    return *simplifiedModel;
                } else {
                    return parametricModel;
                }
            }
            
            template <>
            void ParameterLifting<storm::models::sparse::Dtmc<storm::RationalFunction>, double>::initializeUnderlyingCheckers() {
                parameterLiftingChecker = std::make_unique<SparseDtmcParameterLiftingModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double>>(getConsideredParametricModel());
                instantiationChecker = std::make_unique<SparseDtmcInstantiationModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double>>(getConsideredParametricModel());
            }
    
            template <>
            void ParameterLifting<storm::models::sparse::Mdp<storm::RationalFunction>, double>::initializeUnderlyingCheckers() {
                parameterLiftingChecker = std::make_unique<SparseMdpParameterLiftingModelChecker<storm::models::sparse::Mdp<storm::RationalFunction>, double>>(getConsideredParametricModel());
                instantiationChecker = std::make_unique<SparseMdpInstantiationModelChecker<storm::models::sparse::Mdp<storm::RationalFunction>, double>>(getConsideredParametricModel());
            }
    
            template <>
            void ParameterLifting<storm::models::sparse::Dtmc<storm::RationalFunction>, double>::simplifyParametricModel(CheckTask<logic::Formula, storm::RationalFunction> const& checkTask) {
                storm::transformer::SparseParametricDtmcSimplifier<storm::models::sparse::Dtmc<storm::RationalFunction>> simplifier(parametricModel);
                if(simplifier.simplify(checkTask.getFormula())) {
                    simplifiedModel = simplifier.getSimplifiedModel();
                    currentFormula = simplifier.getSimplifiedFormula();
                } else {
                    simplifiedModel = nullptr;
                    currentFormula = checkTask.getFormula().asSharedPointer();
                }
            }
    
            template <>
            void ParameterLifting<storm::models::sparse::Mdp<storm::RationalFunction>, double>::simplifyParametricModel(CheckTask<logic::Formula, storm::RationalFunction> const& checkTask) {
                storm::transformer::SparseParametricMdpSimplifier<storm::models::sparse::Mdp<storm::RationalFunction>> simplifier(parametricModel);
                if(simplifier.simplify(checkTask.getFormula())) {
                    simplifiedModel = simplifier.getSimplifiedModel();
                    currentFormula = simplifier.getSimplifiedFormula();
                } else {
                    simplifiedModel = nullptr;
                    currentFormula = checkTask.getFormula().asSharedPointer();
                }
            }
    
            template <typename SparseModelType, typename ConstantType>
            std::string ParameterLifting<SparseModelType, ConstantType>::visualizeResult(std::vector<std::pair<storm::storage::ParameterRegion<typename SparseModelType::ValueType>, RegionCheckResult>> const& result, storm::storage::ParameterRegion<typename SparseModelType::ValueType> const& parameterSpace, typename storm::storage::ParameterRegion<typename SparseModelType::ValueType>::VariableType const& x, typename storm::storage::ParameterRegion<typename SparseModelType::ValueType>::VariableType const& y) {
                
                typedef typename storm::storage::ParameterRegion<typename SparseModelType::ValueType>::CoefficientType ValueType;
                
                std::stringstream stream;
                
                uint_fast64_t const sizeX = 128;
                uint_fast64_t const sizeY = 64;
                
                stream << "Parameter lifting result (visualization):" << std::endl;
                stream << " \t x-axis: " << x << "  \t y-axis: " << y << "  \t S=safe, [ ]=unsafe, -=ambiguous " << std::endl;
                for (uint_fast64_t i = 0; i < sizeX+2; ++i) stream << "#"; stream << std::endl;
                
                ValueType deltaX = (parameterSpace.getUpperBoundary(x) - parameterSpace.getLowerBoundary(x)) / storm::utility::convertNumber<ValueType>(sizeX);
                ValueType deltaY = (parameterSpace.getUpperBoundary(y) - parameterSpace.getLowerBoundary(y)) / storm::utility::convertNumber<ValueType>(sizeY);
                ValueType printedRegionArea = deltaX * deltaY;
                for (ValueType yUpper = parameterSpace.getUpperBoundary(y); yUpper != parameterSpace.getLowerBoundary(y); yUpper -= deltaY) {
                    ValueType yLower = yUpper - deltaY;
                    stream << "#";
                    for (ValueType xLower = parameterSpace.getLowerBoundary(x); xLower != parameterSpace.getUpperBoundary(x); xLower += deltaX) {
                        ValueType xUpper = xLower + deltaX;
                        bool currRegionSafe = false;
                        bool currRegionUnSafe = false;
                        bool currRegionComplete = false;
                        ValueType coveredArea = storm::utility::zero<ValueType>();
                        for (auto const& r : result) {
                            ValueType instersectionArea = std::max(storm::utility::zero<ValueType>(), std::min(yUpper, r.first.getUpperBoundary(y)) - std::max(yLower, r.first.getLowerBoundary(y)));
                            instersectionArea *= std::max(storm::utility::zero<ValueType>(), std::min(xUpper, r.first.getUpperBoundary(x)) - std::max(xLower, r.first.getLowerBoundary(x)));
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
#endif
        } // namespace parametric
    } //namespace modelchecker
} //namespace storm

