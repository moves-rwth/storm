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

namespace storm {
namespace parser {
namespace prism {

using namespace storm::ir;
using namespace storm::ir::expressions;

struct VariableState : public storm::ir::VariableAdder {

	public:
	VariableState()
			: keywords(), nextBooleanVariableIndex(0), nextIntegerVariableIndex(0)
	{
	}

	keywordsStruct keywords;

	// Used for indexing the variables.
	uint_fast64_t nextBooleanVariableIndex;
	uint_fast64_t nextIntegerVariableIndex;

	// Structures mapping variable and constant names to the corresponding expression nodes of
	// the intermediate representation.
	struct qi::symbols<char, std::shared_ptr<VariableExpression>> integerVariables_, booleanVariables_;
	struct qi::symbols<char, std::shared_ptr<BaseExpression>> integerConstants_, booleanConstants_, doubleConstants_;
	struct qi::symbols<char, Module> moduleMap_;

	// A structure representing the identity function over identifier names.
	struct variableNamesStruct : qi::symbols<char, std::string> { } integerVariableNames_, booleanVariableNames_, commandNames_, labelNames_, allConstantNames_, moduleNames_,
			localBooleanVariables_, localIntegerVariables_, assignedLocalBooleanVariables_, assignedLocalIntegerVariables_;

	uint_fast64_t addBooleanVariable(const std::string& name, const std::shared_ptr<storm::ir::expressions::BaseExpression> init) {
		std::shared_ptr<VariableExpression> res = this->booleanVariables_.at(name);
		if (res != nullptr) {
			return res->getVariableIndex();
		}
		std::shared_ptr<VariableExpression> varExpr = std::shared_ptr<VariableExpression>(new VariableExpression(storm::ir::expressions::BaseExpression::bool_, this->nextBooleanVariableIndex, name));
		this->booleanVariables_.add(name, varExpr);
		this->booleanVariableNames_.add(name, name);
		this->nextBooleanVariableIndex++;
		return this->nextBooleanVariableIndex-1;
	}

	uint_fast64_t addIntegerVariable(const std::string& name, const std::shared_ptr<storm::ir::expressions::BaseExpression> lower, const std::shared_ptr<storm::ir::expressions::BaseExpression> upper, const std::shared_ptr<storm::ir::expressions::BaseExpression> init) {
		std::shared_ptr<VariableExpression> res = this->integerVariables_.at(name);
		if (res != nullptr) {
			return res->getVariableIndex();
		}
		std::shared_ptr<VariableExpression> varExpr = std::shared_ptr<VariableExpression>(new VariableExpression(storm::ir::expressions::BaseExpression::int_, this->nextIntegerVariableIndex, name, lower, upper));
		this->integerVariables_.add(name, varExpr);
		this->integerVariableNames_.add(name, name);
		this->nextIntegerVariableIndex++;
		return this->nextIntegerVariableIndex-1;
	}

	bool isFreeIdentifier(std::string& s) const {
		if (this->integerVariableNames_.find(s) != nullptr) return false;
		if (this->allConstantNames_.find(s) != nullptr) return false;
		if (this->labelNames_.find(s) != nullptr) return false;
		if (this->moduleNames_.find(s) != nullptr) return false;
		if (this->keywords.find(s) != nullptr) return false;
		return true;
	}
	bool isIdentifier(std::string& s) const {
		if (this->allConstantNames_.find(s) != nullptr) return false;
		if (this->keywords.find(s) != nullptr) return false;
		return true;
	}

	void prepareForSecondRun() {
		integerConstants_.clear();
		booleanConstants_.clear();
		doubleConstants_.clear();
		allConstantNames_.clear();
	}
};

}
}
}

#endif	/* VARIABLESTATE_H */

