/* 
 * File:   ExplicitModelAdapter.h
 * Author: Gereon Kremer
 *
 * Created on March 15, 2013, 11:42 AM
 */

#ifndef EXPLICITMODELADAPTER_H
#define	EXPLICITMODELADAPTER_H

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
 * Model state, represented by the values of all variables.
 */
typedef std::pair<std::vector<bool>, std::vector<int_fast64_t>> StateType;

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

class StateCompare {
public:
	bool operator()(StateType* state1, StateType* state2) const {
		return *state1 == *state2;
	}
};

class ExplicitModelAdapter {
public:
	/*!
	 * Initialize adapter with given program.
	 */
	ExplicitModelAdapter(storm::ir::Program program);
	~ExplicitModelAdapter();

	/*!
	 * Convert program to an AbstractModel.
	 * The model will be of the type specified in the program.
	 * The model will contain rewards that are specified by the given reward model.
	 * @param rewardModelName Name of reward model to be added to the model.
	 * @return Model resulting from the program.
	 */
	std::shared_ptr<storm::models::AbstractModel<double>> getModel(std::string const & rewardModelName = "");

private:
	// Copying/Moving is disabled for this class
	ExplicitModelAdapter(ExplicitModelAdapter const& other) {}
	ExplicitModelAdapter(ExplicitModelAdapter && other) {}

	double precision;

	/*!
	 * Set some boolean variable in the given state object.
	 * @param state State to be changed.
	 * @param index Index of boolean variable.
	 * @param value New value.
	 */
	static void setValue(StateType* const state, uint_fast64_t const index, bool const value);
	/*!
	 * Set some integer variable in the given state object.
	 * @param state State to be changed.
	 * @param index Index of integer variable.
	 * @param value New value.
	 */
	static void setValue(StateType* const state, uint_fast64_t const index, int_fast64_t const value);
	/*!
	 * Transforms a state into a somewhat readable string.
	 * @param state State.
	 * @return String representation of the state.
	 */
	static std::string toString(StateType const * const state);
	/*!
	 * Apply an update to the given state and return the resulting new state object.
	 * This methods creates a copy of the given state.
	 * @params state Current state.
	 * @params update Update to be applied.
	 * @return Resulting state.
	 */
	StateType* applyUpdate(StateType const * const state, storm::ir::Update const& update) const;
	/*!
	 * Apply an update to a given state and return the resulting new state object.
	 * Updates are done using the variable values from a given baseState.
	 * @params state State to be updated.
	 * @params baseState State used for update variables.
	 * @params update Update to be applied.
	 * @return Resulting state.
	 */
	StateType* applyUpdate(StateType const * const state, StateType const * const baseState, storm::ir::Update const& update) const;

	/*!
	 * Reads and combines variables from all program modules and stores them.
	 * Also creates a map to obtain a variable index from a variable map efficiently.
	 */
	void initializeVariables();

	/*!
	 * Calculate state reward for every reachable state based on given reward models.
	 * @param rewards List of state reward models.
	 * @return Reward for every state.
	 */
	std::vector<double> getStateRewards(std::vector<storm::ir::StateReward> const & rewards);
	
	/*!
	 * Determines the labels for every reachable state, based on a list of available labels.
	 * @param labels Mapping from label names to boolean expressions.
	 * @returns The resulting labeling.
	 */
	storm::models::AtomicPropositionsLabeling getStateLabeling(std::map<std::string, std::shared_ptr<storm::ir::expressions::BaseExpression>> labels);

	/*!
	 * Retrieves all active command labeled by some label, ordered by modules.
	 *
	 * This function will iterate over all modules and retrieve all commands that are labeled with the given action and active for the current state.
	 * The result will be a list of lists of commands.
	 *
	 * For each module that has appropriately labeled commands, there will be a list.
	 * If none of these commands is active, this list is empty.
	 * Note the difference between *no list* and *empty list*: Modules that produce no list are not relevant for this action while an empty list means, that it is not possible to do anything with this label.
	 * @param state Current state.
	 * @param action Action label.
	 * @return Active commands.
	 */
	std::unique_ptr<std::list<std::list<storm::ir::Command>>> getActiveCommandsByAction(StateType const * state, std::string& action);

	/*!
	 * Generates all initial states and adds them to allStates.
	 */
	void generateInitialStates();
	
	/*!
	 * Retrieves the state id of the given state.
	 * If the state has not been hit yet, it will be added to allStates and given a new id.
	 * In this case, the pointer must not be deleted, as it is used within allStates.
	 * If the state is already known, the pointer is deleted and the old state id is returned.
	 * Hence, the given state pointer should not be used afterwards.
	 * @param state Pointer to state, shall not be used afterwards.
	 * @returns State id of given state.
	 */
	uint_fast64_t getOrAddStateId(StateType * state);

	/*!
	 * Expands all unlabeled transitions for a given state and adds them to the given list of results.
	 * @params state State to be explored.
	 * @params res Intermediate transition map.
	 */
	void addUnlabeledTransitions(const uint_fast64_t stateID, std::list<std::pair<std::string, std::map<uint_fast64_t, double>>>& res);
	
	/*!
	 * Explores reachable state from given state by using labeled transitions.
	 * Found transitions are stored in given map.
	 * @param stateID State to be explored.
	 * @param res Intermediate transition map.
	 */
	void addLabeledTransitions(const uint_fast64_t stateID, std::list<std::pair<std::string, std::map<uint_fast64_t, double>>>& res);

	/*!
	 * Create matrix from intermediate mapping, assuming it is a dtmc model.
	 * @param intermediate Intermediate representation of transition mapping.
	 * @return result matrix.
	 */
	storm::storage::SparseMatrix<double> buildDeterministicMatrix();

	/*!
	 * Create matrix from intermediate mapping, assuming it is a mdp model.
	 * @param intermediate Intermediate representation of transition mapping.
	 * @param choices Overall number of choices for all nodes.
	 * @return result matrix.
	 */
	storm::storage::SparseMatrix<double> buildNondeterministicMatrix();

	/*!
	 * Generate internal transition map from given model.
	 * Starts with all initial states and explores the reachable state space.
	 */
	void buildTransitionMap();
	
	/*!
	 * Clear all members that are initialized during the computation.
	 */
	void clearInternalState();

	// Program that should be converted.
	storm::ir::Program program;
	// List of all boolean variables.
	std::vector<storm::ir::BooleanVariable> booleanVariables;
	// List of all integer variables.
	std::vector<storm::ir::IntegerVariable> integerVariables;
	// Maps boolean variable names to their index.
	std::map<std::string, uint_fast64_t> booleanVariableToIndexMap;
	// Maps integer variable names to their index.
	std::map<std::string, uint_fast64_t> integerVariableToIndexMap;

	//// Members that are filled during the conversion.
	// Selected reward model.
	std::unique_ptr<storm::ir::RewardModel> rewardModel;
	// List of all reachable states.
	std::vector<StateType*> allStates;
	// Maps states to their index (within allStates).
	std::unordered_map<StateType*, uint_fast64_t, StateHash, StateCompare> stateToIndexMap;
	// Number of transitions.
	uint_fast64_t numberOfTransitions;
	// Number of choices. (Is number of rows in matrix of nondeterministic model.)
	uint_fast64_t numberOfChoices;
	// Number of choices for each state.
	std::vector<uint_fast64_t> choiceIndices;
	// Rewards for transitions.
	boost::optional<storm::storage::SparseMatrix<double>> transitionRewards;

	/*!
	 * Maps a source node to a list of probability distributions over target nodes.
	 * Each such distribution corresponds to an unlabeled command or a feasible combination of labeled commands.
	 * Therefore, each distribution is represented by a label and a mapping from target nodes to their probabilities.
	 */
	std::map<uint_fast64_t, std::list<std::pair<std::string, std::map<uint_fast64_t, double>>>> transitionMap;
};

} // namespace adapters
} // namespace storm

#endif	/* EXPLICITMODELADAPTER_H */
