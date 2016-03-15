#include "src/parser/FormulaParser.h"

#include <fstream>

#include "src/parser/SpiritErrorHandler.h"

// If the parser fails due to ill-formed data, this exception is thrown.
#include "src/exceptions/WrongFormatException.h"

namespace storm {
    namespace parser {
        
        class FormulaParserGrammar : public qi::grammar<Iterator, std::vector<std::shared_ptr<storm::logic::Formula>>(), Skipper> {
        public:
            FormulaParserGrammar(std::shared_ptr<storm::expressions::ExpressionManager const> const& manager = std::shared_ptr<storm::expressions::ExpressionManager>(new storm::expressions::ExpressionManager()));
            
            FormulaParserGrammar(FormulaParserGrammar const& other) = default;
            FormulaParserGrammar& operator=(FormulaParserGrammar const& other) = default;
            
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
            
            struct optimalityOperatorStruct : qi::symbols<char, storm::OptimizationDirection> {
                optimalityOperatorStruct() {
                    add
                    ("min", storm::OptimizationDirection::Minimize)
                    ("max", storm::OptimizationDirection::Maximize);
                }
            };
            
            // A parser used for recognizing the optimality operators.
            optimalityOperatorStruct optimalityOperator_;
            
            struct rewardMeasureTypeStruct : qi::symbols<char, storm::logic::RewardMeasureType> {
                rewardMeasureTypeStruct() {
                    add
                    ("exp", storm::logic::RewardMeasureType::Expectation)
                    ("var", storm::logic::RewardMeasureType::Variance);
                }
            };

            // A parser used for recognizing the reward measure types.
            rewardMeasureTypeStruct rewardMeasureType_;
            
            // Parser and manager used for recognizing expressions.
            storm::parser::ExpressionParser expressionParser;
            
            // A symbol table that is a mapping from identifiers that can be used in expressions to the expressions
            // they are to be replaced with.
            qi::symbols<char, storm::expressions::Expression> identifiers_;
            
            qi::rule<Iterator, std::vector<std::shared_ptr<storm::logic::Formula>>(), Skipper> start;
            
            qi::rule<Iterator, storm::logic::OperatorInformation(), qi::locals<boost::optional<storm::OptimizationDirection>, boost::optional<storm::logic::ComparisonType>, boost::optional<double>>, Skipper> operatorInformation;
            qi::rule<Iterator, storm::logic::RewardMeasureType(), Skipper> rewardMeasureType;
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(), Skipper> probabilityOperator;
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(), Skipper> rewardOperator;
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(), Skipper> timeOperator;
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(), Skipper> longRunAverageOperator;
            
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(), Skipper> simpleFormula;
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(), Skipper> stateFormula;
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(storm::logic::FormulaContext), Skipper> pathFormula;
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(storm::logic::FormulaContext), Skipper> pathFormulaWithoutUntil;
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
            
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(storm::logic::FormulaContext), Skipper> conditionalFormula;
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(storm::logic::FormulaContext), Skipper> eventuallyFormula;
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(storm::logic::FormulaContext), Skipper> nextFormula;
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(storm::logic::FormulaContext), Skipper> globallyFormula;
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(storm::logic::FormulaContext), Skipper> untilFormula;
            qi::rule<Iterator, boost::variant<std::pair<double, double>, uint_fast64_t>(), Skipper> timeBound;
            
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(), Skipper> rewardPathFormula;
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(), Skipper> cumulativeRewardFormula;
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(), Skipper> instantaneousRewardFormula;
            qi::rule<Iterator, std::shared_ptr<storm::logic::Formula>(), Skipper> longRunAverageRewardFormula;
            
            // Parser that is used to recognize doubles only (as opposed to Spirit's double_ parser).
            boost::spirit::qi::real_parser<double, boost::spirit::qi::strict_real_policies<double>> strict_double;
            
            // Methods that actually create the expression objects.
            std::shared_ptr<storm::logic::Formula> createInstantaneousRewardFormula(boost::variant<unsigned, double> const& timeBound) const;
            std::shared_ptr<storm::logic::Formula> createCumulativeRewardFormula(boost::variant<unsigned, double> const& timeBound) const;
            std::shared_ptr<storm::logic::Formula> createLongRunAverageRewardFormula() const;
            std::shared_ptr<storm::logic::Formula> createAtomicExpressionFormula(storm::expressions::Expression const& expression) const;
            std::shared_ptr<storm::logic::Formula> createBooleanLiteralFormula(bool literal) const;
            std::shared_ptr<storm::logic::Formula> createAtomicLabelFormula(std::string const& label) const;
            std::shared_ptr<storm::logic::Formula> createEventuallyFormula(boost::optional<boost::variant<std::pair<double, double>, uint_fast64_t>> const& timeBound, storm::logic::FormulaContext context, std::shared_ptr<storm::logic::Formula> const& subformula) const;
            std::shared_ptr<storm::logic::Formula> createGloballyFormula(std::shared_ptr<storm::logic::Formula> const& subformula) const;
            std::shared_ptr<storm::logic::Formula> createNextFormula(std::shared_ptr<storm::logic::Formula> const& subformula) const;
            std::shared_ptr<storm::logic::Formula> createUntilFormula(std::shared_ptr<storm::logic::Formula> const& leftSubformula, boost::optional<boost::variant<std::pair<double, double>, uint_fast64_t>> const& timeBound, std::shared_ptr<storm::logic::Formula> const& rightSubformula);
            std::shared_ptr<storm::logic::Formula> createConditionalFormula(std::shared_ptr<storm::logic::Formula> const& leftSubformula, std::shared_ptr<storm::logic::Formula> const& rightSubformula, storm::logic::FormulaContext context) const;
            storm::logic::OperatorInformation createOperatorInformation(boost::optional<storm::OptimizationDirection> const& optimizationDirection, boost::optional<storm::logic::ComparisonType> const& comparisonType, boost::optional<double> const& threshold) const;
            std::shared_ptr<storm::logic::Formula> createLongRunAverageOperatorFormula(storm::logic::OperatorInformation const& operatorInformation, std::shared_ptr<storm::logic::Formula> const& subformula) const;
            std::shared_ptr<storm::logic::Formula> createRewardOperatorFormula(boost::optional<storm::logic::RewardMeasureType> const& rewardMeasureType, boost::optional<std::string> const& rewardModelName, storm::logic::OperatorInformation const& operatorInformation, std::shared_ptr<storm::logic::Formula> const& subformula) const;
            std::shared_ptr<storm::logic::Formula> createTimeOperatorFormula(boost::optional<storm::logic::RewardMeasureType> const& rewardMeasureType, storm::logic::OperatorInformation const& operatorInformation, std::shared_ptr<storm::logic::Formula> const& subformula) const;
            std::shared_ptr<storm::logic::Formula> createProbabilityOperatorFormula(storm::logic::OperatorInformation const& operatorInformation, std::shared_ptr<storm::logic::Formula> const& subformula);
            std::shared_ptr<storm::logic::Formula> createBinaryBooleanStateFormula(std::shared_ptr<storm::logic::Formula> const& leftSubformula, std::shared_ptr<storm::logic::Formula> const& rightSubformula, storm::logic::BinaryBooleanStateFormula::OperatorType operatorType);
            std::shared_ptr<storm::logic::Formula> createUnaryBooleanStateFormula(std::shared_ptr<storm::logic::Formula> const& subformula, boost::optional<storm::logic::UnaryBooleanStateFormula::OperatorType> const& operatorType);
            
            // An error handler function.
            phoenix::function<SpiritErrorHandler> handler;
        };
        
        FormulaParser::FormulaParser(std::shared_ptr<storm::expressions::ExpressionManager const> const& manager) : manager(manager->getSharedPointer()), grammar(new FormulaParserGrammar(manager)) {
            // Intentionally left empty.
        }
        
        FormulaParser::FormulaParser(storm::prism::Program const& program) : manager(program.getManager().getSharedPointer()), grammar(new FormulaParserGrammar(manager)) {
            // Make the formulas of the program available to the parser.
            for (auto const& formula : program.getFormulas()) {
                this->addIdentifierExpression(formula.getName(), formula.getExpression());
            }
        }
        
        FormulaParser::FormulaParser(FormulaParser const& other) : FormulaParser(other.manager) {
            other.identifiers_.for_each([=] (std::string const& name, storm::expressions::Expression const& expression) { this->addIdentifierExpression(name, expression); });
        }
        
        FormulaParser& FormulaParser::operator=(FormulaParser const& other) {
            this->manager = other.manager;
            this->grammar = std::shared_ptr<FormulaParserGrammar>(new FormulaParserGrammar(this->manager));
            other.identifiers_.for_each([=] (std::string const& name, storm::expressions::Expression const& expression) { this->addIdentifierExpression(name, expression); });
            return *this;
        }
        
        std::shared_ptr<storm::logic::Formula> FormulaParser::parseSingleFormulaFromString(std::string const& formulaString) const {
            std::vector<std::shared_ptr<storm::logic::Formula>> formulas = parseFromString(formulaString);
            STORM_LOG_THROW(formulas.size() == 1, storm::exceptions::WrongFormatException, "Expected exactly one formula, but found " << formulas.size() << " instead.");
            return formulas.front();
        }
        
        std::vector<std::shared_ptr<storm::logic::Formula>> FormulaParser::parseFromFile(std::string const& filename) const {
            // Open file and initialize result.
            std::ifstream inputFileStream(filename, std::ios::in);
            STORM_LOG_THROW(inputFileStream.good(), storm::exceptions::WrongFormatException, "Unable to read from file '" << filename << "'.");
            
            std::vector<std::shared_ptr<storm::logic::Formula>> formulas;
            
            // Now try to parse the contents of the file.
            try {
                std::string fileContent((std::istreambuf_iterator<char>(inputFileStream)), (std::istreambuf_iterator<char>()));
                formulas = parseFromString(fileContent);
            } catch(std::exception& e) {
                // In case of an exception properly close the file before passing exception.
                inputFileStream.close();
                throw e;
            }
            
            // Close the stream in case everything went smoothly and return result.
            inputFileStream.close();
            return formulas;
        }
        
        std::vector<std::shared_ptr<storm::logic::Formula>> FormulaParser::parseFromString(std::string const& formulaString) const {
            PositionIteratorType first(formulaString.begin());
            PositionIteratorType iter = first;
            PositionIteratorType last(formulaString.end());
            
            // Create empty result;
            std::vector<std::shared_ptr<storm::logic::Formula>> result;
            
            // Create grammar.
            try {
                // Start parsing.
                bool succeeded = qi::phrase_parse(iter, last, *grammar, boost::spirit::ascii::space | qi::lit("//") >> *(qi::char_ - (qi::eol | qi::eoi)) >> (qi::eol | qi::eoi), result);
                STORM_LOG_THROW(succeeded, storm::exceptions::WrongFormatException, "Could not parse formula.");
                STORM_LOG_DEBUG("Parsed formula successfully.");
            } catch (qi::expectation_failure<PositionIteratorType> const& e) {
                STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, e.what_);
            }
            
            return result;
        }
        
        void FormulaParser::addIdentifierExpression(std::string const& identifier, storm::expressions::Expression const& expression) {
            // Record the mapping and hand it over to the grammar.
            this->identifiers_.add(identifier, expression);
            grammar->addIdentifierExpression(identifier, expression);
        }
                
        FormulaParserGrammar::FormulaParserGrammar(std::shared_ptr<storm::expressions::ExpressionManager const> const& manager) : FormulaParserGrammar::base_type(start), expressionParser(*manager, keywords_, true, true) {
            // Register all variables so we can parse them in the expressions.
            for (auto variableTypePair : *manager) {
                identifiers_.add(variableTypePair.first.getName(), variableTypePair.first);
            }
            // Set the identifier mapping to actually generate expressions.
            expressionParser.setIdentifierMapping(&identifiers_);
            
            longRunAverageRewardFormula = (qi::lit("LRA") | qi::lit("S"))[qi::_val = phoenix::bind(&FormulaParserGrammar::createLongRunAverageRewardFormula, phoenix::ref(*this))];
            longRunAverageRewardFormula.name("long run average reward formula");
            
            instantaneousRewardFormula = (qi::lit("I=") >> strict_double)[qi::_val = phoenix::bind(&FormulaParserGrammar::createInstantaneousRewardFormula, phoenix::ref(*this), qi::_1)] | (qi::lit("I=") > qi::uint_)[qi::_val = phoenix::bind(&FormulaParserGrammar::createInstantaneousRewardFormula, phoenix::ref(*this), qi::_1)];
            instantaneousRewardFormula.name("instantaneous reward formula");
            
            cumulativeRewardFormula = (qi::lit("C<=") >> strict_double)[qi::_val = phoenix::bind(&FormulaParserGrammar::createCumulativeRewardFormula, phoenix::ref(*this), qi::_1)] | (qi::lit("C<=") > qi::uint_)[qi::_val = phoenix::bind(&FormulaParserGrammar::createCumulativeRewardFormula, phoenix::ref(*this), qi::_1)];
            cumulativeRewardFormula.name("cumulative reward formula");
            
            rewardPathFormula = conditionalFormula(storm::logic::FormulaContext::Reward) | eventuallyFormula(storm::logic::FormulaContext::Reward) | cumulativeRewardFormula | instantaneousRewardFormula | longRunAverageRewardFormula;
            rewardPathFormula.name("reward path formula");
            
            expressionFormula = expressionParser[qi::_val = phoenix::bind(&FormulaParserGrammar::createAtomicExpressionFormula, phoenix::ref(*this), qi::_1)];
            expressionFormula.name("expression formula");
            
            label = qi::as_string[qi::raw[qi::lexeme[((qi::alpha | qi::char_('_')) >> *(qi::alnum | qi::char_('_')))]]];
            label.name("label");
            
            labelFormula = (qi::lit("\"") >> label >> qi::lit("\""))[qi::_val = phoenix::bind(&FormulaParserGrammar::createAtomicLabelFormula, phoenix::ref(*this), qi::_1)];
            labelFormula.name("label formula");
            
            booleanLiteralFormula = (qi::lit("true")[qi::_a = true] | qi::lit("false")[qi::_a = false])[qi::_val = phoenix::bind(&FormulaParserGrammar::createBooleanLiteralFormula, phoenix::ref(*this), qi::_a)];
            booleanLiteralFormula.name("boolean literal formula");
            
            operatorFormula = probabilityOperator | rewardOperator | longRunAverageOperator | timeOperator;
            operatorFormula.name("operator formulas");
            
            atomicStateFormula = booleanLiteralFormula | labelFormula | expressionFormula | (qi::lit("(") > stateFormula > qi::lit(")")) | operatorFormula;
            atomicStateFormula.name("atomic state formula");

            notStateFormula = (-unaryBooleanOperator_ >> atomicStateFormula)[qi::_val = phoenix::bind(&FormulaParserGrammar::createUnaryBooleanStateFormula, phoenix::ref(*this), qi::_2, qi::_1)];
            notStateFormula.name("negation formula");
            
            eventuallyFormula = (qi::lit("F") >> -timeBound >> pathFormulaWithoutUntil(qi::_r1))[qi::_val = phoenix::bind(&FormulaParserGrammar::createEventuallyFormula, phoenix::ref(*this), qi::_1, qi::_r1, qi::_2)];
            eventuallyFormula.name("eventually formula");
            
            globallyFormula = (qi::lit("G") >> pathFormulaWithoutUntil(qi::_r1))[qi::_val = phoenix::bind(&FormulaParserGrammar::createGloballyFormula, phoenix::ref(*this), qi::_1)];
            globallyFormula.name("globally formula");
            
            nextFormula = (qi::lit("X") >> pathFormulaWithoutUntil(qi::_r1))[qi::_val = phoenix::bind(&FormulaParserGrammar::createNextFormula, phoenix::ref(*this), qi::_1)];
            nextFormula.name("next formula");
            
            pathFormulaWithoutUntil = eventuallyFormula(qi::_r1) | globallyFormula(qi::_r1) | nextFormula(qi::_r1) | stateFormula;
            pathFormulaWithoutUntil.name("path formula");
            
            untilFormula = pathFormulaWithoutUntil(qi::_r1)[qi::_val = qi::_1] >> *(qi::lit("U") >> -timeBound >> pathFormulaWithoutUntil(qi::_r1))[qi::_val = phoenix::bind(&FormulaParserGrammar::createUntilFormula, phoenix::ref(*this), qi::_val, qi::_1, qi::_2)];
            untilFormula.name("until formula");
            
            conditionalFormula = untilFormula(qi::_r1)[qi::_val = qi::_1] >> *(qi::lit("||") >> untilFormula(storm::logic::FormulaContext::Probability))[qi::_val = phoenix::bind(&FormulaParserGrammar::createConditionalFormula, phoenix::ref(*this), qi::_val, qi::_1, qi::_r1)];
            conditionalFormula.name("conditional formula");
            
            timeBound = (qi::lit("[") > qi::double_ > qi::lit(",") > qi::double_ > qi::lit("]"))[qi::_val = phoenix::construct<std::pair<double, double>>(qi::_1, qi::_2)] | (qi::lit("<=") >> strict_double)[qi::_val = phoenix::construct<std::pair<double, double>>(0, qi::_1)] | (qi::lit("<=") >  qi::uint_)[qi::_val = qi::_1];
            timeBound.name("time bound");
            
            pathFormula = conditionalFormula(qi::_r1);
            pathFormula.name("path formula");
            
            rewardMeasureType = qi::lit("[") >> rewardMeasureType_ >> qi::lit("]");
            rewardMeasureType.name("reward measure type");
            
            operatorInformation = (-optimalityOperator_[qi::_a = qi::_1] >> ((relationalOperator_[qi::_b = qi::_1] > qi::double_[qi::_c = qi::_1]) | (qi::lit("=") > qi::lit("?"))))[qi::_val = phoenix::bind(&FormulaParserGrammar::createOperatorInformation, phoenix::ref(*this), qi::_a, qi::_b, qi::_c)];
            operatorInformation.name("operator information");
            
            longRunAverageOperator = ((qi::lit("LRA") | qi::lit("S")) > operatorInformation > qi::lit("[") > stateFormula > qi::lit("]"))[qi::_val = phoenix::bind(&FormulaParserGrammar::createLongRunAverageOperatorFormula, phoenix::ref(*this), qi::_1, qi::_2)];
            longRunAverageOperator.name("long-run average operator");
            
            rewardModelName = qi::lit("{\"") > label > qi::lit("\"}");
            rewardModelName.name("reward model name");
            
            rewardOperator = (qi::lit("R") > -rewardMeasureType > -rewardModelName > operatorInformation > qi::lit("[") > rewardPathFormula > qi::lit("]"))[qi::_val = phoenix::bind(&FormulaParserGrammar::createRewardOperatorFormula, phoenix::ref(*this), qi::_1, qi::_2, qi::_3, qi::_4)];
            rewardOperator.name("reward operator");
            
            timeOperator = (qi::lit("T") > -rewardMeasureType > operatorInformation > qi::lit("[") > eventuallyFormula(storm::logic::FormulaContext::Time) > qi::lit("]"))[qi::_val = phoenix::bind(&FormulaParserGrammar::createTimeOperatorFormula, phoenix::ref(*this), qi::_1, qi::_2, qi::_3)];
            timeOperator.name("time operator");
            
            probabilityOperator = (qi::lit("P") > operatorInformation > qi::lit("[") > pathFormula(storm::logic::FormulaContext::Probability) > qi::lit("]"))[qi::_val = phoenix::bind(&FormulaParserGrammar::createProbabilityOperatorFormula, phoenix::ref(*this), qi::_1, qi::_2)];
            probabilityOperator.name("probability operator");
            
            andStateFormula = notStateFormula[qi::_val = qi::_1] >> *(qi::lit("&") >> notStateFormula)[qi::_val = phoenix::bind(&FormulaParserGrammar::createBinaryBooleanStateFormula, phoenix::ref(*this), qi::_val, qi::_1, storm::logic::BinaryBooleanStateFormula::OperatorType::And)];
            andStateFormula.name("and state formula");
            
            orStateFormula = andStateFormula[qi::_val = qi::_1] >> *(qi::lit("|") >> andStateFormula)[qi::_val = phoenix::bind(&FormulaParserGrammar::createBinaryBooleanStateFormula, phoenix::ref(*this), qi::_val, qi::_1, storm::logic::BinaryBooleanStateFormula::OperatorType::Or)];
            orStateFormula.name("or state formula");
            
            stateFormula = (orStateFormula);
            stateFormula.name("state formula");
            
            start = qi::eps > (stateFormula % +(qi::char_("\n;"))) >> qi::skip(boost::spirit::ascii::space | qi::lit("//") >> *(qi::char_ - (qi::eol | qi::eoi)))[qi::eps] >> qi::eoi;
            start.name("start");
            
            //Enable the following lines to print debug output for most the rules.
            /*
            debug(start);
            debug(stateFormula);
            debug(orStateFormula);
            debug(andStateFormula);
            debug(probabilityOperator);
            debug(rewardOperator);
            debug(longRunAverageOperator);
            debug(timeOperator);
            debug(pathFormulaWithoutUntil);
            debug(pathFormula);
//            debug(conditionalFormula);
            debug(nextFormula);
            debug(globallyFormula);
//            debug(eventuallyFormula);
            debug(atomicStateFormula);
            debug(booleanLiteralFormula);
            debug(labelFormula);
            debug(expressionFormula);
            debug(rewardPathFormula);
            debug(cumulativeRewardFormula);
            debug(instantaneousRewardFormula);
            */

            // Enable error reporting.
            qi::on_error<qi::fail>(start, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(stateFormula, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(orStateFormula, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(andStateFormula, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(probabilityOperator, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(rewardOperator, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(longRunAverageOperator, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(timeOperator, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(operatorInformation, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(pathFormulaWithoutUntil, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(pathFormula, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(conditionalFormula, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(untilFormula, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(nextFormula, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(globallyFormula, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(eventuallyFormula, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(atomicStateFormula, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(booleanLiteralFormula, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(labelFormula, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(expressionFormula, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(rewardPathFormula, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(cumulativeRewardFormula, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(instantaneousRewardFormula, handler(qi::_1, qi::_2, qi::_3, qi::_4));
        }
        
        void FormulaParserGrammar::addIdentifierExpression(std::string const& identifier, storm::expressions::Expression const& expression) {
            this->identifiers_.add(identifier, expression);
        }
        
        std::shared_ptr<storm::logic::Formula> FormulaParserGrammar::createInstantaneousRewardFormula(boost::variant<unsigned, double> const& timeBound) const {
            if (timeBound.which() == 0) {
                return std::shared_ptr<storm::logic::Formula>(new storm::logic::InstantaneousRewardFormula(static_cast<uint_fast64_t>(boost::get<unsigned>(timeBound))));
            } else {
                double timeBoundAsDouble = boost::get<double>(timeBound);
                STORM_LOG_THROW(timeBoundAsDouble >= 0, storm::exceptions::WrongFormatException, "Cumulative reward property must have non-negative bound.");
                return std::shared_ptr<storm::logic::Formula>(new storm::logic::InstantaneousRewardFormula(static_cast<uint_fast64_t>(timeBoundAsDouble)));
            }
        }
        
        std::shared_ptr<storm::logic::Formula> FormulaParserGrammar::createCumulativeRewardFormula(boost::variant<unsigned, double> const& timeBound) const {
            if (timeBound.which() == 0) {
                return std::shared_ptr<storm::logic::Formula>(new storm::logic::CumulativeRewardFormula(static_cast<uint_fast64_t>(boost::get<unsigned>(timeBound))));
            } else {
                double timeBoundAsDouble = boost::get<double>(timeBound);
                STORM_LOG_THROW(timeBoundAsDouble >= 0, storm::exceptions::WrongFormatException, "Cumulative reward property must have non-negative bound.");
                return std::shared_ptr<storm::logic::Formula>(new storm::logic::CumulativeRewardFormula(static_cast<uint_fast64_t>(timeBoundAsDouble)));
            }
        }
        
        std::shared_ptr<storm::logic::Formula> FormulaParserGrammar::createLongRunAverageRewardFormula() const {
            return std::shared_ptr<storm::logic::Formula>(new storm::logic::LongRunAverageRewardFormula());
        }
        
        std::shared_ptr<storm::logic::Formula> FormulaParserGrammar::createAtomicExpressionFormula(storm::expressions::Expression const& expression) const {
            STORM_LOG_THROW(expression.hasBooleanType(), storm::exceptions::WrongFormatException, "Expected expression of boolean type.");
            return std::shared_ptr<storm::logic::Formula>(new storm::logic::AtomicExpressionFormula(expression));
        }
        
        std::shared_ptr<storm::logic::Formula> FormulaParserGrammar::createBooleanLiteralFormula(bool literal) const {
            return std::shared_ptr<storm::logic::Formula>(new storm::logic::BooleanLiteralFormula(literal));
        }
        
        std::shared_ptr<storm::logic::Formula> FormulaParserGrammar::createAtomicLabelFormula(std::string const& label) const {
            return std::shared_ptr<storm::logic::Formula>(new storm::logic::AtomicLabelFormula(label));
        }
        
        std::shared_ptr<storm::logic::Formula> FormulaParserGrammar::createEventuallyFormula(boost::optional<boost::variant<std::pair<double, double>, uint_fast64_t>> const& timeBound, storm::logic::FormulaContext context, std::shared_ptr<storm::logic::Formula> const& subformula) const {
            if (timeBound) {
                if (timeBound.get().which() == 0) {
                    std::pair<double, double> const& bounds = boost::get<std::pair<double, double>>(timeBound.get());
                    return std::shared_ptr<storm::logic::Formula>(new storm::logic::BoundedUntilFormula(createBooleanLiteralFormula(true), subformula, bounds.first, bounds.second));
                } else {
                    return std::shared_ptr<storm::logic::Formula>(new storm::logic::BoundedUntilFormula(createBooleanLiteralFormula(true), subformula, static_cast<uint_fast64_t>(boost::get<uint_fast64_t>(timeBound.get()))));
                }
            } else {
                return std::shared_ptr<storm::logic::Formula>(new storm::logic::EventuallyFormula(subformula, context));
            }
        }
        
        std::shared_ptr<storm::logic::Formula> FormulaParserGrammar::createGloballyFormula(std::shared_ptr<storm::logic::Formula> const& subformula) const {
            return std::shared_ptr<storm::logic::Formula>(new storm::logic::GloballyFormula(subformula));
        }
        
        std::shared_ptr<storm::logic::Formula> FormulaParserGrammar::createNextFormula(std::shared_ptr<storm::logic::Formula> const& subformula) const {
            return std::shared_ptr<storm::logic::Formula>(new storm::logic::NextFormula(subformula));
        }
        
        std::shared_ptr<storm::logic::Formula> FormulaParserGrammar::createUntilFormula(std::shared_ptr<storm::logic::Formula> const& leftSubformula, boost::optional<boost::variant<std::pair<double, double>, uint_fast64_t>> const& timeBound, std::shared_ptr<storm::logic::Formula> const& rightSubformula) {
            if (timeBound) {
                if (timeBound.get().which() == 0) {
                    std::pair<double, double> const& bounds = boost::get<std::pair<double, double>>(timeBound.get());
                    return std::shared_ptr<storm::logic::Formula>(new storm::logic::BoundedUntilFormula(leftSubformula, rightSubformula, bounds.first, bounds.second));
                } else {
                    return std::shared_ptr<storm::logic::Formula>(new storm::logic::BoundedUntilFormula(leftSubformula, rightSubformula, static_cast<uint_fast64_t>(boost::get<uint_fast64_t>(timeBound.get()))));
                }
            } else {
                return std::shared_ptr<storm::logic::Formula>(new storm::logic::UntilFormula(leftSubformula, rightSubformula));
            }
        }
        
        std::shared_ptr<storm::logic::Formula> FormulaParserGrammar::createConditionalFormula(std::shared_ptr<storm::logic::Formula> const& leftSubformula, std::shared_ptr<storm::logic::Formula> const& rightSubformula, storm::logic::FormulaContext context) const {
            return std::shared_ptr<storm::logic::Formula>(new storm::logic::ConditionalFormula(leftSubformula, rightSubformula, context));
        }
        
        storm::logic::OperatorInformation FormulaParserGrammar::createOperatorInformation(boost::optional<storm::OptimizationDirection> const& optimizationDirection, boost::optional<storm::logic::ComparisonType> const& comparisonType, boost::optional<double> const& threshold) const {
            if (comparisonType && threshold) {
                return storm::logic::OperatorInformation(optimizationDirection, storm::logic::Bound<double>(comparisonType.get(), threshold.get()));
            } else {
                return storm::logic::OperatorInformation(optimizationDirection, boost::none);
            }
        }
        
        std::shared_ptr<storm::logic::Formula> FormulaParserGrammar::createLongRunAverageOperatorFormula(storm::logic::OperatorInformation const& operatorInformation, std::shared_ptr<storm::logic::Formula> const& subformula) const {
            return std::shared_ptr<storm::logic::Formula>(new storm::logic::LongRunAverageOperatorFormula(subformula, operatorInformation));
        }
        
        std::shared_ptr<storm::logic::Formula> FormulaParserGrammar::createRewardOperatorFormula(boost::optional<storm::logic::RewardMeasureType> const& rewardMeasureType, boost::optional<std::string> const& rewardModelName, storm::logic::OperatorInformation const& operatorInformation, std::shared_ptr<storm::logic::Formula> const& subformula) const {
            storm::logic::RewardMeasureType measureType = storm::logic::RewardMeasureType::Expectation;
            if (rewardMeasureType) {
                measureType = rewardMeasureType.get();
            }
            return std::shared_ptr<storm::logic::Formula>(new storm::logic::RewardOperatorFormula(subformula, rewardModelName, operatorInformation, measureType));
        }
        
        std::shared_ptr<storm::logic::Formula> FormulaParserGrammar::createTimeOperatorFormula(boost::optional<storm::logic::RewardMeasureType> const& rewardMeasureType, storm::logic::OperatorInformation const& operatorInformation, std::shared_ptr<storm::logic::Formula> const& subformula) const {
            storm::logic::RewardMeasureType measureType = storm::logic::RewardMeasureType::Expectation;
            if (rewardMeasureType) {
                measureType = rewardMeasureType.get();
            }
            return std::shared_ptr<storm::logic::Formula>(new storm::logic::TimeOperatorFormula(subformula, operatorInformation, measureType));
        }
        
        std::shared_ptr<storm::logic::Formula> FormulaParserGrammar::createProbabilityOperatorFormula(storm::logic::OperatorInformation const& operatorInformation, std::shared_ptr<storm::logic::Formula> const& subformula) {
            return std::shared_ptr<storm::logic::Formula>(new storm::logic::ProbabilityOperatorFormula(subformula, operatorInformation));
        }
        
        std::shared_ptr<storm::logic::Formula> FormulaParserGrammar::createBinaryBooleanStateFormula(std::shared_ptr<storm::logic::Formula> const& leftSubformula, std::shared_ptr<storm::logic::Formula> const& rightSubformula, storm::logic::BinaryBooleanStateFormula::OperatorType operatorType) {
            return std::shared_ptr<storm::logic::Formula>(new storm::logic::BinaryBooleanStateFormula(operatorType, leftSubformula, rightSubformula));
        }
        
        std::shared_ptr<storm::logic::Formula> FormulaParserGrammar::createUnaryBooleanStateFormula(std::shared_ptr<storm::logic::Formula> const& subformula, boost::optional<storm::logic::UnaryBooleanStateFormula::OperatorType> const& operatorType) {
            if (operatorType) {
                return std::shared_ptr<storm::logic::Formula>(new storm::logic::UnaryBooleanStateFormula(operatorType.get(), subformula));
            } else {
                return subformula;
            }
        }

    } // namespace parser
} // namespace storm
