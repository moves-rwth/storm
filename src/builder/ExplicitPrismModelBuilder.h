#ifndef STORM_ADAPTERS_EXPLICITPRISMMODELBUILDER_H
#define	STORM_ADAPTERS_EXPLICITPRISMMODELBUILDER_H

#include <memory>
#include <utility>
#include <vector>
#include <queue>
#include <cstdint>
#include <boost/functional/hash.hpp>
#include <boost/container/flat_set.hpp>
#include <boost/container/flat_map.hpp>

#include "src/storage/prism/Program.h"
#include "src/storage/expressions/SimpleValuation.h"
#include "src/storage/expressions/ExprtkExpressionEvaluator.h"
#include "src/storage/BitVectorHashMap.h"
#include "src/logic/Formulas.h"
#include "src/models/AbstractModel.h"
#include "src/models/AtomicPropositionsLabeling.h"
#include "src/storage/SparseMatrix.h"
#include "src/settings/SettingsManager.h"

#include "src/utility/ConstantsComparator.h"
#include "src/utility/PrismUtility.h"

namespace storm {
    namespace builder {
        
        using namespace storm::utility::prism;
                
        template<typename ValueType, typename IndexType = uint32_t>
        class ExplicitPrismModelBuilder {
        public:
            typedef storm::storage::BitVector CompressedState;
            
            // A structure holding information about the reachable state space.
            struct StateInformation {
                StateInformation(uint64_t bitsPerState);

                // This member stores all the states and maps them to their unique indices.
                storm::storage::BitVectorHashMap<IndexType> stateStorage;
                
                // A list of initial states in terms of their global indices.
                std::vector<IndexType> initialStateIndices;

                // The number of bits of each state.
                uint64_t bitsPerState;
                
                // A list of reachable states as indices in the stateToIndexMap.
                std::vector<storm::storage::BitVector> reachableStates;
            };
            
            // A structure storing information about the used variables of the program.
            struct VariableInformation {
                struct BooleanVariableInformation {
                    BooleanVariableInformation(storm::expressions::Variable const& variable, bool initialValue, uint_fast64_t bitOffset);
                    
                    // The boolean variable.
                    storm::expressions::Variable variable;
                    
                    // Its initial value.
                    bool initialValue;
                    
                    // Its bit offset in the compressed state.
                    uint_fast64_t bitOffset;
                };

                struct IntegerVariableInformation {
                    IntegerVariableInformation(storm::expressions::Variable const& variable, int_fast64_t initialValue, int_fast64_t lowerBound, int_fast64_t upperBound, uint_fast64_t bitOffset, uint_fast64_t bitWidth);
                    
                    // The integer variable.
                    storm::expressions::Variable variable;
                    
                    // Its initial value.
                    int_fast64_t initialValue;
                    
                    // The lower bound of its range.
                    int_fast64_t lowerBound;
                    
                    // The upper bound of its range.
                    int_fast64_t upperBound;
                    
                    // Its bit offset in the compressed state.
                    uint_fast64_t bitOffset;
                    
                    // Its bit width in the compressed state.
                    uint_fast64_t bitWidth;
                };
                
                // Provide methods to access the bit offset and width of variables in the compressed state.
                uint_fast64_t getBitOffset(storm::expressions::Variable const& variable) const;
                uint_fast64_t getBitWidth(storm::expressions::Variable const& variable) const;

                // The known boolean variables.
                boost::container::flat_map<storm::expressions::Variable, uint_fast64_t> booleanVariableToIndexMap;
                std::vector<BooleanVariableInformation> booleanVariables;

                // The known integer variables.
                boost::container::flat_map<storm::expressions::Variable, uint_fast64_t> integerVariableToIndexMap;
                std::vector<IntegerVariableInformation> integerVariables;
            };
            
            // A structure holding the individual components of a model.
            struct ModelComponents {
                ModelComponents();
                
                // The transition matrix.
                storm::storage::SparseMatrix<ValueType> transitionMatrix;
                
                // The state labeling.
                storm::models::AtomicPropositionsLabeling stateLabeling;
                
                // The state reward vector.
                std::vector<ValueType> stateRewards;
                
                // A matrix storing the reward for particular transitions.
                storm::storage::SparseMatrix<ValueType> transitionRewardMatrix;
                
                // A vector that stores a labeling for each choice.
                boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> choiceLabeling;
            };
            
            struct Options {
                /*!
                 * Creates an object representing the default building options.
                 */
                Options();
                
                /*! Creates an object representing the suggested building options assuming that the given formula is the
                 * only one to check.
                 *
                 * @param formula The formula based on which to choose the building options.
                 */
                Options(storm::logic::Formula const& formula);
                
                /*!
                 * Sets the constants definitions from the given string. The string must be of the form 'X=a,Y=b,Z=c',
                 * etc. where X,Y,Z are the variable names and a,b,c are the values of the constants.
                 *
                 * @param program The program managing the constants that shall be defined. Note that the program itself
                 * is not modified whatsoever.
                 * @param constantDefinitionString The string from which to parse the constants' values.
                 */
                void addConstantDefinitionsFromString(storm::prism::Program const& program, std::string const& constantDefinitionString);
                
                // A flag that indicates whether or not command labels are to be built.
                bool buildCommandLabels;
                
                // A flag that indicates whether or not a reward model is to be built.
                bool buildRewards;
                
                // An optional string, that, if given, indicates which of the reward models is to be built.
                boost::optional<std::string> rewardModelName;
                
                // An optional mapping that, if given, contains defining expressions for undefined constants.
                boost::optional<std::map<storm::expressions::Variable, storm::expressions::Expression>> constantDefinitions;
            };
            
            /*!
             * Convert the program given at construction time to an abstract model. The type of the model is the one
             * specified in the program. The given reward model name selects the rewards that the model will contain.
             *
             * @param program The program to translate.
             * @param constantDefinitionString A string that contains a comma-separated definition of all undefined
             * constants in the model.
             * @param rewardModel The reward model that is to be built.
             * @return The explicit model that was given by the probabilistic program.
             */
            static std::unique_ptr<storm::models::AbstractModel<ValueType>> translateProgram(storm::prism::Program program, Options const& options = Options());
            
        private:
            static void unpackStateIntoEvaluator(storm::storage::BitVector const& currentState, VariableInformation const& variableInformation, storm::expressions::ExprtkExpressionEvaluator& evaluator);
            
            /*!
             * Applies an update to the given state and returns the resulting new state object. This methods does not
             * modify the given state but returns a new one.
             *
             * @params state The state to which to apply the update.
             * @params update The update to apply.
             * @return The resulting state.
             */
            static CompressedState applyUpdate(VariableInformation const& variableInformation, CompressedState const& state, storm::prism::Update const& update, storm::expressions::ExprtkExpressionEvaluator const& evaluator);
            
            /*!
             * Applies an update to the given state and returns the resulting new state object. The update is evaluated
             * over the variable values of the given base state. This methods does not modify the given state but
             * returns a new one.
             *
             * @param state The state to which to apply the update.
             * @param baseState The state used for evaluating the update.
             * @param update The update to apply.
             * @return The resulting state.
             */
            static CompressedState applyUpdate(VariableInformation const& variableInformation, CompressedState const& state, CompressedState const& baseState, storm::prism::Update const& update, storm::expressions::ExprtkExpressionEvaluator const& evaluator);
            
            /*!
             * Retrieves the state id of the given state. If the state has not been encountered yet, it will be added to
             * the lists of all states with a new id. If the state was already known, the object that is pointed to by
             * the given state pointer is deleted and the old state id is returned. Note that the pointer should not be
             * used after invoking this method.
             *
             * @param state A pointer to a state for which to retrieve the index. This must not be used after the call.
             * @param stateInformation The information about the already explored part of the reachable state space.
             * @return A pair indicating whether the state was already discovered before and the state id of the state.
             */
            static IndexType getOrAddStateIndex(CompressedState const& state, StateInformation& stateInformation, std::queue<storm::storage::BitVector>& stateQueue);
    
            /*!
             * Retrieves all commands that are labeled with the given label and enabled in the given state, grouped by
             * modules.
             *
             * This function will iterate over all modules and retrieve all commands that are labeled with the given
             * action and active (i.e. enabled) in the current state. The result is a list of lists of commands in which
             * the inner lists contain all commands of exactly one module. If a module does not have *any* (including
             * disabled) commands, there will not be a list of commands of that module in the result. If, however, the
             * module has a command with a relevant label, but no enabled one, nothing is returned to indicate that there
             * is no legal transition possible.
             *
             * @param The program in which to search for active commands.
             * @param state The current state.
             * @param actionIndex The index of the action label to select.
             * @return A list of lists of active commands or nothing.
             */
            static boost::optional<std::vector<std::vector<std::reference_wrapper<storm::prism::Command const>>>> getActiveCommandsByActionIndex(storm::prism::Program const& program,storm::expressions::ExprtkExpressionEvaluator const& evaluator, uint_fast64_t const& actionIndex);
                        
            static std::vector<Choice<ValueType>> getUnlabeledTransitions(storm::prism::Program const& program, bool discreteTimeModel, StateInformation& stateInformation, VariableInformation const& variableInformation, storm::storage::BitVector const& currentState, bool choiceLabeling, storm::expressions::ExprtkExpressionEvaluator const& evaluator, std::queue<storm::storage::BitVector>& stateQueue, storm::utility::ConstantsComparator<ValueType> const& comparator);
            
            static std::vector<Choice<ValueType>> getLabeledTransitions(storm::prism::Program const& program, bool discreteTimeModel, StateInformation& stateInformation, VariableInformation const& variableInformation, storm::storage::BitVector const& currentState, bool choiceLabeling, storm::expressions::ExprtkExpressionEvaluator const& evaluator, std::queue<storm::storage::BitVector>& stateQueue, storm::utility::ConstantsComparator<ValueType> const& comparator);
            /*!
             * Builds the transition matrix and the transition reward matrix based for the given program.
             *
             * @param program The program for which to build the matrices.
             * @param variableInformation A structure containing information about the variables in the program.
             * @param transitionRewards A list of transition rewards that are to be considered in the transition reward
             * matrix.
             * @param stateInformation A structure containing information about the states of the program.
             * @param deterministicModel A flag indicating whether the model is supposed to be deterministic or not.
             * @param transitionMatrix A reference to an initialized matrix which is filled with all transitions by this
             * function.
             * @param transitionRewardMatrix A reference to an initialized matrix which is filled with all transition
             * rewards by this function.
             * @return A tuple containing a vector with all rows at which the nondeterministic choices of each state begin
             * and a vector containing the labels associated with each choice.
             */
            static boost::optional<std::vector<boost::container::flat_set<uint_fast64_t>>> buildMatrices(storm::prism::Program const& program, VariableInformation const& variableInformation, std::vector<storm::prism::TransitionReward> const& transitionRewards, StateInformation& stateInformation, bool commandLabels, bool deterministicModel, bool discreteTimeModel, storm::storage::SparseMatrixBuilder<ValueType>& transitionMatrixBuilder, storm::storage::SparseMatrixBuilder<ValueType>& transitionRewardMatrixBuilder);
            
            /*!
             * Explores the state space of the given program and returns the components of the model as a result.
             *
             * @param program The program whose state space to explore.
             * @param rewardModel The reward model that is to be considered.
             * @return A structure containing the components of the resulting model.
             */
            static ModelComponents buildModelComponents(storm::prism::Program const& program, storm::prism::RewardModel const& rewardModel, bool commandLabels = false);
            
            /*!
             * Builds the state labeling for the given program.
             *
             * @param program The program for which to build the state labeling.
             * @param variableInformation Information about the variables in the program.
             * @param stateInformation Information about the state space of the program.
             * @return The state labeling of the given program.
             */
            static storm::models::AtomicPropositionsLabeling buildStateLabeling(storm::prism::Program const& program, VariableInformation const& variableInformation, StateInformation const& stateInformation);

            /*!
             * Builds the state rewards for the given state space.
             *
             * @param rewards A vector of state rewards to consider.
             * @param stateInformation Information about the state space.
             * @return A vector containing the state rewards for the state space.
             */
            static std::vector<ValueType> buildStateRewards(storm::prism::Program const& program, VariableInformation const& variableInformation, std::vector<storm::prism::StateReward> const& rewards, StateInformation const& stateInformation);
        };
        
    } // namespace adapters
} // namespace storm

#endif	/* STORM_ADAPTERS_EXPLICITPRISMMODELBUILDER_H */
