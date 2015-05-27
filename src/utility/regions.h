/* 
 * File:   Regions.h
 * Author: Tim Quatmann
 *
 * Created on May 13, 2015, 12:54 PM
 */

#include "src/modelchecker/reachability/SparseDtmcRegionModelChecker.h" //to get the ParameterRegion type

#ifndef STORM_UTILITY_REGIONS_H
#define	STORM_UTILITY_REGIONS_H

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
            
        }
    }
}


#endif	/* STORM_UTILITY_REGIONS_H */

