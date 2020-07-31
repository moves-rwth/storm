#include "storm-pars/modelchecker/region/ValidatingSparseMdpParameterLiftingModelChecker.h"
#include "storm-pars/transformer/SparseParametricMdpSimplifier.h"

#include "storm/models/sparse/Mdp.h"
#include "storm/utility/macros.h"

#include "storm/exceptions/UnexpectedException.h"

namespace storm {
    namespace modelchecker {
        
        template <typename SparseModelType, typename ImpreciseType, typename PreciseType>
        ValidatingSparseMdpParameterLiftingModelChecker<SparseModelType, ImpreciseType, PreciseType>::ValidatingSparseMdpParameterLiftingModelChecker() {
            // Intentionally left empty
        }


        template <typename SparseModelType, typename ImpreciseType, typename PreciseType>
        void ValidatingSparseMdpParameterLiftingModelChecker<SparseModelType, ImpreciseType, PreciseType>::specify(Environment const& env, std::shared_ptr<storm::models::ModelBase> parametricModel, CheckTask<storm::logic::Formula, typename SparseModelType::ValueType> const& checkTask, bool generateRegionSplitEstimates, bool allowModelSimplifications) {
            STORM_LOG_ASSERT(this->canHandle(parametricModel, checkTask), "specified model and formula can not be handled by this.");

            auto mdp = parametricModel->template as<SparseModelType>();
            auto simplifier = storm::transformer::SparseParametricMdpSimplifier<SparseModelType>(*mdp);

            if (!simplifier.simplify(checkTask.getFormula())) {
                STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Simplifying the model was not successfull.");
            }

            auto simplifiedTask = checkTask.substituteFormula(*simplifier.getSimplifiedFormula());

            impreciseChecker.specify(env, simplifier.getSimplifiedModel(), simplifiedTask, false, true);
            preciseChecker.specify(env, simplifier.getSimplifiedModel(), simplifiedTask, false, true);
        }
        
        template <typename SparseModelType, typename ImpreciseType, typename PreciseType>
        SparseParameterLiftingModelChecker<SparseModelType, ImpreciseType>& ValidatingSparseMdpParameterLiftingModelChecker<SparseModelType, ImpreciseType, PreciseType>::getImpreciseChecker() {
            return impreciseChecker;
        }
        
        template <typename SparseModelType, typename ImpreciseType, typename PreciseType>
        SparseParameterLiftingModelChecker<SparseModelType, ImpreciseType> const& ValidatingSparseMdpParameterLiftingModelChecker<SparseModelType, ImpreciseType, PreciseType>::getImpreciseChecker() const {
            return impreciseChecker;
        }
        
        template <typename SparseModelType, typename ImpreciseType, typename PreciseType>
        SparseParameterLiftingModelChecker<SparseModelType, PreciseType>& ValidatingSparseMdpParameterLiftingModelChecker<SparseModelType, ImpreciseType, PreciseType>::getPreciseChecker() {
            return preciseChecker;
        }
        
        template <typename SparseModelType, typename ImpreciseType, typename PreciseType>
        SparseParameterLiftingModelChecker<SparseModelType, PreciseType> const& ValidatingSparseMdpParameterLiftingModelChecker<SparseModelType, ImpreciseType, PreciseType>::getPreciseChecker() const {
            return preciseChecker;
        }
        
        template <typename SparseModelType, typename ImpreciseType, typename PreciseType>
        void ValidatingSparseMdpParameterLiftingModelChecker<SparseModelType, ImpreciseType, PreciseType>::applyHintsToPreciseChecker() {
        
            if (impreciseChecker.getCurrentMaxScheduler()) {
                preciseChecker.getCurrentMaxScheduler() = impreciseChecker.getCurrentMaxScheduler()->template toValueType<PreciseType>();
            }
            if (impreciseChecker.getCurrentMinScheduler()) {
                preciseChecker.getCurrentMinScheduler() = impreciseChecker.getCurrentMinScheduler()->template toValueType<PreciseType>();
            }
            if (impreciseChecker.getCurrentPlayer1Scheduler()) {
                preciseChecker.getCurrentPlayer1Scheduler() = impreciseChecker.getCurrentPlayer1Scheduler()->template toValueType<PreciseType>();
            }
        }
            
        template class ValidatingSparseMdpParameterLiftingModelChecker<storm::models::sparse::Mdp<storm::RationalFunction>, double, storm::RationalNumber>;
    }
}
