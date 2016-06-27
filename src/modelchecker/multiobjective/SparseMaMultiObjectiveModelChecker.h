#ifndef STORM_MODELCHECKER_MULTIOBJECTIVE_SPARSEMAMULTIOBJECTIVEMODELCHECKER_H_
#define STORM_MODELCHECKER_MULTIOBJECTIVE_SPARSEMAMULTIOBJECTIVEMODELCHECKER_H_

#include "src/modelchecker/csl/SparseMarkovAutomatonCslModelChecker.h"

namespace storm {
    namespace modelchecker {
        template<class SparseMaModelType>
        class SparseMaMultiObjectiveModelChecker : public SparseMarkovAutomatonCslModelChecker<SparseMaModelType> {
        public:
            typedef typename SparseMaModelType::ValueType ValueType;
            typedef typename SparseMaModelType::RewardModelType RewardModelType;
            
            explicit SparseMaMultiObjectiveModelChecker(SparseMaModelType const& model);
            
            virtual bool canHandle(CheckTask<storm::logic::Formula> const& checkTask) const override;

            virtual std::unique_ptr<CheckResult> checkMultiObjectiveFormula(CheckTask<storm::logic::MultiObjectiveFormula> const& checkTask) override;
            
        private:


        };
    } // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_MULTIOBJECTIVE_SPARSEMAMULTIOBJECTIVEMODELCHECKER_H_ */
