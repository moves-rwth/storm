#include "storm/modelchecker/parametric/ParameterLifting.h"

#include "storm/adapters/CarlAdapter.h"

#include "storm/modelchecker/parametric/SparseDtmcParameterLiftingModelChecker.h"
#include "storm/modelchecker/parametric/SparseDtmcInstantiationModelChecker.h"
#include "storm/modelchecker/parametric/SparseMdpParameterLiftingModelChecker.h"
#include "storm/modelchecker/parametric/SparseMdpInstantiationModelChecker.h"
#include "storm/transformer/SparseParametricDtmcSimplifier.h"
#include "storm/transformer/SparseParametricMdpSimplifier.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"

#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Mdp.h"

#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/InvalidStateException.h"

namespace storm {
    namespace modelchecker {
        namespace parametric {

       
            template <typename SparseModelType, typename ConstantType>
            ParameterLifting<SparseModelType, ConstantType>::ParameterLifting(SparseModelType const& parametricModel) : parametricModel(parametricModel){
                STORM_LOG_THROW(parametricModel.getInitialStates().getNumberOfSetBits() == 1, storm::exceptions::NotSupportedException, "Parameter lifting requires models with only one initial state");
            }
    
            template <typename SparseModelType, typename ConstantType>
            void ParameterLifting<SparseModelType, ConstantType>::specifyFormula(CheckTask<storm::logic::Formula, typename SparseModelType::ValueType> const& checkTask) {
                STORM_LOG_THROW(checkTask.isBoundSet(), storm::exceptions::NotSupportedException, "Parameter lifting requires a bounded property.");
                STORM_LOG_THROW(parameterLiftingChecker->canHandle(checkTask), storm::exceptions::NotSupportedException, "Parameter lifting is not supported for this property.");
                
                simplifyParametricModel(checkTask);
                initializeUnderlyingCheckers();
                currentCheckTask = std::make_unique<storm::modelchecker::CheckTask<storm::logic::Formula, typename SparseModelType::ValueType>>(checkTask.substituteFormula(*currentFormula));

                instantiationChecker->specifyFormula(*currentCheckTask);
                parameterLiftingChecker->specifyFormula(*currentCheckTask);
            }
    
            template <typename SparseModelType, typename ConstantType>
            RegionCheckResult ParameterLifting<SparseModelType, ConstantType>::analyzeRegion(storm::storage::ParameterRegion<typename SparseModelType::ValueType> const& region, bool sampleVerticesOfRegion) const {
                // First sample for one point to decide whether we should try to prove AllSat or AllViolated
                if(instantiationChecker->check(region.getCenterPoint())->asExplicitQualitativeCheckResult()[*getConsideredParametricModel().getInitialStates().begin()]) {
                    // try to prove AllSat
                    if(parameterLiftingChecker->check(region, this->currentCheckTask->getOptimizationDirection())->asExplicitQualitativeCheckResult()[*getConsideredParametricModel().getInitialStates().begin()]) {
                        return RegionCheckResult::AllSat;
                    } else if (sampleVerticesOfRegion) {
                        // Check if there is a point in the region for which the property is violated
                        auto vertices = region.getVerticesOfRegion(region.getVariables());
                        for (auto const& v : vertices) {
                            if (!instantiationChecker->check(v)->asExplicitQualitativeCheckResult()[*getConsideredParametricModel().getInitialStates().begin()]) {
                                return  RegionCheckResult::ExistsBoth;
                            }
                        }
                    }
                    // Reaching this point means that we only know that there is (at least) one point in the region for which the property is satisfied
                    return RegionCheckResult::ExistsSat;
                } else {
                    // try to prove AllViolated
                    if(!parameterLiftingChecker->check(region, storm::solver::invert(this->currentCheckTask->getOptimizationDirection()))->asExplicitQualitativeCheckResult()[*getConsideredParametricModel().getInitialStates().begin()]) {
                        return RegionCheckResult::AllViolated;
                    } else if (sampleVerticesOfRegion) {
                        // Check if there is a point in the region for which the property is satisfied
                        auto vertices = region.getVerticesOfRegion(region.getVariables());
                        for (auto const& v : vertices) {
                            if (instantiationChecker->check(v)->asExplicitQualitativeCheckResult()[*getConsideredParametricModel().getInitialStates().begin()]) {
                                return  RegionCheckResult::ExistsBoth;
                            }
                        }
                    }
                    // Reaching this point means that we only know that there is (at least) one point in the region for which the property is violated
                    return RegionCheckResult::ExistsViolated;
                }
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
                storm::storage::BitVector resultRegions(1, true);
                uint_fast64_t indexOfCurrentRegion = 0;
                
                while (fractionOfUndiscoveredArea > threshold) {
                    STORM_LOG_THROW(indexOfCurrentRegion < regions.size(), storm::exceptions::InvalidStateException, "Threshold for undiscovered area not reached but no unprocessed regions left.");
                    auto & currentRegion = regions[indexOfCurrentRegion];
                    RegionCheckResult res = analyzeRegion(currentRegion.first, currentRegion.second, false);
                    switch (res) {
                        case RegionCheckResult::AllSat:
                            fractionOfUndiscoveredArea -= currentRegion.area() / areaOfParameterSpace;
                            fractionOfAllSatArea += currentRegion.area() / areaOfParameterSpace;
                            break;
                        case RegionCheckResult::AllViolated:
                            fractionOfUndiscoveredArea -= currentRegion.area() / areaOfParameterSpace;
                            fractionOfAllViolatedArea += currentRegion.area() / areaOfParameterSpace;
                            break;
                        default:
                            uint_fast64_t oldNumOfRegions = regions.size();
                            std::vector<storm::storage::ParameterRegion<typename SparseModelType::ValueType>> newRegions;
                            currentRegion.split(currentRegion.getCenterPoint(), regions);
                            resultRegions.grow(regions.size());
                            resultRegions.set(resultRegions.begin() + oldNumOfRegions-1?, resultRegions.begin() + regions.size()-1? );
                            resultRegions.set(indexOfCurrentRegion, false);
                            break;
                    }
                    ++indexOfCurrentRegion;
                }
                std::cout << " done! " << std::endl << "Fraction of ALLSAT;ALLVIOLATED;UNDISCOVERED area:" << std::endl;
                std::cout << "REFINEMENTRESULT;" <<storm::utility::convertNumber<double>(fractionOfAllSatArea) << ";" << storm::utility::convertNumber<double>(fractionOfAllViolatedArea) << ";" << storm::utility::convertNumber<double>(fractionOfUndiscoveredArea) << std::endl;
                
                ()
                return ;
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
    
        
#ifdef STORM_HAVE_CARL
        template class ParameterLifting<storm::models::sparse::Dtmc<storm::RationalFunction>, double>;
        template class ParameterLifting<storm::models::sparse::Mdp<storm::RationalFunction>, double>;
#endif
        } // namespace parametric
    } //namespace modelchecker
} //namespace storm

