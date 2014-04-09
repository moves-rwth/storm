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
namespace prism = storm::prism;

typedef std::string::const_iterator BaseIteratorType;
typedef boost::spirit::classic::position_iterator2<BaseIteratorType> PositionIteratorType;
typedef PositionIteratorType Iterator;
typedef BOOST_TYPEOF(boost::spirit::ascii::space | qi::lit("//") >> *(qi::char_ - qi::eol) >> qi::eol) Skipper;
typedef BOOST_TYPEOF(qi::lit("//") >> *(qi::char_ - qi::eol) >> qi::eol | boost::spirit::ascii::space) Skipper2;
typedef boost::spirit::unused_type Unused;

#include "src/parser/prismparser/Tokens.h"

#include "src/storage/prism/Program.h"
#include "src/storage/expressions/Expression.h"
using namespace storm::expressions;

namespace storm {
    namespace parser {
        namespace prism {
            class GlobalProgramInformation {
            public:
                // Default construct the header information.
                GlobalProgramInformation() = default;
                
                // Members for all essential information that needs to be collected.
                prism::Program::ModelType modelType;
                std::set<std::string> undefinedBooleanConstants;
                std::set<std::string> undefinedIntegerConstants;
                std::set<std::string> undefinedDoubleConstants;
                std::map<std::string, storm::expressions::Expression> definedBooleanConstants;
                std::map<std::string, storm::expressions::Expression> definedIntegerConstants;
                std::map<std::string, storm::expressions::Expression> definedDoubleConstants;
                std::map<std::string, storm::expressions::Expression> formulas;
                std::map<std::string, BooleanVariable> globalBooleanVariables;
                std::map<std::string, IntegerVariable> globalIntegerVariables;
                std::map<std::string, Module> modules;
                std::map<std::string, RewardModel> rewardModels;
            };
            
            class PrismGrammar : public qi::grammar<Iterator, Program(), qi::locals<GlobalProgramInformation>, Skipper> {
            public:
                /*!
                 * Default constructor that creates an empty and functional grammar.
                 */
                PrismGrammar();
                
            private:
                // The starting point of the grammar.
                qi::rule<Iterator, Program(), qi::locals<GlobalProgramInformation>, Skipper> start;
                
                // Rules for model type.
                qi::rule<Iterator, Program::ModelType(), Skipper> modelTypeDefinition;
                
                // Rules for parsing the program header.
                qi::rule<Iterator, qi::unused_type(GlobalProgramInformation&), Skipper> programHeader;
                qi::rule<Iterator, qi::unused_type(GlobalProgramInformation&), Skipper> undefinedConstantDefinition;
                qi::rule<Iterator, qi::unused_type(GlobalProgramInformation&), Skipper> definedConstantDefinition;
                qi::rule<Iterator, qi::unused_type(GlobalProgramInformation&), Skipper> undefinedBooleanConstantDefinition;
                qi::rule<Iterator, qi::unused_type(GlobalProgramInformation&), Skipper> undefinedIntegerConstantDefinition;
                qi::rule<Iterator, qi::unused_type(GlobalProgramInformation&), Skipper> undefinedDoubleConstantDefinition;
                qi::rule<Iterator, qi::unused_type(GlobalProgramInformation&), Skipper> definedBooleanConstantDefinition;
                qi::rule<Iterator, qi::unused_type(GlobalProgramInformation&), Skipper> definedIntegerConstantDefinition;
                qi::rule<Iterator, qi::unused_type(GlobalProgramInformation&), Skipper> definedDoubleConstantDefinition;
                
                // Rules for global variable definitions.
                qi::rule<Iterator, qi::unused_type(ProgramHeaderInformation&), Skipper> globalVariableDefinition;
                
                // Rules for modules definition.
                qi::rule<Iterator, std::vector<Module>(), Skipper> moduleDefinitionList;
                qi::rule<Iterator, Module(), qi::locals<std::map<std::string, BooleanVariable>, std::map<std::string, IntegerVariable>>, Skipper> moduleDefinition;
                qi::rule<Iterator, Module(GlobalProgramInformation&>, Skipper> moduleRenaming;
                
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
                qi::rule<Iterator, qi::unused_type(GlobalProgramInformation&), qi::locals<std::vector<StateReward>, std::vector<TransitionReward>>, Skipper> rewardModelDefinition;
                qi::rule<Iterator, StateReward(), Skipper> stateRewardDefinition;
                qi::rule<Iterator, TransitionReward(), Skipper> transitionRewardDefinition;
                
                // Rules for label definitions.
                qi::rule<Iterator, qi::unused_type(GlobalProgramInformation&), Skipper> labelDefinition;
                
                // Rules for formula definitions.
                qi::rule<Iterator, qi::unused_type(GlobalProgramInformation&), Skipper> formulaDefinition;
                
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

