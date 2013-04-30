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
#include <iostream>

namespace storm {
namespace parser {
namespace prism {

using namespace storm::ir;
using namespace storm::ir::expressions;

template<typename T>
std::ostream& operator<<(std::ostream& out, qi::symbols<char, T>& symbols);

struct VariableState : public storm::ir::VariableAdder {

public:
	VariableState(bool firstRun = true);

public:
	bool firstRun;
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
public:
	uint_fast64_t addBooleanVariable(const std::string& name, const std::shared_ptr<storm::ir::expressions::BaseExpression> init);

	uint_fast64_t addIntegerVariable(const std::string& name, const std::shared_ptr<storm::ir::expressions::BaseExpression> lower, const std::shared_ptr<storm::ir::expressions::BaseExpression> upper, const std::shared_ptr<storm::ir::expressions::BaseExpression> init);

	std::shared_ptr<VariableExpression> getBooleanVariable(const std::string& name);

	std::shared_ptr<VariableExpression> getIntegerVariable(const std::string& name);

	void startModule();

	bool isFreeIdentifier(std::string& s) const;
	bool isIdentifier(std::string& s) const;

	void prepareForSecondRun();
};

}
}
}

#endif	/* VARIABLESTATE_H */

