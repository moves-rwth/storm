/*
 * PrismParser.cpp
 *
 *  Created on: 11.01.2013
 *      Author: chris
 */

#include "PrismParser.h"
#include "src/utility/OsDetection.h"

// If the parser fails due to ill-formed data, this exception is thrown.
#include "src/exceptions/WrongFileFormatException.h"

// Used for Boost spirit.
#include <boost/typeof/typeof.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

// Include headers for spirit iterators. Needed for diagnostics and input stream iteration.
#include <boost/spirit/include/classic_position_iterator.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>

// Needed for file IO.
#include <fstream>
#include <iomanip>

// Some typedefs and namespace definitions to reduce code size.
typedef std::istreambuf_iterator<char> base_iterator_type;
typedef boost::spirit::multi_pass<base_iterator_type> forward_iterator_type;
typedef boost::spirit::classic::position_iterator2<forward_iterator_type> pos_iterator_type;
namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;

namespace storm {

namespace parser {

// The Boost spirit grammar used for parsing the input.
template<typename Iterator, typename Skipper>
struct PrismParser::PrismGrammar : qi::grammar<Iterator, storm::ir::Program(), qi::locals<std::map<std::string, std::shared_ptr<storm::ir::expressions::BooleanConstantExpression>>, std::map<std::string, std::shared_ptr<storm::ir::expressions::IntegerConstantExpression>>, std::map<std::string, std::shared_ptr<storm::ir::expressions::DoubleConstantExpression>>, std::map<std::string, storm::ir::RewardModel>, std::map<std::string, std::shared_ptr<storm::ir::expressions::BaseExpression>>>, Skipper> {

	/*
	 * The constructor of the grammar. It defines all rules of the grammar and the corresponding
	 * semantic actions that take care of constructing the intermediate representation during
	 * parsing.
	 *
	 * Note: The grammar takes care of some semantic checks already. For example, in places
	 * where we necessarily require a constant expression, this is enforced by not allowing
	 * variables as subexpressions. Also, variable names are by definition unique and it is
	 * ensured that variables and constants are properly declared.
	 * TODO: It should be ensured that updates of a command only refer to variables of the
	 * current module.
	 */
	PrismGrammar() : PrismGrammar::base_type(start), nextBooleanVariableIndex(0), nextIntegerVariableIndex(0) {
		// This rule defines all identifiers that have not been previously used.
		freeIdentifierName %= qi::raw[qi::lexeme[((qi::alpha | qi::char_('_')) >> *(qi::alnum | qi::char_('_'))) - booleanVariableNames_ - integerVariableNames_ - allConstantNames_ - labelNames_ - moduleNames_ - keywords_]];
		freeIdentifierName.name("unused identifier");

		// This block defines all literal expressions.
		booleanLiteralExpression = qi::bool_[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::BooleanLiteral>(qi::_1))];
		booleanLiteralExpression.name("boolean literal");
		integerLiteralExpression = qi::int_[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::IntegerLiteral>(qi::_1))];
		integerLiteralExpression.name("integer literal");
		doubleLiteralExpression = qi::double_[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::DoubleLiteral>(qi::_1))];
		doubleLiteralExpression.name("double literal");
		literalExpression %= (booleanLiteralExpression | integerLiteralExpression | doubleLiteralExpression);
		literalExpression.name("literal");

		// This block defines all expressions that are variables.
		integerVariableExpression %= integerVariables_;
		integerVariableExpression.name("integer variable");
		booleanVariableExpression %= booleanVariables_;
		booleanVariableExpression.name("boolean variable");
		variableExpression %= (integerVariableExpression | booleanVariableExpression);
		variableExpression.name("variable");

		// This block defines all atomic expressions that are constant, i.e. literals and constants.
		booleanConstantExpression %= (booleanConstants_ | booleanLiteralExpression);
		booleanConstantExpression.name("boolean constant or literal");
		integerConstantExpression %= (integerConstants_ | integerLiteralExpression);
		integerConstantExpression.name("integer constant or literal");
		doubleConstantExpression %= (doubleConstants_ | doubleLiteralExpression);
		doubleConstantExpression.name("double constant or literal");
		constantExpression %= (booleanConstantExpression | integerConstantExpression | doubleConstantExpression);
		constantExpression.name("constant or literal");

		// This block defines all expressions of integral type.
		atomicIntegerExpression %= (integerVariableExpression | qi::lit("(") >> integerExpression >> qi::lit(")") | integerConstantExpression);
		atomicIntegerExpression.name("integer expression");
		integerMultExpression %= atomicIntegerExpression[qi::_val = qi::_1] >> *(qi::lit("*") >> atomicIntegerExpression)[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::BinaryNumericalFunctionExpression>(storm::ir::expressions::BaseExpression::int_, qi::_val, qi::_1, storm::ir::expressions::BinaryNumericalFunctionExpression::TIMES))];
		integerMultExpression.name("integer expression");
		integerPlusExpression = integerMultExpression[qi::_val = qi::_1] >> *(qi::lit("+") >> integerMultExpression)[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::BinaryNumericalFunctionExpression>(storm::ir::expressions::BaseExpression::int_, qi::_val, qi::_1, storm::ir::expressions::BinaryNumericalFunctionExpression::PLUS))];
		integerPlusExpression.name("integer expression");
		integerExpression %= integerPlusExpression;
		integerExpression.name("integer expression");

		// This block defines all expressions of integral type that are by syntax constant. That is, they are evaluable given the values for all constants.
		constantAtomicIntegerExpression %= (qi::lit("(") >> constantIntegerExpression >> qi::lit(")") | integerConstantExpression);
		constantAtomicIntegerExpression.name("constant integer expression");
		constantIntegerMultExpression %= constantAtomicIntegerExpression[qi::_val = qi::_1] >> *(qi::lit("*") >> constantAtomicIntegerExpression)[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::BinaryNumericalFunctionExpression>(storm::ir::expressions::BaseExpression::int_, qi::_val, qi::_1, storm::ir::expressions::BinaryNumericalFunctionExpression::TIMES))];
		constantIntegerMultExpression.name("constant integer expression");
		constantIntegerPlusExpression = constantIntegerMultExpression[qi::_val = qi::_1] >> *(qi::lit("+") >> constantIntegerMultExpression)[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::BinaryNumericalFunctionExpression>(storm::ir::expressions::BaseExpression::int_, qi::_val, qi::_1, storm::ir::expressions::BinaryNumericalFunctionExpression::PLUS))];
		constantIntegerPlusExpression.name("constant integer expression");
		constantIntegerExpression %= constantIntegerPlusExpression;
		constantIntegerExpression.name("constant integer expression");

		// This block defines all expressions of type double that are by syntax constant. That is, they are evaluable given the values for all constants.
		constantAtomicDoubleExpression %= (qi::lit("(") >> constantDoubleExpression >> qi::lit(")") | doubleConstantExpression);
		constantAtomicDoubleExpression.name("constant double expression");
		constantDoubleMultExpression %= constantAtomicDoubleExpression[qi::_val = qi::_1] >> *(qi::lit("*") >> constantAtomicDoubleExpression)[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::BinaryNumericalFunctionExpression>(storm::ir::expressions::BaseExpression::double_, qi::_val, qi::_1, storm::ir::expressions::BinaryNumericalFunctionExpression::TIMES))];
		constantDoubleMultExpression.name("constant double expression");
		constantDoublePlusExpression %= constantDoubleMultExpression[qi::_val = qi::_1] >> *(qi::lit("+") >> constantDoubleMultExpression)[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::BinaryNumericalFunctionExpression>(storm::ir::expressions::BaseExpression::double_, qi::_val, qi::_1, storm::ir::expressions::BinaryNumericalFunctionExpression::PLUS))];
		constantDoublePlusExpression.name("constant double expression");
		constantDoubleExpression %= constantDoublePlusExpression;
		constantDoubleExpression.name("constant double expression");

		// This block defines all expressions of type boolean.
		relativeExpression = (integerExpression >> relations_ >> integerExpression)[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::BinaryRelationExpression>(qi::_1, qi::_3, qi::_2))];
		relativeExpression.name("boolean expression");
		atomicBooleanExpression %= (relativeExpression | booleanVariableExpression | qi::lit("(") >> booleanExpression >> qi::lit(")") | booleanConstantExpression);
		atomicBooleanExpression.name("boolean expression");
		notExpression = atomicBooleanExpression[qi::_val = qi::_1] | (qi::lit("!") >> atomicBooleanExpression)[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::UnaryBooleanFunctionExpression>>(phoenix::new_<storm::ir::expressions::UnaryBooleanFunctionExpression>(qi::_1, storm::ir::expressions::UnaryBooleanFunctionExpression::NOT))];
		notExpression.name("boolean expression");
		andExpression = notExpression[qi::_val = qi::_1] >> *(qi::lit("&") >> notExpression)[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::BinaryBooleanFunctionExpression>(qi::_val, qi::_1, storm::ir::expressions::BinaryBooleanFunctionExpression::AND))];
		andExpression.name("boolean expression");
		orExpression = andExpression[qi::_val = qi::_1] >> *(qi::lit("|") >> andExpression)[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::BinaryBooleanFunctionExpression>(qi::_val, qi::_1, storm::ir::expressions::BinaryBooleanFunctionExpression::OR))];
		orExpression.name("boolean expression");
		booleanExpression %= orExpression;
		booleanExpression.name("boolean expression");

		// This block defines all expressions of type boolean that are by syntax constant. That is, they are evaluable given the values for all constants.
		constantRelativeExpression = (constantIntegerExpression >> relations_ >> constantIntegerExpression)[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::BinaryRelationExpression>(qi::_1, qi::_3, qi::_2))];
		constantRelativeExpression.name("constant boolean expression");
		constantAtomicBooleanExpression %= (constantRelativeExpression | qi::lit("(") >> constantBooleanExpression >> qi::lit(")") | booleanLiteralExpression | booleanConstantExpression);
		constantAtomicBooleanExpression.name("constant boolean expression");
		constantNotExpression = constantAtomicBooleanExpression[qi::_val = qi::_1] | (qi::lit("!") >> constantAtomicBooleanExpression)[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::UnaryBooleanFunctionExpression>>(phoenix::new_<storm::ir::expressions::UnaryBooleanFunctionExpression>(qi::_1, storm::ir::expressions::UnaryBooleanFunctionExpression::NOT))];
		constantNotExpression.name("constant boolean expression");
		constantAndExpression = constantNotExpression[qi::_val = qi::_1] >> *(qi::lit("&") >> constantNotExpression)[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::BinaryBooleanFunctionExpression>(qi::_val, qi::_1, storm::ir::expressions::BinaryBooleanFunctionExpression::AND))];
		constantAndExpression.name("constant boolean expression");
		constantOrExpression = constantAndExpression[qi::_val = qi::_1] >> *(qi::lit("|") >> constantAndExpression)[qi::_val = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(phoenix::new_<storm::ir::expressions::BinaryBooleanFunctionExpression>(qi::_val, qi::_1, storm::ir::expressions::BinaryBooleanFunctionExpression::OR))];
		constantOrExpression.name("constant boolean expression");
		constantBooleanExpression %= constantOrExpression;
		constantBooleanExpression.name("constant boolean expression");

		// This block defines the general root of all expressions. Most of the time, however, you may want to start with a more specialized rule.
		expression %= (booleanExpression | integerExpression | constantDoubleExpression);
		expression.name("expression");

		// This block defines all entities that are needed for parsing labels.
		labelDefinition = (qi::lit("label") >> -qi::lit("\"") >> freeIdentifierName >> -qi::lit("\"") >> qi::lit("=") >> booleanExpression >> qi::lit(";"))[phoenix::insert(qi::_r1, phoenix::construct<std::pair<std::string, std::shared_ptr<storm::ir::expressions::BaseExpression>>>(qi::_1, qi::_2)), phoenix::bind(labelNames_.add, qi::_1, qi::_1)];
		labelDefinition.name("label declaration");
		labelDefinitionList %= *labelDefinition(qi::_r1);
		labelDefinitionList.name("label declaration list");

		// This block defines all entities that are needed for parsing a reward model.
		stateRewardDefinition = (booleanExpression > qi::lit(":") > constantDoubleExpression >> qi::lit(";"))[qi::_val = phoenix::construct<storm::ir::StateReward>(qi::_1, qi::_2)];
		stateRewardDefinition.name("state reward definition");
		transitionRewardDefinition = (qi::lit("[") > -(commandName[qi::_a = qi::_1]) > qi::lit("]") > booleanExpression > qi::lit(":") > constantDoubleExpression > qi::lit(";"))[qi::_val = phoenix::construct<storm::ir::TransitionReward>(qi::_a, qi::_2, qi::_3)];
		transitionRewardDefinition.name("transition reward definition");
		rewardDefinition = (qi::lit("rewards") > qi::lit("\"") > freeIdentifierName > qi::lit("\"") > +(stateRewardDefinition[phoenix::push_back(qi::_a, qi::_1)] | transitionRewardDefinition[phoenix::push_back(qi::_b, qi::_1)]) >> qi::lit("endrewards"))[phoenix::insert(qi::_r1, phoenix::construct<std::pair<std::string, storm::ir::RewardModel>>(qi::_1, phoenix::construct<storm::ir::RewardModel>(qi::_1, qi::_a, qi::_b)))];
		rewardDefinition.name("reward definition");
		rewardDefinitionList = *rewardDefinition(qi::_r1);
		rewardDefinitionList.name("reward definition list");

		// This block defines auxiliary entities that are used to check whether a certain variable exist.
		booleanVariableName %= booleanVariableNames_;
		booleanVariableName.name("boolean variable");
		integerVariableName %= integerVariableNames_;
		integerVariableName.name("integer variable");
		commandName %= commandNames_;
		commandName.name("command name");
		unassignedLocalBooleanVariableName %= localBooleanVariables_ - assignedLocalBooleanVariables_;
		unassignedLocalBooleanVariableName.name("unassigned local boolean variable");
		unassignedLocalIntegerVariableName %= localIntegerVariables_ - assignedLocalIntegerVariables_;
		unassignedLocalIntegerVariableName.name("unassigned local integer variable");

		// This block defines all entities that are needed for parsing a single command.
		assignmentDefinition = (qi::lit("(") >> unassignedLocalIntegerVariableName > qi::lit("'") > qi::lit("=") > integerExpression > qi::lit(")"))[phoenix::bind(assignedLocalIntegerVariables_.add, qi::_1, qi::_1), phoenix::insert(qi::_r2, phoenix::construct<std::pair<std::string, storm::ir::Assignment>>(qi::_1, phoenix::construct<storm::ir::Assignment>(qi::_1, qi::_2)))] | (qi::lit("(") > unassignedLocalBooleanVariableName > qi::lit("'") > qi::lit("=") > booleanExpression > qi::lit(")"))[phoenix::bind(assignedLocalBooleanVariables_.add, qi::_1, qi::_1), phoenix::insert(qi::_r1, phoenix::construct<std::pair<std::string, storm::ir::Assignment>>(qi::_1, phoenix::construct<storm::ir::Assignment>(qi::_1, qi::_2)))];
		assignmentDefinition.name("assignment");
		assignmentDefinitionList = assignmentDefinition(qi::_r1, qi::_r2) % "&";
		assignmentDefinitionList.name("assignment list");
		updateDefinition = (constantDoubleExpression > qi::lit(":")[phoenix::clear(phoenix::ref(assignedLocalBooleanVariables_)), phoenix::clear(phoenix::ref(assignedLocalIntegerVariables_))] > assignmentDefinitionList(qi::_a, qi::_b))[qi::_val = phoenix::construct<storm::ir::Update>(qi::_1, qi::_a, qi::_b)];
		updateDefinition.name("update");
		updateListDefinition = +updateDefinition % "+";
		updateListDefinition.name("update list");
		commandDefinition = (qi::lit("[") > -((freeIdentifierName[phoenix::bind(commandNames_.add, qi::_1, qi::_1)] | commandName)[qi::_a = qi::_1]) > qi::lit("]") > booleanExpression > qi::lit("->") > updateListDefinition > qi::lit(";"))[qi::_val = phoenix::construct<storm::ir::Command>(qi::_a, qi::_2, qi::_3)];
		commandDefinition.name("command");

		// This block defines all entities that are neede for parsing variable definitions.
		booleanVariableDefinition = (freeIdentifierName >> qi::lit(":") >> qi::lit("bool") > -(qi::lit("init") > constantBooleanExpression[qi::_b = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(qi::_1)]) > qi::lit(";"))[phoenix::push_back(qi::_r1, phoenix::construct<storm::ir::BooleanVariable>(phoenix::val(nextBooleanVariableIndex), phoenix::val(qi::_1), qi::_b)), phoenix::insert(qi::_r2, phoenix::construct<std::pair<std::string, uint_fast64_t>>(qi::_1, phoenix::val(nextBooleanVariableIndex))), qi::_a = phoenix::construct<std::shared_ptr<storm::ir::expressions::VariableExpression>>(phoenix::new_<storm::ir::expressions::VariableExpression>(storm::ir::expressions::BaseExpression::bool_, phoenix::val(nextBooleanVariableIndex), qi::_1)), phoenix::bind(booleanVariables_.add, qi::_1, qi::_a), phoenix::bind(booleanVariableNames_.add, qi::_1, qi::_1), phoenix::bind(localBooleanVariables_.add, qi::_1, qi::_1), phoenix::ref(nextBooleanVariableIndex)++];
		booleanVariableDefinition.name("boolean variable declaration");
		integerVariableDefinition = (freeIdentifierName > qi::lit(":") > qi::lit("[") > constantIntegerExpression > qi::lit("..") > constantIntegerExpression > qi::lit("]") > -(qi::lit("init") > constantIntegerExpression[qi::_b = phoenix::construct<std::shared_ptr<storm::ir::expressions::BaseExpression>>(qi::_1)]) > qi::lit(";"))[phoenix::push_back(qi::_r1, phoenix::construct<storm::ir::IntegerVariable>(phoenix::val(nextIntegerVariableIndex), qi::_1, qi::_2, qi::_3, qi::_b)), phoenix::insert(qi::_r2, phoenix::construct<std::pair<std::string, uint_fast64_t>>(qi::_1, phoenix::val(nextIntegerVariableIndex))), qi::_a = phoenix::construct<std::shared_ptr<storm::ir::expressions::VariableExpression>>(phoenix::new_<storm::ir::expressions::VariableExpression>(storm::ir::expressions::BaseExpression::int_, phoenix::val(nextIntegerVariableIndex), qi::_1)), phoenix::bind(integerVariables_.add, qi::_1, qi::_a), phoenix::bind(integerVariableNames_.add, qi::_1, qi::_1), phoenix::bind(localIntegerVariables_.add, qi::_1, qi::_1), phoenix::ref(nextIntegerVariableIndex)++];
		integerVariableDefinition.name("integer variable declaration");
		variableDefinition = (booleanVariableDefinition(qi::_r1, qi::_r3) | integerVariableDefinition(qi::_r2, qi::_r4));
		variableDefinition.name("variable declaration");

		// This block defines all entities that are needed for parsing a module.
		moduleDefinition = (qi::lit("module")[phoenix::clear(phoenix::ref(localBooleanVariables_)), phoenix::clear(phoenix::ref(localIntegerVariables_))] > freeIdentifierName > *(variableDefinition(qi::_a, qi::_b, qi::_c, qi::_d)) > +commandDefinition > qi::lit("endmodule"))[phoenix::bind(moduleNames_.add, qi::_1, qi::_1), qi::_val = phoenix::construct<storm::ir::Module>(qi::_1, qi::_a, qi::_b, qi::_c, qi::_d, qi::_2)];
		moduleDefinition.name("module");
		moduleDefinitionList %= +moduleDefinition;
		moduleDefinitionList.name("module list");

		// This block defines all entities that are needed for parsing constant definitions.
		definedBooleanConstantDefinition = (qi::lit("const") >> qi::lit("bool") >> freeIdentifierName >> qi::lit("=") > booleanLiteralExpression > qi::lit(";"))[phoenix::bind(booleanConstants_.add, qi::_1, qi::_2), phoenix::bind(allConstantNames_.add, qi::_1, qi::_1), qi::_val = qi::_2];
		definedBooleanConstantDefinition.name("defined boolean constant declaration");
		definedIntegerConstantDefinition = (qi::lit("const") >> qi::lit("int") >> freeIdentifierName >> qi::lit("=") > integerLiteralExpression > qi::lit(";"))[phoenix::bind(integerConstants_.add, qi::_1, qi::_2), phoenix::bind(allConstantNames_.add, qi::_1, qi::_1), qi::_val = qi::_2];
		definedIntegerConstantDefinition.name("defined integer constant declaration");
		definedDoubleConstantDefinition = (qi::lit("const") >> qi::lit("double") >> freeIdentifierName >> qi::lit("=") > doubleLiteralExpression > qi::lit(";"))[phoenix::bind(doubleConstants_.add, qi::_1, qi::_2), phoenix::bind(allConstantNames_.add, qi::_1, qi::_1), qi::_val = qi::_2];
		definedDoubleConstantDefinition.name("defined double constant declaration");
		undefinedBooleanConstantDefinition = (qi::lit("const") >> qi::lit("bool") > freeIdentifierName > qi::lit(";"))[qi::_a = phoenix::construct<std::shared_ptr<storm::ir::expressions::BooleanConstantExpression>>(phoenix::new_<storm::ir::expressions::BooleanConstantExpression>(qi::_1)), phoenix::insert(qi::_r1, phoenix::construct<std::pair<std::string, std::shared_ptr<storm::ir::expressions::BooleanConstantExpression>>>(qi::_1, qi::_a)), phoenix::bind(booleanConstants_.add, qi::_1, qi::_a), phoenix::bind(allConstantNames_.add, qi::_1, qi::_1)];
		undefinedBooleanConstantDefinition.name("undefined boolean constant declaration");
		undefinedIntegerConstantDefinition = (qi::lit("const") >> qi::lit("int") > freeIdentifierName > qi::lit(";"))[qi::_a = phoenix::construct<std::shared_ptr<storm::ir::expressions::IntegerConstantExpression>>(phoenix::new_<storm::ir::expressions::IntegerConstantExpression>(qi::_1)), phoenix::insert(qi::_r1, phoenix::construct<std::pair<std::string, std::shared_ptr<storm::ir::expressions::IntegerConstantExpression>>>(qi::_1, qi::_a)), phoenix::bind(integerConstants_.add, qi::_1, qi::_a), phoenix::bind(allConstantNames_.add, qi::_1, qi::_1)];
		undefinedIntegerConstantDefinition.name("undefined integer constant declaration");
		undefinedDoubleConstantDefinition = (qi::lit("const") >> qi::lit("double") > freeIdentifierName > qi::lit(";"))[qi::_a = phoenix::construct<std::shared_ptr<storm::ir::expressions::DoubleConstantExpression>>(phoenix::new_<storm::ir::expressions::DoubleConstantExpression>(qi::_1)), phoenix::insert(qi::_r1, phoenix::construct<std::pair<std::string, std::shared_ptr<storm::ir::expressions::DoubleConstantExpression>>>(qi::_1, qi::_a)), phoenix::bind(doubleConstants_.add, qi::_1, qi::_a), phoenix::bind(allConstantNames_.add, qi::_1, qi::_1)];
		undefinedDoubleConstantDefinition.name("undefined double constant declaration");
		definedConstantDefinition %= (definedBooleanConstantDefinition | definedIntegerConstantDefinition | definedDoubleConstantDefinition);
		definedConstantDefinition.name("defined constant declaration");
		undefinedConstantDefinition = (undefinedBooleanConstantDefinition(qi::_r1) | undefinedIntegerConstantDefinition(qi::_r2) | undefinedDoubleConstantDefinition(qi::_r3));
		undefinedConstantDefinition.name("undefined constant declaration");
		constantDefinitionList = *(definedConstantDefinition | undefinedConstantDefinition(qi::_r1, qi::_r2, qi::_r3));
		constantDefinitionList.name("constant declaration list");

		// This block defines all entities that are needed for parsing a program.
		modelTypeDefinition = modelType_;
		modelTypeDefinition.name("model type");
		start = (modelTypeDefinition > constantDefinitionList(qi::_a, qi::_b, qi::_c) > moduleDefinitionList > rewardDefinitionList(qi::_d) > labelDefinitionList(qi::_e))[qi::_val = phoenix::construct<storm::ir::Program>(qi::_1, qi::_a, qi::_b, qi::_c, qi::_2, qi::_d, qi::_e)];
		start.name("probabilistic program declaration");
	}

	// The starting point of the grammar.
	qi::rule<Iterator, storm::ir::Program(), qi::locals<std::map<std::string, std::shared_ptr<storm::ir::expressions::BooleanConstantExpression>>, std::map<std::string, std::shared_ptr<storm::ir::expressions::IntegerConstantExpression>>, std::map<std::string, std::shared_ptr<storm::ir::expressions::DoubleConstantExpression>>, std::map<std::string, storm::ir::RewardModel>, std::map<std::string, std::shared_ptr<storm::ir::expressions::BaseExpression>>>, Skipper> start;
	qi::rule<Iterator, storm::ir::Program::ModelType(), Skipper> modelTypeDefinition;
	qi::rule<Iterator, qi::unused_type(std::map<std::string, std::shared_ptr<storm::ir::expressions::BooleanConstantExpression>>&, std::map<std::string, std::shared_ptr<storm::ir::expressions::IntegerConstantExpression>>&, std::map<std::string, std::shared_ptr<storm::ir::expressions::DoubleConstantExpression>>&), Skipper> constantDefinitionList;
	qi::rule<Iterator, std::vector<storm::ir::Module>(), Skipper> moduleDefinitionList;

	// Rules for module definition.
	qi::rule<Iterator, storm::ir::Module(), qi::locals<std::vector<storm::ir::BooleanVariable>, std::vector<storm::ir::IntegerVariable>, std::map<std::string, uint_fast64_t>, std::map<std::string, uint_fast64_t>>, Skipper> moduleDefinition;

	// Rules for variable definitions.
	qi::rule<Iterator, qi::unused_type(std::vector<storm::ir::BooleanVariable>&, std::vector<storm::ir::IntegerVariable>&, std::map<std::string, uint_fast64_t>&, std::map<std::string, uint_fast64_t>&), Skipper> variableDefinition;
	qi::rule<Iterator, qi::unused_type(std::vector<storm::ir::BooleanVariable>&, std::map<std::string, uint_fast64_t>&), qi::locals<std::shared_ptr<storm::ir::expressions::VariableExpression>, std::shared_ptr<storm::ir::expressions::BaseExpression>>, Skipper> booleanVariableDefinition;
	qi::rule<Iterator, qi::unused_type(std::vector<storm::ir::IntegerVariable>&, std::map<std::string, uint_fast64_t>&), qi::locals<std::shared_ptr<storm::ir::expressions::VariableExpression>, std::shared_ptr<storm::ir::expressions::BaseExpression>>, Skipper> integerVariableDefinition;

	// Rules for command definitions.
	qi::rule<Iterator, storm::ir::Command(), qi::locals<std::string>, Skipper> commandDefinition;
	qi::rule<Iterator, std::vector<storm::ir::Update>(), Skipper> updateListDefinition;
	qi::rule<Iterator, storm::ir::Update(), qi::locals<std::map<std::string, storm::ir::Assignment>, std::map<std::string, storm::ir::Assignment>>, Skipper> updateDefinition;
	qi::rule<Iterator, qi::unused_type(std::map<std::string, storm::ir::Assignment>&, std::map<std::string, storm::ir::Assignment>&), Skipper> assignmentDefinitionList;
	qi::rule<Iterator, qi::unused_type(std::map<std::string, storm::ir::Assignment>&, std::map<std::string, storm::ir::Assignment>&), Skipper> assignmentDefinition;

	// Rules for variable/command names.
	qi::rule<Iterator, std::string(), Skipper> integerVariableName;
	qi::rule<Iterator, std::string(), Skipper> booleanVariableName;
	qi::rule<Iterator, std::string(), Skipper> commandName;
	qi::rule<Iterator, std::string(), Skipper> unassignedLocalBooleanVariableName;
	qi::rule<Iterator, std::string(), Skipper> unassignedLocalIntegerVariableName;

	// Rules for reward definitions.
	qi::rule<Iterator, qi::unused_type(std::map<std::string, storm::ir::RewardModel>&), Skipper> rewardDefinitionList;
	qi::rule<Iterator, qi::unused_type(std::map<std::string, storm::ir::RewardModel>&), qi::locals<std::vector<storm::ir::StateReward>, std::vector<storm::ir::TransitionReward>>, Skipper> rewardDefinition;
	qi::rule<Iterator, storm::ir::StateReward(), Skipper> stateRewardDefinition;
	qi::rule<Iterator, storm::ir::TransitionReward(), qi::locals<std::string>, Skipper> transitionRewardDefinition;

	// Rules for label definitions.
	qi::rule<Iterator, qi::unused_type(std::map<std::string, std::shared_ptr<storm::ir::expressions::BaseExpression>>&), Skipper> labelDefinitionList;
	qi::rule<Iterator, qi::unused_type(std::map<std::string, std::shared_ptr<storm::ir::expressions::BaseExpression>>&), Skipper> labelDefinition;

	// Rules for constant definitions.
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> constantDefinition;
	qi::rule<Iterator, qi::unused_type(std::map<std::string, std::shared_ptr<storm::ir::expressions::BooleanConstantExpression>>&, std::map<std::string, std::shared_ptr<storm::ir::expressions::IntegerConstantExpression>>&, std::map<std::string, std::shared_ptr<storm::ir::expressions::DoubleConstantExpression>>&), Skipper> undefinedConstantDefinition;
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> definedConstantDefinition;
	qi::rule<Iterator, qi::unused_type(std::map<std::string, std::shared_ptr<storm::ir::expressions::BooleanConstantExpression>>&), qi::locals<std::shared_ptr<storm::ir::expressions::BooleanConstantExpression>>, Skipper> undefinedBooleanConstantDefinition;
	qi::rule<Iterator, qi::unused_type(std::map<std::string, std::shared_ptr<storm::ir::expressions::IntegerConstantExpression>>&), qi::locals<std::shared_ptr<storm::ir::expressions::IntegerConstantExpression>>, Skipper> undefinedIntegerConstantDefinition;
	qi::rule<Iterator, qi::unused_type(std::map<std::string, std::shared_ptr<storm::ir::expressions::DoubleConstantExpression>>&), qi::locals<std::shared_ptr<storm::ir::expressions::DoubleConstantExpression>>, Skipper> undefinedDoubleConstantDefinition;
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> definedBooleanConstantDefinition;
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> definedIntegerConstantDefinition;
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> definedDoubleConstantDefinition;

	qi::rule<Iterator, std::string(), Skipper> freeIdentifierName;
	qi::rule<Iterator, std::string(), Skipper> identifierName;

	// The starting point for arbitrary expressions.
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> expression;

	// Rules with boolean result type.
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> booleanExpression;
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> orExpression;
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> andExpression;
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> notExpression;
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> atomicBooleanExpression;
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> relativeExpression;
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> constantBooleanExpression;
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> constantOrExpression;
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> constantAndExpression;
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> constantNotExpression;
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> constantAtomicBooleanExpression;
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> constantRelativeExpression;

	// Rules with integer result type.
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> integerExpression;
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> integerPlusExpression;
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> integerMultExpression;
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> atomicIntegerExpression;
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> constantIntegerExpression;
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> constantIntegerPlusExpression;
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> constantIntegerMultExpression;
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> constantAtomicIntegerExpression;

	// Rules with double result type.
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> constantDoubleExpression;
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> constantDoublePlusExpression;
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> constantDoubleMultExpression;
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> constantAtomicDoubleExpression;

	// Rules for variable recognition.
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> variableExpression;
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> booleanVariableExpression;
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> integerVariableExpression;

	// Rules for constant recognition.
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> constantExpression;
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> booleanConstantExpression;
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> integerConstantExpression;
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> doubleConstantExpression;

	// Rules for literal recognition.
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> literalExpression;
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> booleanLiteralExpression;
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> integerLiteralExpression;
	qi::rule<Iterator, std::shared_ptr<storm::ir::expressions::BaseExpression>(), Skipper> doubleLiteralExpression;

	// A structure defining the keywords that are not allowed to be chosen as identifiers.
	struct keywordsStruct : qi::symbols<char, unsigned> {
		keywordsStruct() {
			add
				("dtmc", 1)
				("ctmc", 2)
				("mdp", 3)
				("ctmdp", 4)
				("const", 5)
				("int", 6)
				("bool", 7)
				("module", 8)
				("endmodule", 9)
				("rewards", 10)
				("endrewards", 11)
				("true", 12)
				("false", 13)
			;
		}
	} keywords_;

	// A structure mapping the textual representation of a model type to the model type
	// representation of the intermediate representation.
	struct modelTypeStruct : qi::symbols<char, storm::ir::Program::ModelType> {
		modelTypeStruct() {
			add
				("dtmc", storm::ir::Program::ModelType::DTMC)
				("ctmc", storm::ir::Program::ModelType::CTMC)
				("mdp", storm::ir::Program::ModelType::MDP)
				("ctmdp", storm::ir::Program::ModelType::CTMDP)
			;
		}
	} modelType_;

	// A structure mapping the textual representation of a binary relation to the representation
	// of the intermediate representation.
	struct relationalOperatorStruct : qi::symbols<char, storm::ir::expressions::BinaryRelationExpression::RelationType> {
		relationalOperatorStruct() {
			add
				("=", storm::ir::expressions::BinaryRelationExpression::EQUAL)
				("<", storm::ir::expressions::BinaryRelationExpression::LESS)
				("<=", storm::ir::expressions::BinaryRelationExpression::LESS_OR_EQUAL)
				(">", storm::ir::expressions::BinaryRelationExpression::GREATER)
				(">=", storm::ir::expressions::BinaryRelationExpression::GREATER_OR_EQUAL)
			;
		}
	} relations_;

	// Used for indexing the variables.
	uint_fast64_t nextBooleanVariableIndex;
	uint_fast64_t nextIntegerVariableIndex;

	// Structures mapping variable and constant names to the corresponding expression nodes of
	// the intermediate representation.
	struct qi::symbols<char, std::shared_ptr<storm::ir::expressions::BaseExpression>> integerVariables_, booleanVariables_;
	struct qi::symbols<char, std::shared_ptr<storm::ir::expressions::BaseExpression>> integerConstants_, booleanConstants_, doubleConstants_;

	// A structure representing the identity function over identifier names.
	struct variableNamesStruct : qi::symbols<char, std::string> { } integerVariableNames_, booleanVariableNames_, commandNames_, labelNames_, allConstantNames_, moduleNames_,
		localBooleanVariables_, localIntegerVariables_, assignedLocalBooleanVariables_, assignedLocalIntegerVariables_;
};

/*!
 * Opens the given file for parsing, invokes the helper function to parse the actual content and
 * closes the file properly, even if an exception is thrown in the parser. In this case, the
 * exception is passed on to the caller.
 */
std::shared_ptr<storm::ir::Program> PrismParser::parseFile(std::string const& filename) const {
	// Open file and initialize result.
	std::ifstream inputFileStream(filename, std::ios::in);
	std::shared_ptr<storm::ir::Program> result(nullptr);

	// Now try to parse the contents of the file.
	try {
		result = std::shared_ptr<storm::ir::Program>(parse(inputFileStream, filename));
	} catch(std::exception& e) {
		// In case of an exception properly close the file before passing exception.
		inputFileStream.close();
		throw e;
	}

	// Close the stream in case everything went smoothly and return result.
	inputFileStream.close();
	return result;
}

/*!
 * Passes iterators to the input stream to the Boost spirit parser and thereby parses the input.
 * If the parser throws an expectation failure exception, i.e. expected input different than the one
 * provided, this is caught and displayed properly before the exception is passed on.
 */
std::shared_ptr<storm::ir::Program> PrismParser::parse(std::istream& inputStream, std::string const& filename) const {
	// Prepare iterators to input.
	base_iterator_type in_begin(inputStream);
	forward_iterator_type fwd_begin = boost::spirit::make_default_multi_pass(in_begin);
	forward_iterator_type fwd_end;
	pos_iterator_type position_begin(fwd_begin, fwd_end, filename);
	pos_iterator_type position_end;

	// Prepare resulting intermediate representation of input.
	std::shared_ptr<storm::ir::Program> result(new storm::ir::Program());

	// In order to instantiate the grammar, we have to pass the type of the skipping parser.
	// As this is more complex, we let Boost figure out the actual type for us.
	PrismGrammar<pos_iterator_type,  BOOST_TYPEOF(boost::spirit::ascii::space | qi::lit("//") >> *(qi::char_ - qi::eol) >> qi::eol)> grammar;
	try {
		// Now parse the content using phrase_parse in order to be able to supply a skipping
		// parser.
		qi::phrase_parse(position_begin, position_end, grammar, boost::spirit::ascii::space | qi::lit("//") >> *(qi::char_ - qi::eol) >> qi::eol, *result);
	} catch(const qi::expectation_failure<pos_iterator_type>& e) {
		// If the parser expected content different than the one provided, display information
		// about the location of the error.
		const boost::spirit::classic::file_position_base<std::string>& pos = e.first.get_position();

		// Construct the error message including a caret display of the position in the
		// erroneous line.
		std::stringstream msg;
		msg << pos.file << ", line " << pos.line << ", column " << pos.column
				<< ": parse error: expected " << e.what_ << std::endl << "\t"
				<< e.first.get_currentline() << std::endl << "\t";
		int i = 0;
		for (i = 0; i < pos.column; ++i) {
			msg << "-";
		}
		msg << "^";
		for (; i < 80; ++i) {
			msg << "-";
		}
		msg << std::endl;

		std::cerr << msg.str();

		// Now propagate exception.
		throw storm::exceptions::WrongFileFormatException() << msg.str();
	}

	return result;
}

} // namespace parser

} // namespace storm
