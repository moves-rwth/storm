#ifndef STORM_PARSER_PRISMPARSER_H_
#define	STORM_PARSER_PRISMPARSER_H_

// Include files for file input.
#include <fstream>
#include <memory>
#include <iomanip>

// Include boost spirit.
#define BOOST_SPIRIT_USE_PHOENIX_V3
#include <boost/typeof/typeof.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/support_line_pos_iterator.hpp>
#include <boost/spirit/home/classic/iterator/position_iterator.hpp>

namespace qi = boost::spirit::qi;
namespace phoenix = boost::phoenix;

typedef std::string::const_iterator BaseIteratorType;
typedef boost::spirit::line_pos_iterator<BaseIteratorType> PositionIteratorType;
typedef PositionIteratorType Iterator;
typedef BOOST_TYPEOF(boost::spirit::ascii::space | qi::lit("//") >> *(qi::char_ - qi::eol) >> qi::eol) Skipper;
typedef BOOST_TYPEOF(qi::lit("//") >> *(qi::char_ - qi::eol) >> qi::eol | boost::spirit::ascii::space) Skipper2;

#include "src/storage/prism/Program.h"
#include "src/storage/expressions/Expression.h"
#include "src/storage/expressions/Expressions.h"
#include "src/exceptions/ExceptionMacros.h"
#include "src/exceptions/WrongFormatException.h"

namespace storm {
    namespace parser {
        // A class that stores information about the parsed program.
        class GlobalProgramInformation {
        public:
            // Default construct the header information.
			GlobalProgramInformation() : hasInitialStatesExpression(false), currentCommandIndex(0), currentUpdateIndex(0) {}
            
            // Members for all essential information that needs to be collected.
            storm::prism::Program::ModelType modelType;
            std::vector<storm::prism::Constant> constants;
            std::vector<storm::prism::Formula> formulas;
            std::vector<storm::prism::BooleanVariable> globalBooleanVariables;
            std::vector<storm::prism::IntegerVariable> globalIntegerVariables;
            std::map<std::string, uint_fast64_t> moduleToIndexMap;
            std::vector<storm::prism::Module> modules;
            std::vector<storm::prism::RewardModel> rewardModels;
            std::vector<storm::prism::Label> labels;
            storm::expressions::Expression initialStatesExpression;
            bool hasInitialStatesExpression;
            
            // Counters to provide unique indexing for commands and updates.
            uint_fast64_t currentCommandIndex;
            uint_fast64_t currentUpdateIndex;
        };

        class PrismParser : public qi::grammar<Iterator, storm::prism::Program(), qi::locals<GlobalProgramInformation>, Skipper> {
        public:
            /*!
             * Parses the given file into the PRISM storage classes assuming it complies with the PRISM syntax.
             *
             * @param filename the name of the file to parse.
             * @param typeCheck Sets whether the expressions are generated and therefore typechecked.
             * @return The resulting PRISM program.
             */
            static storm::prism::Program parse(std::string const& filename, bool typeCheck = true);
            
            /*!
             * Parses the given input stream into the PRISM storage classes assuming it complies with the PRISM syntax.
             *
             * @param input The input string to parse.
             * @param filename The name of the file from which the input was read.
             * @param typeCheck Sets whether the expressions are generated and therefore typechecked.
             * @return The resulting PRISM program.
             */
            static storm::prism::Program parseFromString(std::string const& input, std::string const& filename, bool typeCheck = true);
            
        private:
            struct modelTypeStruct : qi::symbols<char, storm::prism::Program::ModelType> {
                modelTypeStruct() {
                    add
                    ("dtmc", storm::prism::Program::ModelType::DTMC)
                    ("ctmc", storm::prism::Program::ModelType::CTMC)
                    ("mdp", storm::prism::Program::ModelType::MDP)
                    ("ctmdp", storm::prism::Program::ModelType::CTMDP)
                    ("ma", storm::prism::Program::ModelType::MA);
                }
            };
            
            struct keywordsStruct : qi::symbols<char, uint_fast64_t> {
                keywordsStruct() {
                    add
                    ("dtmc", 1)
                    ("ctmc", 2)
                    ("mdp", 3)
                    ("ctmdp", 4)
                    ("ma", 5)
                    ("const", 6)
                    ("int", 7)
                    ("bool", 8)
                    ("module", 9)
                    ("endmodule", 10)
                    ("rewards", 11)
                    ("endrewards", 12)
                    ("true", 13)
                    ("min", 14)
                    ("max", 15)
                    ("floor", 16)
                    ("ceil", 17)
                    ("init", 18)
                    ("endinit", 19);
                }
            };
            
            // Functor used for displaying error information.
            struct ErrorHandler {
                typedef qi::error_handler_result result_type;
                
                template<typename T1, typename T2, typename T3, typename T4>
                qi::error_handler_result operator()(T1 b, T2 e, T3 where, T4 const& what) const {
                    LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << get_line(where) << ": " << " expecting " << what << ".");
                    return qi::fail;
                }
            };
            
            // Functor used for annotating entities with line number information.
            class PositionAnnotation {
            public:
                typedef void result_type;
                
                PositionAnnotation(Iterator first) : first(first) {
                    // Intentionally left empty.
                }
                
                template<typename Entity, typename First, typename Last>
                result_type operator()(Entity& entity, First f, Last l) const {
                    entity.setLineNumber(get_line(f));
                }
            private:
                std::string filename;
                Iterator const first;
            };
            
            /*!
             * Creates a grammar for the given filename and the iterator to the first input to parse.
             *
             * @param filename The filename that is to be read. This is used for proper error reporting.
             * @param first The iterator to the beginning of the input.
             */
            PrismParser(std::string const& filename, Iterator first);
            
            /*!
             * Sets an internal flag that indicates the second run is now taking place.
             */
            void moveToSecondRun();
            
            // A flag that stores whether the grammar is currently doing the second run.
            bool secondRun;
            
            /*!
             * Sets whether doubles literals are allowed in the parsed expression.
             *
             * @param flag Indicates whether to allow or forbid double literals in the parsed expression.
             */
            void allowDoubleLiterals(bool flag);
            
            // A flag that stores wether to allow or forbid double literals in parsed expressions.
            bool allowDoubleLiteralsFlag;
            
            // The name of the file being parsed.
            std::string filename;
            
            /*!
             * Retrieves the name of the file currently being parsed.
             *
             * @return The name of the file currently being parsed.
             */
            std::string const& getFilename() const;
            
            // A function used for annotating the entities with their position.
            phoenix::function<ErrorHandler> handler;
            phoenix::function<PositionAnnotation> annotate;
            
            // The starting point of the grammar.
            qi::rule<Iterator, storm::prism::Program(), qi::locals<GlobalProgramInformation>, Skipper> start;
            
            // Rules for model type.
            qi::rule<Iterator, storm::prism::Program::ModelType(), Skipper> modelTypeDefinition;
            
            // Rules for parsing the program header.
            qi::rule<Iterator, qi::unused_type(GlobalProgramInformation&), Skipper> programHeader;
            qi::rule<Iterator, storm::prism::Constant(), Skipper> undefinedConstantDefinition;
            qi::rule<Iterator, storm::prism::Constant(), Skipper> undefinedBooleanConstantDefinition;
            qi::rule<Iterator, storm::prism::Constant(), Skipper> undefinedIntegerConstantDefinition;
            qi::rule<Iterator, storm::prism::Constant(), Skipper> undefinedDoubleConstantDefinition;
            qi::rule<Iterator, storm::prism::Constant(), Skipper> definedConstantDefinition;
            qi::rule<Iterator, storm::prism::Constant(), Skipper> definedBooleanConstantDefinition;
            qi::rule<Iterator, storm::prism::Constant(), Skipper> definedIntegerConstantDefinition;
            qi::rule<Iterator, storm::prism::Constant(), Skipper> definedDoubleConstantDefinition;
            
            // Rules for global variable definitions.
            qi::rule<Iterator, qi::unused_type(GlobalProgramInformation&), Skipper> globalVariableDefinition;
            qi::rule<Iterator, qi::unused_type(GlobalProgramInformation&), Skipper> globalBooleanVariableDefinition;
            qi::rule<Iterator, qi::unused_type(GlobalProgramInformation&), Skipper> globalIntegerVariableDefinition;
            
            // Rules for modules definition.
            qi::rule<Iterator, std::vector<storm::prism::Module>(GlobalProgramInformation&), Skipper> moduleDefinitionList;
            qi::rule<Iterator, storm::prism::Module(GlobalProgramInformation&), qi::locals<std::vector<storm::prism::BooleanVariable>, std::vector<storm::prism::IntegerVariable>>, Skipper> moduleDefinition;
            qi::rule<Iterator, storm::prism::Module(GlobalProgramInformation&), qi::locals<std::map<std::string, std::string>>, Skipper> moduleRenaming;
            
            // Rules for variable definitions.
            qi::rule<Iterator, qi::unused_type(std::vector<storm::prism::BooleanVariable>&, std::vector<storm::prism::IntegerVariable>&), Skipper> variableDefinition;
            qi::rule<Iterator, storm::prism::BooleanVariable(), Skipper> booleanVariableDefinition;
            qi::rule<Iterator, storm::prism::IntegerVariable(), qi::locals<storm::expressions::Expression>, Skipper> integerVariableDefinition;
            
            // Rules for command definitions.
            qi::rule<Iterator, storm::prism::Command(GlobalProgramInformation&), qi::locals<std::string>, Skipper> commandDefinition;
            qi::rule<Iterator, std::vector<storm::prism::Update>(GlobalProgramInformation&), Skipper> updateListDefinition;
            qi::rule<Iterator, storm::prism::Update(GlobalProgramInformation&), Skipper> updateDefinition;
            qi::rule<Iterator, std::vector<storm::prism::Assignment>(), Skipper> assignmentDefinitionList;
            qi::rule<Iterator, storm::prism::Assignment(), Skipper> assignmentDefinition;
            
            // Rules for reward definitions.
            qi::rule<Iterator, storm::prism::RewardModel(), qi::locals<std::string, std::vector<storm::prism::StateReward>, std::vector<storm::prism::TransitionReward>>, Skipper> rewardModelDefinition;
            qi::rule<Iterator, storm::prism::StateReward(), Skipper> stateRewardDefinition;
            qi::rule<Iterator, storm::prism::TransitionReward(), qi::locals<std::string>, Skipper> transitionRewardDefinition;
            
            // Rules for initial states expression.
            qi::rule<Iterator, qi::unused_type(GlobalProgramInformation&), Skipper> initialStatesConstruct;
            
            // Rules for label definitions.
            qi::rule<Iterator, storm::prism::Label(), Skipper> labelDefinition;
            
            // Rules for formula definitions.
            qi::rule<Iterator, storm::prism::Formula(), Skipper> formulaDefinition;
            
            // Rules for identifier parsing.
            qi::rule<Iterator, std::string(), Skipper> identifier;
            
            // Rules for parsing a composed expression.
            qi::rule<Iterator, storm::expressions::Expression(), Skipper> expression;
            qi::rule<Iterator, storm::expressions::Expression(), Skipper> iteExpression;
            qi::rule<Iterator, storm::expressions::Expression(), qi::locals<bool>, Skipper> orExpression;
            qi::rule<Iterator, storm::expressions::Expression(), Skipper> andExpression;
            qi::rule<Iterator, storm::expressions::Expression(), Skipper> relativeExpression;
            qi::rule<Iterator, storm::expressions::Expression(), qi::locals<bool>, Skipper> equalityExpression;
            qi::rule<Iterator, storm::expressions::Expression(), qi::locals<bool>, Skipper> plusExpression;
            qi::rule<Iterator, storm::expressions::Expression(), qi::locals<bool>, Skipper> multiplicationExpression;
            qi::rule<Iterator, storm::expressions::Expression(), Skipper> unaryExpression;
            qi::rule<Iterator, storm::expressions::Expression(), Skipper> atomicExpression;
            qi::rule<Iterator, storm::expressions::Expression(), Skipper> literalExpression;
            qi::rule<Iterator, storm::expressions::Expression(), Skipper> identifierExpression;
            qi::rule<Iterator, storm::expressions::Expression(), qi::locals<bool>, Skipper> minMaxExpression;
            qi::rule<Iterator, storm::expressions::Expression(), qi::locals<bool>, Skipper> floorCeilExpression;
            
            // Parser that is used to recognize doubles only (as opposed to Spirit's double_ parser).
            boost::spirit::qi::real_parser<double, boost::spirit::qi::strict_real_policies<double>> strict_double;
            
            // Parsers that recognize special keywords and model types.
            storm::parser::PrismParser::keywordsStruct keywords_;
            storm::parser::PrismParser::modelTypeStruct modelType_;
            qi::symbols<char, storm::expressions::Expression> identifiers_;
            
            // Helper methods used in the grammar.
            bool isValidIdentifier(std::string const& identifier);
            bool addInitialStatesExpression(storm::expressions::Expression initialStatesExpression, GlobalProgramInformation& globalProgramInformation);
            
            storm::expressions::Expression createIteExpression(storm::expressions::Expression e1, storm::expressions::Expression e2, storm::expressions::Expression e3) const;
            storm::expressions::Expression createImpliesExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const;
            storm::expressions::Expression createOrExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const;
            storm::expressions::Expression createAndExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const;
            storm::expressions::Expression createGreaterExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const;
            storm::expressions::Expression createGreaterOrEqualExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const;
            storm::expressions::Expression createLessExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const;
            storm::expressions::Expression createLessOrEqualExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const;
            storm::expressions::Expression createEqualsExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const;
            storm::expressions::Expression createNotEqualsExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const;
            storm::expressions::Expression createPlusExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const;
            storm::expressions::Expression createMinusExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const;
            storm::expressions::Expression createMultExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const;
            storm::expressions::Expression createDivExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const;
            storm::expressions::Expression createNotExpression(storm::expressions::Expression e1) const;
            storm::expressions::Expression createMinusExpression(storm::expressions::Expression e1) const;
            storm::expressions::Expression createTrueExpression() const;
            storm::expressions::Expression createFalseExpression() const;
            storm::expressions::Expression createDoubleLiteralExpression(double value, bool& pass) const;
            storm::expressions::Expression createIntegerLiteralExpression(int value) const;
            storm::expressions::Expression createMinimumExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const;
            storm::expressions::Expression createMaximumExpression(storm::expressions::Expression e1, storm::expressions::Expression e2) const;
            storm::expressions::Expression createFloorExpression(storm::expressions::Expression e1) const;
            storm::expressions::Expression createCeilExpression(storm::expressions::Expression e1) const;
            storm::expressions::Expression getIdentifierExpression(std::string const& identifier) const;
            
            storm::prism::Constant createUndefinedBooleanConstant(std::string const& newConstant) const;
            storm::prism::Constant createUndefinedIntegerConstant(std::string const& newConstant) const;
            storm::prism::Constant createUndefinedDoubleConstant(std::string const& newConstant) const;
            storm::prism::Constant createDefinedBooleanConstant(std::string const& newConstant, storm::expressions::Expression expression) const;
            storm::prism::Constant createDefinedIntegerConstant(std::string const& newConstant, storm::expressions::Expression expression) const;
            storm::prism::Constant createDefinedDoubleConstant(std::string const& newConstant, storm::expressions::Expression expression) const;
            storm::prism::Formula createFormula(std::string const& formulaName, storm::expressions::Expression expression) const;
            storm::prism::Label createLabel(std::string const& labelName, storm::expressions::Expression expression) const;
            storm::prism::RewardModel createRewardModel(std::string const& rewardModelName, std::vector<storm::prism::StateReward> const& stateRewards, std::vector<storm::prism::TransitionReward> const& transitionRewards) const;
            storm::prism::StateReward createStateReward(storm::expressions::Expression statePredicateExpression, storm::expressions::Expression rewardValueExpression) const;
            storm::prism::TransitionReward createTransitionReward(std::string const& actionName, storm::expressions::Expression statePredicateExpression, storm::expressions::Expression rewardValueExpression) const;
            storm::prism::Assignment createAssignment(std::string const& variableName, storm::expressions::Expression assignedExpression) const;
            storm::prism::Update createUpdate(storm::expressions::Expression likelihoodExpression, std::vector<storm::prism::Assignment> const& assignments, GlobalProgramInformation& globalProgramInformation) const;
            storm::prism::Command createCommand(std::string const& actionName, storm::expressions::Expression guardExpression, std::vector<storm::prism::Update> const& updates, GlobalProgramInformation& globalProgramInformation) const;
            storm::prism::BooleanVariable createBooleanVariable(std::string const& variableName, storm::expressions::Expression initialValueExpression) const;
            storm::prism::IntegerVariable createIntegerVariable(std::string const& variableName, storm::expressions::Expression lowerBoundExpression, storm::expressions::Expression upperBoundExpression, storm::expressions::Expression initialValueExpression) const;
            storm::prism::Module createModule(std::string const& moduleName, std::vector<storm::prism::BooleanVariable> const& booleanVariables, std::vector<storm::prism::IntegerVariable> const& integerVariables, std::vector<storm::prism::Command> const& commands, GlobalProgramInformation& globalProgramInformation) const;
            storm::prism::Module createRenamedModule(std::string const& newModuleName, std::string const& oldModuleName, std::map<std::string, std::string> const& renaming, GlobalProgramInformation& globalProgramInformation) const;
            storm::prism::Program createProgram(GlobalProgramInformation const& globalProgramInformation) const;
        };
    } // namespace parser
} // namespace storm

#endif	/* STORM_PARSER_PRISMPARSER_H_ */

