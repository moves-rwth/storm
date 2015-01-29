#ifndef STORM_MODELCHECKER_SPARSEPROPOSITIONALMODELCHECKER_H_
#define STORM_MODELCHECKER_SPARSEPROPOSITIONALMODELCHECKER_H_

#include "src/modelchecker/AbstractModelChecker.h"

#include "src/models/AbstractModel.h"

namespace storm {
    namespace modelchecker {
        
        template<typename ValueType>
        class SparsePropositionalModelChecker : public AbstractModelChecker {
        public:
            explicit SparsePropositionalModelChecker(storm::models::AbstractModel<ValueType> const& model);
            
            // The implemented methods of the AbstractModelChecker interface.
            virtual std::unique_ptr<CheckResult> checkBooleanLiteralFormula(storm::logic::BooleanLiteralFormula const& stateFormula) override;
            virtual std::unique_ptr<CheckResult> checkAtomicLabelFormula(storm::logic::AtomicLabelFormula const& stateFormula) override;
            
        protected:
            /*!
             * Retrieves the model associated with this model checker instance.
             *
             * @return The model associated with this model checker instance.
             */
            virtual storm::models::AbstractModel<ValueType> const& getModel() const;
            
            /*!
             * Retrieves the model associated with this model checker instance as the given template parameter type.
             *
             * @return The model associated with this model checker instance.
             */
            template<typename ModelType>
            ModelType const& getModelAs() const;
            
        private:
            // The model that is to be analyzed by the model checker.
            storm::models::AbstractModel<ValueType> const& model;
        };
    }
}

#endif /* STORM_MODELCHECKER_SPARSEPROPOSITIONALMODELCHECKER_H_ */