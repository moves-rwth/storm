#ifndef STORM_MODELCHECKER_SPARSEPROPOSITIONALMODELCHECKER_H_
#define STORM_MODELCHECKER_SPARSEPROPOSITIONALMODELCHECKER_H_

#include "src/modelchecker/AbstractModelChecker.h"


namespace storm {
    namespace modelchecker {
        
        template<typename SparseModelType>
        class SparsePropositionalModelChecker : public AbstractModelChecker {
        public:
            typedef typename SparseModelType::ValueType ValueType;
            typedef typename SparseModelType::RewardModelType RewardModelType;
            
            explicit SparsePropositionalModelChecker(SparseModelType const& model);
            
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
            SparseModelType const& getModel() const;
            
        private:
            // The model that is to be analyzed by the model checker.
            SparseModelType const& model;
        };
    }
}

#endif /* STORM_MODELCHECKER_SPARSEPROPOSITIONALMODELCHECKER_H_ */