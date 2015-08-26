#include "src/parser/FormulaParser.h"

#include <fstream>

// If the parser fails due to ill-formed data, this exception is thrown.
#include "src/exceptions/WrongFormatException.h"

namespace storm {
    namespace parser {
        
        FormulaParser::FormulaParser(std::shared_ptr<storm::expressions::ExpressionManager const> const& manager) : FormulaParser::base_type(start), expressionParser(*manager, keywords_, true) {
            // Register all variables so we can parse them in the expressions.
            for (auto variableTypePair : *manager) {
                identifiers_.add(variableTypePair.first.getName(), variableTypePair.first);
            }
            // Set the identifier mapping to actually generate expressions.
            expressionParser.setIdentifierMapping(&identifiers_);
            
            instantaneousRewardFormula = (qi::lit("I=") >> strict_double)[qi::_val = phoenix::bind(&FormulaParser::createInstantaneousRewardFormula, phoenix::ref(*this), qi::_1)] | (qi::lit("I=") > qi::uint_)[qi::_val = phoenix::bind(&FormulaParser::createInstantaneousRewardFormula, phoenix::ref(*this), qi::_1)];
            instantaneousRewardFormula.name("instantaneous reward formula");
            
            cumulativeRewardFormula = (qi::lit("C<=") >> strict_double)[qi::_val = phoenix::bind(&FormulaParser::createCumulativeRewardFormula, phoenix::ref(*this), qi::_1)] | (qi::lit("C<=") > qi::uint_)[qi::_val = phoenix::bind(&FormulaParser::createCumulativeRewardFormula, phoenix::ref(*this), qi::_1)];
            cumulativeRewardFormula.name("cumulative reward formula");
            
            reachabilityRewardFormula = (qi::lit("F") > stateFormula)[qi::_val = phoenix::bind(&FormulaParser::createReachabilityRewardFormula, phoenix::ref(*this), qi::_1)];
            reachabilityRewardFormula.name("reachability reward formula");
            
            rewardPathFormula = reachabilityRewardFormula | cumulativeRewardFormula | instantaneousRewardFormula;
            rewardPathFormula.name("reward path formula");
            
            expressionFormula = expressionParser[qi::_val = phoenix::bind(&FormulaParser::createAtomicExpressionFormula, phoenix::ref(*this), qi::_1)];
            expressionFormula.name("expression formula");
            
            label = qi::as_string[qi::raw[qi::lexeme[((qi::alpha | qi::char_('_')) >> *(qi::alnum | qi::char_('_')))]]];
            label.name("label");
            
            labelFormula = (qi::lit("\"") >> label >> qi::lit("\""))[qi::_val = phoenix::bind(&FormulaParser::createAtomicLabelFormula, phoenix::ref(*this), qi::_1)];
            labelFormula.name("label formula");
            
            booleanLiteralFormula = (qi::lit("true")[qi::_a = true] | qi::lit("false")[qi::_a = false])[qi::_val = phoenix::bind(&FormulaParser::createBooleanLiteralFormula, phoenix::ref(*this), qi::_a)];
            booleanLiteralFormula.name("boolean literal formula");
            
            operatorFormula = probabilityOperator | rewardOperator | steadyStateOperator;
            operatorFormula.name("operator formulas");
            
            atomicStateFormula = booleanLiteralFormula | labelFormula | expressionFormula | (qi::lit("(") > stateFormula > qi::lit(")")) | operatorFormula;
            atomicStateFormula.name("atomic state formula");

            notStateFormula = (-unaryBooleanOperator_ >> atomicStateFormula)[qi::_val = phoenix::bind(&FormulaParser::createUnaryBooleanStateFormula, phoenix::ref(*this), qi::_2, qi::_1)];
            notStateFormula.name("negation formula");
            
            eventuallyFormula = (qi::lit("F") >> -timeBound >> pathFormulaWithoutUntil)[qi::_val = phoenix::bind(&FormulaParser::createEventuallyFormula, phoenix::ref(*this), qi::_1, qi::_2)];
            eventuallyFormula.name("eventually formula");
            
            globallyFormula = (qi::lit("G") >> pathFormulaWithoutUntil)[qi::_val = phoenix::bind(&FormulaParser::createGloballyFormula, phoenix::ref(*this), qi::_1)];
            globallyFormula.name("globally formula");
            
            nextFormula = (qi::lit("X") >> pathFormulaWithoutUntil)[qi::_val = phoenix::bind(&FormulaParser::createNextFormula, phoenix::ref(*this), qi::_1)];
            nextFormula.name("next formula");
            
            pathFormulaWithoutUntil = eventuallyFormula | globallyFormula | nextFormula | stateFormula;
            pathFormulaWithoutUntil.name("path formula");
            
            untilFormula = pathFormulaWithoutUntil[qi::_val = qi::_1] >> *(qi::lit("U") >> -timeBound >> pathFormulaWithoutUntil)[qi::_val = phoenix::bind(&FormulaParser::createUntilFormula, phoenix::ref(*this), qi::_val, qi::_1, qi::_2)];
            untilFormula.name("until formula");
            
            conditionalFormula = untilFormula[qi::_val = qi::_1] >> *(qi::lit("||") >> untilFormula)[qi::_val = phoenix::bind(&FormulaParser::createConditionalFormula, phoenix::ref(*this), qi::_val, qi::_1)];
            conditionalFormula.name("conditional formula");
            
            timeBound = (qi::lit("[") > qi::double_ > qi::lit(",") > qi::double_ > qi::lit("]"))[qi::_val = phoenix::construct<std::pair<double, double>>(qi::_1, qi::_2)] | (qi::lit("<=") >> strict_double)[qi::_val = phoenix::construct<std::pair<double, double>>(0, qi::_1)] | (qi::lit("<=") >  qi::uint_)[qi::_val = qi::_1];
            timeBound.name("time bound");
            
            pathFormula = conditionalFormula;
            pathFormula.name("path formula");
            
            operatorInformation = (-optimalityOperator_[qi::_a = qi::_1] >> ((relationalOperator_[qi::_b = qi::_1] > qi::double_[qi::_c = qi::_1]) | (qi::lit("=") > qi::lit("?"))))[qi::_val = phoenix::construct<std::tuple<boost::optional<storm::logic::OptimalityType>, boost::optional<storm::logic::ComparisonType>, boost::optional<double>>>(qi::_a, qi::_b, qi::_c)];
            operatorInformation.name("operator information");
            
            steadyStateOperator = (qi::lit("LRA") > operatorInformation > qi::lit("[") > stateFormula > qi::lit("]"))[qi::_val = phoenix::bind(&FormulaParser::createLongRunAverageOperatorFormula, phoenix::ref(*this), qi::_1, qi::_2)];
            steadyStateOperator.name("long-run average operator");
            
            rewardModelName = qi::lit("{\"") > label > qi::lit("\"}");
            rewardModelName.name("reward model name");
            
            rewardOperator = (qi::lit("R") > -rewardModelName > operatorInformation > qi::lit("[") > rewardPathFormula > qi::lit("]"))[qi::_val = phoenix::bind(&FormulaParser::createRewardOperatorFormula, phoenix::ref(*this), qi::_1, qi::_2, qi::_3)];
            rewardOperator.name("reward operator");
            
            expectedTimeOperator = (qi::lit("ET") > operatorInformation > qi::lit("[") > eventuallyFormula > qi::lit("]"))[qi::_val = phoenix::bind(&FormulaParser::createExpectedTimeOperatorFormula, phoenix::ref(*this), qi::_1, qi::_2)];
            expectedTimeOperator.name("expected time operator");
            
            probabilityOperator = (qi::lit("P") > operatorInformation > qi::lit("[") > pathFormula > qi::lit("]"))[qi::_val = phoenix::bind(&FormulaParser::createProbabilityOperatorFormula, phoenix::ref(*this), qi::_1, qi::_2)];
            probabilityOperator.name("probability operator");
            
            andStateFormula = notStateFormula[qi::_val = qi::_1] >> *(qi::lit("&") >> notStateFormula)[qi::_val = phoenix::bind(&FormulaParser::createBinaryBooleanStateFormula, phoenix::ref(*this), qi::_val, qi::_1, storm::logic::BinaryBooleanStateFormula::OperatorType::And)];
            andStateFormula.name("and state formula");
            
            orStateFormula = andStateFormula[qi::_val = qi::_1] >> *(qi::lit("|") >> andStateFormula)[qi::_val = phoenix::bind(&FormulaParser::createBinaryBooleanStateFormula, phoenix::ref(*this), qi::_val, qi::_1, storm::logic::BinaryBooleanStateFormula::OperatorType::Or)];
            orStateFormula.name("or state formula");
            
            stateFormula = (orStateFormula);
            stateFormula.name("state formula");
            
            start = qi::eps > (stateFormula % +(qi::char_("\n;"))) >> qi::skip(boost::spirit::ascii::space | qi::lit("//") >> *(qi::char_ - (qi::eol | qi::eoi)))[qi::eps] >> qi::eoi;
            start.name("start");
            
            /*!
             * Enable the following lines to print debug output for most the rules.
            debug(start);
            debug(stateFormula);
            debug(orStateFormula);
            debug(andStateFormula);
            debug(probabilityOperator);
            debug(rewardOperator);
            debug(steadyStateOperator);
            debug(pathFormulaWithoutUntil);
            debug(pathFormula);
            debug(conditionalFormula);
            debug(nextFormula);
            debug(globallyFormula);
            debug(eventuallyFormula);
            debug(atomicStateFormula);
            debug(booleanLiteralFormula);
            debug(labelFormula);
            debug(expressionFormula);
            debug(rewardPathFormula);
            debug(reachabilityRewardFormula);
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
            qi::on_error<qi::fail>(steadyStateOperator, handler(qi::_1, qi::_2, qi::_3, qi::_4));
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
            qi::on_error<qi::fail>(reachabilityRewardFormula, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(cumulativeRewardFormula, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(instantaneousRewardFormula, handler(qi::_1, qi::_2, qi::_3, qi::_4));
        }
        
        void FormulaParser::addIdentifierExpression(std::string const& identifier, storm::expressions::Expression const& expression) {
            this->identifiers_.add(identifier, expression);
        }
        
        std::shared_ptr<storm::logic::Formula> FormulaParser::parseSingleFormulaFromString(std::string const& formulaString) {
            std::vector<std::shared_ptr<storm::logic::Formula>> formulas = parseFromString(formulaString);
            STORM_LOG_THROW(formulas.size() == 1, storm::exceptions::WrongFormatException, "Expected exactly one formula, but found " << formulas.size() << " instead.");
            return formulas.front();
        }
        
        std::vector<std::shared_ptr<storm::logic::Formula>> FormulaParser::parseFromFile(std::string const& filename) {
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
        
        std::vector<std::shared_ptr<storm::logic::Formula>> FormulaParser::parseFromString(std::string const& formulaString) {
            PositionIteratorType first(formulaString.begin());
            PositionIteratorType iter = first;
            PositionIteratorType last(formulaString.end());
            
            // Create empty result;
            std::vector<std::shared_ptr<storm::logic::Formula>> result;
            
            // Create grammar.
            try {
                // Start parsing.
                bool succeeded = qi::phrase_parse(iter, last, *this, boost::spirit::ascii::space | qi::lit("//") >> *(qi::char_ - (qi::eol | qi::eoi)) >> (qi::eol | qi::eoi), result);
                STORM_LOG_THROW(succeeded, storm::exceptions::WrongFormatException, "Could not parse formula.");
                STORM_LOG_DEBUG("Parsed formula successfully.");
            } catch (qi::expectation_failure<PositionIteratorType> const& e) {
                STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, e.what_);
            }
            
            return result;
        }
        
        std::shared_ptr<storm::logic::Formula> FormulaParser::createInstantaneousRewardFormula(boost::variant<unsigned, double> const& timeBound) const {
            if (timeBound.which() == 0) {
                return std::shared_ptr<storm::logic::Formula>(new storm::logic::InstantaneousRewardFormula(static_cast<uint_fast64_t>(boost::get<unsigned>(timeBound))));
            } else {
                double timeBoundAsDouble = boost::get<double>(timeBound);
                STORM_LOG_THROW(timeBoundAsDouble >= 0, storm::exceptions::WrongFormatException, "Cumulative reward property must have non-negative bound.");
                return std::shared_ptr<storm::logic::Formula>(new storm::logic::InstantaneousRewardFormula(static_cast<uint_fast64_t>(timeBoundAsDouble)));
            }
        }
        
        std::shared_ptr<storm::logic::Formula> FormulaParser::createCumulativeRewardFormula(boost::variant<unsigned, double> const& timeBound) const {
            if (timeBound.which() == 0) {
                return std::shared_ptr<storm::logic::Formula>(new storm::logic::CumulativeRewardFormula(static_cast<uint_fast64_t>(boost::get<unsigned>(timeBound))));
            } else {
                double timeBoundAsDouble = boost::get<double>(timeBound);
                STORM_LOG_THROW(timeBoundAsDouble >= 0, storm::exceptions::WrongFormatException, "Cumulative reward property must have non-negative bound.");
                return std::shared_ptr<storm::logic::Formula>(new storm::logic::CumulativeRewardFormula(static_cast<uint_fast64_t>(timeBoundAsDouble)));
            }
        }
        
        std::shared_ptr<storm::logic::Formula> FormulaParser::createReachabilityRewardFormula(std::shared_ptr<storm::logic::Formula> const& stateFormula) const {
            return std::shared_ptr<storm::logic::Formula>(new storm::logic::ReachabilityRewardFormula(stateFormula));
        }
        
        std::shared_ptr<storm::logic::Formula> FormulaParser::createAtomicExpressionFormula(storm::expressions::Expression const& expression) const {
            STORM_LOG_THROW(expression.hasBooleanType(), storm::exceptions::WrongFormatException, "Expected expression of boolean type.");
            return std::shared_ptr<storm::logic::Formula>(new storm::logic::AtomicExpressionFormula(expression));
        }
        
        std::shared_ptr<storm::logic::Formula> FormulaParser::createBooleanLiteralFormula(bool literal) const {
            return std::shared_ptr<storm::logic::Formula>(new storm::logic::BooleanLiteralFormula(literal));
        }
        
        std::shared_ptr<storm::logic::Formula> FormulaParser::createAtomicLabelFormula(std::string const& label) const {
            return std::shared_ptr<storm::logic::Formula>(new storm::logic::AtomicLabelFormula(label));
        }
        
        std::shared_ptr<storm::logic::Formula> FormulaParser::createEventuallyFormula(boost::optional<boost::variant<std::pair<double, double>, uint_fast64_t>> const& timeBound, std::shared_ptr<storm::logic::Formula> const& subformula) const {
            if (timeBound) {
                if (timeBound.get().which() == 0) {
                    std::pair<double, double> const& bounds = boost::get<std::pair<double, double>>(timeBound.get());
                    return std::shared_ptr<storm::logic::Formula>(new storm::logic::BoundedUntilFormula(createBooleanLiteralFormula(true), subformula, bounds.first, bounds.second));
                } else {
                    return std::shared_ptr<storm::logic::Formula>(new storm::logic::BoundedUntilFormula(createBooleanLiteralFormula(true), subformula, static_cast<uint_fast64_t>(boost::get<uint_fast64_t>(timeBound.get()))));
                }
            } else {
                return std::shared_ptr<storm::logic::Formula>(new storm::logic::EventuallyFormula(subformula));
            }
        }
        
        std::shared_ptr<storm::logic::Formula> FormulaParser::createGloballyFormula(std::shared_ptr<storm::logic::Formula> const& subformula) const {
            return std::shared_ptr<storm::logic::Formula>(new storm::logic::GloballyFormula(subformula));
        }
        
        std::shared_ptr<storm::logic::Formula> FormulaParser::createNextFormula(std::shared_ptr<storm::logic::Formula> const& subformula) const {
            return std::shared_ptr<storm::logic::Formula>(new storm::logic::NextFormula(subformula));
        }
        
        std::shared_ptr<storm::logic::Formula> FormulaParser::createUntilFormula(std::shared_ptr<storm::logic::Formula> const& leftSubformula, boost::optional<boost::variant<std::pair<double, double>, uint_fast64_t>> const& timeBound, std::shared_ptr<storm::logic::Formula> const& rightSubformula) {
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
        
        std::shared_ptr<storm::logic::Formula> FormulaParser::createConditionalFormula(std::shared_ptr<storm::logic::Formula> const& leftSubformula, std::shared_ptr<storm::logic::Formula> const& rightSubformula) const {
            return std::shared_ptr<storm::logic::Formula>(new storm::logic::ConditionalPathFormula(leftSubformula, rightSubformula));
        }
        
        std::shared_ptr<storm::logic::Formula> FormulaParser::createLongRunAverageOperatorFormula(std::tuple<boost::optional<storm::logic::OptimalityType>, boost::optional<storm::logic::ComparisonType>, boost::optional<double>> const& operatorInformation, std::shared_ptr<storm::logic::Formula> const& subformula) const {
            return std::shared_ptr<storm::logic::Formula>(new storm::logic::LongRunAverageOperatorFormula(std::get<0>(operatorInformation), std::get<1>(operatorInformation), std::get<2>(operatorInformation), subformula));
        }
        
        std::shared_ptr<storm::logic::Formula> FormulaParser::createRewardOperatorFormula(boost::optional<std::string> const& rewardModelName, std::tuple<boost::optional<storm::logic::OptimalityType>, boost::optional<storm::logic::ComparisonType>, boost::optional<double>> const& operatorInformation, std::shared_ptr<storm::logic::Formula> const& subformula) const {
            return std::shared_ptr<storm::logic::Formula>(new storm::logic::RewardOperatorFormula(rewardModelName, std::get<0>(operatorInformation), std::get<1>(operatorInformation), std::get<2>(operatorInformation), subformula));
        }
        
        std::shared_ptr<storm::logic::Formula> FormulaParser::createExpectedTimeOperatorFormula(std::tuple<boost::optional<storm::logic::OptimalityType>, boost::optional<storm::logic::ComparisonType>, boost::optional<double>> const& operatorInformation, std::shared_ptr<storm::logic::Formula> const& subformula) const {
            return std::shared_ptr<storm::logic::Formula>(new storm::logic::ExpectedTimeOperatorFormula(std::get<0>(operatorInformation), std::get<1>(operatorInformation), std::get<2>(operatorInformation), subformula));
        }
        
        std::shared_ptr<storm::logic::Formula> FormulaParser::createProbabilityOperatorFormula(std::tuple<boost::optional<storm::logic::OptimalityType>, boost::optional<storm::logic::ComparisonType>, boost::optional<double>> const& operatorInformation, std::shared_ptr<storm::logic::Formula> const& subformula) {
            return std::shared_ptr<storm::logic::Formula>(new storm::logic::ProbabilityOperatorFormula(std::get<0>(operatorInformation), std::get<1>(operatorInformation), std::get<2>(operatorInformation), subformula));
        }
        
        std::shared_ptr<storm::logic::Formula> FormulaParser::createBinaryBooleanStateFormula(std::shared_ptr<storm::logic::Formula> const& leftSubformula, std::shared_ptr<storm::logic::Formula> const& rightSubformula, storm::logic::BinaryBooleanStateFormula::OperatorType operatorType) {
            return std::shared_ptr<storm::logic::Formula>(new storm::logic::BinaryBooleanStateFormula(operatorType, leftSubformula, rightSubformula));
        }
        
        std::shared_ptr<storm::logic::Formula> FormulaParser::createUnaryBooleanStateFormula(std::shared_ptr<storm::logic::Formula> const& subformula, boost::optional<storm::logic::UnaryBooleanStateFormula::OperatorType> const& operatorType) {
            if (operatorType) {
                return std::shared_ptr<storm::logic::Formula>(new storm::logic::UnaryBooleanStateFormula(operatorType.get(), subformula));
            } else {
                return subformula;
            }
        }

    } // namespace parser
} // namespace storm
