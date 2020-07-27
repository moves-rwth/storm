#include "storm-pars/modelchecker/region/ValidatingSparseDtmcParameterLiftingModelChecker.h"
#include "storm-pars/transformer/SparseParametricDtmcSimplifier.h"

#include "storm/models/sparse/Dtmc.h"
#include "storm/utility/macros.h"

#include "storm/exceptions/UnexpectedException.h"

namespace storm {
    namespace modelchecker {
        
        template <typename SparseModelType, typename ImpreciseType, typename PreciseType>
        ValidatingSparseDtmcParameterLiftingModelChecker<SparseModelType, ImpreciseType, PreciseType>::ValidatingSparseDtmcParameterLiftingModelChecker() {
            // Intentionally left empty
        }

        template <typename SparseModelType, typename ImpreciseType, typename PreciseType>
        void ValidatingSparseDtmcParameterLiftingModelChecker<SparseModelType, ImpreciseType, PreciseType>::specify(Environment const& env, std::shared_ptr<storm::models::ModelBase> parametricModel, CheckTask<storm::logic::Formula, typename SparseModelType::ValueType> const& checkTask, bool generateRegionSplitEstimates, bool allowModelSimplifications) {
            STORM_LOG_ASSERT(this->canHandle(parametricModel, checkTask), "specified model and formula can not be handled by this.");

            auto dtmc = parametricModel->template as<SparseModelType>();
            auto simplifier = storm::transformer::SparseParametricDtmcSimplifier<SparseModelType>(*dtmc);

            if (!simplifier.simplify(checkTask.getFormula())) {
                STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Simplifying the model was not successfull.");
            }

            auto simplifiedTask = checkTask.substituteFormula(*simplifier.getSimplifiedFormula());

            impreciseChecker.specify(env, simplifier.getSimplifiedModel(), simplifiedTask, false, true);
            preciseChecker.specify(env, simplifier.getSimplifiedModel(), simplifiedTask, false, true);
        }
        
        template <typename SparseModelType, typename ImpreciseType, typename PreciseType>
        SparseParameterLiftingModelChecker<SparseModelType, ImpreciseType>& ValidatingSparseDtmcParameterLiftingModelChecker<SparseModelType, ImpreciseType, PreciseType>::getImpreciseChecker() {
            return impreciseChecker;
        }
        
        template <typename SparseModelType, typename ImpreciseType, typename PreciseType>
        SparseParameterLiftingModelChecker<SparseModelType, ImpreciseType> const& ValidatingSparseDtmcParameterLiftingModelChecker<SparseModelType, ImpreciseType, PreciseType>::getImpreciseChecker() const {
            return impreciseChecker;
        }
        
        template <typename SparseModelType, typename ImpreciseType, typename PreciseType>
        SparseParameterLiftingModelChecker<SparseModelType, PreciseType>& ValidatingSparseDtmcParameterLiftingModelChecker<SparseModelType, ImpreciseType, PreciseType>::getPreciseChecker() {
            return preciseChecker;
        }
        
        template <typename SparseModelType, typename ImpreciseType, typename PreciseType>
        SparseParameterLiftingModelChecker<SparseModelType, PreciseType> const& ValidatingSparseDtmcParameterLiftingModelChecker<SparseModelType, ImpreciseType, PreciseType>::getPreciseChecker() const {
            return preciseChecker;
        }
        
        template <typename SparseModelType, typename ImpreciseType, typename PreciseType>
        void ValidatingSparseDtmcParameterLiftingModelChecker<SparseModelType, ImpreciseType, PreciseType>::applyHintsToPreciseChecker() {
        
            if (impreciseChecker.getCurrentMaxScheduler()) {
                preciseChecker.getCurrentMaxScheduler() = impreciseChecker.getCurrentMaxScheduler()->template toValueType<PreciseType>();
            }
            if (impreciseChecker.getCurrentMinScheduler()) {
                preciseChecker.getCurrentMinScheduler() = impreciseChecker.getCurrentMinScheduler()->template toValueType<PreciseType>();
            }
        }
        
        template class ValidatingSparseDtmcParameterLiftingModelChecker<storm::models::sparse::Dtmc<storm::RationalFunction>, double, storm::RationalNumber>;
    }
}
