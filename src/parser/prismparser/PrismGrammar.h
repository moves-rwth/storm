#ifndef STORM_PARSER_PRISMPARSER_PRISMGRAMMAR_H_
#define	STORM_PARSER_PRISMPARSER_PRISMGRAMMAR_H_

// Include files for file input.
#include <istream>
#include <memory>

// Include boost spirit.
#include <boost/typeof/typeof.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

// Include headers for spirit iterators. Needed for diagnostics and input stream iteration.
#include <boost/spirit/include/classic_position_iterator.hpp>
#include <boost/spirit/include/support_multi_pass.hpp>

namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;

typedef std::string::const_iterator BaseIteratorType;
typedef boost::spirit::classic::position_iterator2<BaseIteratorType> PositionIteratorType;
typedef PositionIteratorType Iterator;
typedef BOOST_TYPEOF(boost::spirit::ascii::space | qi::lit("//") >> *(qi::char_ - qi::eol) >> qi::eol) Skipper;
typedef BOOST_TYPEOF(qi::lit("//") >> *(qi::char_ - qi::eol) >> qi::eol | boost::spirit::ascii::space) Skipper2;
typedef boost::spirit::unused_type Unused;

#include "src/parser/prismparser/Tokens.h"

#include "src/storage/prism/Program.h"
#include "src/storage/expressions/Expression.h"
using namespace storm::prism;
using namespace storm::expressions;

namespace storm {
    namespace parser {
        namespace prism {
            class PrismGrammar : public qi::grammar<Iterator, Program(), qi::locals<std::set<std::string>, std::set<std::string>, std::set<std::string>, std::map<std::string, storm::expressions::Expression>, std::map<std::string, storm::expressions::Expression>, std::map<std::string, storm::expressions::Expression>, std::map<std::string, Module>>, Skipper> {
            public:
                /*!
                 * Default constructor that creates an empty and functional grammar.
                 */
                PrismGrammar();
                
            private:
                // The starting point of the grammar.
                // The locals are used for: (a) undefined boolean constants, (b) undefined integer constants, (c) undefined double constants, (d) defined boolean constants, (e) defined integer constants, (f) defined double constants, (g) module name to module map
                qi::rule<Iterator, Program(), qi::locals<std::set<std::string>, std::set<std::string>, std::set<std::string>, std::map<std::string, storm::expressions::Expression>, std::map<std::string, storm::expressions::Expression>, std::map<std::string, storm::expressions::Expression>, std::map<std::string, Module>>, Skipper> start;
                
                // Rules for model type.
                qi::rule<Iterator, Program::ModelType(), Skipper> modelTypeDefinition;
                
                // Rules for constant definitions.
                qi::rule<Iterator, qi::unused_type(std::set<std::string>&, std::set<std::string>&, std::set<std::string>&, std::map<std::string, storm::expressions::Expression>&, std::map<std::string, storm::expressions::Expression>&, std::map<std::string, storm::expressions::Expression>&), Skipper> constantDefinitionList;
                qi::rule<Iterator, qi::unused_type(std::set<std::string>&, std::set<std::string>&, std::set<std::string>&), Skipper> undefinedConstantDefinition;
                qi::rule<Iterator, qi::unused_type(std::map<std::string, storm::expressions::Expression>&, std::map<std::string, storm::expressions::Expression>&, std::map<std::string, storm::expressions::Expression>&), Skipper> definedConstantDefinition;
                qi::rule<Iterator, qi::unused_type(std::set<std::string>&), Skipper> undefinedBooleanConstantDefinition;
                qi::rule<Iterator, qi::unused_type(std::set<std::string>&), Skipper> undefinedIntegerConstantDefinition;
                qi::rule<Iterator, qi::unused_type(std::set<std::string>&), Skipper> undefinedDoubleConstantDefinition;
                qi::rule<Iterator, qi::unused_type(std::map<std::string, storm::expressions::Expression>&), Skipper> definedBooleanConstantDefinition;
                qi::rule<Iterator, qi::unused_type(std::map<std::string, storm::expressions::Expression>&), Skipper> definedIntegerConstantDefinition;
                qi::rule<Iterator, qi::unused_type(std::map<std::string, storm::expressions::Expression>&), Skipper> definedDoubleConstantDefinition;
                
                // Rules for global variable definitions.
                qi::rule<Iterator, qi::unused_type(std::map<std::string, BooleanVariable>&, std::map<std::string, IntegerVariable>), Skipper> globalVariableDefinitionList;
                
                // Rules for modules definition.
                qi::rule<Iterator, std::vector<Module>(), Skipper> moduleDefinitionList;
                qi::rule<Iterator, Module(), qi::locals<std::map<std::string, BooleanVariable>, std::map<std::string, IntegerVariable>>, Skipper> moduleDefinition;
                qi::rule<Iterator, Module(std::map<std::string, Module>&), qi::locals<std::map<std::string, std::string>>, Skipper> moduleRenaming;
                
                // Rules for variable definitions.
                qi::rule<Iterator, qi::unused_type(std::map<std::string, BooleanVariable>&, std::map<std::string, IntegerVariable>&), Skipper> variableDefinition;
                qi::rule<Iterator, qi::unused_type(std::map<std::string, BooleanVariable>&), Skipper> booleanVariableDefinition;
                qi::rule<Iterator, qi::unused_type(std::map<std::string, IntegerVariable>&), Skipper> integerVariableDefinition;
                
                // Rules for command definitions.
                qi::rule<Iterator, Command(), qi::locals<std::string>, Skipper> commandDefinition;
                qi::rule<Iterator, std::vector<Update>(), Skipper> updateListDefinition;
                qi::rule<Iterator, Update(), qi::locals<std::map<std::string, Assignment>>, Skipper> updateDefinition;
                qi::rule<Iterator, qi::unused_type(std::map<std::string, Assignment>&), Skipper> assignmentDefinitionList;
                qi::rule<Iterator, Assignment(), Skipper> assignmentDefinition;
                
                // Rules for reward definitions.
                qi::rule<Iterator, std::map<std::string, RewardModel>(), Skipper> rewardDefinitionList;
                qi::rule<Iterator, qi::unused_type(std::map<std::string, RewardModel>&), qi::locals<std::vector<StateReward>, std::vector<TransitionReward>>, Skipper> rewardDefinition;
                qi::rule<Iterator, StateReward(), Skipper> stateRewardDefinition;
                qi::rule<Iterator, TransitionReward(), Skipper> transitionRewardDefinition;
                
                // Rules for label definitions.
                qi::rule<Iterator, std::map<std::string, storm::expressions::Expression>(), Skipper> labelDefinitionList;
                qi::rule<Iterator, qi::unused_type(std::map<std::string, storm::expressions::Expression>&), Skipper> labelDefinition;
                
                // Rules for formula definitions.
                qi::rule<Iterator, std::map<std::string, storm::expressions::Expression>(), Skipper> formulaDefinitionList;
                qi::rule<Iterator, qi::unused_type(std::map<std::string, storm::expressions::Expression>&), Skipper> formulaDefinition;
                
                // Rules for identifier parsing.
                qi::rule<Iterator, std::string(), Skipper> identifier;
                
                // Rules for parsing a composed expression.
                qi::rule<Iterator, storm::expressions::Expression(), Skipper> expression;
                qi::rule<Iterator, storm::expressions::Expression(), Skipper> booleanExpression;
                qi::rule<Iterator, storm::expressions::Expression(), Skipper> orExpression;
                qi::rule<Iterator, storm::expressions::Expression(), Skipper> andExpression;
                qi::rule<Iterator, storm::expressions::Expression(), Skipper> notExpression;
                qi::rule<Iterator, storm::expressions::Expression(), Skipper> atomicBooleanExpression;
                qi::rule<Iterator, storm::expressions::Expression(), Skipper> relativeExpression;
                qi::rule<Iterator, storm::expressions::Expression(), Skipper> booleanVariableExpression;
                qi::rule<Iterator, storm::expressions::Expression(), Skipper> numericalExpression;
                qi::rule<Iterator, storm::expressions::Expression(), qi::locals<bool>, Skipper> plusExpression;
                qi::rule<Iterator, storm::expressions::Expression(), Skipper> multiplicationExpression;
                qi::rule<Iterator, storm::expressions::Expression(), Skipper> atomicNumericalExpression;
                qi::rule<Iterator, storm::expressions::Expression(), Skipper> numericalVariableExpression;
                qi::rule<Iterator, storm::expressions::Expression(), qi::locals<bool>, Skipper> minMaxExpression;
                qi::rule<Iterator, storm::expressions::Expression(), qi::locals<bool>, Skipper> floorCeilExpression;
                
                // Parsers that recognize special keywords and operations.
                storm::parser::prism::keywordsStruct keywords_;
                storm::parser::prism::modelTypeStruct modelType_;
                storm::parser::prism::BinaryRelationOperatorStruct relationOperator_;
                storm::parser::prism::BinaryBooleanOperatorStruct binaryBooleanOperator_;
                storm::parser::prism::UnaryBooleanOperatorStruct unaryBooleanOperator_;
                storm::parser::prism::BinaryNumericalOperatorStruct binaryNumericalOperator_;
                storm::parser::prism::UnaryNumericalOperatorStruct unaryNumericalOperator_;
                
                // Helper methods that add data to data structures.
                
            };
        } // namespace prism
    } // namespace parser
} // namespace storm


#endif	/* STORM_PARSER_PRISMPARSER_PRISMGRAMMAR_H_ */

