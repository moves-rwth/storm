/*
 * SymbolicModelAdapter.h
 *
 *  Created on: 25.01.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_ADAPTERS_SYMBOLICMODELADAPTER_H_
#define STORM_ADAPTERS_SYMBOLICMODELADAPTER_H_

#include "src/exceptions/WrongFormatException.h"

#include "src/utility/CuddUtility.h"
#include "SymbolicExpressionAdapter.h"

#include "cuddObj.hh"
#include <iostream>
#include <unordered_map>

namespace storm {

namespace adapters {

class SymbolicModelAdapter {
public:

	SymbolicModelAdapter() : cuddUtility(storm::utility::cuddUtilityInstance()), allDecisionDiagramVariables(),
		allRowDecisionDiagramVariables(), allColumnDecisionDiagramVariables(), booleanRowDecisionDiagramVariables(),
		integerRowDecisionDiagramVariables(), booleanColumnDecisionDiagramVariables(), integerColumnDecisionDiagramVariables(),
		variableToRowDecisionDiagramVariableMap(), variableToColumnDecisionDiagramVariableMap(),
		variableToIdentityDecisionDiagramMap(),
		rowExpressionAdapter(variableToRowDecisionDiagramVariableMap), columnExpressionAdapter(variableToColumnDecisionDiagramVariableMap) {

	}

	void toMTBDD(storm::ir::Program const& program) {
		LOG4CPLUS_INFO(logger, "Creating MTBDD representation for probabilistic program.");
		createDecisionDiagramVariables(program);
		createIdentityDecisionDiagrams(program);

		ADD* systemAdd = cuddUtility->getZero();
		for (uint_fast64_t i = 0; i < program.getNumberOfModules(); ++i) {
			storm::ir::Module const& module = program.getModule(i);

			ADD* moduleAdd = cuddUtility->getZero();
			for (uint_fast64_t j = 0; j < module.getNumberOfCommands(); ++j) {
				storm::ir::Command const& command = module.getCommand(j);

				ADD* commandAdd = cuddUtility->getZero();

				ADD* guard = rowExpressionAdapter.translateExpression(command.getGuard());
				if (*guard != *cuddUtility->getZero()) {
					for (uint_fast64_t i = 0; i < command.getNumberOfUpdates(); ++i) {
						ADD* updateAdd = cuddUtility->getOne();

						storm::ir::Update const& update = command.getUpdate(i);

						std::map<std::string, storm::ir::Assignment> booleanAssignments = update.getBooleanAssignments();
						for (auto assignmentPair : booleanAssignments) {
							ADD* updateExpr = rowExpressionAdapter.translateExpression(assignmentPair.second.getExpression());

							ADD* temporary = cuddUtility->getZero();
							cuddUtility->setValueAtIndex(temporary, 0, variableToColumnDecisionDiagramVariableMap[assignmentPair.first], 0);
							cuddUtility->setValueAtIndex(temporary, 1, variableToColumnDecisionDiagramVariableMap[assignmentPair.first], 1);

							ADD* result = new ADD(*updateExpr * *guard);
							result = new ADD(result->Equals(*temporary));

							*updateAdd = *updateAdd * *result;
						}

						std::map<std::string, storm::ir::Assignment> integerAssignments = update.getIntegerAssignments();
						for (auto assignmentPair : integerAssignments) {
							ADD* updateExpr = rowExpressionAdapter.translateExpression(assignmentPair.second.getExpression());

							ADD* temporary = cuddUtility->getZero();

							uint_fast64_t variableIndex = module.getIntegerVariableIndex(assignmentPair.first);
							storm::ir::IntegerVariable integerVariable = module.getIntegerVariable(variableIndex);
							int_fast64_t low = integerVariable.getLowerBound()->getValueAsInt(nullptr);
							int_fast64_t high = integerVariable.getUpperBound()->getValueAsInt(nullptr);

							for (uint_fast64_t i = low; i <= high; ++i) {
								cuddUtility->setValueAtIndex(temporary, i - low, variableToColumnDecisionDiagramVariableMap[assignmentPair.first], i);
							}

							ADD* result = new ADD(*updateExpr * *guard);
							result = new ADD(result->Equals(*temporary));
							*result *= *guard;

							*updateAdd = *updateAdd * *result;
						}
						for (uint_fast64_t i = 0; i < module.getNumberOfBooleanVariables(); ++i) {
							storm::ir::BooleanVariable const& booleanVariable = module.getBooleanVariable(i);

							if (update.getBooleanAssignments().find(booleanVariable.getName()) == update.getBooleanAssignments().end()) {
								*updateAdd = *updateAdd * *variableToIdentityDecisionDiagramMap[booleanVariable.getName()];
							}
						}
						for (uint_fast64_t i = 0; i < module.getNumberOfIntegerVariables(); ++i) {
							storm::ir::IntegerVariable const& integerVariable = module.getIntegerVariable(i);

							if (update.getIntegerAssignments().find(integerVariable.getName()) == update.getIntegerAssignments().end()) {
								*updateAdd = *updateAdd * *variableToIdentityDecisionDiagramMap[integerVariable.getName()];
							}
						}

						*commandAdd += *updateAdd * *cuddUtility->getConstant(update.getLikelihoodExpression()->getValueAsDouble(nullptr));
					}
					*moduleAdd += *commandAdd;
				} else {
					LOG4CPLUS_WARN(logger, "Guard " << command.getGuard()->toString() << " is unsatisfiable.");
				}
			}
			*systemAdd += *moduleAdd;
		}

		performReachability(program, systemAdd);

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

	std::unordered_map<std::string, ADD*> variableToIdentityDecisionDiagramMap;

	SymbolicExpressionAdapter rowExpressionAdapter;
	SymbolicExpressionAdapter columnExpressionAdapter;

	ADD* getInitialStateDecisionDiagram(storm::ir::Program const& program) {
		ADD* initialStates = cuddUtility->getOne();
		for (uint_fast64_t i = 0; i < program.getNumberOfModules(); ++i) {
			storm::ir::Module const& module = program.getModule(i);

			for (uint_fast64_t j = 0; j < module.getNumberOfBooleanVariables(); ++j) {
				storm::ir::BooleanVariable const& booleanVariable = module.getBooleanVariable(j);
				bool initialValue = booleanVariable.getInitialValue()->getValueAsBool(nullptr);
				*initialStates *= *cuddUtility->getConstantEncoding(1, variableToRowDecisionDiagramVariableMap[booleanVariable.getName()]);
			}
			for (uint_fast64_t j = 0; j < module.getNumberOfIntegerVariables(); ++j) {
				storm::ir::IntegerVariable const& integerVariable = module.getIntegerVariable(j);
				int_fast64_t initialValue = integerVariable.getInitialValue()->getValueAsInt(nullptr);
				int_fast64_t low = integerVariable.getLowerBound()->getValueAsInt(nullptr);
				*initialStates *= *cuddUtility->getConstantEncoding(initialValue - low, variableToRowDecisionDiagramVariableMap[integerVariable.getName()]);
			}
		}

		cuddUtility->dumpDotToFile(initialStates, "initstates.add");
		return initialStates;
	}

	void performReachability(storm::ir::Program const& program, ADD* systemAdd) {
		ADD* systemAdd01 = new ADD(systemAdd->GreaterThan(*cuddUtility->getZero()));
		cuddUtility->dumpDotToFile(systemAdd01, "system01.add");

		cuddUtility->dumpDotToFile(systemAdd, "reachtransold.add");
		ADD* reachableStates = getInitialStateDecisionDiagram(program);
		cuddUtility->dumpDotToFile(reachableStates, "init.add");
		ADD* newReachableStates = new ADD(*reachableStates);

		ADD* rowCube = cuddUtility->getOne();
		for (auto variablePtr : allRowDecisionDiagramVariables) {
			*rowCube *= *variablePtr;
		}

		bool changed;
		int iter = 0;
		do {
			changed = false;
			*newReachableStates = *reachableStates * *systemAdd01;
			newReachableStates = new ADD(newReachableStates->ExistAbstract(*rowCube));

			cuddUtility->dumpDotToFile(newReachableStates, "reach1.add");

			newReachableStates = cuddUtility->permuteVariables(newReachableStates, allColumnDecisionDiagramVariables, allRowDecisionDiagramVariables, allDecisionDiagramVariables.size());

			*newReachableStates += *reachableStates;
			newReachableStates = new ADD(newReachableStates->GreaterThan(*cuddUtility->getZero()));

			if (*newReachableStates != *reachableStates) changed = true;
			*reachableStates = *newReachableStates;
		} while (changed);

		*systemAdd *= *reachableStates;
		std::cout << "got " << systemAdd->nodeCount() << " nodes" << std::endl;
		std::cout << "and " << systemAdd->CountMinterm(allRowDecisionDiagramVariables.size() + allColumnDecisionDiagramVariables.size()) << std::endl;
	}

	void createIdentityDecisionDiagrams(storm::ir::Program const& program) {
		for (uint_fast64_t i = 0; i < program.getNumberOfModules(); ++i) {
			storm::ir::Module const& module = program.getModule(i);

			for (uint_fast64_t j = 0; j < module.getNumberOfBooleanVariables(); ++j) {
				storm::ir::BooleanVariable const& booleanVariable = module.getBooleanVariable(j);
				ADD* identity = cuddUtility->getZero();
				cuddUtility->setValueAtIndices(identity, 0, 0,
						variableToRowDecisionDiagramVariableMap[booleanVariable.getName()],
						variableToColumnDecisionDiagramVariableMap[booleanVariable.getName()], 1);
				cuddUtility->setValueAtIndices(identity, 1, 1,
						variableToRowDecisionDiagramVariableMap[booleanVariable.getName()],
						variableToColumnDecisionDiagramVariableMap[booleanVariable.getName()], 1);
				variableToIdentityDecisionDiagramMap[booleanVariable.getName()] = identity;
			}

			for (uint_fast64_t j = 0; j < module.getNumberOfIntegerVariables(); ++j) {
				storm::ir::IntegerVariable const& integerVariable = module.getIntegerVariable(j);

				ADD* identity = cuddUtility->getZero();

				int_fast64_t low = integerVariable.getLowerBound()->getValueAsInt(nullptr);
				int_fast64_t high = integerVariable.getUpperBound()->getValueAsInt(nullptr);

				for (uint_fast64_t i = low; i <= high; ++i) {
					cuddUtility->setValueAtIndices(identity, i - low, i - low,
							variableToRowDecisionDiagramVariableMap[integerVariable.getName()],
							variableToColumnDecisionDiagramVariableMap[integerVariable.getName()], 1);
				}
				variableToIdentityDecisionDiagramMap[integerVariable.getName()] = identity;
			}
		}
	}

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
					throw storm::exceptions::WrongFormatException() << "Range of variable "
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
