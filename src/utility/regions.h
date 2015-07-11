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

#include "src/modelchecker/reachability/SparseDtmcRegionModelChecker.h"
#include "src/logic/ComparisonType.h"

// Forward-declare region modelchecker  class.
    namespace storm {
        namespace modelchecker{
        template<typename ParametricType, typename ConstantType>
        class SparseDtmcRegionModelChecker;
        }
    }

namespace storm {
    namespace utility{
        namespace regions {
            template<typename ParametricType, typename ConstantType>
            class RegionParser{
                public:
                    typedef typename storm::modelchecker::SparseDtmcRegionModelChecker<ParametricType,ConstantType>::ParameterRegion ParameterRegion;
                    typedef typename storm::modelchecker::SparseDtmcRegionModelChecker<ParametricType,ConstantType>::VariableType VariableType;
                    typedef typename storm::modelchecker::SparseDtmcRegionModelChecker<ParametricType,ConstantType>::BoundType BoundType;
                    
                /*
                 * Can be used to parse a single parameter with its bounds from a string of the form "0.3<=p<=0.5".
                 * The numbers are parsed as doubles and then converted to SparseDtmcRegionModelChecker::Boundtype.
                 * According to the given precision, the lower bound may be rounded down and the upper bound may be rounded up.
                 * If no precision is given, the one from the settings is used.
                 * The results will be inserted in the given maps
                 * 
                 */
                static void parseParameterBounds( 
                        std::map<VariableType, BoundType>& lowerBounds,
                        std::map<VariableType, BoundType>& upperBounds,
                        std::string const& parameterBoundsString,
                        double const precision=0.0
                );

                /*
                 * Can be used to parse a single region from a string of the form "0.3<=p<=0.5,0.4<=q<=0.7".
                 * The numbers are parsed as doubles and then converted to SparseDtmcRegionModelChecker::Boundtype.
                 * According to the given precision, the lower bound may be rounded down and the upper bound may be rounded up.
                 * If no precision is given, the one from the settings is used.
                 * 
                 */
                static ParameterRegion parseRegion(
                        std::string const& regionString,
                        double precision=0.0);

                /*
                 * Can be used to parse a vector of region from a string of the form "0.3<=p<=0.5,0.4<=q<=0.7;0.1<=p<=0.3,0.2<=q<=0.4".
                 * The numbers are parsed as doubles and then converted to SparseDtmcRegionModelChecker::Boundtype.
                 * According to the given precision, the lower bound may be rounded down and the upper bound may be rounded up.
                 * If no precision is given, the one from the settings is used.
                 * 
                 */
                static std::vector<ParameterRegion> parseMultipleRegions(
                        std::string const& regionsString,
                        double precision=0.0);
            

                /*
                 * Retrieves the regions that are specified in the settings.
                 * The numbers are parsed as doubles and then converted to SparseDtmcRegionModelChecker::Boundtype.
                 * According to the given precision, the lower bound may be rounded down and the upper bound may be rounded up.
                 * If no precision is given, the one from the settings is used.
                 * 
                 */
                static std::vector<ParameterRegion> getRegionsFromSettings(double precision=0.0);
            
            };
            
            
            /*
             * Converts a number from one type to a number from the other.
             * If no exact conversion is possible, the number is rounded up or down, using the given precision or the one from the settings.
             */
            template<typename SourceType, typename TargetType>
            TargetType convertNumber(SourceType const& number, bool const& roundDown=true, double const& precision=0.0);
            
            /*
             * retrieves the variable object from the given string
             * Throws an exception if variable not found
             */
            template<typename VariableType>
            VariableType getVariableFromString(std::string variableString);
            
            
            enum class VariableSort {VS_BOOL, VS_REAL, VS_INT};
            
            /*
             * Creates a new variable with the given name and the given sort
             * If there is already a variable with that name, that variable is returned.
             * An exception is thrown if there already is a variable with the given name, but with a different sort.
             */
            template<typename VariableType>
            VariableType getNewVariable(std::string variableName, VariableSort sort);
            
            /*
             * retrieves the variable name  from the given variable
             */
            template<typename VariableType>
            std::string getVariableName(VariableType variable);
            
            template<typename ParametricType, typename ConstantType>
            typename storm::modelchecker::SparseDtmcRegionModelChecker<ParametricType,ConstantType>::BoundType evaluateFunction(ParametricType const& function, std::map<typename storm::modelchecker::SparseDtmcRegionModelChecker<ParametricType,ConstantType>::VariableType, typename storm::modelchecker::SparseDtmcRegionModelChecker<ParametricType,ConstantType>::BoundType> const& point);
            
            /*!
             * Returns true if the function is rational. Note that the function might be simplified.
             */
            template<typename ParametricType>
            bool functionIsLinear(ParametricType const& function);
            
            /*!
             *  Add all variables that occur in the given function to the the given set
             */
            template<typename ParametricType, typename VariableType>
            void gatherOccurringVariables(ParametricType const& function, std::set<VariableType>& variableSet);
            
            
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
            template<typename SolverType, typename ParametricType, typename VariableType>
            void addGuardedConstraintToSmtSolver(std::shared_ptr<SolverType> solver, VariableType const& guard, ParametricType const& leftHandSide, storm::logic::ComparisonType relation, ParametricType const& rightHandSide);
            
            /*!
             * Adds the given constraint to the given Smt solver
             * The constraint is of the form 'variable relation bound'
             */
            template<typename SolverType, typename VariableType, typename BoundType>
            void addParameterBoundsToSmtSolver(std::shared_ptr<SolverType> solver, VariableType const& variable, storm::logic::ComparisonType relation, BoundType const& bound);
            
            /*!
             * Adds the given (boolean) variable to the solver. Can be used to assert that guards are true/false
             */
            template<typename SolverType, typename VariableType>
            void addBoolVariableToSmtSolver(std::shared_ptr<SolverType> solver, VariableType const& variable, bool value);
            
            
            /*!
             * Returns a new function that initially has the given value (which might be a constant or a variable)
             * 
             */
            template<typename ParametricType, typename ValueType>
            ParametricType getNewFunction(ValueType initialValue);
        }
    }
}


#endif	/* STORM_UTILITY_REGIONS_H */

