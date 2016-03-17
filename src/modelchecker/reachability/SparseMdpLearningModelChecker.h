#ifndef STORM_MODELCHECKER_REACHABILITY_SPARSEMDPLEARNINGMODELCHECKER_H_
#define STORM_MODELCHECKER_REACHABILITY_SPARSEMDPLEARNINGMODELCHECKER_H_

#include "src/modelchecker/AbstractModelChecker.h"

#include "src/storage/prism/Program.h"

#include "src/generator/PrismNextStateGenerator.h"
#include "src/generator/VariableInformation.h"

#include "src/utility/constants.h"

namespace storm {
    namespace modelchecker {
        
        template<typename ValueType>
        class SparseMdpLearningModelChecker : public AbstractModelChecker {
        public:
            typedef uint32_t StateType;
            
            SparseMdpLearningModelChecker(storm::prism::Program const& program);
            
            virtual bool canHandle(CheckTask<storm::logic::Formula> const& checkTask) const override;

            virtual std::unique_ptr<CheckResult> computeReachabilityProbabilities(CheckTask<storm::logic::EventuallyFormula> const& checkTask) override;
            
        private:
            StateType getOrAddStateIndex(storm::generator::CompressedState const& state);
            
            // The program that defines the model to check.
            storm::prism::Program program;
            
            // The variable information.
            storm::generator::VariableInformation variableInformation;
            
            // A generator used to explore the model.
            storm::generator::PrismNextStateGenerator<ValueType, StateType> generator;
        };
    }
}

#endif /* STORM_MODELCHECKER_REACHABILITY_SPARSEMDPLEARNINGMODELCHECKER_H_ */