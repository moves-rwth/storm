/* 
 * File:   PrismGrammar.h
 * Author: nafur
 *
 * Created on April 30, 2013, 5:20 PM
 */

#ifndef PRISMGRAMMAR_H
#define	PRISMGRAMMAR_H

// All classes of the intermediate representation are used.
#include "src/ir/IR.h"
#include "src/parser/prismparser/Includes.h"
#include "src/parser/prismparser/Tokens.h"
#include "src/parser/prismparser/IdentifierGrammars.h"
#include "src/parser/prismparser/VariableState.h"
#include "src/parser/prismparser/ConstBooleanExpressionGrammar.h"
#include "src/parser/prismparser/ConstDoubleExpressionGrammar.h"
#include "src/parser/prismparser/ConstIntegerExpressionGrammar.h"
#include "src/parser/prismparser/BooleanExpressionGrammar.h"
#include "src/parser/prismparser/IntegerExpressionGrammar.h"

// Used for file input.
#include <istream>
#include <memory>

namespace storm {
namespace parser {
namespace prism {

using namespace storm::ir;
using namespace storm::ir::expressions;

/*!
 * The Boost spirit grammar for the PRISM language. Returns the intermediate representation of
 * the input that complies with the PRISM syntax.
 */
class PrismGrammar : public qi::grammar<
	Iterator,
	Program(),
	qi::locals<
		std::map<std::string, std::shared_ptr<BooleanConstantExpression>>,
		std::map<std::string, std::shared_ptr<IntegerConstantExpression>>,
		std::map<std::string, std::shared_ptr<DoubleConstantExpression>>,
		std::map<std::string, RewardModel>,
		std::map<std::string, std::shared_ptr<BaseExpression>>
	>,
	Skipper> {
public:
	PrismGrammar();
	void prepareForSecondRun();
	void resetGrammars();

private:

	std::shared_ptr<storm::parser::prism::VariableState> state;

	// The starting point of the grammar.
	qi::rule<
			Iterator,
			Program(),
			qi::locals<
				std::map<std::string, std::shared_ptr<BooleanConstantExpression>>,
				std::map<std::string, std::shared_ptr<IntegerConstantExpression>>,
				std::map<std::string, std::shared_ptr<DoubleConstantExpression>>,
				std::map<std::string, RewardModel>,
				std::map<std::string, std::shared_ptr<BaseExpression>>
			>,
			Skipper> start;
	qi::rule<Iterator, Program::ModelType(), Skipper> modelTypeDefinition;
	qi::rule<Iterator, qi::unused_type(std::map<std::string, std::shared_ptr<BooleanConstantExpression>>&, std::map<std::string, std::shared_ptr<IntegerConstantExpression>>&, std::map<std::string, std::shared_ptr<DoubleConstantExpression>>&), Skipper> constantDefinitionList;
	qi::rule<Iterator, std::vector<Module>(), Skipper> moduleDefinitionList;

	// Rules for module definition.
	qi::rule<Iterator, Module(), qi::locals<std::vector<BooleanVariable>, std::vector<IntegerVariable>, std::map<std::string, uint_fast64_t>, std::map<std::string, uint_fast64_t>>, Skipper> moduleDefinition;
	qi::rule<Iterator, Module(), qi::locals<std::map<std::string, std::string>>, Skipper> moduleRenaming;

	// Rules for variable definitions.
	qi::rule<Iterator, qi::unused_type(std::vector<BooleanVariable>&, std::vector<IntegerVariable>&, std::map<std::string, uint_fast64_t>&, std::map<std::string, uint_fast64_t>&), Skipper> variableDefinition;
	qi::rule<Iterator, qi::unused_type(std::vector<BooleanVariable>&, std::map<std::string, uint_fast64_t>&), qi::locals<uint_fast64_t, std::shared_ptr<BaseExpression>>, Skipper> booleanVariableDefinition;
	qi::rule<Iterator, qi::unused_type(std::vector<IntegerVariable>&, std::map<std::string, uint_fast64_t>&), qi::locals<uint_fast64_t, std::shared_ptr<BaseExpression>>, Skipper> integerVariableDefinition;

	// Rules for command definitions.
	qi::rule<Iterator, Command(), qi::locals<std::string>, Skipper> commandDefinition;
	qi::rule<Iterator, std::vector<Update>(), Skipper> updateListDefinition;
	qi::rule<Iterator, Update(), qi::locals<std::map<std::string, Assignment>, std::map<std::string, Assignment>>, Skipper> updateDefinition;
	qi::rule<Iterator, qi::unused_type(std::map<std::string, Assignment>&, std::map<std::string, Assignment>&), Skipper> assignmentDefinitionList;
	qi::rule<Iterator, qi::unused_type(std::map<std::string, Assignment>&, std::map<std::string, Assignment>&), Skipper> assignmentDefinition;

	// Rules for variable/command names.
	qi::rule<Iterator, std::string(), Skipper> commandName;
	qi::rule<Iterator, std::string(), Skipper> unassignedLocalBooleanVariableName;
	qi::rule<Iterator, std::string(), Skipper> unassignedLocalIntegerVariableName;

	// Rules for reward definitions.
	qi::rule<Iterator, qi::unused_type(std::map<std::string, RewardModel>&), Skipper> rewardDefinitionList;
	qi::rule<Iterator, qi::unused_type(std::map<std::string, RewardModel>&), qi::locals<std::vector<StateReward>, std::vector<TransitionReward>>, Skipper> rewardDefinition;
	qi::rule<Iterator, StateReward(), Skipper> stateRewardDefinition;
	qi::rule<Iterator, TransitionReward(), qi::locals<std::string>, Skipper> transitionRewardDefinition;

	// Rules for label definitions.
	qi::rule<Iterator, qi::unused_type(std::map<std::string, std::shared_ptr<BaseExpression>>&), Skipper> labelDefinitionList;
	qi::rule<Iterator, qi::unused_type(std::map<std::string, std::shared_ptr<BaseExpression>>&), Skipper> labelDefinition;

	// Rules for constant definitions.
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> constantDefinition;
	qi::rule<Iterator, qi::unused_type(std::map<std::string, std::shared_ptr<BooleanConstantExpression>>&, std::map<std::string, std::shared_ptr<IntegerConstantExpression>>&, std::map<std::string, std::shared_ptr<DoubleConstantExpression>>&), Skipper> undefinedConstantDefinition;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> definedConstantDefinition;
	qi::rule<Iterator, qi::unused_type(std::map<std::string, std::shared_ptr<BooleanConstantExpression>>&), qi::locals<std::shared_ptr<BooleanConstantExpression>>, Skipper> undefinedBooleanConstantDefinition;
	qi::rule<Iterator, qi::unused_type(std::map<std::string, std::shared_ptr<IntegerConstantExpression>>&), qi::locals<std::shared_ptr<IntegerConstantExpression>>, Skipper> undefinedIntegerConstantDefinition;
	qi::rule<Iterator, qi::unused_type(std::map<std::string, std::shared_ptr<DoubleConstantExpression>>&), qi::locals<std::shared_ptr<DoubleConstantExpression>>, Skipper> undefinedDoubleConstantDefinition;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> definedBooleanConstantDefinition;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> definedIntegerConstantDefinition;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> definedDoubleConstantDefinition;

	// Rules for variable recognition.
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> booleanVariableCreatorExpression;
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), qi::locals<std::shared_ptr<BaseExpression>>, Skipper> integerVariableCreatorExpression;

	storm::parser::prism::keywordsStruct keywords_;
	storm::parser::prism::modelTypeStruct modelType_;
	storm::parser::prism::relationalOperatorStruct relations_;

	std::shared_ptr<BaseExpression> addIntegerConstant(const std::string& name, const std::shared_ptr<BaseExpression> value);
	void addLabel(const std::string& name, std::shared_ptr<BaseExpression> value, std::map<std::string, std::shared_ptr<BaseExpression>>& mapping);
	void addBoolAssignment(const std::string& variable, std::shared_ptr<BaseExpression> value, std::map<std::string, Assignment>& mapping);
	void addIntAssignment(const std::string& variable, std::shared_ptr<BaseExpression> value, std::map<std::string, Assignment>& mapping);
	Module renameModule(const std::string& name, const std::string& oldname, std::map<std::string, std::string>& mapping);
	Module createModule(const std::string name, std::vector<BooleanVariable>& bools, std::vector<IntegerVariable>& ints, std::map<std::string, uint_fast64_t>& boolids, std::map<std::string, uint_fast64_t> intids, std::vector<storm::ir::Command> commands);

	void createIntegerVariable(const std::string name, std::shared_ptr<BaseExpression> lower, std::shared_ptr<BaseExpression> upper, std::shared_ptr<BaseExpression> init, std::vector<IntegerVariable>& vars, std::map<std::string, uint_fast64_t>& varids);
	void createBooleanVariable(const std::string name, std::shared_ptr<BaseExpression> init, std::vector<BooleanVariable>& vars, std::map<std::string, uint_fast64_t>& varids);
};


} // namespace prism
} // namespace parser
} // namespace storm


#endif	/* PRISMGRAMMAR_H */

