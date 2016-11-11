#include "JSONExporter.h"

#include <iostream>
#include <fstream>
#include <vector>

#include "src/utility/macros.h"
#include "src/exceptions/FileIoException.h"
#include "src/exceptions/NotSupportedException.h"

#include "src/exceptions/InvalidJaniException.h"
#include "src/exceptions/NotImplementedException.h"

#include "src/storage/expressions/RationalLiteralExpression.h"
#include "src/storage/expressions/IntegerLiteralExpression.h"
#include "src/storage/expressions/BooleanLiteralExpression.h"
#include "src/storage/expressions/UnaryBooleanFunctionExpression.h"
#include "src/storage/expressions/UnaryNumericalFunctionExpression.h"
#include "src/storage/expressions/BinaryBooleanFunctionExpression.h"
#include "src/storage/expressions/BinaryNumericalFunctionExpression.h"
#include "src/storage/expressions/IfThenElseExpression.h"
#include "src/storage/expressions/BinaryRelationExpression.h"
#include "src/storage/expressions/VariableExpression.h"

#include "src/logic/Formulas.h"

#include "src/storage/jani/AutomatonComposition.h"
#include "src/storage/jani/ParallelComposition.h"
#include "src/storage/jani/Property.h"

namespace storm {
    namespace jani {
        
        
        class CompositionJsonExporter : public CompositionVisitor {
        public:
            CompositionJsonExporter(bool allowRecursion) : allowRecursion(allowRecursion){
                
            }
            
            static modernjson::json translate(storm::jani::Composition const& comp, bool allowRecursion = true) {
                CompositionJsonExporter visitor(allowRecursion);
                return boost::any_cast<modernjson::json>(comp.accept(visitor, boost::none));
            }
            
            virtual boost::any visit(AutomatonComposition const& composition, boost::any const& data) {
                modernjson::json compDecl;
                modernjson::json autDecl;
                autDecl["automaton"] = composition.getAutomatonName();
                std::vector<modernjson::json> elements;
                elements.push_back(autDecl);
                compDecl["elements"] = elements;
                return compDecl;
            }
            
            virtual boost::any visit(ParallelComposition const& composition, boost::any const& data) {
                modernjson::json compDecl;
                
                std::vector<modernjson::json> elems;
                for (auto const& subcomp : composition.getSubcompositions()) {
                    modernjson::json elemDecl;
                    if (subcomp->isAutomaton()) {
                        elemDecl["automaton"] = std::static_pointer_cast<AutomatonComposition>(subcomp)->getAutomatonName();
                    } else {
                        STORM_LOG_THROW(allowRecursion, storm::exceptions::InvalidJaniException, "Nesting composition " << *subcomp << " is not supported by JANI.");
                        elemDecl = boost::any_cast<modernjson::json>(subcomp->accept(*this, boost::none));
                    }
                    elems.push_back(elemDecl);
                }
                compDecl["elements"] = elems;
                std::vector<modernjson::json> synElems;
                for (auto const& syncs : composition.getSynchronizationVectors()) {
                    modernjson::json syncDecl;
                    syncDecl["synchronise"] = std::vector<std::string>();
                    for (auto const& syncIn : syncs.getInput()) {
                        if (syncIn == SynchronizationVector::NO_ACTION_INPUT) {
                            syncDecl["synchronise"].push_back(nullptr);
                        } else {
                            syncDecl["synchronise"].push_back(syncIn);
                        }
                    }
                    syncDecl["result"] = syncs.getOutput();
                    synElems.push_back(syncDecl);
                }
                if (!synElems.empty()) {
                    compDecl["syncs"] = synElems;
                }
                
                return compDecl;
            }
            
        private:
            bool allowRecursion;
        };
        
        std::string comparisonTypeToJani(storm::logic::ComparisonType ct) {
            switch(ct) {
                case storm::logic::ComparisonType::Less:
                    return "<";
                case storm::logic::ComparisonType::LessEqual:
                    return "≤";
                case storm::logic::ComparisonType::Greater:
                    return ">";
                case storm::logic::ComparisonType::GreaterEqual:
                    return "≥";
                default:
                    assert(false);
            }
        }
        
        modernjson::json numberToJson(storm::RationalNumber rn) {
            modernjson::json numDecl;
            numDecl = storm::utility::convertNumber<double>(rn);
//            if(carl::isOne(carl::getDenom(rn))) {
//                numDecl = modernjson::json(carl::toString(carl::getNum(rn)));
//            } else {
//                numDecl["op"] = "/";
//                // TODO set json lib to work with arbitrary precision ints.
//                assert(carl::toInt<int64_t>(carl::getNum(rn)) == carl::getNum(rn));
//                assert(carl::toInt<int64_t>(carl::getDenom(rn)) == carl::getDenom(rn));
//                numDecl["left"] = carl::toInt<int64_t>(carl::getNum(rn));
//                numDecl["right"] = carl::toInt<int64_t>(carl::getDenom(rn));
//            }
            return numDecl;
        }
        
        
        modernjson::json constructPropertyInterval(uint64_t lower, uint64_t upper) {
            modernjson::json iDecl;
            iDecl["lower"] = lower;
            iDecl["upper"] = upper;
            return iDecl;
        }
        
        modernjson::json constructPropertyInterval(double lower, double upper) {
            modernjson::json iDecl;
            iDecl["lower"] = lower;
            iDecl["upper"] = upper;
            return iDecl;
        }
        
        modernjson::json FormulaToJaniJson::translate(storm::logic::Formula const& formula, bool continuousTime) {
            FormulaToJaniJson translator(continuousTime);
            return boost::any_cast<modernjson::json>(formula.accept(translator));
        }
        
        boost::any FormulaToJaniJson::visit(storm::logic::AtomicExpressionFormula const& f, boost::any const& data) const {
            return ExpressionToJson::translate(f.getExpression());
        }
        
        boost::any FormulaToJaniJson::visit(storm::logic::AtomicLabelFormula const& f, boost::any const& data) const {
            modernjson::json opDecl(f.getLabel());
            return opDecl;
        }
        boost::any FormulaToJaniJson::visit(storm::logic::BinaryBooleanStateFormula const& f, boost::any const& data) const{
            modernjson::json opDecl;
            storm::logic::BinaryBooleanStateFormula::OperatorType op = f.getOperator();
            opDecl["op"] = op == storm::logic::BinaryBooleanStateFormula::OperatorType::And ? "∧" : "∨";
            opDecl["left"] = boost::any_cast<modernjson::json>(f.getLeftSubformula().accept(*this, boost::none));
            opDecl["right"] = boost::any_cast<modernjson::json>(f.getRightSubformula().accept(*this, boost::none));
            return opDecl;
        }
        boost::any FormulaToJaniJson::visit(storm::logic::BooleanLiteralFormula const& f, boost::any const& data) const {
            modernjson::json opDecl(f.isTrueFormula() ? true : false);
            return opDecl;
        }
        boost::any FormulaToJaniJson::visit(storm::logic::BoundedUntilFormula const& f, boost::any const& data) const {
            modernjson::json opDecl;
            opDecl["op"] = "U";
            opDecl["left"] = boost::any_cast<modernjson::json>(f.getLeftSubformula().accept(*this, boost::none));
            opDecl["right"] = boost::any_cast<modernjson::json>(f.getRightSubformula().accept(*this, boost::none));
            if(f.hasDiscreteTimeBound()) {
                opDecl["step-bounds"] = constructPropertyInterval(0, f.getDiscreteTimeBound());
            } else {
                opDecl["time-bounds"] = constructPropertyInterval(f.getIntervalBounds().first, f.getIntervalBounds().second);
            }
            return opDecl;
        }
        
        boost::any FormulaToJaniJson::visit(storm::logic::ConditionalFormula const& f, boost::any const& data) const {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Jani currently does not support conditional formulae");
        }
        
        boost::any FormulaToJaniJson::visit(storm::logic::CumulativeRewardFormula const& f, boost::any const& data) const {
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Storm currently does not support translating  cummulative reward formulae");
        }
        
        boost::any FormulaToJaniJson::visit(storm::logic::EventuallyFormula const& f, boost::any const& data) const {
            modernjson::json opDecl;
            opDecl["op"] = "U";
            opDecl["left"] = boost::any_cast<modernjson::json>(f.getTrueFormula()->accept(*this, boost::none));
            opDecl["right"] = boost::any_cast<modernjson::json>(f.getSubformula().accept(*this, boost::none));
            return opDecl;
        }
        
        boost::any FormulaToJaniJson::visit(storm::logic::TimeOperatorFormula const& f, boost::any const& data) const {
            modernjson::json opDecl;
            std::vector<std::string> tvec;
            tvec.push_back("time");
            if(f.hasBound()) {
                auto bound = f.getBound();
                opDecl["op"] = comparisonTypeToJani(bound.comparisonType);
                if(f.hasOptimalityType()) {
                    opDecl["left"]["op"] = f.getOptimalityType() == storm::solver::OptimizationDirection::Minimize ? "Emin" : "Emax";
                    opDecl["left"]["reach"] = boost::any_cast<modernjson::json>(f.getSubformula().accept(*this, boost::none));
                } else {
                    opDecl["left"]["op"] = (bound.comparisonType == storm::logic::ComparisonType::Less || bound.comparisonType == storm::logic::ComparisonType::LessEqual) ? "Emax" : "Emin";
                    opDecl["left"]["reach"] = boost::any_cast<modernjson::json>(f.getSubformula().accept(*this, boost::none));
                }
                opDecl["left"]["exp"] = modernjson::json(1);
                opDecl["left"]["accumulate"] = modernjson::json(tvec);
                opDecl["right"] = numberToJson(bound.threshold);
            } else {
                if(f.hasOptimalityType()) {
                    opDecl["op"] = f.getOptimalityType() == storm::solver::OptimizationDirection::Minimize ? "Emin" : "Emax";
                    opDecl["reach"] = boost::any_cast<modernjson::json>(f.getSubformula().accept(*this, boost::none));
                    
                } else {
                    // TODO add checks
                    opDecl["op"] = "Emin";
                    opDecl["reach"] = boost::any_cast<modernjson::json>(f.getSubformula().accept(*this, boost::none));
                }
                opDecl["exp"] = modernjson::json(1);
                opDecl["accumulate"] = modernjson::json(tvec);
            }
            return opDecl;
        }
        
        boost::any FormulaToJaniJson::visit(storm::logic::GloballyFormula const& f, boost::any const& data) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Jani currently does not support conversion of a globally formulae");
        }
        
        boost::any FormulaToJaniJson::visit(storm::logic::InstantaneousRewardFormula const& f, boost::any const& data) const {
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Jani currently does not support conversion of an instanteneous reward formula");
        }
        
        boost::any FormulaToJaniJson::visit(storm::logic::LongRunAverageOperatorFormula const& f, boost::any const& data) const {
             modernjson::json opDecl;
            if(f.hasBound()) {
                auto bound = f.getBound();
                opDecl["op"] = comparisonTypeToJani(bound.comparisonType);
                if(f.hasOptimalityType()) {
                    opDecl["left"]["op"] = f.getOptimalityType() == storm::solver::OptimizationDirection::Minimize ? "Smin" : "Smax";
                    opDecl["left"]["exp"] = boost::any_cast<modernjson::json>(f.getSubformula().accept(*this, boost::none));
                } else {
                    opDecl["left"]["op"] = (bound.comparisonType == storm::logic::ComparisonType::Less || bound.comparisonType == storm::logic::ComparisonType::LessEqual) ? "Smax" : "Smin";
                    opDecl["left"]["exp"] = boost::any_cast<modernjson::json>(f.getSubformula().accept(*this, boost::none));
                }
                opDecl["right"] = numberToJson(bound.threshold);
            } else {
                if(f.hasOptimalityType()) {
                    opDecl["op"] = f.getOptimalityType() == storm::solver::OptimizationDirection::Minimize ? "Smin" : "Smax";
                    opDecl["exp"] = boost::any_cast<modernjson::json>(f.getSubformula().accept(*this, boost::none));
                    
                } else {
                    // TODO add checks
                    opDecl["op"] = "Pmin";
                    opDecl["exp"] = boost::any_cast<modernjson::json>(f.getSubformula().accept(*this, boost::none));
                }
            }
            return opDecl;

        }
        
        boost::any FormulaToJaniJson::visit(storm::logic::LongRunAverageRewardFormula const& f, boost::any const& data) const {
//            modernjson::json opDecl;
//            if(f.()) {
//                auto bound = f.getBound();
//                opDecl["op"] = comparisonTypeToJani(bound.comparisonType);
//                if(f.hasOptimalityType()) {
//                    opDecl["left"]["op"] = f.getOptimalityType() == storm::solver::OptimizationDirection::Minimize ? "Smin" : "Smax";
//                    opDecl["left"]["exp"] = boost::any_cast<modernjson::json>(f.getSubformula().accept(*this, boost::none));
//                } else {
//                    opDecl["left"]["op"] = (bound.comparisonType == storm::logic::ComparisonType::Less || bound.comparisonType == storm::logic::ComparisonType::LessEqual) ? "Smax" : "Smin";
//                    opDecl["left"]["exp"] = boost::any_cast<modernjson::json>(f.getSubformula().accept(*this, boost::none));
//                }
//                opDecl["right"] = numberToJson(bound.threshold);
//            } else {
//                if(f.hasOptimalityType()) {
//                    opDecl["op"] = f.getOptimalityType() == storm::solver::OptimizationDirection::Minimize ? "Smin" : "Smax";
//                    opDecl["exp"] = boost::any_cast<modernjson::json>(f.getSubformula().accept(*this, boost::none));
//                    
//                } else {
//                    // TODO add checks
//                    opDecl["op"] = "Pmin";
//                    opDecl["exp"] = boost::any_cast<modernjson::json>(f.getSubformula().accept(*this, boost::none));
//                }
//            }
//            return opDecl;
            
            STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Jani currently does not support conversion of an LRA reward formula");
        }
        
        boost::any FormulaToJaniJson::visit(storm::logic::NextFormula const& f, boost::any const& data) const {
            modernjson::json opDecl;
            opDecl["op"] = "U";
            opDecl["left"] = boost::any_cast<modernjson::json>(f.getTrueFormula()->accept(*this, boost::none));
            opDecl["right"] = boost::any_cast<modernjson::json>(f.getSubformula().accept(*this, boost::none));
            opDecl["step-bounds"] = constructPropertyInterval((uint64_t)1, (uint64_t)1);
            return opDecl;
        }
        
      
        
        
        boost::any FormulaToJaniJson::visit(storm::logic::ProbabilityOperatorFormula const& f, boost::any const& data) const {
            modernjson::json opDecl;
            
            if(f.hasBound()) {
                auto bound = f.getBound();
                opDecl["op"] = comparisonTypeToJani(bound.comparisonType);
                if(f.hasOptimalityType()) {
                    opDecl["left"]["op"] = f.getOptimalityType() == storm::solver::OptimizationDirection::Minimize ? "Pmin" : "Pmax";
                    opDecl["left"]["exp"] = boost::any_cast<modernjson::json>(f.getSubformula().accept(*this, boost::none));
                } else {
                    opDecl["left"]["op"] = (bound.comparisonType == storm::logic::ComparisonType::Less || bound.comparisonType == storm::logic::ComparisonType::LessEqual) ? "Pmax" : "Pmin";
                    opDecl["left"]["exp"] = boost::any_cast<modernjson::json>(f.getSubformula().accept(*this, boost::none));
                }
                opDecl["right"] = numberToJson(bound.threshold);
            } else {
                if(f.hasOptimalityType()) {
                    opDecl["op"] = f.getOptimalityType() == storm::solver::OptimizationDirection::Minimize ? "Pmin" : "Pmax";
                    opDecl["exp"] = boost::any_cast<modernjson::json>(f.getSubformula().accept(*this, boost::none));
                    
                } else {
                    // TODO add checks
                    opDecl["op"] = "Pmin";
                    opDecl["exp"] = boost::any_cast<modernjson::json>(f.getSubformula().accept(*this, boost::none));
                }
            }
            return opDecl;
        }
        
        boost::any FormulaToJaniJson::visit(storm::logic::RewardOperatorFormula const& f, boost::any const& data) const {
            modernjson::json opDecl;
            std::vector<std::string> accvec;
            if(continuous) {
                accvec.push_back("time");
            } else {
                accvec.push_back("steps");
            }
            if(f.hasBound()) {
                auto bound = f.getBound();
                opDecl["op"] = comparisonTypeToJani(bound.comparisonType);
                if(f.hasOptimalityType()) {
                    opDecl["left"]["op"] = f.getOptimalityType() == storm::solver::OptimizationDirection::Minimize ? "Emin" : "Emax";
                    opDecl["left"]["reach"] = boost::any_cast<modernjson::json>(f.getSubformula().accept(*this, boost::none));
                } else {
                    opDecl["left"]["op"] = (bound.comparisonType == storm::logic::ComparisonType::Less || bound.comparisonType == storm::logic::ComparisonType::LessEqual) ? "Emax" : "Emin";
                    opDecl["left"]["reach"] = boost::any_cast<modernjson::json>(f.getSubformula().accept(*this, boost::none));
                }
                opDecl["left"]["exp"] = f.hasRewardModelName() ? f.getRewardModelName() : "DEFAULT";
                opDecl["left"]["accumulate"] = modernjson::json(accvec);
                opDecl["right"] = numberToJson(bound.threshold);
            } else {
                if(f.hasOptimalityType()) {
                    opDecl["op"] = f.getOptimalityType() == storm::solver::OptimizationDirection::Minimize ? "Emin" : "Emax";
                    opDecl["reach"] = boost::any_cast<modernjson::json>(f.getSubformula().accept(*this, boost::none));
                    
                } else {
                    // TODO add checks
                    opDecl["op"] = "Emin";
                    opDecl["reach"] = boost::any_cast<modernjson::json>(f.getSubformula().accept(*this, boost::none));
                }
                opDecl["exp"] = f.hasRewardModelName() ? f.getRewardModelName() : "DEFAULT";
                opDecl["accumulate"] = modernjson::json(accvec);
            }
            return opDecl;
        }
        
        boost::any FormulaToJaniJson::visit(storm::logic::UnaryBooleanStateFormula const& f, boost::any const& data) const {
            modernjson::json opDecl;
            storm::logic::UnaryBooleanStateFormula::OperatorType op = f.getOperator();
            assert(op == storm::logic::UnaryBooleanStateFormula::OperatorType::Not);
            opDecl["op"] = "¬";
            opDecl["exp"] = boost::any_cast<modernjson::json>(f.getSubformula().accept(*this, boost::none));
            return opDecl;
        }
        
        boost::any FormulaToJaniJson::visit(storm::logic::UntilFormula const& f, boost::any const& data) const {
            modernjson::json opDecl;
            opDecl["op"] = "U";
            opDecl["left"] = boost::any_cast<modernjson::json>(f.getLeftSubformula().accept(*this, boost::none));
            opDecl["right"] = boost::any_cast<modernjson::json>(f.getRightSubformula().accept(*this, boost::none));
            return opDecl;
        }
        
        std::string operatorTypeToJaniString(storm::expressions::OperatorType optype) {
            
            using OpType = storm::expressions::OperatorType;
            switch(optype) {
                case OpType::And:
                    return "∧";
                case OpType::Or:
                    return "∨";
                case OpType::Xor:
                    return "≠";
                case OpType::Implies:
                    return "⇒";
                case OpType::Iff:
                    return "=";
                case OpType::Plus:
                    return "+";
                case OpType::Minus:
                    return "-";
                case OpType::Times:
                    return "*";
                case OpType::Divide:
                    return "/";
                case OpType::Min:
                    return "min";
                case OpType::Max:
                    return "max";
                case OpType::Power:
                    return "pow";
                case OpType::Equal:
                    return "=";
                case OpType::NotEqual:
                    return "≠";
                case OpType::Less:
                    return "<";
                case OpType::LessOrEqual:
                    return "≤";
                case OpType::Greater:
                    return ">";
                case OpType::GreaterOrEqual:
                    return "≥";
                case OpType::Not:
                    return "¬";
                case OpType::Floor:
                    return "floor";
                case OpType::Ceil:
                    return "ceil";
                case OpType::Ite:
                    return "ite";
                default:
                    STORM_LOG_THROW(false, storm::exceptions::InvalidJaniException, "Operator not supported by Jani");
            }
        }
        
        modernjson::json ExpressionToJson::translate(storm::expressions::Expression const& expr) {
            ExpressionToJson visitor;
            return boost::any_cast<modernjson::json>(expr.accept(visitor, boost::none));
        }
        
        
        boost::any ExpressionToJson::visit(storm::expressions::IfThenElseExpression const& expression, boost::any const& data) {
            modernjson::json opDecl;
            opDecl["op"] = "ite";
            opDecl["if"] = boost::any_cast<modernjson::json>(expression.getCondition()->accept(*this, boost::none));
            opDecl["then"] = boost::any_cast<modernjson::json>(expression.getThenExpression()->accept(*this, boost::none));
            opDecl["else"] = boost::any_cast<modernjson::json>(expression.getElseExpression()->accept(*this, boost::none));
            return opDecl;
        }
        boost::any ExpressionToJson::visit(storm::expressions::BinaryBooleanFunctionExpression const& expression, boost::any const& data) {
            modernjson::json opDecl;
            opDecl["op"] = operatorTypeToJaniString(expression.getOperator());
            opDecl["left"] = boost::any_cast<modernjson::json>(expression.getOperand(0)->accept(*this, boost::none));
            opDecl["right"] = boost::any_cast<modernjson::json>(expression.getOperand(1)->accept(*this, boost::none));
            return opDecl;
        }
        boost::any ExpressionToJson::visit(storm::expressions::BinaryNumericalFunctionExpression const& expression, boost::any const& data) {
            modernjson::json opDecl;
            opDecl["op"] = operatorTypeToJaniString(expression.getOperator());
            opDecl["left"] = boost::any_cast<modernjson::json>(expression.getOperand(0)->accept(*this, boost::none));
            opDecl["right"] = boost::any_cast<modernjson::json>(expression.getOperand(1)->accept(*this, boost::none));
            return opDecl;
        }
        boost::any ExpressionToJson::visit(storm::expressions::BinaryRelationExpression const& expression, boost::any const& data) {
            modernjson::json opDecl;
            opDecl["op"] = operatorTypeToJaniString(expression.getOperator());
            opDecl["left"] = boost::any_cast<modernjson::json>(expression.getOperand(0)->accept(*this, boost::none));
            opDecl["right"] = boost::any_cast<modernjson::json>(expression.getOperand(1)->accept(*this, boost::none));
            return opDecl;
        }
        boost::any ExpressionToJson::visit(storm::expressions::VariableExpression const& expression, boost::any const& data) {
            return modernjson::json(expression.getVariableName());
        }
        boost::any ExpressionToJson::visit(storm::expressions::UnaryBooleanFunctionExpression const& expression, boost::any const& data) {
            modernjson::json opDecl;
            opDecl["op"] = operatorTypeToJaniString(expression.getOperator());
            opDecl["exp"] = boost::any_cast<modernjson::json>(expression.getOperand()->accept(*this, boost::none));
            return opDecl;
        }
        boost::any ExpressionToJson::visit(storm::expressions::UnaryNumericalFunctionExpression const& expression, boost::any const& data) {
            modernjson::json opDecl;
            opDecl["op"] = operatorTypeToJaniString(expression.getOperator());
            opDecl["exp"] = boost::any_cast<modernjson::json>(expression.getOperand()->accept(*this, boost::none));
            return opDecl;
        }
        boost::any ExpressionToJson::visit(storm::expressions::BooleanLiteralExpression const& expression, boost::any const& data) {
            return modernjson::json(expression.getValue());
        }
        boost::any ExpressionToJson::visit(storm::expressions::IntegerLiteralExpression const& expression, boost::any const& data) {
            return modernjson::json(expression.getValue());
        }
        boost::any ExpressionToJson::visit(storm::expressions::RationalLiteralExpression const& expression, boost::any const& data) {
            return modernjson::json(expression.getValueAsDouble());
        }
        
        
        
        
        
        
        
        void JsonExporter::toFile(storm::jani::Model const& janiModel, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, std::string const& filepath, bool checkValid) {
            std::ofstream ofs;
            ofs.open (filepath, std::ofstream::out );
            if(ofs.is_open()) {
                toStream(janiModel, formulas, ofs, checkValid);
            } else {
                STORM_LOG_THROW(false, storm::exceptions::FileIoException, "Cannot open " << filepath);
            }
        }
        
        void JsonExporter::toStream(storm::jani::Model const& janiModel,  std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, std::ostream& os, bool checkValid) {
            if(checkValid) {
                janiModel.checkValid();
            }
            JsonExporter exporter;
            exporter.convertModel(janiModel);
            exporter.convertProperties(formulas, !janiModel.isDiscreteTimeModel());
            os << exporter.finalize().dump(4) << std::endl;
        }
        
        modernjson::json buildActionArray(std::vector<storm::jani::Action> const& actions) {
            std::vector<modernjson::json> actionReprs;
            uint64_t actIndex = 0;
            for(auto const& act : actions) {
                if(actIndex == storm::jani::Model::SILENT_ACTION_INDEX) {
                    actIndex++;
                    continue;
                }
                actIndex++;
                modernjson::json actEntry;
                actEntry["name"] = act.getName();
                actionReprs.push_back(actEntry);
            }
            
            return modernjson::json(actionReprs);
            
        }
        
        
        modernjson::json buildExpression(storm::expressions::Expression const& exp) {
            return ExpressionToJson::translate(exp);
        }
        
        
        modernjson::json buildConstantsArray(std::vector<storm::jani::Constant> const& constants) {
            std::vector<modernjson::json> constantDeclarations;
            for(auto const& constant : constants) {
                modernjson::json constantEntry;
                constantEntry["name"] = constant.getName();
                modernjson::json typeDesc;
                if(constant.isBooleanConstant()) {
                    typeDesc = "bool";
                } else if(constant.isRealConstant()) {
                    typeDesc = "real";
                } else {
                    assert(constant.isIntegerConstant());
                    typeDesc = "int";
                }
                constantEntry["type"] = typeDesc;
                if(constant.isDefined()) {
                    constantEntry["value"] = buildExpression(constant.getExpression());
                }
                constantDeclarations.push_back(constantEntry);
            }
            return modernjson::json(constantDeclarations);
        }
        
        modernjson::json buildVariablesArray(storm::jani::VariableSet const& varSet) {
            std::vector<modernjson::json> variableDeclarations;
            for(auto const& variable : varSet) {
                modernjson::json varEntry;
                varEntry["name"] = variable.getName();
                varEntry["transient"] = variable.isTransient();
                modernjson::json typeDesc;
                if(variable.isBooleanVariable()) {
                    typeDesc = "bool";
                } else if(variable.isRealVariable()) {
                    typeDesc = "real";
                } else if(variable.isUnboundedIntegerVariable()) {
                    typeDesc = "int";
                } else {
                    assert(variable.isBoundedIntegerVariable());
                    typeDesc["kind"] = "bounded";
                    typeDesc["base"] = "int";
                    typeDesc["lower-bound"] = buildExpression(variable.asBoundedIntegerVariable().getLowerBound());
                    typeDesc["upper-bound"] = buildExpression(variable.asBoundedIntegerVariable().getUpperBound());
                }
                
                varEntry["type"] = typeDesc;
                if(variable.hasInitExpression()) {
                    varEntry["initial-value"] = buildExpression(variable.getInitExpression());
                }
                variableDeclarations.push_back(varEntry);
            }
            return modernjson::json(variableDeclarations);
            
        }
        
        modernjson::json buildAssignmentArray(storm::jani::OrderedAssignments const& orderedAssignments) {
            std::vector<modernjson::json> assignmentDeclarations;
            bool addIndex = orderedAssignments.hasMultipleLevels();
            for(auto const& assignment : orderedAssignments) {
                modernjson::json assignmentEntry;
                assignmentEntry["ref"] = assignment.getVariable().getName();
                assignmentEntry["value"] = buildExpression(assignment.getAssignedExpression());
                if(addIndex) {
                    assignmentEntry["index"] = assignment.getLevel();
                }
                assignmentDeclarations.push_back(assignmentEntry);
            }
            return modernjson::json(assignmentDeclarations);
        }
        
        modernjson::json buildLocationsArray(std::vector<storm::jani::Location> const& locations) {
            std::vector<modernjson::json> locationDeclarations;
            for(auto const& location : locations) {
                modernjson::json locEntry;
                locEntry["name"] = location.getName();
                // TODO support invariants?
                locEntry["transient-values"] = buildAssignmentArray(location.getAssignments());
                locationDeclarations.push_back(locEntry);
            }
            return modernjson::json(locationDeclarations);
        }
        
        modernjson::json buildInitialLocations(storm::jani::Automaton const& automaton) {
            std::vector<std::string> names;
            for(auto const& initLocIndex : automaton.getInitialLocationIndices()) {
                names.push_back(automaton.getLocation(initLocIndex).getName());
            }
            return modernjson::json(names);
        }
        
        modernjson::json buildDestinations(std::vector<EdgeDestination> const& destinations, std::map<uint64_t, std::string> const& locationNames) {
            std::vector<modernjson::json> destDeclarations;
            for(auto const& destination : destinations) {
                modernjson::json destEntry;
                destEntry["location"] = locationNames.at(destination.getLocationIndex());
                destEntry["probability"]["exp"] = buildExpression(destination.getProbability());
                destEntry["assignments"] = buildAssignmentArray(destination.getOrderedAssignments());
                destDeclarations.push_back(destEntry);
            }
            return modernjson::json(destDeclarations);
        }
        
        modernjson::json buildEdges(std::vector<Edge> const& edges , std::map<uint64_t, std::string> const& actionNames, std::map<uint64_t, std::string> const& locationNames) {
            std::vector<modernjson::json> edgeDeclarations;
            for(auto const& edge : edges) {
                modernjson::json edgeEntry;
                edgeEntry["location"] = locationNames.at(edge.getSourceLocationIndex());
                if(!edge.hasSilentAction()) {
                    edgeEntry["action"] = actionNames.at(edge.getActionIndex());
                }
                if(edge.hasRate()) {
                    edgeEntry["rate"]["exp"] = buildExpression(edge.getRate());
                }
                edgeEntry["guard"]["exp"] = buildExpression(edge.getGuard());
                edgeEntry["destinations"] = buildDestinations(edge.getDestinations(), locationNames);
                
                edgeDeclarations.push_back(edgeEntry);
            }
            return modernjson::json(edgeDeclarations);
        }
        
        modernjson::json buildAutomataArray(std::vector<storm::jani::Automaton> const& automata, std::map<uint64_t, std::string> const& actionNames) {
            std::vector<modernjson::json> automataDeclarations;
            for(auto const& automaton : automata) {
                modernjson::json autoEntry;
                autoEntry["name"] = automaton.getName();
                autoEntry["variables"] = buildVariablesArray(automaton.getVariables());
                if(automaton.hasRestrictedInitialStates()) {
                    autoEntry["restrict-initial"]["exp"] = buildExpression(automaton.getInitialStatesRestriction());
                }
                autoEntry["locations"] = buildLocationsArray(automaton.getLocations());
                autoEntry["initial-locations"] = buildInitialLocations(automaton);
                autoEntry["edges"] = buildEdges(automaton.getEdges(), actionNames, automaton.buildIdToLocationNameMap());
                automataDeclarations.push_back(autoEntry);
            }
            return modernjson::json(automataDeclarations);
        }
        
        
        void JsonExporter::convertModel(storm::jani::Model const& janiModel) {
            jsonStruct["jani-version"] = janiModel.getJaniVersion();
            jsonStruct["name"] = janiModel.getName();
            jsonStruct["type"] = to_string(janiModel.getModelType());
            jsonStruct["actions"] = buildActionArray(janiModel.getActions());
            jsonStruct["constants"] = buildConstantsArray(janiModel.getConstants());
            jsonStruct["variables"] = buildVariablesArray(janiModel.getGlobalVariables());
            jsonStruct["restrict-initial"]["exp"] = buildExpression(janiModel.getInitialStatesRestriction());
            jsonStruct["automata"] = buildAutomataArray(janiModel.getAutomata(), janiModel.getActionIndexToNameMap());
            jsonStruct["system"] = CompositionJsonExporter::translate(janiModel.getSystemComposition());
            std::vector<std::string> standardFeatureVector = {"derived-operators"};
            jsonStruct["features"] = standardFeatureVector;
            
        }
        
        
        
        
        std::string janiFilterTypeString(storm::modelchecker::FilterType const& ft) {
            switch(ft) {
                case storm::modelchecker::FilterType::MIN:
                    return "min";
                case storm::modelchecker::FilterType::MAX:
                    return "max";
                case storm::modelchecker::FilterType::SUM:
                    return "sum";
                case storm::modelchecker::FilterType::AVG:
                    return "avg";
                case storm::modelchecker::FilterType::COUNT:
                    return "count";
                case storm::modelchecker::FilterType::EXISTS:
                    return "∃";
                case storm::modelchecker::FilterType::FORALL:
                    return "∀";
                case storm::modelchecker::FilterType::ARGMIN:
                    return "argmin";
                case storm::modelchecker::FilterType::ARGMAX:
                    return "argmax";
                case storm::modelchecker::FilterType::VALUES:
                    return "values";
                default:
                    assert(false);
                    
            }
        }
        
        modernjson::json convertFilterExpression(storm::jani::FilterExpression const& fe, bool continuousModel) {
            modernjson::json propDecl;
            propDecl["states"]["op"] = "initial";
            propDecl["op"] = "filter";
            propDecl["fun"] = janiFilterTypeString(fe.getFilterType());
            propDecl["values"] = FormulaToJaniJson::translate(*fe.getFormula(), continuousModel);
            return propDecl;
        }
        
        
        void JsonExporter::convertProperties( std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, bool continuousModel) {
            std::vector<modernjson::json> properties;
            uint64_t index = 0;
            for(auto const& f : formulas) {
                modernjson::json propDecl;
                propDecl["name"] = "prop" + std::to_string(index);
                propDecl["expression"] = convertFilterExpression(storm::jani::FilterExpression(f), continuousModel);
                ++index;
                properties.push_back(propDecl);
            }
            jsonStruct["properties"] = properties;
        }
        
        
    }
}
