#ifndef STORM_MODELCHECKER_REACHABILITY_SPARSEMDPLEARNINGMODELCHECKER_H_
#define STORM_MODELCHECKER_REACHABILITY_SPARSEMDPLEARNINGMODELCHECKER_H_

#include <random>

#include "src/modelchecker/AbstractModelChecker.h"

#include "src/storage/prism/Program.h"

#include "src/generator/CompressedState.h"
#include "src/generator/VariableInformation.h"

#include "src/utility/ConstantsComparator.h"
#include "src/utility/constants.h"

namespace storm {
    namespace storage {
        namespace sparse {
            template<typename StateType>
            class StateStorage;
        }
    }
    
    namespace generator {
        template<typename ValueType, typename StateType>
        class PrismNextStateGenerator;
    }
    
    namespace modelchecker {
        
        template<typename ValueType>
        class SparseMdpLearningModelChecker : public AbstractModelChecker {
        public:
            typedef uint32_t StateType;
            typedef boost::container::flat_set<StateType> ChoiceSet;
            typedef std::shared_ptr<ChoiceSet> ChoiceSetPointer;
            
            SparseMdpLearningModelChecker(storm::prism::Program const& program, boost::optional<std::map<storm::expressions::Variable, storm::expressions::Expression>> const& constantDefinitions);
            
            virtual bool canHandle(CheckTask<storm::logic::Formula> const& checkTask) const override;

            virtual std::unique_ptr<CheckResult> computeReachabilityProbabilities(CheckTask<storm::logic::EventuallyFormula> const& checkTask) override;
            
        private:
            std::tuple<StateType, ValueType, ValueType> performLearningProcedure(storm::expressions::Expression const& targetStateExpression, storm::storage::sparse::StateStorage<StateType>& stateStorage, storm::generator::PrismNextStateGenerator<ValueType, StateType>& generator, std::function<StateType (storm::generator::CompressedState const&)> const& stateToIdCallback, std::vector<std::vector<storm::storage::MatrixEntry<StateType, ValueType>>>& matrix, std::vector<StateType>& rowGroupIndices, std::vector<StateType>& stateToRowGroupMapping, std::unordered_map<StateType, storm::generator::CompressedState>& unexploredStates, StateType const& unexploredMarker);
            
            void updateProbabilities(StateType const& sourceStateId, uint32_t action, std::vector<std::vector<storm::storage::MatrixEntry<StateType, ValueType>>> const& transitionMatrix, std::vector<StateType> const& rowGroupIndices, std::vector<StateType> const& stateToRowGroupMapping, std::vector<ValueType>& lowerBounds, std::vector<ValueType>& upperBounds, std::vector<ValueType>& lowerBoundsPerState, std::vector<ValueType>& upperBoundsPerState, StateType const& unexploredMarker) const;
            
            void updateProbabilitiesUsingStack(std::vector<std::pair<StateType, uint32_t>>& stateActionStack, std::vector<std::vector<storm::storage::MatrixEntry<StateType, ValueType>>> const& transitionMatrix, std::vector<StateType> const& rowGroupIndices, std::vector<StateType> const& stateToRowGroupMapping, std::vector<ValueType>& lowerBounds, std::vector<ValueType>& upperBounds, std::vector<ValueType>& lowerBoundsPerState, std::vector<ValueType>& upperBoundsPerState, StateType const& unexploredMarker) const;
            
            uint32_t sampleFromMaxActions(StateType currentStateId, std::vector<std::vector<storm::storage::MatrixEntry<StateType, ValueType>>> const& transitionMatrix, std::vector<StateType> const& rowGroupIndices, std::vector<StateType> const& stateToRowGroupMapping, std::vector<ValueType> const& upperBoundsPerState, std::unordered_map<StateType, ChoiceSetPointer> const& stateToLeavingChoicesOfEndComponent, StateType const& unexploredMarker);
            
            StateType sampleSuccessorFromAction(StateType chosenAction, std::vector<std::vector<storm::storage::MatrixEntry<StateType, ValueType>>> const& transitionMatrix, std::vector<StateType> const& rowGroupIndices, std::vector<StateType> const& stateToRowGroupMapping);
            
            void detectEndComponents(std::vector<std::pair<StateType, uint32_t>> const& stateActionStack, boost::container::flat_set<StateType>& terminalStates, std::vector<std::vector<storm::storage::MatrixEntry<StateType, ValueType>>>& transitionMatrix, std::vector<StateType> const& rowGroupIndices, std::vector<StateType> const& stateToRowGroupMapping, std::vector<ValueType>& lowerBoundsPerAction, std::vector<ValueType>& upperBoundsPerAction, std::vector<ValueType>& lowerBoundsPerState, std::vector<ValueType>& upperBoundsPerState, std::unordered_map<StateType, ChoiceSetPointer>& stateToLeavingChoicesOfEndComponent, StateType const& unexploredMarker) const;
            
            std::pair<ValueType, ValueType> computeValuesOfChoice(uint32_t action, std::vector<std::vector<storm::storage::MatrixEntry<StateType, ValueType>>> const& transitionMatrix, std::vector<StateType> const& stateToRowGroupMapping, std::vector<ValueType> const& lowerBoundsPerState, std::vector<ValueType> const& upperBoundsPerState, StateType const& unexploredMarker);
            
            std::pair<ValueType, ValueType> computeValuesOfState(StateType currentStateId, std::vector<std::vector<storm::storage::MatrixEntry<StateType, ValueType>>> const& transitionMatrix, std::vector<StateType> const& rowGroupIndices, std::vector<StateType> const& stateToRowGroupMapping, std::vector<ValueType> const& lowerBounds, std::vector<ValueType> const& upperBounds, std::vector<ValueType> const& lowerBoundsPerState, std::vector<ValueType> const& upperBoundsPerState, StateType const& unexploredMarker);
            
            storm::expressions::Expression getTargetStateExpression(storm::logic::Formula const& subformula);
                
            // The program that defines the model to check.
            storm::prism::Program program;
            
            // The variable information.
            storm::generator::VariableInformation variableInformation;
            
            // The random number generator.
            std::default_random_engine randomGenerator;
            
            // A comparator used to determine whether values are equal.
            storm::utility::ConstantsComparator<ValueType> comparator;
        };
    }
}

#endif /* STORM_MODELCHECKER_REACHABILITY_SPARSEMDPLEARNINGMODELCHECKER_H_ */