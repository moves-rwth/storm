/* 
 * File:   ParameterRegion.h
 * Author: tim
 *
 * Created on August 10, 2015, 1:51 PM
 */

#ifndef STORM_MODELCHECKER_REGION_PARAMETERREGION_H
#define	STORM_MODELCHECKER_REGION_PARAMETERREGION_H

#include "src/modelchecker/region/SparseDtmcRegionModelChecker.h"

namespace storm {
    namespace modelchecker{

        template<typename ParametricType, typename ConstantType>
        class SparseDtmcRegionModelChecker;
        
        template<typename ParametricType, typename ConstantType>
        class SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ParameterRegion{
        public:
            typedef typename SparseDtmcRegionModelChecker<ParametricType, ConstantType>::VariableType VariableType;
            typedef typename SparseDtmcRegionModelChecker<ParametricType, ConstantType>::CoefficientType CoefficientType;
            
            ParameterRegion(std::map<VariableType, CoefficientType> lowerBounds, std::map<VariableType, CoefficientType> upperBounds);
            virtual ~ParameterRegion();
                
            std::set<VariableType> getVariables() const;
            CoefficientType const& getLowerBound(VariableType const& variable) const;
            CoefficientType const& getUpperBound(VariableType const& variable) const;
            const std::map<VariableType, CoefficientType> getUpperBounds() const;
            const std::map<VariableType, CoefficientType> getLowerBounds() const;
                
            /*
             * Returns a vector of all possible combinations of lower and upper bounds of the given variables.
             * The first entry of the returned vector will map every variable to its lower bound
             * The second entry will map every variable to its lower bound, except the first one (i.e. *getVariables.begin())
             * ...
             * The last entry will map every variable to its upper bound
             * 
             * If the given set of variables is empty, the returned vector will contain an empty map
             */
            std::vector<std::map<VariableType, CoefficientType>> getVerticesOfRegion(std::set<VariableType> const& consideredVariables) const;
         
            RegionCheckResult getCheckResult() const;
            void setCheckResult(RegionCheckResult checkResult);
 
            /*!
             * Retrieves a point in the region for which is considered property is not satisfied.
             * If such a point is not known, the returned map is empty.
             */
            std::map<VariableType, CoefficientType> getViolatedPoint() const;

            /*!
             * Sets a point in the region for which the considered property is not satisfied. 
             */
            void setViolatedPoint(std::map<VariableType, CoefficientType> const& violatedPoint);
                
            /*!
             * Retrieves a point in the region for which is considered property is satisfied.
             * If such a point is not known, the returned map is empty.
             */
            std::map<VariableType, CoefficientType> getSatPoint() const;
                
            /*!
             * Sets a point in the region for which the considered property is satisfied. 
             */
            void setSatPoint(std::map<VariableType, CoefficientType> const& satPoint);
            
            //returns the currently set check result as a string
            std::string checkResultToString() const;
     
            //returns the region as string in the format 0.3<=p<=0.4,0.2<=q<=0.5;
            std::string toString() const;

            /*
             * Can be used to parse a single parameter with its bounds from a string of the form "0.3<=p<=0.5".
             * The numbers are parsed as doubles and then converted to SparseDtmcRegionModelChecker::CoefficientType.
             * According to the given precision, the lower bound may be rounded down and the upper bound may be rounded up.
             * If no precision is given, the one from the settings is used.
             * The results will be inserted in the given maps
             * 
             */
            static void parseParameterBounds( 
                    std::map<VariableType, CoefficientType>& lowerBounds,
                    std::map<VariableType, CoefficientType>& upperBounds,
                    std::string const& parameterBoundsString,
                    double const precision=0.0
            );

            /*
             * Can be used to parse a single region from a string of the form "0.3<=p<=0.5,0.4<=q<=0.7".
             * The numbers are parsed as doubles and then converted to SparseDtmcRegionModelChecker::CoefficientType.
             * According to the given precision, the lower bound may be rounded down and the upper bound may be rounded up.
             * If no precision is given, the one from the settings is used.
             * 
             */
            static ParameterRegion parseRegion(
                    std::string const& regionString,
                    double precision=0.0);

            /*
             * Can be used to parse a vector of region from a string of the form "0.3<=p<=0.5,0.4<=q<=0.7;0.1<=p<=0.3,0.2<=q<=0.4".
             * The numbers are parsed as doubles and then converted to SparseDtmcRegionModelChecker::CoefficientType.
             * According to the given precision, the lower bound may be rounded down and the upper bound may be rounded up.
             * If no precision is given, the one from the settings is used.
             * 
             */
            static std::vector<ParameterRegion> parseMultipleRegions(
                    std::string const& regionsString,
                    double precision=0.0);


            /*
             * Retrieves the regions that are specified in the settings.
             * The numbers are parsed as doubles and then converted to SparseDtmcRegionModelChecker::CoefficientType.
             * According to the given precision, the lower bound may be rounded down and the upper bound may be rounded up.
             * If no precision is given, the one from the settings is used.
             * 
             */
            static std::vector<ParameterRegion> getRegionsFromSettings(double precision=0.0);

            private:

            std::map<VariableType, CoefficientType> const lowerBounds;
            std::map<VariableType, CoefficientType> const upperBounds;
            std::set<VariableType> variables;
            RegionCheckResult checkResult;
            std::map<VariableType, CoefficientType> satPoint;
            std::map<VariableType, CoefficientType> violatedPoint;
        };
    }
}

#endif	/* STORM_MODELCHECKER_REGION_PARAMETERREGION_H */

