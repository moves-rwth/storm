/*
 * Program.h
 *
 *  Created on: 04.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_IR_PROGRAM_H_
#define STORM_IR_PROGRAM_H_

#include "expressions/BaseExpression.h"
#include "expressions/BooleanConstantExpression.h"
#include "expressions/IntegerConstantExpression.h"
#include "expressions/DoubleConstantExpression.h"
#include "Module.h"
#include "RewardModel.h"

#include <map>
#include <vector>
#include <memory>
#include <set>

namespace storm {

namespace ir {

/*!
 * A class representing a program.
 */
class Program {
public:

	/*!
	 * An enum for the different model types.
	 */
	enum ModelType {UNDEFINED, DTMC, CTMC, MDP, CTMDP};

	/*!
	 * Default constructor. Creates an empty program.
	 */
	Program();

	/*!
	 * Creates a program with the given model type, undefined constants, modules, rewards and labels.
	 * @param modelType the type of the model that this program gives rise to.
	 * @param booleanUndefinedConstantExpressions a map of undefined boolean constants to their
	 * expression nodes.
	 * @param integerUndefinedConstantExpressions a map of undefined integer constants to their
	 * expression nodes.
	 * @param doubleUndefinedConstantExpressions a map of undefined double constants to their
	 * expression nodes.
	 * @param modules The modules of the program.
	 * @param rewards The reward models of the program.
	 * @param labels The labels defined for this model.
	 */
	Program(
				ModelType modelType,
				std::map<std::string, std::shared_ptr<storm::ir::expressions::BooleanConstantExpression>> booleanUndefinedConstantExpressions,
				std::map<std::string, std::shared_ptr<storm::ir::expressions::IntegerConstantExpression>> integerUndefinedConstantExpressions,
				std::map<std::string, std::shared_ptr<storm::ir::expressions::DoubleConstantExpression>> doubleUndefinedConstantExpressions,
				std::vector<storm::ir::Module> modules,
				std::map<std::string, storm::ir::RewardModel> rewards,
				std::map<std::string, std::shared_ptr<storm::ir::expressions::BaseExpression>> labels);

	/*!
	 * Retrieves the number of modules in the program.
	 * @returns the number of modules in the program.
	 */
	uint_fast64_t getNumberOfModules() const;

	/*!
	 * Retrieves a reference to the module with the given index.
	 * @param index the index of the module to retrieve.
	 */
	storm::ir::Module const& getModule(uint_fast64_t index) const;

	/*!
	 * Retrieves the model type of the model.
	 * @returns the type of the model.
	 */
	ModelType getModelType() const;

	/*!
	 * Retrieves a string representation of this program.
	 * @returns a string representation of this program.
	 */
	std::string toString() const;
	
	/*!
	 * Retrieves the set of actions present in this module.
	 * @returns the set of actions present in this module.
	 */
	std::set<std::string> const& getActions() const;
	
	/*!
	 * Retrieved the indices of all Modules within this program that contain
	 * commands that are labelled with the given action.
	 * @param action Name of the action.
	 * @returns Indices of all matching modules.
	 */
	std::set<uint_fast64_t> const getModulesByAction(std::string const& action) const;

	/*!
	 * Retrieve reward model with given name.
	 * @param name Name of the reward model.
	 * @return Reward model with given name.
	 */
	storm::ir::RewardModel getRewardModel(std::string const & name) const;

	/*!
	 * Retrieves all labels.
	 * @return All labels.
	 */
	std::map<std::string, std::shared_ptr<storm::ir::expressions::BaseExpression>> getLabels() const;

private:
	// The type of the model.
	ModelType modelType;

	// A map of undefined boolean constants to their expression nodes.
	std::map<std::string, std::shared_ptr<storm::ir::expressions::BooleanConstantExpression>> booleanUndefinedConstantExpressions;

	// A map of undefined integer constants to their expressions nodes.
	std::map<std::string, std::shared_ptr<storm::ir::expressions::IntegerConstantExpression>> integerUndefinedConstantExpressions;

	// A map of undefined double constants to their expressions nodes.
	std::map<std::string, std::shared_ptr<storm::ir::expressions::DoubleConstantExpression>> doubleUndefinedConstantExpressions;

	// The modules associated with the program.
	std::vector<storm::ir::Module> modules;

	// The reward models associated with the program.
	std::map<std::string, storm::ir::RewardModel> rewards;

	// The labels that are defined for this model.
	std::map<std::string, std::shared_ptr<storm::ir::expressions::BaseExpression>> labels;
	
	// The set of actions present in this program.
	std::set<std::string> actions;
	
	// A map of actions to the set of modules containing commands labelled with this action.
	std::map<std::string, std::set<uint_fast64_t>> actionsToModuleIndexMap;
};

} // namespace ir

} // namespace storm

#endif /* STORM_IR_PROGRAM_H_ */
