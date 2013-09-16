/* 
 * File:   ExplicitModelAdapter.h
 * Author: Gereon Kremer
 *
 * Created on March 15, 2013, 11:42 AM
 */

#ifndef STORM_ADAPTERS_EXPLICITMODELADAPTER_H
#define	STORM_ADAPTERS_EXPLICITMODELADAPTER_H

#include <memory>
#include <unordered_map>
#include <utility>
#include <vector>
#include <boost/functional/hash.hpp>

#include "src/ir/Program.h"
#include "src/ir/StateReward.h"
#include "src/ir/TransitionReward.h"

#include "src/models/AbstractModel.h"
#include "src/models/AtomicPropositionsLabeling.h"
#include "src/storage/SparseMatrix.h"

namespace storm {
    namespace adapters {
        
        /*!
         * A state of the model, i.e. a valuation of all variables.
         */
        typedef std::pair<std::vector<bool>, std::vector<int_fast64_t>> StateType;
        
        /*!
         * A helper class that provides the functionality to compute a hash value for states of the model.
         */
        class StateHash {
        public:
            std::size_t operator()(StateType* state) const {
                size_t seed = 0;
                for (auto it : state->first) {
                    boost::hash_combine<bool>(seed, it);
                }
                for (auto it : state->second) {
                    boost::hash_combine<int_fast64_t>(seed, it);
                }
                return seed;
            }
        };
        
        /*!
         * A helper class that provides the functionality to compare states of the model.
         */
        class StateCompare {
        public:
            bool operator()(StateType* state1, StateType* state2) const {
                return *state1 == *state2;
            }
        };
        
        class ExplicitModelAdapter {
        public:
            /*!
             * Initializes the adapter with the given program.
             *
             * @param program The program from which to build the explicit model.
             */
            ExplicitModelAdapter(storm::ir::Program program);
            
            /*!
             * Destroys the adapter.
             */
            ~ExplicitModelAdapter();
            
            /*!
             * Convert the program given at construction time to an abstract model. The type of the model is the one
             * specified in the program. The given reward model name selects the rewards that the model will contain.
             *
             * @param constantDefinitionString A string that contains a comma-separated definition of all undefined
             * constants in the model.
             * @param rewardModelName The name of reward model to be added to the model. This must be either a reward
             * model of the program or the empty string. In the latter case, the constructed model will contain no 
             * rewards.
             * @return The explicit model that was given by the probabilistic program.
             */
            std::shared_ptr<storm::models::AbstractModel<double>> getModel(std::string const& constantDefinitionString = "", std::string const& rewardModelName = "");
            
        private:
            // Copying/Moving is disabled for this class
            ExplicitModelAdapter(ExplicitModelAdapter const& other) { }
            ExplicitModelAdapter(ExplicitModelAdapter && other) { }
            
            /*!
             * The precision that is to be used for sanity checks internally.
             */
            double precision;
            
            /*!
             * Sets some boolean variable in the given state object.
             *
             * @param state The state to modify.
             * @param index The index of the boolean variable to modify.
             * @param value The new value of the variable.
             */
            static void setValue(StateType* state, uint_fast64_t index, bool value);
            
            /*!
             * Set some integer variable in the given state object.
             *
             * @param state The state to modify.
             * @param index index of the integer variable to modify.
             * @param value The new value of the variable.
             */
            static void setValue(StateType* state, uint_fast64_t index, int_fast64_t value);
            
            /*!
             * Transforms a state into a somewhat readable string.
             *
             * @param state The state to transform into a string.
             * @return A string representation of the state.
             */
            static std::string toString(StateType const* state);
            
            /*!
             * Applies an update to the given state and returns the resulting new state object. This methods does not
             * modify the given state but returns a new one.
             *
             * @params state The state to which to apply the update.
             * @params update The update to apply.
             * @return The resulting state.
             */
            StateType* applyUpdate(StateType const* state, storm::ir::Update const& update) const;
            
            /*!
             * Applies an update to the given state and returns the resulting new state object. The update is evaluated
             * over the variable values of the given base state. This methods does not modify the given state but
             * returns a new one.
             *
             * @params state The state to which to apply the update.
             * @params baseState The state used for evaluating the update.
             * @params update The update to apply.
             * @return The resulting state.
             */
            StateType* applyUpdate(StateType const* state, StateType const* baseState, storm::ir::Update const& update) const;
            
            /*!
             * Prepares internal data structures associated with the variables in the program that are used during the
             * translation.
             */
            void initializeVariables();
            
            /*!
             * Retrieves the state rewards for every reachable state based on the given state rewards.
             *
             * @param rewards The rewards to use.
             * @return The reward values for every (reachable) state.
             */
            std::vector<double> getStateRewards(std::vector<storm::ir::StateReward> const& rewards);
            
            /*!
             * Computes the labels for every reachable state based on a list of available labels.
             *
             * @param labels A mapping from label names to boolean expressions to use for the labeling.
             * @return The resulting labeling.
             */
            storm::models::AtomicPropositionsLabeling getStateLabeling(std::map<std::string, std::shared_ptr<storm::ir::expressions::BaseExpression>> labels);
            
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
             * @param state The current state.
             * @param action The action label to select.
             * @return A list of lists of active commands or nothing.
             */
            boost::optional<std::vector<std::list<storm::ir::Command>>> getActiveCommandsByAction(StateType const* state, std::string const& action);
            
            /*!
             * Generates the initial state.
             *
             * @return The initial state.
             */
            StateType* getInitialState();
            
            /*!
             * Retrieves the state id of the given state. If the state has not been encountered yet, it will be added to
             * the lists of all states with a new id. If the state was already known, the object that is pointed to by
             * the given state pointer is deleted and the old state id is returned. Note that the pointer should not be
             * used after invoking this method.
             *
             * @param state A pointer to a state for which to retrieve the index. This must not be used after the call.
             * @return The state id of the given state.
             */
            uint_fast64_t getOrAddStateIndex(StateType* state);
            
            /*!
             * Expands all unlabeled (i.e. independent) transitions of the given state and adds them to the transition list.
             *
             * @param stateIndex The state to expand.
             * @params transitionList The current list of transitions for this state.
             */
            void addUnlabeledTransitions(uint_fast64_t stateIndex, std::list<std::pair<std::pair<std::string, std::list<uint_fast64_t>>, std::map<uint_fast64_t, double>>>& transitionList);
            
            /*!
             * Expands all labeled (i.e. synchronizing) transitions of the given state and adds them to the transition list.
             *
             * @param stateIndex The index of the state to expand.
             * @param transitionList The current list of transitions for this state.
             */
            void addLabeledTransitions(uint_fast64_t stateIndex, std::list<std::pair<std::pair<std::string, std::list<uint_fast64_t>>, std::map<uint_fast64_t, double>>>& transitionList);
            
            /*!
             * Builds the transition matrix of a deterministic model from the current list of transitions.
             *
             * @return The transition matrix.
             */
            storm::storage::SparseMatrix<double> buildDeterministicMatrix();
            
            /*!
             * Builds the transition matrix of a nondeterministic model from the current list of transitions.
             *
             * @return result The transition matrix.
             */
            storm::storage::SparseMatrix<double> buildNondeterministicMatrix();
            
            /*!
             * Generate the (internal) list of all transitions of the model.
             */
            void buildTransitionMap();
            
            /*!
             * Clear all members that are initialized during the computation.
             */
            void clearInternalState();
            
            // Program that is to be converted.
            storm::ir::Program program;
            
            // List of all boolean variables.
            std::vector<storm::ir::BooleanVariable> booleanVariables;
            
            // List of all integer variables.
            std::vector<storm::ir::IntegerVariable> integerVariables;
            
            // A mapping of boolean variable names to their indices.
            std::map<std::string, uint_fast64_t> booleanVariableToIndexMap;
            
            // A mapping of integer variable names to their indices.
            std::map<std::string, uint_fast64_t> integerVariableToIndexMap;
            
            //// Members that are filled during the conversion.
            // The selected reward model.
            std::unique_ptr<storm::ir::RewardModel> rewardModel;
            
            // A list of all reachable states.
            std::vector<StateType*> allStates;
            
            // A mapping of states to their indices (within the list of all states).
            std::unordered_map<StateType*, uint_fast64_t, StateHash, StateCompare> stateToIndexMap;
            
            // The number of transitions.
            uint_fast64_t numberOfTransitions;
            
            // The number of choices in a nondeterminstic model. (This corresponds to the number of rows of the matrix
            // used to represent the nondeterministic model.)
            uint_fast64_t numberOfChoices;
            
            // The number of choices for each state of a nondeterministic model.
            std::vector<uint_fast64_t> choiceIndices;
            
            // The result of the translation of transition rewards to a sparse matrix (if any).
            boost::optional<storm::storage::SparseMatrix<double>> transitionRewards;
            
            // A labeling for the choices of each state.
            std::vector<std::list<uint_fast64_t>> choiceLabeling;
            
            /*!
             * Maps a source state to a list of probability distributions over target states. Each distribution
             * corresponds to an unlabeled command or a feasible combination of labeled commands. Therefore, each
             * distribution is represented by a structure that contains the label of the participating commands, a list
             * of labels associated with that particular command combination and a mapping from target states to their
             * probabilities.
             */
            std::map<uint_fast64_t, std::list<std::pair<std::pair<std::string, std::list<uint_fast64_t>>, std::map<uint_fast64_t, double>>>> transitionMap;
        };
        
    } // namespace adapters
} // namespace storm

#endif	/* STORM_ADAPTERS_EXPLICITMODELADAPTER_H */
