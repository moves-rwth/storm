/* 
 * File:   Regions.h
 * Author: Tim Quatmann
 *
 * Created on May 13, 2015, 12:54 PM
 * 
 * This file provides some auxiliary functions for the Region Model Checker.
 * The purpose of many of this functions is to deal with the different types (e.g., carl expressions)
 */


#ifndef STORM_UTILITY_REGIONS_H
#define	STORM_UTILITY_REGIONS_H

#include <type_traits>
#include <map>
#include <set>
#include <memory>

#include "storm/logic/ComparisonType.h"
#include "storm/adapters/CarlAdapter.h"

namespace storm {
    namespace utility{
        namespace region {
            
            //Obtain the correct type for Variables and Coefficients out of a given Function type
#ifdef STORM_HAVE_CARL
            template<typename FunctionType>
            using VariableType    = typename std::conditional<(std::is_same<FunctionType, storm::RationalFunction>::value), storm::RationalFunctionVariable, std::nullptr_t>::type;
            template<typename FunctionType>
            using CoefficientType = typename std::conditional<(std::is_same<FunctionType, storm::RationalFunction>::value), storm::RationalFunction::CoeffType, std::nullptr_t>::type;
#else
            template<typename Functiontype>
            using VariableType = std::nullptr_t;
            template<typename Functiontype>
            using CoefficientType = std::nullptr_t;
#endif
            
            /*
             * retrieves the variable object from the given string
             * Throws an exception if variable not found
             */
            template<typename VarType>
            VarType getVariableFromString(std::string variableString);
            
            enum class VariableSort {VS_BOOL, VS_REAL, VS_INT};
            /*
             * Creates a new variable with the given name and the given sort
             * If there is already a variable with that name, that variable is returned.
             * An exception is thrown if there already is a variable with the given name, but with a different sort.
             */
            template<typename VarType>
            VarType getNewVariable(std::string variableName, VariableSort sort);
            
            /*
             * retrieves the variable name  from the given variable
             */
            template<typename VarType>
            std::string getVariableName(VarType variable);
            
            /*
             * evaluates the given function at the given point and returns the result
             */
            template<typename FunctionType>
            CoefficientType<FunctionType> evaluateFunction(FunctionType const& function, std::map<VariableType<FunctionType>, CoefficientType<FunctionType>> const& point);
            
            /*!
             * retrieves the constant part of the given function.
             * If the function is constant, then the result is the same value as the original function
             */
            template<typename FunctionType>
            CoefficientType<FunctionType> getConstantPart(FunctionType const& function);
            
            /*!
             * Returns true if the function is rational. Note that the function might be simplified.
             */
            template<typename FunctionType>
            bool functionIsLinear(FunctionType const& function);
            
            /*!
             *  Add all variables that occur in the given function to the the given set
             */
            template<typename FunctionType>
            void gatherOccurringVariables(FunctionType const& function, std::set<VariableType<FunctionType>>& variableSet);
            
            
            /*!
             * Adds the given constraint to the given Smt solver
             * The constraint is of the form 'guard implies leftHandSide relation rightHandSide'
             * @attention the numerators and denominators of the left and right hand side should be positive!
             * 
             * @param guard variable of type bool
             * @param leftHandSide left hand side of the constraint
             * @param relation relation of the constraint
             * @param rightHandSide right hand side of the constraint
             */
            template<typename SolverType, typename FunctionType, typename VarType>
            void addGuardedConstraintToSmtSolver(std::shared_ptr<SolverType> solver, VarType const& guard, FunctionType const& leftHandSide, storm::logic::ComparisonType relation, FunctionType const& rightHandSide);
            
            /*!
             * Adds the given constraint to the given Smt solver
             * The constraint is of the form 'variable relation bound'
             */
            template<typename SolverType, typename VarType, typename BoundType>
            void addParameterBoundsToSmtSolver(std::shared_ptr<SolverType> solver, VarType const& variable, storm::logic::ComparisonType relation, BoundType const& bound);
            
            /*!
             * Adds the given (boolean) variable to the solver. Can be used to assert that guards are true/false
             */
            template<typename SolverType, typename VarType>
            void addBoolVariableToSmtSolver(std::shared_ptr<SolverType> solver, VarType const& variable, bool value);
            
            
            /*!
             * Returns a new function that initially has the given value (which might be a constant or a variable)
             */
            template<typename FunctionType, typename ValueType>
            FunctionType getNewFunction(ValueType initialValue);
        }
    }
}


#endif	/* STORM_UTILITY_REGIONS_H */

