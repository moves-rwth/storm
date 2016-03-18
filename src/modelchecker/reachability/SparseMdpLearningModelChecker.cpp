#include "src/modelchecker/reachability/SparseMdpLearningModelChecker.h"

#include "src/storage/SparseMatrix.h"
#include "src/storage/sparse/StateStorage.h"

#include "src/generator/PrismNextStateGenerator.h"

#include "src/logic/FragmentSpecification.h"

#include "src/utility/prism.h"

#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"

#include "src/utility/macros.h"
#include "src/exceptions/NotSupportedException.h"

namespace storm {
    namespace modelchecker {
        template<typename ValueType>
        SparseMdpLearningModelChecker<ValueType>::SparseMdpLearningModelChecker(storm::prism::Program const& program) : program(storm::utility::prism::preprocessProgram<ValueType>(program)), variableInformation(this->program) {
            // Intentionally left empty.
        }
        
        template<typename ValueType>
        bool SparseMdpLearningModelChecker<ValueType>::canHandle(CheckTask<storm::logic::Formula> const& checkTask) const {
            storm::logic::Formula const& formula = checkTask.getFormula();
            storm::logic::FragmentSpecification fragment = storm::logic::propositional().setProbabilityOperatorsAllowed(true).setReachabilityProbabilityFormulasAllowed(true).setNestedOperatorsAllowed(false);
            return formula.isInFragment(fragment) && checkTask.isOnlyInitialStatesRelevantSet();
        }
        
        template<typename ValueType>
        void SparseMdpLearningModelChecker<ValueType>::updateProbabilities(StateType const& sourceStateId, uint32_t action, StateType const& targetStateId, std::vector<std::vector<storm::storage::MatrixEntry<StateType, ValueType>>> const& transitionMatrix, std::vector<StateType> const& rowGroupIndices, std::vector<StateType> const& stateToRowGroupMapping, std::vector<ValueType>& lowerBounds, std::vector<ValueType>& upperBounds) const {
            // Find out which row of the matrix we have to consider for the given action.
            StateType sourceRowGroup = stateToRowGroupMapping[sourceStateId];
            StateType sourceRow = sourceRowGroup + action;
            
            // Compute the new lower/upper values of the action.
            ValueType newLowerValue = storm::utility::zero<ValueType>();
            ValueType newUpperValue = storm::utility::zero<ValueType>();
            for (auto const& element : transitionMatrix[sourceRow]) {
                newLowerValue += element.getValue() * upperBounds[stateToRowGroupMapping[element.getColumn()]];
                newUpperValue += element.getValue() * lowerBounds[stateToRowGroupMapping[element.getColumn()]];
            }
            
            // And set them as the current value.
            lowerBounds[stateToRowGroupMapping[sourceStateId]] = newLowerValue;
            upperBounds[stateToRowGroupMapping[sourceStateId]] = newUpperValue;
        }
        
        template<typename ValueType>
        std::unique_ptr<CheckResult> SparseMdpLearningModelChecker<ValueType>::computeReachabilityProbabilities(CheckTask<storm::logic::EventuallyFormula> const& checkTask) {
            storm::logic::EventuallyFormula const& eventuallyFormula = checkTask.getFormula();
            storm::logic::Formula const& subformula = eventuallyFormula.getSubformula();
            STORM_LOG_THROW(subformula.isAtomicExpressionFormula() || subformula.isAtomicLabelFormula(), storm::exceptions::NotSupportedException, "Learning engine can only deal with formulas of the form 'F \"label\"' or 'F expression'.");
            storm::expressions::Expression targetStateExpression;
            if (subformula.isAtomicExpressionFormula()) {
                targetStateExpression = subformula.asAtomicExpressionFormula().getExpression();
            } else {
                targetStateExpression = program.getLabelExpression(subformula.asAtomicLabelFormula().getLabel());
            }
            
            // A container for the encountered states.
            storm::storage::sparse::StateStorage<StateType> stateStorage(variableInformation.getTotalBitOffset(true));
            
            // A container that stores the states that were already expanded.
            storm::storage::BitVector expandedStates;
            
            // A generator used to explore the model.
            storm::generator::PrismNextStateGenerator<ValueType, StateType> generator(program, variableInformation, false);
            
            // A container that stores the transitions found so far.
            std::vector<std::vector<storm::storage::MatrixEntry<StateType, ValueType>>> matrix;

            // A vector storing where the row group of each state starts.
            std::vector<StateType> rowGroupIndices;
            
            // A vector storing the mapping from state ids to row groups.
            std::vector<StateType> stateToRowGroupMapping;
            
            // Vectors to store the lower/upper bounds for each action (in each state).
            std::vector<ValueType> lowerBounds;
            std::vector<ValueType> upperBounds;

            // Create a callback for the next-state generator to enable it to request the index of states.
            std::function<StateType (storm::generator::CompressedState const&)> stateToIdCallback = [&stateStorage] (storm::generator::CompressedState const& state) -> StateType {
                StateType newIndex = stateStorage.numberOfStates;
                
                // Check, if the state was already registered.
                std::pair<uint32_t, std::size_t> actualIndexBucketPair = stateStorage.stateToId.findOrAddAndGetBucket(state, newIndex);
                
                if (actualIndexBucketPair.first == newIndex) {
                    ++stateStorage.numberOfStates;
                }
                
                return actualIndexBucketPair.first;
            };

            stateStorage.initialStateIndices = generator.getInitialStates(stateToIdCallback);
            STORM_LOG_THROW(stateStorage.initialStateIndices.size() == 1, storm::exceptions::NotSupportedException, "Currently only models with one initial state are supported by the learning engine.");
            
            // Now perform the actual sampling.
            std::unordered_map<StateType, storm::generator::CompressedState> unexploredStates;
            std::vector<std::pair<StateType, uint32_t>> stateActionStack;
            stateActionStack.push_back(std::make_pair(stateStorage.initialStateIndices.front(), 0));
            bool foundTargetState = false;
            
            while (!foundTargetState) {
                StateType const& currentStateId = stateActionStack.back().first;
                
                // If the state is not yet expanded, we need to retrieve its behaviors.
                if (!expandedStates.get(currentStateId)) {
                    // First, we need to get the compressed state back from the id.
                    auto it = unexploredStates.find(currentStateId);
                    STORM_LOG_ASSERT(it != unexploredStates.end(), "Unable to find unexplored state.");
                    storm::storage::BitVector currentState = it->second;
                    
                    // Before generating the behavior of the state, we need to determine whether it's a target state that
                    // does not need to be expanded.
                    generator.load(currentState);
                    if (generator.satisfies(targetStateExpression)) {
                        // If it's in fact a goal state, we need to go backwards in the stack and update the probabilities.
                        foundTargetState = true;
                        stateActionStack.pop_back();
                        
                        while (!stateActionStack.empty()) {
                            updateProbabilities(stateActionStack.back().first, stateActionStack.back().second, currentStateId, matrix, rowGroupIndices, stateToRowGroupMapping, lowerBounds, upperBounds);
                        }
                        break;
                    } else {
                        // If it needs to be expanded, we use the generator to retrieve the behavior of the new state.
                        storm::generator::StateBehavior<ValueType, StateType> behavior = generator.expand(stateToIdCallback);
                        
                        stateToRowGroupMapping.push_back(rowGroupIndices.size());
                        rowGroupIndices.push_back(matrix.size());
                        
                        // Next, we insert the behavior into our matrix structure.
                        for (auto const& choice : behavior) {
                            matrix.resize(matrix.size() + 1);
                            for (auto const& entry : choice) {
                                matrix.back().push_back(storm::storage::MatrixEntry<StateType, ValueType>(entry.first, entry.second));
                            }
                        }
                        
                        // Now that we have explored the state, we can dispose of it.
                        unexploredStates.erase(it);
                    }
                }
                
                // At this point, we can be sure that the state was expanded and that we can sample according to the probabilities.
                // TODO: set action of topmost stack element
            }

            return nullptr;
        }
        
        template class SparseMdpLearningModelChecker<double>;
    }
}