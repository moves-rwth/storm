#include <string>

#include "storm/utility/region.h"
#include "storm/utility/constants.h"
#include "storm/utility/macros.h"
#include "storm/settings/SettingsManager.h"
#include "storm/solver/SmtlibSmtSolver.h"
#include "storm/exceptions/IllegalArgumentException.h"
#include "storm/exceptions/NotImplementedException.h"

#ifdef STORM_HAVE_CARL
#include<carl/numbers/numbers.h>
#include<carl/core/VariablePool.h>
#endif

namespace storm {
    namespace utility{
        namespace region {
            
#ifdef STORM_HAVE_CARL
            template<>
            storm::RationalFunctionVariable getVariableFromString<storm::RationalFunctionVariable>(std::string variableString){
                storm::RationalFunctionVariable const& var = carl::VariablePool::getInstance().findVariableWithName(variableString);
                STORM_LOG_THROW(var!=carl::Variable::NO_VARIABLE, storm::exceptions::IllegalArgumentException, "Variable '" + variableString + "' could not be found.");
                return var;
            }
            
            template<>
            storm::RationalFunctionVariable getNewVariable<storm::RationalFunctionVariable>(std::string variableName, VariableSort sort){
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
                
                storm::RationalFunctionVariable const& var = carl::VariablePool::getInstance().findVariableWithName(variableName);
                if(var!=carl::Variable::NO_VARIABLE){
                    STORM_LOG_THROW(var.getType()==carlVarType, storm::exceptions::IllegalArgumentException, "Tried to create a new variable but the name " << variableName << " is already in use for a variable of a different sort.");
                    return var;
                }
                
                return carl::freshVariable(variableName, carlVarType);
            }
                      
            template<>
            std::string getVariableName<storm::RationalFunctionVariable>(storm::RationalFunctionVariable variable){
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
            void addGuardedConstraintToSmtSolver<storm::solver::SmtlibSmtSolver, storm::RationalFunction, storm::RationalFunctionVariable>(std::shared_ptr<storm::solver::SmtlibSmtSolver> solver, storm::RationalFunctionVariable const& guard, storm::RationalFunction const& leftHandSide, storm::logic::ComparisonType relation, storm::RationalFunction const& rightHandSide){
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
            void addParameterBoundsToSmtSolver<storm::solver::SmtlibSmtSolver, storm::RationalFunctionVariable, storm::RationalFunctionCoefficient>(std::shared_ptr<storm::solver::SmtlibSmtSolver> solver, storm::RationalFunctionVariable const& variable, storm::logic::ComparisonType relation, storm::RationalFunctionCoefficient const& bound){
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
            void addBoolVariableToSmtSolver<storm::solver::SmtlibSmtSolver, storm::RationalFunctionVariable>(std::shared_ptr<storm::solver::SmtlibSmtSolver> solver, storm::RationalFunctionVariable const& variable, bool value){
                STORM_LOG_THROW(variable.getType()==carl::VariableType::VT_BOOL, storm::exceptions::IllegalArgumentException, "Tried to add a constraint to the solver that is a non boolean variable. Only boolean variables are allowed");
                solver->add(variable, value);
            }
            
            template<>
            storm::RationalFunction getNewFunction<storm::RationalFunction, storm::RationalFunctionCoefficient>(storm::RationalFunctionCoefficient initialValue) {
                std::shared_ptr<carl::Cache<carl::PolynomialFactorizationPair<storm::RawPolynomial>>> cache(new carl::Cache<carl::PolynomialFactorizationPair<storm::RawPolynomial>>());
                return storm::RationalFunction(storm::RationalFunction::PolyType(storm::RationalFunction::PolyType::PolyType(initialValue), cache));
            }
            
            template<>
            storm::RationalFunction getNewFunction<storm::RationalFunction, storm::RationalFunctionVariable>(storm::RationalFunctionVariable initialValue) {
                std::shared_ptr<carl::Cache<carl::PolynomialFactorizationPair<storm::RawPolynomial>>> cache(new carl::Cache<carl::PolynomialFactorizationPair<storm::RawPolynomial>>());
                return storm::RationalFunction(storm::RationalFunction::PolyType(storm::RationalFunction::PolyType::PolyType(initialValue), cache));
            }
#endif
        }
    }
}
