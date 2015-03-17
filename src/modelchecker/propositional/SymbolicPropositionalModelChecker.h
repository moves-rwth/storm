#ifndef STORM_MODELCHECKER_SYMBOLICPROPOSITIONALMODELCHECKER_H_
#define STORM_MODELCHECKER_SYMBOLICPROPOSITIONALMODELCHECKER_H_

#include "src/modelchecker/AbstractModelChecker.h"

#include "src/models/symbolic/Model.h"

namespace storm {
    namespace modelchecker {
        
        template<storm::dd::DdType Type>
        class SymbolicPropositionalModelChecker : public AbstractModelChecker {
        public:
            explicit SymbolicPropositionalModelChecker(storm::models::symbolic::Model<Type> const& model);
            
            // The implemented methods of the AbstractModelChecker interface.
            virtual bool canHandle(storm::logic::Formula const& formula) const override;
            virtual std::unique_ptr<CheckResult> checkBooleanLiteralFormula(storm::logic::BooleanLiteralFormula const& stateFormula) override;
            virtual std::unique_ptr<CheckResult> checkAtomicLabelFormula(storm::logic::AtomicLabelFormula const& stateFormula) override;
            
        protected:
            /*!
             * Retrieves the model associated with this model checker instance.
             *
             * @return The model associated with this model checker instance.
             */
            virtual storm::models::symbolic::Model<Type> const& getModel() const;
            
            /*!
             * Retrieves the model associated with this model checker instance as the given template parameter type.
             *
             * @return The model associated with this model checker instance.
             */
            template<typename ModelType>
            ModelType const& getModelAs() const;
            
        private:
            // The model that is to be analyzed by the model checker.
            storm::models::symbolic::Model<Type> const& model;
        };
    }
}

#endif /* STORM_MODELCHECKER_SYMBOLICPROPOSITIONALMODELCHECKER_H_ */