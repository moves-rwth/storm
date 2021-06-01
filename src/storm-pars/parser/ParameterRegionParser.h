#pragma once

#include <map>

#include "storm-pars/storage/ParameterRegion.h"

namespace storm {
    namespace parser {
        template<typename ParametricType>
        class ParameterRegionParser{
        public:

            typedef typename storm::storage::ParameterRegion<ParametricType>::VariableType VariableType;
            typedef typename storm::storage::ParameterRegion<ParametricType>::CoefficientType CoefficientType;
            typedef typename storm::storage::ParameterRegion<ParametricType>::Valuation Valuation;
            
            /*
             * Parse a single parameter with its boundaries from a string of the form "0.3<=p<=0.5".
             * The results will be inserted in the given maps
             *
             */
            static void parseParameterBoundaries( Valuation& lowerBoundaries, Valuation& upperBoundaries, std::string const& parameterBoundariesString, std::set<VariableType> const& consideredVariables);

            /*
             * Parse a single region from a string of the form "0.3<=p<=0.5,0.4<=q<=0.7".
             *
             */
            static storm::storage::ParameterRegion<ParametricType> parseRegion(std::string const& regionString, std::set<VariableType> const& consideredVariables, boost::optional<int> const& splittingThreshold = boost::none);
            static storm::storage::ParameterRegion<ParametricType> createRegion(std::string const& regionBound, std::set<VariableType> const& consideredVariables, boost::optional<int> const& splittingThreshold = boost::none);

            /*
             * Parse a vector of region from a string of the form "0.3<=p<=0.5,0.4<=q<=0.7;0.1<=p<=0.3,0.2<=q<=0.4".
             *
             */
            static std::vector<storm::storage::ParameterRegion<ParametricType>> parseMultipleRegions(std::string const& regionsString, std::set<VariableType> const& consideredVariables, boost::optional<int> const& splittingThreshold = boost::none);
            

            /*
             * Parse multiple regions from a file
             *
             */
            static std::vector<storm::storage::ParameterRegion<ParametricType>> parseMultipleRegionsFromFile(std::string const& fileName, std::set<VariableType> const& consideredVariables, boost::optional<int> const& splittingThreshold = boost::none);
            
        };
    }
}


