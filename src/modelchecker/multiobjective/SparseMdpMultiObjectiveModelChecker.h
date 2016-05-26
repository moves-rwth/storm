#ifndef STORM_MODELCHECKER_MULTIOBJECTIVE_SPARSEMDPMULTIOBJECTIVEMODELCHECKER_H_
#define STORM_MODELCHECKER_MULTIOBJECTIVE_SPARSEMDPMULTIOBJECTIVEMODELCHECKER_H_

#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"

namespace storm {
    namespace modelchecker {
        template<class SparseMdpModelType>
        class SparseMdpMultiObjectiveModelChecker : public SparseMdpPrctlModelChecker<SparseMdpModelType> {
        public:
            typedef typename SparseMdpModelType::ValueType ValueType;
            typedef typename SparseMdpModelType::RewardModelType RewardModelType;
            
            explicit SparseMdpMultiObjectiveModelChecker(SparseMdpModelType const& model);
            
            virtual bool canHandle(CheckTask<storm::logic::Formula> const& checkTask) const override;

            virtual std::unique_ptr<CheckResult> checkMultiObjectiveFormula(CheckTask<storm::logic::MultiObjectiveFormula> const& checkTask) override;
            
        private:


        };
    } // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_MULTIOBJECTIVE_SPARSEMDPMULTIOBJECTIVEMODELCHECKER_H_ */
