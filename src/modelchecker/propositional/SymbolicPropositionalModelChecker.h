#ifndef STORM_MODELCHECKER_SYMBOLICPROPOSITIONALMODELCHECKER_H_
#define STORM_MODELCHECKER_SYMBOLICPROPOSITIONALMODELCHECKER_H_

#include "src/modelchecker/AbstractModelChecker.h"

#include "src/storage/dd/DdType.h"

namespace storm {
    namespace models {
        namespace symbolic {
            template<storm::dd::DdType Type, typename ValueType>
            class Model;
        }
    }
    
    namespace modelchecker {
        
        template<typename ModelType>
        class SymbolicPropositionalModelChecker : public AbstractModelChecker<ModelType> {
        public:
            typedef typename ModelType::ValueType ValueType;
            static const storm::dd::DdType DdType = ModelType::DdType;
            explicit SymbolicPropositionalModelChecker(ModelType const& model);
            
            // The implemented methods of the AbstractModelChecker interface.
            virtual bool canHandle(CheckTask<storm::logic::Formula, ValueType> const& checkTask) const override;
            virtual std::unique_ptr<CheckResult> checkBooleanLiteralFormula(CheckTask<storm::logic::BooleanLiteralFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> checkAtomicLabelFormula(CheckTask<storm::logic::AtomicLabelFormula, ValueType> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> checkAtomicExpressionFormula(CheckTask<storm::logic::AtomicExpressionFormula, ValueType> const& checkTask) override;

        protected:
            /*!
             * Retrieves the model associated with this model checker instance.
             *
             * @return The model associated with this model checker instance.
             */
            virtual ModelType const& getModel() const;
            
        private:
            // The model that is to be analyzed by the model checker.
            ModelType const& model;
        };
        
    }
}

#endif /* STORM_MODELCHECKER_SYMBOLICPROPOSITIONALMODELCHECKER_H_ */