#pragma once

#include <map>

#include "storm/utility/parametric.h"

namespace storm {
    namespace storage {
        template<typename ParametricType>
        class ParameterRegion{
        public:
            typedef typename storm::utility::parametric::VariableType<ParametricType>::type VariableType;
            typedef typename storm::utility::parametric::CoefficientType<ParametricType>::type CoefficientType;
            typedef typename storm::utility::parametric::Valuation<ParametricType> Valuation;
            
            ParameterRegion(Valuation const& lowerBoundaries, Valuation const& upperBoundaries);
            ParameterRegion(Valuation&& lowerBoundaries, Valuation&& upperBoundaries);
            ParameterRegion(ParameterRegion const& other) = default;
            ParameterRegion(ParameterRegion&& other) = default;
            
            virtual ~ParameterRegion() = default;

            std::set<VariableType> const& getVariables() const;
            CoefficientType const& getLowerBoundary(VariableType const& variable) const;
            CoefficientType const& getUpperBoundary(VariableType const& variable) const;
            Valuation const& getLowerBoundaries() const;
            Valuation const& getUpperBoundaries() const;

            /*!
             * Returns a vector of all possible combinations of lower and upper bounds of the given variables.
             * The first entry of the returned vector will map every variable to its lower bound
             * The second entry will map every variable to its lower bound, except the first one (i.e. *getVariables.begin())
             * ...
             * The last entry will map every variable to its upper bound
             *
             * If the given set of variables is empty, the returned vector will contain an empty map
             */
            std::vector<Valuation> getVerticesOfRegion(std::set<VariableType> const& consideredVariables) const;

            /*!
             * Returns some point that lies within this region
             */
            Valuation getSomePoint() const;

            /*!
             * Returns the center point of this region
             */
            Valuation getCenterPoint() const;
                
            /*!
             * Returns the area of this region
             */
            CoefficientType area() const;
            
            /*!
             * Splits the region at the given point and inserts the resulting subregions at the end of the given vector.
             * It is assumed that the point lies within this region.
             * Subregions with area()==0 are not inserted in the vector.
             */
            void split(Valuation const& splittingPoint, std::vector<ParameterRegion<ParametricType>>& regionVector) const;

            //returns the region as string in the format 0.3<=p<=0.4,0.2<=q<=0.5;
            std::string toString(bool boundariesAsDouble = false) const;

            /*
             * Can be used to parse a single parameter with its boundaries from a string of the form "0.3<=p<=0.5".
             * The numbers are parsed as doubles and then converted to SparseDtmcRegionModelChecker::CoefficientType.
             * The results will be inserted in the given maps
             *
             */
            static void parseParameterBoundaries( Valuation& lowerBoundaries, Valuation& upperBoundaries, std::string const& parameterBoundariesString, std::set<VariableType> const& consideredVariables);

            /*
             * Can be used to parse a single region from a string of the form "0.3<=p<=0.5,0.4<=q<=0.7".
             * The numbers are parsed as doubles and then converted to SparseDtmcRegionModelChecker::CoefficientType.
             *
             */
            static ParameterRegion parseRegion(std::string const& regionString, std::set<VariableType> const& consideredVariables);

            /*
             * Can be used to parse a vector of region from a string of the form "0.3<=p<=0.5,0.4<=q<=0.7;0.1<=p<=0.3,0.2<=q<=0.4".
             * The numbers are parsed as doubles and then converted to SparseDtmcRegionModelChecker::CoefficientType.
             *
             */
            static std::vector<ParameterRegion> parseMultipleRegions(std::string const& regionsString, std::set<VariableType> const& consideredVariables);

        private:

            void init();
            
            Valuation lowerBoundaries;
            Valuation upperBoundaries;
            std::set<VariableType> variables;
        };
    }
}


