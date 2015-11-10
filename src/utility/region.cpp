/* 
 * File:   Regions.cpp
 * Author: Tim Quatmann
 * 
 * Created on May 13, 2015, 12:54 PM
 */

#include <string>

#include "src/utility/region.h"
#include "src/utility/constants.h"
#include "src/utility/macros.h"
#include "src/settings/SettingsManager.h"
#include "src/solver/Smt2SmtSolver.h"
#include "src/exceptions/IllegalArgumentException.h"
#include "src/exceptions/NotImplementedException.h"

#ifdef STORM_HAVE_CARL
#include<carl/numbers/numbers.h>
#include<carl/core/VariablePool.h>
#endif

namespace storm {
    namespace utility{
        namespace region {
            
            template<>
            double convertNumber<double, double>(double const& number){
                return number;
            }
            
            template<>
            double&& convertNumber<double>(double&& number){
                return std::move(number);
           }
            
            template<>
            double convertNumber<double, std::string>(std::string const& number){
                return std::stod(number);
            }
            
#ifdef STORM_HAVE_CARL
            template<>
            storm::RationalNumber convertNumber<storm::RationalNumber, double>(double const& number){
                return carl::rationalize<storm::RationalNumber>(number);
            }
            
            template<>
            storm::RationalFunction convertNumber<storm::RationalFunction, double>(double const& number){
                return storm::RationalFunction(convertNumber<storm::RationalNumber>(number));
            }
            
            template<>
            double convertNumber<double, storm::RationalNumber>(storm::RationalNumber const& number){
                return carl::toDouble(number);
            }
            
            template<>
            storm::RationalNumber convertNumber<storm::RationalNumber, storm::RationalNumber>(storm::RationalNumber const& number){
                return number;
            }
            
            template<>
            storm::RationalNumber&& convertNumber<storm::RationalNumber>(storm::RationalNumber&& number){
                return std::move(number);
            }
            
            template<>
            storm::RationalNumber convertNumber<storm::RationalNumber, std::string>(std::string const& number){
                //We parse the number as double and then convert it to a a rational number.
                return convertNumber<storm::RationalNumber>(convertNumber<double>(number));
            }
            
            template<>
            storm::Variable getVariableFromString<storm::Variable>(std::string variableString){
                storm::Variable const& var = carl::VariablePool::getInstance().findVariableWithName(variableString);
                STORM_LOG_THROW(var!=carl::Variable::NO_VARIABLE, storm::exceptions::IllegalArgumentException, "Variable '" + variableString + "' could not be found.");
                return var;
            }
            
            template<>
            storm::Variable getNewVariable<storm::Variable>(std::string variableName, VariableSort sort){
                carl::VariableType carlVarType;
                switch(sort){
                    case VariableSort::VS_BOOL:
                        carlVarType = carl::VariableType::VT_BOOL;
                        break;
                    case VariableSort::VS_REAL:
                        carlVarType = carl::VariableType::VT_REAL;
                        break;
                    case VariableSort::VS_INT:
                        carlVarType = carl::VariableType::VT_INT;
                        break;
                    default:
                        STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "The given variable sort is not implemented");
                }
                
                storm::Variable const& var = carl::VariablePool::getInstance().findVariableWithName(variableName);
                if(var!=carl::Variable::NO_VARIABLE){
                    STORM_LOG_THROW(var.getType()==carlVarType, storm::exceptions::IllegalArgumentException, "Tried to create a new variable but the name " << variableName << " is already in use for a variable of a different sort.");
                    return var;
                }
                
                return carl::freshVariable(variableName, carlVarType);
            }
                      
            template<>
            std::string getVariableName<storm::Variable>(storm::Variable variable){
                return carl::VariablePool::getInstance().getName(variable);
            }
                        
            template<>
            CoefficientType<storm::RationalFunction> evaluateFunction<storm::RationalFunction>(storm::RationalFunction const& function, std::map<VariableType<storm::RationalFunction>, CoefficientType<storm::RationalFunction>> const& point){
                return function.evaluate(point);
            }
            
            template<>
            CoefficientType<storm::RationalFunction> getConstantPart<storm::RationalFunction>(storm::RationalFunction const& function){
                return function.constantPart();
            }
            
            template<>
            bool functionIsLinear<storm::RationalFunction>(storm::RationalFunction const& function){
                // Note: At this moment there is no function in carl for rationalFunctions.
                // We therefore check whether the numerator is linear and the denominator constant.
                // We simplify the function to (hopefully) avoid wrong answers for situations like x^2/x
                storm::utility::simplify(function);
                bool result=(function.nominator().isLinear() && function.denominator().isConstant());
                STORM_LOG_WARN_COND(result, "The function " << function << "is not considered as linear.");
                return result;
            }
            
            template<>
            void gatherOccurringVariables<storm::RationalFunction>(storm::RationalFunction const& function, std::set<VariableType<storm::RationalFunction>>& variableSet){
                function.gatherVariables(variableSet);
            }
            
            template<>
            void addGuardedConstraintToSmtSolver<storm::solver::Smt2SmtSolver, storm::RationalFunction, storm::Variable>(std::shared_ptr<storm::solver::Smt2SmtSolver> solver,storm::Variable const& guard, storm::RationalFunction const& leftHandSide, storm::logic::ComparisonType relation, storm::RationalFunction const& rightHandSide){
                STORM_LOG_THROW(guard.getType()==carl::VariableType::VT_BOOL, storm::exceptions::IllegalArgumentException, "Tried to add a constraint to the solver whose guard is not of type bool");
                storm::CompareRelation compRel;
                switch (relation){
                    case storm::logic::ComparisonType::Greater:
                        compRel=storm::CompareRelation::GREATER;
                        break;
                    case storm::logic::ComparisonType::GreaterEqual:
                        compRel=storm::CompareRelation::GEQ;
                        break;
                    case storm::logic::ComparisonType::Less:
                        compRel=storm::CompareRelation::LESS;
                        break;
                    case storm::logic::ComparisonType::LessEqual:
                        compRel=storm::CompareRelation::LEQ;
                        break;
                    default:
                        STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentException, "the comparison relation of the formula is not supported");
                }        
               //Note: this only works if numerators and denominators are positive...
                storm::ArithConstraint<storm::Polynomial> constraint((leftHandSide.nominator() * rightHandSide.denominator()) - (rightHandSide.nominator() * leftHandSide.denominator()), compRel);
                solver->add(guard,constraint);
            }
            
            template<>
            void addParameterBoundsToSmtSolver<storm::solver::Smt2SmtSolver, storm::Variable, storm::RationalNumber>(std::shared_ptr<storm::solver::Smt2SmtSolver> solver, storm::Variable const& variable, storm::logic::ComparisonType relation, storm::RationalNumber const& bound){
                storm::CompareRelation compRel;
                switch (relation){
                    case storm::logic::ComparisonType::Greater:
                        compRel=storm::CompareRelation::GREATER;
                        break;
                    case storm::logic::ComparisonType::GreaterEqual:
                        compRel=storm::CompareRelation::GEQ;
                        break;
                    case storm::logic::ComparisonType::Less:
                        compRel=storm::CompareRelation::LESS;
                        break;
                    case storm::logic::ComparisonType::LessEqual:
                        compRel=storm::CompareRelation::LEQ;
                        break;
                    default:
                        STORM_LOG_THROW(false, storm::exceptions::IllegalArgumentException, "the comparison relation of the formula is not supported");
                }
                storm::RawPolynomial leftHandSide(variable);
                leftHandSide -= bound;
                solver->add(storm::ArithConstraint<storm::RawPolynomial>(leftHandSide,compRel));
            }
            
            template<>
            void addBoolVariableToSmtSolver<storm::solver::Smt2SmtSolver, storm::Variable>(std::shared_ptr<storm::solver::Smt2SmtSolver> solver,storm::Variable const& variable, bool value){
                STORM_LOG_THROW(variable.getType()==carl::VariableType::VT_BOOL, storm::exceptions::IllegalArgumentException, "Tried to add a constraint to the solver that is a non boolean variable. Only boolean variables are allowed");
                solver->add(variable, value);
            }
            
            template<>
            storm::RationalFunction getNewFunction<storm::RationalFunction, storm::RationalNumber>(storm::RationalNumber initialValue) {
                std::shared_ptr<carl::Cache<carl::PolynomialFactorizationPair<storm::RawPolynomial>>> cache(new carl::Cache<carl::PolynomialFactorizationPair<storm::RawPolynomial>>());
                return storm::RationalFunction(storm::RationalFunction::PolyType(storm::RationalFunction::PolyType::PolyType(initialValue), cache));
            }
            
            template<>
            storm::RationalFunction getNewFunction<storm::RationalFunction, storm::Variable>(storm::Variable initialValue) {
                std::shared_ptr<carl::Cache<carl::PolynomialFactorizationPair<storm::RawPolynomial>>> cache(new carl::Cache<carl::PolynomialFactorizationPair<storm::RawPolynomial>>());
                return storm::RationalFunction(storm::RationalFunction::PolyType(storm::RationalFunction::PolyType::PolyType(initialValue), cache));
            }
#endif
        }
    }
}
