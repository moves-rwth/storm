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
	Program(ModelType modelType, std::map<std::string, std::shared_ptr<storm::ir::expressions::BooleanConstantExpression>> booleanUndefinedConstantExpressions, std::map<std::string, std::shared_ptr<storm::ir::expressions::IntegerConstantExpression>> integerUndefinedConstantExpressions, std::map<std::string, std::shared_ptr<storm::ir::expressions::DoubleConstantExpression>> doubleUndefinedConstantExpressions, std::vector<storm::ir::Module> modules, std::map<std::string, storm::ir::RewardModel> rewards, std::map<std::string, std::shared_ptr<storm::ir::expressions::BaseExpression>> labels);

	/*!
	 * Retrieves a string representation of this program.
	 * @returns a string representation of this program.
	 */
	std::string toString() const;

private:
	// The type of the model.
	ModelType modelType;

	// A map of undefined boolean constants to their expression nodes.
	std::map<std::string, std::shared_ptr<storm::ir::expressions::BooleanConstantExpression>> booleanUndefinedConstantExpressions;

	// A map of undefined integer constants to their expressions nodes.
	std::map<std::string, std::shared_ptr<storm::ir::expressions::IntegerConstantExpression>> integerUndefinedConstantExpressions;

	// A mpa of undefined double constants to their expressions nodes.
	std::map<std::string, std::shared_ptr<storm::ir::expressions::DoubleConstantExpression>> doubleUndefinedConstantExpressions;

	// The modules associated with the program.
	std::vector<storm::ir::Module> modules;

	// The reward models associated with the program.
	std::map<std::string, storm::ir::RewardModel> rewards;

	// The labels that are defined for this model.
	std::map<std::string, std::shared_ptr<storm::ir::expressions::BaseExpression>> labels;
};

} // namespace ir

} // namespace storm

#endif /* STORM_IR_PROGRAM_H_ */
