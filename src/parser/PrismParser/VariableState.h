/* 
 * File:   VariableState.h
 * Author: nafur
 *
 * Created on April 10, 2013, 4:43 PM
 */

#ifndef VARIABLESTATE_H
#define	VARIABLESTATE_H

#include "src/ir/IR.h"
#include "Includes.h"
#include "Tokens.h"

namespace storm {
namespace parser {
namespace prism {

using namespace storm::ir;
using namespace storm::ir::expressions;


struct VariableState : public storm::ir::VariableAdder {

public:
	VariableState(bool firstRun = true)
			: firstRun(firstRun), keywords(), nextBooleanVariableIndex(0), nextIntegerVariableIndex(0) {
	}

	bool firstRun;
	keywordsStruct keywords;

	// Used for indexing the variables.
	uint_fast64_t nextBooleanVariableIndex;
	uint_fast64_t nextIntegerVariableIndex;

	// Structures mapping variable and constant names to the corresponding expression nodes of
	// the intermediate representation.
	struct qi::symbols<char, std::shared_ptr<VariableExpression>> integerVariables_, booleanVariables_;
	struct qi::symbols<char, std::shared_ptr<BaseExpression>> integerConstants_, booleanConstants_, doubleConstants_;
	struct qi::symbols<char, std::shared_ptr<Module>> moduleMap_;

	// A structure representing the identity function over identifier names.
	struct variableNamesStruct : qi::symbols<char, std::string> { } integerVariableNames_, booleanVariableNames_, commandNames_, labelNames_, allConstantNames_, moduleNames_,
			localBooleanVariables_, localIntegerVariables_, assignedLocalBooleanVariables_, assignedLocalIntegerVariables_;

	uint_fast64_t addBooleanVariable(const std::string& name, const std::shared_ptr<storm::ir::expressions::BaseExpression> init) {
		//std::cerr << "adding boolean variable " << name << std::endl;
		if (firstRun) {
			std::shared_ptr<VariableExpression> varExpr = std::shared_ptr<VariableExpression>(new VariableExpression(storm::ir::expressions::BaseExpression::bool_, this->nextBooleanVariableIndex, name));
			this->booleanVariables_.add(name, varExpr);
			this->booleanVariableNames_.add(name, name);
			this->nextBooleanVariableIndex++;
			return this->nextBooleanVariableIndex-1;
		} else {
			std::shared_ptr<VariableExpression> res = this->booleanVariables_.at(name);
			if (res != nullptr) {
				return res->getVariableIndex();
			} else {
				std::cerr << "Variable " << name << " was not created in first run" << std::endl;
				return 0;
			}
		}
	}

	uint_fast64_t addIntegerVariable(const std::string& name, const std::shared_ptr<storm::ir::expressions::BaseExpression> lower, const std::shared_ptr<storm::ir::expressions::BaseExpression> upper, const std::shared_ptr<storm::ir::expressions::BaseExpression> init) {
		//std::cerr << "adding integer variable " << name << std::endl;
		if (firstRun) {
			std::shared_ptr<VariableExpression> varExpr = std::shared_ptr<VariableExpression>(new VariableExpression(storm::ir::expressions::BaseExpression::int_, this->nextIntegerVariableIndex, name, lower, upper));
			this->integerVariables_.add(name, varExpr);
			this->integerVariableNames_.add(name, name);
			this->nextIntegerVariableIndex++;
			return this->nextIntegerVariableIndex-1;
		} else {
			std::shared_ptr<VariableExpression> res = this->integerVariables_.at(name);
			if (res != nullptr) {
				return res->getVariableIndex();
			} else {
				std::cerr << "Variable " << name << " was not created in first run" << std::endl;
				return 0;
			}
		}
	}

	std::shared_ptr<VariableExpression> getBooleanVariable(const std::string& name) {
		//std::cerr << "getting boolen variable " << name << std::endl;
		std::shared_ptr<VariableExpression> res = this->booleanVariables_.at(name);
		if (res != nullptr) {
			return res;
		} else {
			if (firstRun) {
				return std::shared_ptr<VariableExpression>(new VariableExpression(BaseExpression::bool_, std::numeric_limits<uint_fast64_t>::max(), "bool", std::shared_ptr<BaseExpression>(nullptr), std::shared_ptr<BaseExpression>(nullptr)));
			} else {
				std::cerr << "Variable " << name << " was not created in first run" << std::endl;
				return std::shared_ptr<VariableExpression>(nullptr);
			}
		}
	}

	std::shared_ptr<VariableExpression> getIntegerVariable(const std::string& name) {
		//std::cerr << "getting integer variable " << name << std::endl;
		std::shared_ptr<VariableExpression> res = this->integerVariables_.at(name);
		if (res != nullptr) {
			return res;
		} else {
			if (firstRun) {
				return std::shared_ptr<VariableExpression>(new VariableExpression(BaseExpression::int_, std::numeric_limits<uint_fast64_t>::max(), "int", std::shared_ptr<BaseExpression>(nullptr), std::shared_ptr<BaseExpression>(nullptr)));
			} else {
				std::cerr << "Variable " << name << " was not created in first run" << std::endl;
				return std::shared_ptr<VariableExpression>(nullptr);
			}
		}
	}

	void startModule() {
		//std::cerr << "starting new module" << std::endl;
		this->localBooleanVariables_.clear();
		this->localIntegerVariables_.clear();
	}

	bool isFreeIdentifier(std::string& s) const {
		//std::cerr << "Checking if " << s << " is free" << std::endl;
		if (this->integerVariableNames_.find(s) != nullptr) return false;
		if (this->allConstantNames_.find(s) != nullptr) return false;
		if (this->labelNames_.find(s) != nullptr) return false;
		if (this->moduleNames_.find(s) != nullptr) return false;
		if (this->keywords.find(s) != nullptr) return false;
		return true;
	}
	bool isIdentifier(std::string& s) const {
		//std::cerr << "Checking if " << s << " is identifier" << std::endl;
		if (this->allConstantNames_.find(s) != nullptr) return false;
		if (this->keywords.find(s) != nullptr) return false;
		return true;
	}

	void prepareForSecondRun() {
		std::cerr << "starting second run" << std::endl;
		integerConstants_.clear();
		booleanConstants_.clear();
		doubleConstants_.clear();
		allConstantNames_.clear();
		this->firstRun = false;
	}
};

}
}
}

#endif	/* VARIABLESTATE_H */

