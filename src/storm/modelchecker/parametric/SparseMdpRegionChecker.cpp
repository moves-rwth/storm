#include "storm/modelchecker/parametric/SparseMdpRegionChecker.h"

#include "storm/adapters/CarlAdapter.h"

#include "storm/modelchecker/parametric/SparseMdpParameterLiftingModelChecker.h"
#include "storm/modelchecker/parametric/SparseMdpInstantiationModelChecker.h"
#include "storm/transformer/SparseParametricMdpSimplifier.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/models/sparse/Mdp.h"
#include "SparseMdpRegionChecker.h"

namespace storm {
    namespace modelchecker {
        namespace parametric {

            template <typename SparseModelType, typename ConstantType>
            SparseMdpRegionChecker<SparseModelType, ConstantType>::SparseMdpRegionChecker(SparseModelType const& parametricModel) : RegionChecker<SparseModelType, ConstantType>(parametricModel) {
                // Intentionally left empty
            }
    
            template <typename SparseModelType, typename ConstantType>
            void SparseMdpRegionChecker<SparseModelType, ConstantType>::simplifyParametricModel(CheckTask<storm::logic::Formula, typename SparseModelType::ValueType> const& checkTask) {
                storm::transformer::SparseParametricMdpSimplifier<SparseModelType> simplifier(this->parametricModel);
                if(simplifier.simplify(checkTask.getFormula())) {
                    this->simplifiedModel = simplifier.getSimplifiedModel();
                    this->currentFormula = simplifier.getSimplifiedFormula();
                } else {
                    this->simplifiedModel = nullptr;
                    this->currentFormula = checkTask.getFormula().asSharedPointer();
                }
            }
            
            template <typename SparseModelType, typename ConstantType>
            void SparseMdpRegionChecker<SparseModelType, ConstantType>::initializeUnderlyingCheckers() {
                if (this->settings.applyExactValidation) {
                    //STORM_LOG_WARN_COND(!(std::is_same<ConstantType, typename RegionChecker<SparseModelType, ConstantType>::CoefficientType>::value), "Exact validation is not necessarry if the original computation is already exact"); // todo: use templated argument instead of storm::RationalNumber
                    STORM_LOG_WARN_COND(!(std::is_same<ConstantType, storm::RationalNumber>::value), "Exact validation is not necessarry if the original computation is already exact");
                    //this->exactParameterLiftingChecker = std::make_unique<SparseMdpParameterLiftingModelChecker<SparseModelType, typename RegionChecker<SparseModelType, ConstantType>::CoefficientType>>(this->getConsideredParametricModel()); // todo: use templated argument instead of storm::RationalNumber
                    this->exactParameterLiftingChecker = std::make_unique<SparseMdpParameterLiftingModelChecker<SparseModelType, storm::RationalNumber>>(this->getConsideredParametricModel());
                }
                this->parameterLiftingChecker = std::make_unique<SparseMdpParameterLiftingModelChecker<SparseModelType, ConstantType>>(this->getConsideredParametricModel());
                this->instantiationChecker = std::make_unique<SparseMdpInstantiationModelChecker<SparseModelType, ConstantType>>(this->getConsideredParametricModel());
                this->instantiationChecker->setInstantiationsAreGraphPreserving(true);
            }
    
            template <typename SparseModelType, typename ConstantType>
            void SparseMdpRegionChecker<SparseModelType, ConstantType>::applyHintsToExactChecker() {
                auto MdpPLChecker = dynamic_cast<storm::modelchecker::parametric::SparseMdpParameterLiftingModelChecker<SparseModelType, ConstantType>*>(this->parameterLiftingChecker.get());
                STORM_LOG_ASSERT(MdpPLChecker, "Underlying Parameter lifting checker has unexpected type");
                auto exactMdpPLChecker = dynamic_cast<storm::modelchecker::parametric::SparseMdpParameterLiftingModelChecker<SparseModelType, storm::RationalNumber>*>(this->exactParameterLiftingChecker.get());
//                auto exactMdpPLChecker = dynamic_cast<storm::modelchecker::parametric::SparseMdpParameterLiftingModelChecker<SparseModelType, typename RegionChecker<SparseModelType, ConstantType>::CoefficientType>*>(this->exactParameterLiftingChecker.get()); // todo: use template argument instead of storm::RationalNumber
                STORM_LOG_ASSERT(exactMdpPLChecker, "Underlying exact parameter lifting checker has unexpected type");
                exactMdpPLChecker->getCurrentMaxScheduler() = MdpPLChecker->getCurrentMaxScheduler();
                exactMdpPLChecker->getCurrentMinScheduler() = MdpPLChecker->getCurrentMinScheduler();
                exactMdpPLChecker->getCurrentPlayer1Scheduler() = MdpPLChecker->getCurrentPlayer1Scheduler();
            }
            
       
#ifdef STORM_HAVE_CARL
            template class SparseMdpRegionChecker<storm::models::sparse::Mdp<storm::RationalFunction>, double>;
            template class SparseMdpRegionChecker<storm::models::sparse::Mdp<storm::RationalFunction>, storm::RationalNumber>;
#endif
        } // namespace parametric
    } //namespace modelchecker
} //namespace storm

