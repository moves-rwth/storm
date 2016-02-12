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
        
        template<storm::dd::DdType Type, typename ValueType>
        class SymbolicPropositionalModelChecker : public AbstractModelChecker {
        public:
            explicit SymbolicPropositionalModelChecker(storm::models::symbolic::Model<Type, ValueType> const& model);
            
            // The implemented methods of the AbstractModelChecker interface.
            virtual bool canHandle(CheckTask<storm::logic::Formula> const& checkTask) const override;
            virtual std::unique_ptr<CheckResult> checkBooleanLiteralFormula(CheckTask<storm::logic::BooleanLiteralFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> checkAtomicLabelFormula(CheckTask<storm::logic::AtomicLabelFormula> const& checkTask) override;
            virtual std::unique_ptr<CheckResult> checkAtomicExpressionFormula(CheckTask<storm::logic::AtomicExpressionFormula> const& checkTask) override;

        protected:
            /*!
             * Retrieves the model associated with this model checker instance.
             *
             * @return The model associated with this model checker instance.
             */
            virtual storm::models::symbolic::Model<Type, ValueType> const& getModel() const;
            
            /*!
             * Retrieves the model associated with this model checker instance as the given template parameter type.
             *
             * @return The model associated with this model checker instance.
             */
            template<typename ModelType>
            ModelType const& getModelAs() const;
            
        private:
            // The model that is to be analyzed by the model checker.
            storm::models::symbolic::Model<Type, ValueType> const& model;
        };
        
    }
}

#endif /* STORM_MODELCHECKER_SYMBOLICPROPOSITIONALMODELCHECKER_H_ */