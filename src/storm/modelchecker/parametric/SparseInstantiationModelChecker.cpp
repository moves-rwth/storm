#include "SparseInstantiationModelChecker.h"

#include "storm/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace modelchecker {
        namespace parametric {
            
            template <typename SparseModelType, typename ConstantType>
            SparseInstantiationModelChecker<SparseModelType, ConstantType>::SparseInstantiationModelChecker(SparseModelType const& parametricModel) : parametricModel(parametricModel) {
                //Intentionally left empty
            }
            
                
            template <typename SparseModelType, typename ConstantType>
            void SparseInstantiationModelChecker<SparseModelType, ConstantType>::specifyFormula(storm::modelchecker::CheckTask<storm::logic::Formula, typename SparseModelType::ValueType> const& checkTask) {
                storm::logic::Formula const& formula = checkTask.getFormula();
                STORM_LOG_THROW(this->canHandle(checkTask), storm::exceptions::InvalidArgumentException, "The model checker is not able to check the formula '" << formula << "'.");
                
                currentCheckTask = std::make_unique<storm::modelchecker::CheckTask<storm::logic::Formula, ConstantType>>(checkTask.getFormula(), checkTask.isOnlyInitialStatesRelevantSet());
                currentCheckTask->setProduceSchedulers(checkTask.isProduceSchedulersSet());
            }
            
        }
    }
}