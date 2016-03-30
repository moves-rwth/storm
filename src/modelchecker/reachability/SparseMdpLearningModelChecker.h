#ifndef STORM_MODELCHECKER_REACHABILITY_SPARSEMDPLEARNINGMODELCHECKER_H_
#define STORM_MODELCHECKER_REACHABILITY_SPARSEMDPLEARNINGMODELCHECKER_H_

#include <random>

#include "src/modelchecker/AbstractModelChecker.h"

#include "src/storage/prism/Program.h"

#include "src/generator/CompressedState.h"
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
            void updateProbabilities(StateType const& sourceStateId, uint32_t action, std::vector<std::vector<storm::storage::MatrixEntry<StateType, ValueType>>> const& transitionMatrix, std::vector<StateType> const& rowGroupIndices, std::vector<StateType> const& stateToRowGroupMapping, std::vector<ValueType>& lowerBounds, std::vector<ValueType>& upperBounds, std::vector<ValueType>& lowerBoundsPerState, std::vector<ValueType>& upperBoundsPerState, StateType const& unexploredMarker) const;
            
            void updateProbabilitiesUsingStack(std::vector<std::pair<StateType, uint32_t>>& stateActionStack, std::vector<std::vector<storm::storage::MatrixEntry<StateType, ValueType>>> const& transitionMatrix, std::vector<StateType> const& rowGroupIndices, std::vector<StateType> const& stateToRowGroupMapping, std::vector<ValueType>& lowerBounds, std::vector<ValueType>& upperBounds, std::vector<ValueType>& lowerBoundsPerState, std::vector<ValueType>& upperBoundsPerState, StateType const& unexploredMarker) const;
            
            uint32_t sampleFromMaxActions(StateType currentStateId, std::vector<std::vector<storm::storage::MatrixEntry<StateType, ValueType>>> const& transitionMatrix, std::vector<StateType> const& rowGroupIndices, std::vector<StateType> const& stateToRowGroupMapping, std::vector<ValueType>& upperBounds, StateType const& unexploredMarker);
            
            StateType sampleSuccessorFromAction(StateType currentStateId, std::vector<std::vector<storm::storage::MatrixEntry<StateType, ValueType>>> const& transitionMatrix, std::vector<StateType> const& rowGroupIndices, std::vector<StateType> const& stateToRowGroupMapping);
                        
            // The program that defines the model to check.
            storm::prism::Program program;
            
            // The variable information.
            storm::generator::VariableInformation variableInformation;
            
            // The random number generator.
            std::default_random_engine generator;
        };
    }
}

#endif /* STORM_MODELCHECKER_REACHABILITY_SPARSEMDPLEARNINGMODELCHECKER_H_ */