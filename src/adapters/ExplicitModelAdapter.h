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
	ExplicitModelAdapter(std::shared_ptr<storm::ir::Program> program);
	~ExplicitModelAdapter();

	std::shared_ptr<storm::models::AbstractModel> getModel(std::string const & rewardModelName = "");

private:

	// First some generic routines to operate on states.

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
	 * Apply an update to the given state and return the resulting new state object.
	 * This methods creates a copy of the given state.
	 * @params state Current state.
	 * @params update Update to be applied.
	 * @return Resulting state.
	 */
	StateType* applyUpdate(StateType const * const state, storm::ir::Update const& update) const;

	/*!
	 * Reads and combines variables from all program modules and stores them.
	 * Also creates a map to obtain a variable index from a variable map efficiently.
	 */
	void initializeVariables();

	std::shared_ptr<std::vector<double>> getStateRewards(std::vector<std::shared_ptr<storm::ir::StateReward>> const & rewards);
	std::shared_ptr<storm::models::AtomicPropositionsLabeling> getStateLabeling(std::map<std::string, std::shared_ptr<storm::ir::expressions::BaseExpression>> labels);

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
	std::shared_ptr<storm::storage::SparseMatrix<double>> buildDeterministicMatrix();

	/*!
	 * Create matrix from intermediate mapping, assuming it is a mdp model.
	 * @param intermediate Intermediate representation of transition mapping.
	 * @param choices Overall number of choices for all nodes.
	 * @return result matrix.
	 */
	std::shared_ptr<storm::storage::SparseMatrix<double>> buildNondeterministicMatrix();

	/*!
	 * Build matrix from model. Starts with all initial states and explores the reachable state space.
	 * While exploring, the transitions are stored in a temporary map.
	 * Afterwards, we transform this map into the actual matrix.
	 * @return result matrix.
	 */
	void buildTransitionMap();
	
	void clearInternalState();

	// Program that should be converted.
	std::shared_ptr<storm::ir::Program> program;
	std::vector<storm::ir::BooleanVariable> booleanVariables;
	std::vector<storm::ir::IntegerVariable> integerVariables;
	std::map<std::string, uint_fast64_t> booleanVariableToIndexMap;
	std::map<std::string, uint_fast64_t> integerVariableToIndexMap;

	// Members that are filled during the conversion.
	std::shared_ptr<storm::ir::RewardModel> rewardModel;
	std::vector<StateType*> allStates;
	std::unordered_map<StateType*, uint_fast64_t, StateHash, StateCompare> stateToIndexMap;
	uint_fast64_t numberOfTransitions;
	uint_fast64_t numberOfChoices;
	std::shared_ptr<storm::storage::SparseMatrix<double>> transitionRewards;

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
