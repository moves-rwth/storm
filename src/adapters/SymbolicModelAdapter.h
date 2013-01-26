/*
 * SymbolicModelAdapter.h
 *
 *  Created on: 25.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_ADAPTERS_SYMBOLICMODELADAPTER_H_
#define STORM_ADAPTERS_SYMBOLICMODELADAPTER_H_

#include "src/exceptions/WrongFileFormatException.h"

#include "src/utility/CuddUtility.h"
#include "src/ir/expressions/ExpressionVisitor.h"

#include "cuddObj.hh"
#include <iostream>

namespace storm {

namespace adapters {

class SymbolicModelAdapter {
public:

	SymbolicModelAdapter() : cuddUtility(storm::utility::cuddUtilityInstance()) {

	}

	void toMTBDD(storm::ir::Program const& program) {
		LOG4CPLUS_INFO(logger, "Creating MTBDD representation for probabilistic program.");
		createDecisionDiagramVariables(program);

		for (uint_fast64_t i = 0; i < program.getNumberOfModules(); ++i) {
			storm::ir::Module const& module = program.getModule(i);

			for (uint_fast64_t j = 0; j < module.getNumberOfCommands(); ++j) {
				storm::ir::Command const& command = module.getCommand(j);


			}
		}

		LOG4CPLUS_INFO(logger, "Done creating MTBDD representation for probabilistic program.");
	}

private:
	storm::utility::CuddUtility* cuddUtility;

	std::vector<ADD*> allDecisionDiagramVariables;
	std::vector<ADD*> allRowDecisionDiagramVariables;
	std::vector<ADD*> allColumnDecisionDiagramVariables;
	std::vector<ADD*> booleanRowDecisionDiagramVariables;
	std::vector<ADD*> integerRowDecisionDiagramVariables;
	std::vector<ADD*> booleanColumnDecisionDiagramVariables;
	std::vector<ADD*> integerColumnDecisionDiagramVariables;
	std::unordered_map<std::string, std::vector<ADD*>> variableToRowDecisionDiagramVariableMap;
	std::unordered_map<std::string, std::vector<ADD*>> variableToColumnDecisionDiagramVariableMap;

	void createDecisionDiagramVariables(storm::ir::Program const& program) {
		for (uint_fast64_t i = 0; i < program.getNumberOfModules(); ++i) {
			storm::ir::Module const& module = program.getModule(i);

			for (uint_fast64_t j = 0; j < module.getNumberOfBooleanVariables(); ++j) {
				storm::ir::BooleanVariable const& booleanVariable = module.getBooleanVariable(j);

				ADD* newRowDecisionDiagramVariable = cuddUtility->getNewAddVariable();
				variableToRowDecisionDiagramVariableMap[booleanVariable.getName()].push_back(newRowDecisionDiagramVariable);
				booleanRowDecisionDiagramVariables.push_back(newRowDecisionDiagramVariable);
				allRowDecisionDiagramVariables.push_back(newRowDecisionDiagramVariable);
				allDecisionDiagramVariables.push_back(newRowDecisionDiagramVariable);

				ADD* newColumnDecisionDiagramVariable = cuddUtility->getNewAddVariable();
				variableToColumnDecisionDiagramVariableMap[booleanVariable.getName()].push_back(newColumnDecisionDiagramVariable);
				booleanColumnDecisionDiagramVariables.push_back(newColumnDecisionDiagramVariable);
				allColumnDecisionDiagramVariables.push_back(newColumnDecisionDiagramVariable);
				allDecisionDiagramVariables.push_back(newColumnDecisionDiagramVariable);
			}

			for (uint_fast64_t j = 0; j < module.getNumberOfIntegerVariables(); ++j) {
				storm::ir::IntegerVariable const& integerVariable = module.getIntegerVariable(j);
				uint_fast64_t integerRange = integerVariable.getUpperBound()->getValueAsInt(nullptr) - integerVariable.getLowerBound()->getValueAsInt(nullptr);
				if (integerRange <= 0) {
					throw storm::exceptions::WrongFileFormatException() << "Range of variable "
							<< integerVariable.getName() << " is empty or negativ.";
				}
				uint_fast64_t numberOfDecisionDiagramVariables = static_cast<uint_fast64_t>(std::ceil(std::log2(integerRange)));

				std::vector<ADD*> allRowDecisionDiagramVariablesForVariable;
				std::vector<ADD*> allColumnDecisionDiagramVariablesForVariable;
				for (uint_fast64_t k = 0; k < numberOfDecisionDiagramVariables; ++k) {
					ADD* newRowDecisionDiagramVariable = cuddUtility->getNewAddVariable();
					allRowDecisionDiagramVariablesForVariable.push_back(newRowDecisionDiagramVariable);
					integerRowDecisionDiagramVariables.push_back(newRowDecisionDiagramVariable);
					allRowDecisionDiagramVariables.push_back(newRowDecisionDiagramVariable);
					allDecisionDiagramVariables.push_back(newRowDecisionDiagramVariable);

					ADD* newColumnDecisionDiagramVariable = cuddUtility->getNewAddVariable();
					allColumnDecisionDiagramVariablesForVariable.push_back(newColumnDecisionDiagramVariable);
					integerColumnDecisionDiagramVariables.push_back(newColumnDecisionDiagramVariable);
					allColumnDecisionDiagramVariables.push_back(newColumnDecisionDiagramVariable);
					allDecisionDiagramVariables.push_back(newColumnDecisionDiagramVariable);
				}
				variableToRowDecisionDiagramVariableMap[integerVariable.getName()] = allRowDecisionDiagramVariablesForVariable;
				variableToColumnDecisionDiagramVariableMap[integerVariable.getName()] = allColumnDecisionDiagramVariablesForVariable;
			}
		}
	}
};

} // namespace adapters

} // namespace storm

#endif /* STORM_ADAPTERS_SYMBOLICMODELADAPTER_H_ */
