/*
 * PrismParser.h
 *
 *  Created on: Jan 3, 2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_PARSER_PRISMPARSER_H_
#define STORM_PARSER_PRISMPARSER_H_

// All classes of the intermediate representation are used.
#include "src/ir/IR.h"
#include "src/parser/PrismParser/Includes.h"
#include "src/parser/PrismParser/Tokens.h"
#include "src/parser/PrismParser/IdentifierGrammars.h"
#include "src/parser/PrismParser/VariableState.h"
#include "src/parser/PrismParser/ConstBooleanExpressionGrammar.h"
#include "src/parser/PrismParser/ConstDoubleExpressionGrammar.h"
#include "src/parser/PrismParser/ConstIntegerExpressionGrammar.h"
#include "src/parser/PrismParser/BooleanExpressionGrammar.h"
#include "src/parser/PrismParser/IntegerExpressionGrammar.h"

// Used for file input.
#include <istream>
#include <memory>

namespace storm {

namespace parser {

using namespace storm::ir;
using namespace storm::ir::expressions;

/*!
 * This class parses the format of the PRISM model checker into an intermediate representation.
 */
class PrismParser {
public:
	/*!
	 * Parses the given file into the intermediate representation assuming it complies with the
	 * PRISM syntax.
	 * @param filename the name of the file to parse.
	 * @return a shared pointer to the intermediate representation of the PRISM file.
	 */
	std::shared_ptr<storm::ir::Program> parseFile(std::string const& filename) const;

	/*!
	 * The Boost spirit grammar for the PRISM language. Returns the intermediate representation of
	 * the input that complies with the PRISM syntax.
	 */
	class PrismGrammar : public qi::grammar<
		Iterator,
		std::shared_ptr<Program>(),
		qi::locals<
			std::map<std::string, std::shared_ptr<BooleanConstantExpression>>,
			std::map<std::string, std::shared_ptr<IntegerConstantExpression>>,
			std::map<std::string, std::shared_ptr<DoubleConstantExpression>>, std::map<std::string, std::shared_ptr<RewardModel>>, std::map<std::string, std::shared_ptr<BaseExpression>>>, Skipper> {
	public:
		PrismGrammar();
		void prepareForSecondRun();

	private:

	std::shared_ptr<storm::parser::prism::VariableState> state;

	// The starting point of the grammar.
	qi::rule<
			Iterator,
			std::shared_ptr<Program>(),
			qi::locals<std::map<std::string, std::shared_ptr<BooleanConstantExpression>>, std::map<std::string, std::shared_ptr<IntegerConstantExpression>>, std::map<std::string, std::shared_ptr<DoubleConstantExpression>>, std::map<std::string, std::shared_ptr<RewardModel>>, std::map<std::string, std::shared_ptr<BaseExpression>>>,
			Skipper> start;
	qi::rule<Iterator, Program::ModelType(), Skipper> modelTypeDefinition;
	qi::rule<Iterator, qi::unused_type(std::map<std::string, std::shared_ptr<BooleanConstantExpression>>&, std::map<std::string, std::shared_ptr<IntegerConstantExpression>>&, std::map<std::string, std::shared_ptr<DoubleConstantExpression>>&), Skipper> constantDefinitionList;
	qi::rule<Iterator, std::vector<std::shared_ptr<Module>>(), Skipper> moduleDefinitionList;

	// Rules for module definition.
	qi::rule<Iterator, std::shared_ptr<Module>(), qi::locals<std::vector<BooleanVariable>, std::vector<IntegerVariable>, std::map<std::string, uint_fast64_t>, std::map<std::string, uint_fast64_t>>, Skipper> moduleDefinition;
	qi::rule<Iterator, std::shared_ptr<Module>(), qi::locals<std::map<std::string, std::string>>, Skipper> moduleRenaming;

	// Rules for variable definitions.
	qi::rule<Iterator, std::shared_ptr<BaseExpression>(), Skipper> integerLiteralExpression;
	qi::rule<Iterator, qi::unused_type(std::vector<BooleanVariable>&, std::vector<IntegerVariable>&, std::map<std::string, uint_fast64_t>&, std::map<std::string, uint_fast64_t>&), Skipper> variableDefinition;
	qi::rule<Iterator, qi::unused_type(std::vector<BooleanVariable>&, std::map<std::string, uint_fast64_t>&), qi::locals<uint_fast64_t, std::shared_ptr<BaseExpression>>, Skipper> booleanVariableDefinition;
	qi::rule<Iterator, qi::unused_type(std::vector<IntegerVariable>&, std::map<std::string, uint_fast64_t>&), qi::locals<uint_fast64_t, std::shared_ptr<BaseExpression>>, Skipper> integerVariableDefinition;

	// Rules for command definitions.
	qi::rule<Iterator, std::shared_ptr<Command>(), qi::locals<std::string>, Skipper> commandDefinition;
	qi::rule<Iterator, std::vector<std::shared_ptr<Update>>(), Skipper> updateListDefinition;
	qi::rule<Iterator, std::shared_ptr<Update>(), qi::locals<std::map<std::string, Assignment>, std::map<std::string, Assignment>>, Skipper> updateDefinition;
	qi::rule<Iterator, qi::unused_type(std::map<std::string, Assignment>&, std::map<std::string, Assignment>&), Skipper> assignmentDefinitionList;
	qi::rule<Iterator, qi::unused_type(std::map<std::string, Assignment>&, std::map<std::string, Assignment>&), Skipper> assignmentDefinition;

	// Rules for variable/command names.
	qi::rule<Iterator, std::string(), Skipper> commandName;
	qi::rule<Iterator, std::string(), Skipper> unassignedLocalBooleanVariableName;
	qi::rule<Iterator, std::string(), Skipper> unassignedLocalIntegerVariableName;

	// Rules for reward definitions.
	qi::rule<Iterator, qi::unused_type(std::map<std::string, std::shared_ptr<RewardModel>>&), Skipper> rewardDefinitionList;
	qi::rule<Iterator, qi::unused_type(std::map<std::string, std::shared_ptr<RewardModel>>&), qi::locals<std::vector<std::shared_ptr<StateReward>>, std::vector<std::shared_ptr<TransitionReward>>>, Skipper> rewardDefinition;
	qi::rule<Iterator, std::shared_ptr<StateReward>(), Skipper> stateRewardDefinition;
	qi::rule<Iterator, std::shared_ptr<TransitionReward>(), qi::locals<std::string>, Skipper> transitionRewardDefinition;

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
	std::shared_ptr<Module> renameModule(const std::string& name, const std::string& oldname, std::map<std::string, std::string>& mapping);
	std::shared_ptr<Module> createModule(const std::string name, std::vector<BooleanVariable>& bools, std::vector<IntegerVariable>& ints, std::map<std::string, uint_fast64_t>& boolids, std::map<std::string, uint_fast64_t> intids, std::vector<std::shared_ptr<storm::ir::Command>> commands);

	};
	
private:
	/*!
	 * Parses the given input stream into the intermediate representation assuming it complies with
	 * the PRISM syntax.
	 * @param inputStream the input stream to parse.
	 * @param filename the name of the file the input stream belongs to. Used for diagnostics.
	 * @return a shared pointer to the intermediate representation of the PRISM file.
	 */
	std::shared_ptr<storm::ir::Program> parse(std::istream& inputStream, std::string const& filename) const;
};

} // namespace parser

} // namespace storm

#endif /* STORM_PARSER_PRISMPARSER_H_ */
