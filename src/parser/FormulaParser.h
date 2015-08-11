#ifndef STORM_PARSER_PRCTLPARSER_H_
#define STORM_PARSER_PRCTLPARSER_H_

#include <sstream>

#include "src/parser/SpiritParserDefinitions.h"
#include "src/parser/ExpressionParser.h"
#include "src/logic/Formulas.h"
#include "src/storage/expressions/Expression.h"
#include "src/utility/macros.h"

namespace storm {
    namespace parser {
        
        class FormulaParser : public qi::grammar<Iterator, std::shared_ptr<storm::logic::Formula>(), Skipper> {
        public:
            FormulaParser(std::shared_ptr<storm::expressions::ExpressionManager const> const& manager = std::shared_ptr<storm::expressions::ExpressionManager>(new storm::expressions::ExpressionManager()));

            /*!
             * Parses the formula given by the provided string.
             *
             * @param formulaString The formula as a string.
             * @return The resulting formula representation.
             */
            std::shared_ptr<storm::logic::Formula> parseFromString(std::string const& formulaString);
            
            /*!
             * Adds an identifier and the expression it is supposed to be replaced with. This can, for example be used
             * to substitute special identifiers in the formula by expressions.
             *
             * @param identifier The identifier that is supposed to be substituted.
             * @param expression The expression it is to be substituted with.
             */
            void addIdentifierExpression(std::string const& identifier, storm::expressions::Expression const& expression);
            
        private:
            struct keywordsStruct : qi::symbols<char, uint_fast64_t> {
                keywordsStruct() {
                    add
                    ("true", 1)
                    ("false", 2)
                    ("min", 3)
                    ("max", 4)
                    ("F", 5)
                    ("G", 6)
                    ("X", 7);
                }
            };
            
            // A parser used for recognizing the keywords.
            keywordsStruct keywords_;
            
            struct relationalOperatorStruct : qi::symbols<char, storm::logic::ComparisonType> {
                relationalOperatorStruct() {
                    add
                    (">=", storm::logic::ComparisonType::GreaterEqual)
                    (">", storm::logic::ComparisonType::Greater)
                    ("<=", storm::logic::ComparisonType::LessEqual)
                    ("<", storm::logic::ComparisonType::Less);
                }
            };
            
            // A parser used for recognizing the operators at the "relational" precedence level.
            relationalOperatorStruct relationalOperator_;
            
            struct binaryBooleanOperatorStruct : qi::symbols<char, storm::logic::BinaryBooleanStateFormula::OperatorType> {
                binaryBooleanOperatorStruct() {
                    add
                    ("&", storm::logic::BinaryBooleanStateFormula::OperatorType::And)
                    ("|", storm::logic::BinaryBooleanStateFormula::OperatorType::Or);
                }
            };
            
            // A parser used for recognizing the operators at the "binary" precedence level.
            binaryBooleanOperatorStruct binaryBooleanOperator_;
            
            struct unaryBooleanOperatorStruct : qi::symbols<char, storm::logic::UnaryBooleanStateFormula::OperatorType> {
                unaryBooleanOperatorStruct() {
                    add
                    ("!", storm::logic::UnaryBooleanStateFormula::OperatorType::Not);
                }
            };
            
            // A parser used for recognizing the operators at the "unary" precedence level.
            unaryBooleanOperatorStruct unaryBooleanOperator_;
            
            struct optimalityOperatorStruct : qi::symbols<char, storm::logic::OptimalityType> {
                optimalityOperatorStruct() {
                    add
                    ("min", storm::logic::OptimalityType::Minimize)
                    ("max", storm::logic::OptimalityType::Maximize);
                }
            };
            
            // A parser used for recognizing the optimality operators.
            optimalityOperatorStruct optimalityOperator_;
            
            // Parser and manager used for recognizing expressions.
            storm::parser::ExpressionParser expressionParser;
            
            // Functor used for displaying error information.
            struct ErrorHandler {
                typedef qi::error_handler_result result_type;
                
                template<typename T1, typename T2, typename T3, typename T4>
                qi::error_handler_result operator()(T1 b, T2 e, T3 where, T4 const& what) const {
                    std::stringstream whatAsString;
                    whatAsString << what;
                    STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Parsing error in line " << get_line(where) << ": " << " expecting " << whatAsString.str() << ".");
                    return qi::fail;
                }
            };
            
            // An error handler function.
            phoenix::function<ErrorHandler> handler;
            
            // A symbol table that is a mapping from identifiers that can be used in expressions to the expressions
            // they are to be replaced with.
            qi::symbols<char, storm::expressions::Expression> identifiers_;
            
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(), Skipper> start;
            
            qi::rule<Iterator, std::tuple<boost::optional<storm::logic::OptimalityType>, boost::optional<storm::logic::ComparisonType>, boost::optional<double>>(), qi::locals<boost::optional<storm::logic::OptimalityType>, boost::optional<storm::logic::ComparisonType>, boost::optional<double>>, Skipper> operatorInformation;
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(), Skipper> probabilityOperator;
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(), Skipper> rewardOperator;
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(), Skipper> expectedTimeOperator;
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(), Skipper> steadyStateOperator;
            
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(), Skipper> simpleFormula;
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(), Skipper> stateFormula;
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(), Skipper> pathFormula;
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(), Skipper> pathFormulaWithoutUntil;
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(), Skipper> simplePathFormula;
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(), Skipper> atomicStateFormula;
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(), Skipper> operatorFormula;
            qi::rule<Iterator, std::string(), Skipper> label;
            qi::rule<Iterator, std::string(), Skipper> rewardModelName;
            
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(), Skipper> andStateFormula;
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(), Skipper> orStateFormula;
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(), Skipper> notStateFormula;
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(), Skipper> labelFormula;
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(), Skipper> expressionFormula;
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(), qi::locals<bool>, Skipper> booleanLiteralFormula;

            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(), Skipper> conditionalFormula;
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(), Skipper> eventuallyFormula;
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(), Skipper> nextFormula;
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(), Skipper> globallyFormula;
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(), Skipper> untilFormula;
            qi::rule<Iterator, boost::variant<std::pair<double, double>, uint_fast64_t>(), Skipper> timeBound;
            
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(), Skipper> rewardPathFormula;
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(), Skipper> cumulativeRewardFormula;
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(), Skipper> reachabilityRewardFormula;
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(), Skipper> instantaneousRewardFormula;
            
            // Parser that is used to recognize doubles only (as opposed to Spirit's double_ parser).
            boost::spirit::qi::real_parser<double, boost::spirit::qi::strict_real_policies<double>> strict_double;
            
            // Methods that actually create the expression objects.
            std::shared_ptr<storm::logic::Formula> createInstantaneousRewardFormula(boost::variant<unsigned, double> const& timeBound) const;
            std::shared_ptr<storm::logic::Formula> createCumulativeRewardFormula(boost::variant<unsigned, double> const& timeBound) const;
            std::shared_ptr<storm::logic::Formula> createReachabilityRewardFormula(std::shared_ptr<storm::logic::Formula> const& stateFormula) const;
            std::shared_ptr<storm::logic::Formula> createAtomicExpressionFormula(storm::expressions::Expression const& expression) const;
            std::shared_ptr<storm::logic::Formula> createBooleanLiteralFormula(bool literal) const;
            std::shared_ptr<storm::logic::Formula> createAtomicLabelFormula(std::string const& label) const;
            std::shared_ptr<storm::logic::Formula> createEventuallyFormula(boost::optional<boost::variant<std::pair<double, double>, uint_fast64_t>> const& timeBound, std::shared_ptr<storm::logic::Formula> const& subformula) const;
            std::shared_ptr<storm::logic::Formula> createGloballyFormula(std::shared_ptr<storm::logic::Formula> const& subformula) const;
            std::shared_ptr<storm::logic::Formula> createNextFormula(std::shared_ptr<storm::logic::Formula> const& subformula) const;
            std::shared_ptr<storm::logic::Formula> createUntilFormula(std::shared_ptr<storm::logic::Formula> const& leftSubformula, boost::optional<boost::variant<std::pair<double, double>, uint_fast64_t>> const& timeBound, std::shared_ptr<storm::logic::Formula> const& rightSubformula);
            std::shared_ptr<storm::logic::Formula> createConditionalFormula(std::shared_ptr<storm::logic::Formula> const& leftSubformula, std::shared_ptr<storm::logic::Formula> const& rightSubformula) const;
            std::shared_ptr<storm::logic::Formula> createLongRunAverageOperatorFormula(std::tuple<boost::optional<storm::logic::OptimalityType>, boost::optional<storm::logic::ComparisonType>, boost::optional<double>> const& operatorInformation, std::shared_ptr<storm::logic::Formula> const& subformula) const;
            std::shared_ptr<storm::logic::Formula> createRewardOperatorFormula(boost::optional<std::string> const& rewardModelName, std::tuple<boost::optional<storm::logic::OptimalityType>, boost::optional<storm::logic::ComparisonType>, boost::optional<double>> const& operatorInformation, std::shared_ptr<storm::logic::Formula> const& subformula) const;
            std::shared_ptr<storm::logic::Formula> createExpectedTimeOperatorFormula(std::tuple<boost::optional<storm::logic::OptimalityType>, boost::optional<storm::logic::ComparisonType>, boost::optional<double>> const& operatorInformation, std::shared_ptr<storm::logic::Formula> const& subformula) const;
            std::shared_ptr<storm::logic::Formula> createProbabilityOperatorFormula(std::tuple<boost::optional<storm::logic::OptimalityType>, boost::optional<storm::logic::ComparisonType>, boost::optional<double>> const& operatorInformation, std::shared_ptr<storm::logic::Formula> const& subformula);
            std::shared_ptr<storm::logic::Formula> createBinaryBooleanStateFormula(std::shared_ptr<storm::logic::Formula> const& leftSubformula, std::shared_ptr<storm::logic::Formula> const& rightSubformula, storm::logic::BinaryBooleanStateFormula::OperatorType operatorType);
            std::shared_ptr<storm::logic::Formula> createUnaryBooleanStateFormula(std::shared_ptr<storm::logic::Formula> const& subformula, boost::optional<storm::logic::UnaryBooleanStateFormula::OperatorType> const& operatorType);
        };
        
    } // namespace parser
} // namespace storm

#endif /* STORM_PARSER_PRCTLPARSER_H_ */
