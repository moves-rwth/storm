#include "storm/modelchecker/parametric/SparseDtmcRegionChecker.h"

#include "storm/adapters/RationalFunctionAdapter.h"

#include "storm/modelchecker/parametric/SparseDtmcParameterLiftingModelChecker.h"
#include "storm/modelchecker/parametric/SparseDtmcInstantiationModelChecker.h"
#include "storm/transformer/SparseParametricDtmcSimplifier.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/utility/NumberTraits.h"

namespace storm {
    namespace modelchecker {
        namespace parametric {

            template <typename SparseModelType, typename ConstantType, typename ExactConstantType>
            SparseDtmcRegionChecker<SparseModelType, ConstantType, ExactConstantType>::SparseDtmcRegionChecker(SparseModelType const& parametricModel) : RegionChecker<SparseModelType, ConstantType, ExactConstantType>(parametricModel) {
                // Intentionally left empty
            }
    
            template <typename SparseModelType, typename ConstantType, typename ExactConstantType>
            void SparseDtmcRegionChecker<SparseModelType, ConstantType, ExactConstantType>::simplifyParametricModel(CheckTask<storm::logic::Formula, typename SparseModelType::ValueType> const& checkTask) {
                storm::transformer::SparseParametricDtmcSimplifier<SparseModelType> simplifier(this->parametricModel);
                if(simplifier.simplify(checkTask.getFormula())) {
                    this->simplifiedModel = simplifier.getSimplifiedModel();
                    this->currentFormula = simplifier.getSimplifiedFormula();
                } else {
                    this->simplifiedModel = nullptr;
                    this->currentFormula = checkTask.getFormula().asSharedPointer();
                }
            }
            
            template <typename SparseModelType, typename ConstantType, typename ExactConstantType>
            void SparseDtmcRegionChecker<SparseModelType, ConstantType, ExactConstantType>::initializeUnderlyingCheckers() {
                if (this->settings.applyExactValidation) {
                    STORM_LOG_WARN_COND(!storm::NumberTraits<ConstantType>::IsExact, "Exact validation is not necessarry if the original computation is already exact");
                    this->exactParameterLiftingChecker = std::make_unique<SparseDtmcParameterLiftingModelChecker<SparseModelType, ExactConstantType>>(this->getConsideredParametricModel());
                }
                this->parameterLiftingChecker = std::make_unique<SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>>(this->getConsideredParametricModel());
                this->instantiationChecker = std::make_unique<SparseDtmcInstantiationModelChecker<SparseModelType, ConstantType>>(this->getConsideredParametricModel());
                this->instantiationChecker->setInstantiationsAreGraphPreserving(true);
            }
    
            template <typename SparseModelType, typename ConstantType, typename ExactConstantType>
            void SparseDtmcRegionChecker<SparseModelType, ConstantType, ExactConstantType>::applyHintsToExactChecker() {
                auto dtmcPLChecker = dynamic_cast<storm::modelchecker::parametric::SparseDtmcParameterLiftingModelChecker<SparseModelType, ConstantType>*>(this->parameterLiftingChecker.get());
                STORM_LOG_ASSERT(dtmcPLChecker, "Underlying Parameter lifting checker has unexpected type");
                auto exactDtmcPLChecker = dynamic_cast<storm::modelchecker::parametric::SparseDtmcParameterLiftingModelChecker<SparseModelType, ExactConstantType>*>(this->exactParameterLiftingChecker.get());
                STORM_LOG_ASSERT(exactDtmcPLChecker, "Underlying exact parameter lifting checker has unexpected type");
                exactDtmcPLChecker->getCurrentMaxScheduler() = dtmcPLChecker->getCurrentMaxScheduler();
                exactDtmcPLChecker->getCurrentMinScheduler() = dtmcPLChecker->getCurrentMinScheduler();
            }
       
#ifdef STORM_HAVE_CARL
            template class SparseDtmcRegionChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double, storm::RationalNumber>;
            template class SparseDtmcRegionChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, storm::RationalNumber>;
#endif
        } // namespace parametric
    } //namespace modelchecker
} //namespace storm

