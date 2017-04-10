#include "storm/modelchecker/parametric/SparseMdpRegionChecker.h"

#include "storm/adapters/CarlAdapter.h"

#include "storm/modelchecker/parametric/SparseMdpParameterLiftingModelChecker.h"
#include "storm/modelchecker/parametric/SparseMdpInstantiationModelChecker.h"
#include "storm/transformer/SparseParametricMdpSimplifier.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/utility/NumberTraits.h"


namespace storm {
    namespace modelchecker {
        namespace parametric {

            template <typename SparseModelType, typename ConstantType, typename ExactConstantType>
            SparseMdpRegionChecker<SparseModelType, ConstantType, ExactConstantType>::SparseMdpRegionChecker(SparseModelType const& parametricModel) : RegionChecker<SparseModelType, ConstantType, ExactConstantType>(parametricModel) {
                // Intentionally left empty
            }
    
            template <typename SparseModelType, typename ConstantType, typename ExactConstantType>
            void SparseMdpRegionChecker<SparseModelType, ConstantType, ExactConstantType>::simplifyParametricModel(CheckTask<storm::logic::Formula, typename SparseModelType::ValueType> const& checkTask) {
                storm::transformer::SparseParametricMdpSimplifier<SparseModelType> simplifier(this->parametricModel);
                if(simplifier.simplify(checkTask.getFormula())) {
                    this->simplifiedModel = simplifier.getSimplifiedModel();
                    this->currentFormula = simplifier.getSimplifiedFormula();
                } else {
                    this->simplifiedModel = nullptr;
                    this->currentFormula = checkTask.getFormula().asSharedPointer();
                }
            }
            
            template <typename SparseModelType, typename ConstantType, typename ExactConstantType>
            void SparseMdpRegionChecker<SparseModelType, ConstantType, ExactConstantType>::initializeUnderlyingCheckers() {
                if (this->settings.applyExactValidation) {
                    STORM_LOG_WARN_COND(!storm::NumberTraits<ConstantType>::IsExact, "Exact validation is not necessarry if the original computation is already exact");
                    this->exactParameterLiftingChecker = std::make_unique<SparseMdpParameterLiftingModelChecker<SparseModelType, ExactConstantType>>(this->getConsideredParametricModel());
                }
                this->parameterLiftingChecker = std::make_unique<SparseMdpParameterLiftingModelChecker<SparseModelType, ConstantType>>(this->getConsideredParametricModel());
                this->instantiationChecker = std::make_unique<SparseMdpInstantiationModelChecker<SparseModelType, ConstantType>>(this->getConsideredParametricModel());
                this->instantiationChecker->setInstantiationsAreGraphPreserving(true);
            }
    
            template <typename SparseModelType, typename ConstantType, typename ExactConstantType>
            void SparseMdpRegionChecker<SparseModelType, ConstantType, ExactConstantType>::applyHintsToExactChecker() {
                auto MdpPLChecker = dynamic_cast<storm::modelchecker::parametric::SparseMdpParameterLiftingModelChecker<SparseModelType, ConstantType>*>(this->parameterLiftingChecker.get());
                STORM_LOG_ASSERT(MdpPLChecker, "Underlying Parameter lifting checker has unexpected type");
                auto exactMdpPLChecker = dynamic_cast<storm::modelchecker::parametric::SparseMdpParameterLiftingModelChecker<SparseModelType, ExactConstantType>*>(this->exactParameterLiftingChecker.get());
                STORM_LOG_ASSERT(exactMdpPLChecker, "Underlying exact parameter lifting checker has unexpected type");
                exactMdpPLChecker->getCurrentMaxScheduler() = MdpPLChecker->getCurrentMaxScheduler();
                exactMdpPLChecker->getCurrentMinScheduler() = MdpPLChecker->getCurrentMinScheduler();
                exactMdpPLChecker->getCurrentPlayer1Scheduler() = MdpPLChecker->getCurrentPlayer1Scheduler();
            }
            
       
#ifdef STORM_HAVE_CARL
            template class SparseMdpRegionChecker<storm::models::sparse::Mdp<storm::RationalFunction>, double, storm::RationalNumber>;
            template class SparseMdpRegionChecker<storm::models::sparse::Mdp<storm::RationalFunction>, storm::RationalNumber>;
#endif
        } // namespace parametric
    } //namespace modelchecker
} //namespace storm

