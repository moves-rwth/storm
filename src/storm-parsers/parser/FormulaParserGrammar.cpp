#include "FormulaParserGrammar.h"
#include "storm/storage/expressions/ExpressionManager.h"

namespace storm {
    namespace parser {
        
        FormulaParserGrammar::FormulaParserGrammar(std::shared_ptr<storm::expressions::ExpressionManager const> const& manager) : FormulaParserGrammar::base_type(start), constManager(manager), manager(nullptr), expressionParser(*manager, keywords_, true, true), propertyCount(0) {
            initialize();
        }

        FormulaParserGrammar::FormulaParserGrammar(std::shared_ptr<storm::expressions::ExpressionManager> const& manager) : FormulaParserGrammar::base_type(start), constManager(manager), manager(manager), expressionParser(*manager, keywords_, true, true), propertyCount(0) {
            initialize();
        }

        void FormulaParserGrammar::initialize() {
            // Register all variables so we can parse them in the expressions.
            for (auto variableTypePair : *constManager) {
                identifiers_.add(variableTypePair.first.getName(), variableTypePair.first);
            }            
            // Set the identifier mapping to actually generate expressions.
            expressionParser.setIdentifierMapping(&identifiers_);
            
            longRunAverageRewardFormula = (qi::lit("LRA") | qi::lit("S") | qi::lit("MP"))[qi::_val = phoenix::bind(&FormulaParserGrammar::createLongRunAverageRewardFormula, phoenix::ref(*this))];
            longRunAverageRewardFormula.name("long run average reward formula");
            
            instantaneousRewardFormula = (qi::lit("I=") > expressionParser)[qi::_val = phoenix::bind(&FormulaParserGrammar::createInstantaneousRewardFormula, phoenix::ref(*this), qi::_1)];
            instantaneousRewardFormula.name("instantaneous reward formula");
            
            cumulativeRewardFormula = (qi::lit("C") >> timeBounds)[qi::_val = phoenix::bind(&FormulaParserGrammar::createCumulativeRewardFormula, phoenix::ref(*this), qi::_1)];
            cumulativeRewardFormula.name("cumulative reward formula");
            
            totalRewardFormula = (qi::lit("C"))[qi::_val = phoenix::bind(&FormulaParserGrammar::createTotalRewardFormula, phoenix::ref(*this))];
            totalRewardFormula.name("total reward formula");
            
            rewardPathFormula = longRunAverageRewardFormula | conditionalFormula(storm::logic::FormulaContext::Reward) | eventuallyFormula(storm::logic::FormulaContext::Reward) | cumulativeRewardFormula | instantaneousRewardFormula | totalRewardFormula;
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
            
            atomicStateFormulaWithoutExpression = booleanLiteralFormula | labelFormula | (qi::lit("(") > stateFormula > qi::lit(")")) | operatorFormula;
            atomicStateFormula.name("atomic state formula without expression");
            
            notStateFormula = (unaryBooleanOperator_ >> atomicStateFormulaWithoutExpression)[qi::_val = phoenix::bind(&FormulaParserGrammar::createUnaryBooleanStateFormula, phoenix::ref(*this), qi::_2, qi::_1)] | atomicStateFormula[qi::_val = qi::_1];
            notStateFormula.name("negation formula");
            
            eventuallyFormula = (qi::lit("F") >> (-timeBounds) >> pathFormulaWithoutUntil(qi::_r1))[qi::_val = phoenix::bind(&FormulaParserGrammar::createEventuallyFormula, phoenix::ref(*this), qi::_1, qi::_r1, qi::_2)];
            eventuallyFormula.name("eventually formula");
            
            globallyFormula = (qi::lit("G") >> pathFormulaWithoutUntil(qi::_r1))[qi::_val = phoenix::bind(&FormulaParserGrammar::createGloballyFormula, phoenix::ref(*this), qi::_1)];
            globallyFormula.name("globally formula");
            
            nextFormula = (qi::lit("X") >> pathFormulaWithoutUntil(qi::_r1))[qi::_val = phoenix::bind(&FormulaParserGrammar::createNextFormula, phoenix::ref(*this), qi::_1)];
            nextFormula.name("next formula");
            
            pathFormulaWithoutUntil = eventuallyFormula(qi::_r1) | globallyFormula(qi::_r1) | nextFormula(qi::_r1) | stateFormula;
            pathFormulaWithoutUntil.name("path formula");
            
            untilFormula = pathFormulaWithoutUntil(qi::_r1)[qi::_val = qi::_1] >> *(qi::lit("U") >> (-timeBounds) >> pathFormulaWithoutUntil(qi::_r1))[qi::_val = phoenix::bind(&FormulaParserGrammar::createUntilFormula, phoenix::ref(*this), qi::_val, qi::_1, qi::_2)];
            untilFormula.name("until formula");
            
            conditionalFormula = untilFormula(qi::_r1)[qi::_val = qi::_1] >> *(qi::lit("||") >> untilFormula(storm::logic::FormulaContext::Probability))[qi::_val = phoenix::bind(&FormulaParserGrammar::createConditionalFormula, phoenix::ref(*this), qi::_val, qi::_1, qi::_r1)];
            conditionalFormula.name("conditional formula");
            
            timeBoundReference = (-qi::lit("rew") >> rewardModelName)[qi::_val = phoenix::bind(&FormulaParserGrammar::createTimeBoundReference, phoenix::ref(*this), storm::logic::TimeBoundType::Reward, qi::_1)]
                                 | (qi::lit("steps"))[qi::_val = phoenix::bind(&FormulaParserGrammar::createTimeBoundReference, phoenix::ref(*this), storm::logic::TimeBoundType::Steps, boost::none)]
                                 | (-qi::lit("time"))[qi::_val = phoenix::bind(&FormulaParserGrammar::createTimeBoundReference, phoenix::ref(*this), storm::logic::TimeBoundType::Time, boost::none)];
            timeBoundReference.name("time bound reference");
            
            timeBound = ((timeBoundReference >> qi::lit("[")) > expressionParser > qi::lit(",") > expressionParser > qi::lit("]"))
                            [qi::_val = phoenix::bind(&FormulaParserGrammar::createTimeBoundFromInterval, phoenix::ref(*this), qi::_2, qi::_3, qi::_1)]
                        | ( timeBoundReference >> (qi::lit("<=")[qi::_a = true, qi::_b = false] | qi::lit("<")[qi::_a = true, qi::_b = true] | qi::lit(">=")[qi::_a = false, qi::_b = false] | qi::lit(">")[qi::_a = false, qi::_b = true]) >> expressionParser)
                            [qi::_val = phoenix::bind(&FormulaParserGrammar::createTimeBoundFromSingleBound, phoenix::ref(*this), qi::_2, qi::_a, qi::_b, qi::_1)]
                        | ( timeBoundReference >> qi::lit("=") >> expressionParser)
                            [qi::_val = phoenix::bind(&FormulaParserGrammar::createTimeBoundFromInterval, phoenix::ref(*this), qi::_2, qi::_2, qi::_1)];
            timeBound.name("time bound");
            
            timeBounds = (timeBound % qi::lit(",")) | (((-qi::lit("^") >> qi::lit("{")) >> (timeBound % qi::lit(","))) >> qi::lit("}"));
            timeBounds.name("time bounds");
            
            pathFormula = conditionalFormula(qi::_r1);
            pathFormula.name("path formula");
            
            rewardMeasureType = qi::lit("[") >> rewardMeasureType_ >> qi::lit("]");
            rewardMeasureType.name("reward measure type");
            
            operatorInformation = (-optimalityOperator_[qi::_a = qi::_1] >> ((relationalOperator_[qi::_b = qi::_1] > expressionParser[qi::_c = qi::_1]) | (qi::lit("=") > qi::lit("?"))))[qi::_val = phoenix::bind(&FormulaParserGrammar::createOperatorInformation, phoenix::ref(*this), qi::_a, qi::_b, qi::_c)];
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
            
            multiFormula = (qi::lit("multi") > qi::lit("(") >> ((pathFormula(storm::logic::FormulaContext::Probability) | stateFormula) % qi::lit(",")) >> qi::lit(")"))[qi::_val = phoenix::bind(&FormulaParserGrammar::createMultiFormula, phoenix::ref(*this), qi::_1)];
            multiFormula.name("Multi formula");

            identifier %= qi::as_string[qi::raw[qi::lexeme[((qi::alpha | qi::char_('_') | qi::char_('.')) >> *(qi::alnum | qi::char_('_')))]]];
            identifier.name("identifier");
            
            quantileBoundVariable = (-(qi::lit("min")[qi::_a = storm::solver::OptimizationDirection::Minimize] | qi::lit("max")[qi::_a = storm::solver::OptimizationDirection::Maximize]) >> identifier >> qi::lit(","))[qi::_val = phoenix::bind(&FormulaParserGrammar::createQuantileBoundVariables, phoenix::ref(*this), qi::_a,  qi::_1)];
            quantileBoundVariable.name("quantile bound variable");
            quantileFormula = (qi::lit("quantile") > qi::lit("(") >> *(quantileBoundVariable) >> stateFormula > qi::lit(")"))[qi::_val = phoenix::bind(&FormulaParserGrammar::createQuantileFormula, phoenix::ref(*this), qi::_1, qi::_2)];
            quantileFormula.name("Quantile formula");
            
            stateFormula = (orStateFormula | multiFormula | quantileFormula);
            stateFormula.name("state formula");
            
            formulaName = qi::lit("\"") >> identifier >> qi::lit("\"") >> qi::lit(":");
            formulaName.name("formula name");
            
            constantDefinition = (qi::lit("const") > -(qi::lit("int")[qi::_a = ConstantDataType::Integer] | qi::lit("bool")[qi::_a = ConstantDataType::Bool] | qi::lit("double")[qi::_a = ConstantDataType::Rational]) >> identifier >> -(qi::lit("=") > expressionParser))[phoenix::bind(&FormulaParserGrammar::addConstant, phoenix::ref(*this), qi::_1, qi::_a, qi::_2)];
            constantDefinition.name("constant definition");
            
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Woverloaded-shift-op-parentheses"
            
            filterProperty = (-formulaName >> qi::lit("filter") > qi::lit("(") > filterType_ > qi::lit(",") > stateFormula > qi::lit(",") > stateFormula > qi::lit(")"))[qi::_val = phoenix::bind(&FormulaParserGrammar::createProperty, phoenix::ref(*this), qi::_1, qi::_2, qi::_3, qi::_4)] | (-formulaName >> stateFormula)[qi::_val = phoenix::bind(&FormulaParserGrammar::createPropertyWithDefaultFilterTypeAndStates, phoenix::ref(*this), qi::_1, qi::_2)];
            filterProperty.name("filter property");

#pragma clang diagnostic pop

            start = (qi::eps >> filterProperty[phoenix::push_back(qi::_val, qi::_1)]
                     | qi::eps(phoenix::bind(&FormulaParserGrammar::areConstantDefinitionsAllowed, phoenix::ref(*this))) >> constantDefinition
                     | qi::eps)
                     % +(qi::char_("\n;")) >> qi::skip(storm::spirit_encoding::space_type() | qi::lit("//") >> *(qi::char_ - (qi::eol | qi::eoi)))[qi::eps] >> qi::eoi;
            start.name("start");
            
            // Enable the following lines to print debug output for most the rules.
//            debug(start);
//            debug(constantDefinition);
//            debug(stateFormula);
//            debug(orStateFormula);
//            debug(andStateFormula);
//            debug(probabilityOperator);
//            debug(rewardOperator);
//            debug(longRunAverageOperator);
//            debug(timeOperator);
//            debug(pathFormulaWithoutUntil);
//            debug(pathFormula);
//            debug(conditionalFormula);
//            debug(nextFormula);
//            debug(globallyFormula);
//            debug(eventuallyFormula);
//            debug(atomicStateFormula);
//            debug(booleanLiteralFormula);
//            debug(labelFormula);
//            debug(expressionFormula);
//            debug(rewardPathFormula);
//            debug(cumulativeRewardFormula);
//            debug(totalRewardFormula);
//            debug(instantaneousRewardFormula);
//            debug(multiFormula);
            
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
            qi::on_error<qi::fail>(totalRewardFormula, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(instantaneousRewardFormula, handler(qi::_1, qi::_2, qi::_3, qi::_4));
            qi::on_error<qi::fail>(multiFormula, handler(qi::_1, qi::_2, qi::_3, qi::_4));
        }
        
        void FormulaParserGrammar::addIdentifierExpression(std::string const& identifier, storm::expressions::Expression const& expression) {
            this->identifiers_.add(identifier, expression);
        }
        
        void FormulaParserGrammar::addConstant(std::string const& name, ConstantDataType type, boost::optional<storm::expressions::Expression> const& expression) {
            STORM_LOG_ASSERT(manager, "Mutable expression manager required to define new constants.");
            storm::expressions::Variable newVariable;
            STORM_LOG_THROW(!manager->hasVariable(name), storm::exceptions::WrongFormatException, "Invalid constant definition '" << name << "' in property: variable already exists.");
            
            if (type == ConstantDataType::Bool) {
                newVariable = manager->declareBooleanVariable(name);
            } else if (type == ConstantDataType::Integer)  {
                newVariable = manager->declareIntegerVariable(name);
            } else {
                newVariable = manager->declareRationalVariable(name);
            }
            
            if (expression) {
                addIdentifierExpression(name, expression.get());
            } else {
                undefinedConstants.insert(newVariable);
                addIdentifierExpression(name, newVariable);
            }
        }
        
        bool FormulaParserGrammar::areConstantDefinitionsAllowed() const {
            return static_cast<bool>(manager);
        }
        
        std::shared_ptr<storm::logic::TimeBoundReference> FormulaParserGrammar::createTimeBoundReference(storm::logic::TimeBoundType const& type, boost::optional<std::string> const& rewardModelName) const {
            if (type == storm::logic::TimeBoundType::Reward) {
                STORM_LOG_THROW(rewardModelName, storm::exceptions::WrongFormatException, "Reward bound does not specify a reward model name.");
                return std::make_shared<storm::logic::TimeBoundReference>(rewardModelName.get());
            } else {
                return std::make_shared<storm::logic::TimeBoundReference>(type);
            }
        }
        
        std::tuple<boost::optional<storm::logic::TimeBound>, boost::optional<storm::logic::TimeBound>, std::shared_ptr<storm::logic::TimeBoundReference>> FormulaParserGrammar::createTimeBoundFromInterval(storm::expressions::Expression const& lowerBound, storm::expressions::Expression const& upperBound, std::shared_ptr<storm::logic::TimeBoundReference> const& timeBoundReference) const {
            // As soon as it somehow does not break everything anymore, I will change return types here.

            storm::logic::TimeBound lower(false, lowerBound);
            storm::logic::TimeBound upper(false, upperBound);
            return std::make_tuple(lower, upper, timeBoundReference);
        }
        
        std::tuple<boost::optional<storm::logic::TimeBound>, boost::optional<storm::logic::TimeBound>, std::shared_ptr<storm::logic::TimeBoundReference>> FormulaParserGrammar::createTimeBoundFromSingleBound(storm::expressions::Expression const& bound, bool upperBound, bool strict, std::shared_ptr<storm::logic::TimeBoundReference> const& timeBoundReference) const {
            // As soon as it somehow does not break everything anymore, I will change return types here.
            if (upperBound) {
                return std::make_tuple(boost::none, storm::logic::TimeBound(strict, bound), timeBoundReference);
            } else {
                return std::make_tuple(storm::logic::TimeBound(strict, bound), boost::none, timeBoundReference);
            }
        }
        
        std::shared_ptr<storm::logic::Formula const> FormulaParserGrammar::createInstantaneousRewardFormula(storm::expressions::Expression const& timeBound) const {
            return std::shared_ptr<storm::logic::Formula const>(new storm::logic::InstantaneousRewardFormula(timeBound));
        }
        
        std::shared_ptr<storm::logic::Formula const> FormulaParserGrammar::createCumulativeRewardFormula(std::vector<std::tuple<boost::optional<storm::logic::TimeBound>, boost::optional<storm::logic::TimeBound>, std::shared_ptr<storm::logic::TimeBoundReference>>> const& timeBounds) const {
            std::vector<storm::logic::TimeBound> bounds;
            std::vector<storm::logic::TimeBoundReference> timeBoundReferences;
            for (auto const& timeBound : timeBounds) {
                STORM_LOG_THROW(!std::get<0>(timeBound), storm::exceptions::WrongFormatException, "Cumulative reward formulas with lower time bound are not allowed.");
                STORM_LOG_THROW(std::get<1>(timeBound), storm::exceptions::WrongFormatException, "Cumulative reward formulas require an upper bound.");
                bounds.push_back(std::get<1>(timeBound).get());
                timeBoundReferences.emplace_back(*std::get<2>(timeBound));
            }
            return std::shared_ptr<storm::logic::Formula const>(new storm::logic::CumulativeRewardFormula(bounds, timeBoundReferences));
        }
        
        std::shared_ptr<storm::logic::Formula const> FormulaParserGrammar::createTotalRewardFormula() const {
            return std::shared_ptr<storm::logic::Formula const>(new storm::logic::TotalRewardFormula());
        }
        
        std::shared_ptr<storm::logic::Formula const> FormulaParserGrammar::createLongRunAverageRewardFormula() const {
            return std::shared_ptr<storm::logic::Formula const>(new storm::logic::LongRunAverageRewardFormula());
        }
        
        std::shared_ptr<storm::logic::Formula const> FormulaParserGrammar::createAtomicExpressionFormula(storm::expressions::Expression const& expression) const {
            STORM_LOG_THROW(expression.hasBooleanType(), storm::exceptions::WrongFormatException, "Expected expression of boolean type.");
            return std::shared_ptr<storm::logic::Formula const>(new storm::logic::AtomicExpressionFormula(expression));
        }
        
        std::shared_ptr<storm::logic::Formula const> FormulaParserGrammar::createBooleanLiteralFormula(bool literal) const {
            return std::shared_ptr<storm::logic::Formula const>(new storm::logic::BooleanLiteralFormula(literal));
        }
        
        std::shared_ptr<storm::logic::Formula const> FormulaParserGrammar::createAtomicLabelFormula(std::string const& label) const {
            return std::shared_ptr<storm::logic::Formula const>(new storm::logic::AtomicLabelFormula(label));
        }
        
        std::shared_ptr<storm::logic::Formula const> FormulaParserGrammar::createEventuallyFormula(boost::optional<std::vector<std::tuple<boost::optional<storm::logic::TimeBound>, boost::optional<storm::logic::TimeBound>, std::shared_ptr<storm::logic::TimeBoundReference>>>> const& timeBounds, storm::logic::FormulaContext context, std::shared_ptr<storm::logic::Formula const> const& subformula) const {
            if (timeBounds && !timeBounds.get().empty()) {
                std::vector<boost::optional<storm::logic::TimeBound>> lowerBounds, upperBounds;
                std::vector<storm::logic::TimeBoundReference> timeBoundReferences;
                for (auto const& timeBound : timeBounds.get()) {
                    lowerBounds.push_back(std::get<0>(timeBound));
                    upperBounds.push_back(std::get<1>(timeBound));
                    timeBoundReferences.emplace_back(*std::get<2>(timeBound));
                }
                return std::shared_ptr<storm::logic::Formula const>(new storm::logic::BoundedUntilFormula(createBooleanLiteralFormula(true), subformula, lowerBounds, upperBounds, timeBoundReferences));
            } else {
                return std::shared_ptr<storm::logic::Formula const>(new storm::logic::EventuallyFormula(subformula, context));
            }
        }
        
        std::shared_ptr<storm::logic::Formula const> FormulaParserGrammar::createGloballyFormula(std::shared_ptr<storm::logic::Formula const> const& subformula) const {
            return std::shared_ptr<storm::logic::Formula const>(new storm::logic::GloballyFormula(subformula));
        }
        
        std::shared_ptr<storm::logic::Formula const> FormulaParserGrammar::createNextFormula(std::shared_ptr<storm::logic::Formula const> const& subformula) const {
            return std::shared_ptr<storm::logic::Formula const>(new storm::logic::NextFormula(subformula));
        }
        
        std::shared_ptr<storm::logic::Formula const> FormulaParserGrammar::createUntilFormula(std::shared_ptr<storm::logic::Formula const> const& leftSubformula, boost::optional<std::vector<std::tuple<boost::optional<storm::logic::TimeBound>, boost::optional<storm::logic::TimeBound>, std::shared_ptr<storm::logic::TimeBoundReference>>>> const& timeBounds, std::shared_ptr<storm::logic::Formula const> const& rightSubformula) {
            if (timeBounds && !timeBounds.get().empty()) {
                std::vector<boost::optional<storm::logic::TimeBound>> lowerBounds, upperBounds;
                std::vector<storm::logic::TimeBoundReference> timeBoundReferences;
                for (auto const& timeBound : timeBounds.get()) {
                    lowerBounds.push_back(std::get<0>(timeBound));
                    upperBounds.push_back(std::get<1>(timeBound));
                    timeBoundReferences.emplace_back(*std::get<2>(timeBound));
                }
                return std::shared_ptr<storm::logic::Formula const>(new storm::logic::BoundedUntilFormula(leftSubformula, rightSubformula, lowerBounds, upperBounds, timeBoundReferences));
            } else {
                return std::shared_ptr<storm::logic::Formula const>(new storm::logic::UntilFormula(leftSubformula, rightSubformula));
            }
        }
        
        std::shared_ptr<storm::logic::Formula const> FormulaParserGrammar::createConditionalFormula(std::shared_ptr<storm::logic::Formula const> const& leftSubformula, std::shared_ptr<storm::logic::Formula const> const& rightSubformula, storm::logic::FormulaContext context) const {
            return std::shared_ptr<storm::logic::Formula const>(new storm::logic::ConditionalFormula(leftSubformula, rightSubformula, context));
        }
        
        storm::logic::OperatorInformation FormulaParserGrammar::createOperatorInformation(boost::optional<storm::OptimizationDirection> const& optimizationDirection, boost::optional<storm::logic::ComparisonType> const& comparisonType, boost::optional<storm::expressions::Expression> const& threshold) const {
            if (comparisonType && threshold) {
                storm::expressions::ExpressionEvaluator<storm::RationalNumber> evaluator(*constManager);
                return storm::logic::OperatorInformation(optimizationDirection, storm::logic::Bound(comparisonType.get(), threshold.get()));
            } else {
                return storm::logic::OperatorInformation(optimizationDirection, boost::none);
            }
        }
        
        std::shared_ptr<storm::logic::Formula const> FormulaParserGrammar::createLongRunAverageOperatorFormula(storm::logic::OperatorInformation const& operatorInformation, std::shared_ptr<storm::logic::Formula const> const& subformula) const {
            return std::shared_ptr<storm::logic::Formula const>(new storm::logic::LongRunAverageOperatorFormula(subformula, operatorInformation));
        }
        
        std::shared_ptr<storm::logic::Formula const> FormulaParserGrammar::createRewardOperatorFormula(boost::optional<storm::logic::RewardMeasureType> const& rewardMeasureType, boost::optional<std::string> const& rewardModelName, storm::logic::OperatorInformation const& operatorInformation, std::shared_ptr<storm::logic::Formula const> const& subformula) const {
            storm::logic::RewardMeasureType measureType = storm::logic::RewardMeasureType::Expectation;
            if (rewardMeasureType) {
                measureType = rewardMeasureType.get();
            }
            return std::shared_ptr<storm::logic::Formula const>(new storm::logic::RewardOperatorFormula(subformula, rewardModelName, operatorInformation, measureType));
        }
        
        std::shared_ptr<storm::logic::Formula const> FormulaParserGrammar::createTimeOperatorFormula(boost::optional<storm::logic::RewardMeasureType> const& rewardMeasureType, storm::logic::OperatorInformation const& operatorInformation, std::shared_ptr<storm::logic::Formula const> const& subformula) const {
            storm::logic::RewardMeasureType measureType = storm::logic::RewardMeasureType::Expectation;
            if (rewardMeasureType) {
                measureType = rewardMeasureType.get();
            }
            return std::shared_ptr<storm::logic::Formula const>(new storm::logic::TimeOperatorFormula(subformula, operatorInformation, measureType));
        }
        
        std::shared_ptr<storm::logic::Formula const> FormulaParserGrammar::createProbabilityOperatorFormula(storm::logic::OperatorInformation const& operatorInformation, std::shared_ptr<storm::logic::Formula const> const& subformula) {
            return std::shared_ptr<storm::logic::Formula const>(new storm::logic::ProbabilityOperatorFormula(subformula, operatorInformation));
        }
        
        std::shared_ptr<storm::logic::Formula const> FormulaParserGrammar::createBinaryBooleanStateFormula(std::shared_ptr<storm::logic::Formula const> const& leftSubformula, std::shared_ptr<storm::logic::Formula const> const& rightSubformula, storm::logic::BinaryBooleanStateFormula::OperatorType operatorType) {
            return std::shared_ptr<storm::logic::Formula const>(new storm::logic::BinaryBooleanStateFormula(operatorType, leftSubformula, rightSubformula));
        }
        
        std::shared_ptr<storm::logic::Formula const> FormulaParserGrammar::createUnaryBooleanStateFormula(std::shared_ptr<storm::logic::Formula const> const& subformula, boost::optional<storm::logic::UnaryBooleanStateFormula::OperatorType> const& operatorType) {
            if (operatorType) {
                return std::shared_ptr<storm::logic::Formula const>(new storm::logic::UnaryBooleanStateFormula(operatorType.get(), subformula));
            } else {
                return subformula;
            }
        }
        
        std::shared_ptr<storm::logic::Formula const> FormulaParserGrammar::createMultiFormula(std::vector<std::shared_ptr<storm::logic::Formula const>> const& subformulas) {
            bool isMultiDimensionalBoundedUntilFormula = !subformulas.empty();
            for (auto const& subformula : subformulas) {
                if (!subformula->isBoundedUntilFormula()) {
                    isMultiDimensionalBoundedUntilFormula = false;
                    break;
                }
            }
            
            if (isMultiDimensionalBoundedUntilFormula) {
                std::vector<std::shared_ptr<storm::logic::Formula const>> leftSubformulas, rightSubformulas;
                std::vector<boost::optional<storm::logic::TimeBound>> lowerBounds, upperBounds;
                std::vector<storm::logic::TimeBoundReference> timeBoundReferences;
                for (auto const& subformula : subformulas) {
                    auto const& f = subformula->asBoundedUntilFormula();
                    STORM_LOG_THROW(!f.isMultiDimensional(), storm::exceptions::WrongFormatException, "Composition of multidimensional bounded until formula must consist of single dimension subformulas. Got '" << f << "' instead.");
                    leftSubformulas.push_back(f.getLeftSubformula().asSharedPointer());
                    rightSubformulas.push_back(f.getRightSubformula().asSharedPointer());
                    if (f.hasLowerBound()) {
                        lowerBounds.emplace_back(storm::logic::TimeBound(f.isLowerBoundStrict(), f.getLowerBound()));
                    } else {
                        lowerBounds.emplace_back();
                    }
                     if (f.hasUpperBound()) {
                        upperBounds.emplace_back(storm::logic::TimeBound(f.isUpperBoundStrict(), f.getUpperBound()));
                    } else {
                        upperBounds.emplace_back();
                    }
                    timeBoundReferences.push_back(f.getTimeBoundReference());
                }
                return std::shared_ptr<storm::logic::Formula const>(new storm::logic::BoundedUntilFormula(leftSubformulas, rightSubformulas, lowerBounds, upperBounds, timeBoundReferences));
            } else {
                return std::shared_ptr<storm::logic::Formula const>(new storm::logic::MultiObjectiveFormula(subformulas));
            }
        }
        
        storm::expressions::Variable FormulaParserGrammar::createQuantileBoundVariables(boost::optional<storm::solver::OptimizationDirection> const& dir, std::string const& variableName) {
            STORM_LOG_ASSERT(manager, "Mutable expression manager required to define quantile bound variable.");
            storm::expressions::Variable var;
            if (manager->hasVariable(variableName)) {
                var = manager->getVariable(variableName);
                STORM_LOG_THROW(quantileFormulaVariables.count(var) > 0, storm::exceptions::WrongFormatException, "Invalid quantile variable name '" << variableName << "' in quantile formula: variable already exists.");
            } else {
                var = manager->declareRationalVariable(variableName);
                quantileFormulaVariables.insert(var);
            }
            STORM_LOG_WARN_COND(!dir.is_initialized(), "Optimization direction '" << dir.get() << "' for quantile variable " << variableName << " is ignored. This information will be derived from the subformula of the quantile.");
            addIdentifierExpression(variableName, var);
            return var;
        }

        std::shared_ptr<storm::logic::Formula const> FormulaParserGrammar::createQuantileFormula(std::vector<storm::expressions::Variable> const& boundVariables, std::shared_ptr<storm::logic::Formula const> const& subformula) {
            return std::shared_ptr<storm::logic::Formula const>(new storm::logic::QuantileFormula(boundVariables, subformula));
        }
        
        std::set<storm::expressions::Variable> FormulaParserGrammar::getUndefinedConstants(std::shared_ptr<storm::logic::Formula const> const& formula) const {
            std::set<storm::expressions::Variable> result;
            std::set<storm::expressions::Variable> usedVariables = formula->getUsedVariables();
            std::set_intersection(usedVariables.begin(), usedVariables.end(), undefinedConstants.begin(), undefinedConstants.end(), std::inserter(result, result.begin()));
            return result;
        }
        
        storm::jani::Property FormulaParserGrammar::createProperty(boost::optional<std::string> const& propertyName, storm::modelchecker::FilterType const& filterType, std::shared_ptr<storm::logic::Formula const> const& formula, std::shared_ptr<storm::logic::Formula const> const& states) {
            storm::jani::FilterExpression filterExpression(formula, filterType, states);
            
            ++propertyCount;
            if (propertyName) {
                return storm::jani::Property(propertyName.get(), filterExpression, this->getUndefinedConstants(formula));
            } else {
                return storm::jani::Property(std::to_string(propertyCount - 1), filterExpression, this->getUndefinedConstants(formula));
            }
        }
                                               
        storm::jani::Property FormulaParserGrammar::createPropertyWithDefaultFilterTypeAndStates(boost::optional<std::string> const& propertyName, std::shared_ptr<storm::logic::Formula const> const& formula) {
            ++propertyCount;
            if (propertyName) {
                return storm::jani::Property(propertyName.get(), formula, this->getUndefinedConstants(formula));
            } else {
                return storm::jani::Property(std::to_string(propertyCount), formula, this->getUndefinedConstants(formula));
            }
        }

    }
}
