#ifndef STORM_MODELCHECKER_REACHABILITY_SPARSEMDPLEARNINGMODELCHECKER_H_
#define STORM_MODELCHECKER_REACHABILITY_SPARSEMDPLEARNINGMODELCHECKER_H_

#include "src/modelchecker/AbstractModelChecker.h"

#include "src/storage/prism/Program.h"

#include "src/utility/constants.h"

namespace storm {
    namespace modelchecker {
        
        template<typename ValueType>
        class SparseMdpLearningModelChecker : public AbstractModelChecker {
        public:
            SparseMdpLearningModelChecker(storm::prism::Program const& program);
            
            virtual bool canHandle(CheckTask<storm::logic::Formula> const& checkTask) const override;

            virtual std::unique_ptr<CheckResult> computeReachabilityProbabilities(CheckTask<storm::logic::EventuallyFormula> const& checkTask) override;
            
        private:
            // The program that defines the model to check.
            storm::prism::Program program;
        };
    }
}

#endif /* STORM_MODELCHECKER_REACHABILITY_SPARSEMDPLEARNINGMODELCHECKER_H_ */