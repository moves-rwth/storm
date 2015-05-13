/* 
 * File:   Regions.cpp
 * Author: Tim Quatmann
 * 
 * Created on May 13, 2015, 12:54 PM
 */

#include <string>

#include "src/utility/regions.h"
#include "src/exceptions/NotImplementedException.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "adapters/CarlAdapter.h"

namespace storm {
    namespace utility{
        namespace regions {
            
            template<typename ParametricType, typename ConstantType>
            void RegionParser<ParametricType, ConstantType>::parseParameterBounds(
                    std::map<VariableType, BoundType>& lowerBounds,
                    std::map<VariableType, BoundType>& upperBounds,
                    std::string const& parameterBoundsString,
                    double const precision){
                double actualPrecision = (precision==0.0 ? storm::settings::generalSettings().getPrecision() : precision);
                
                std::string::size_type positionOfFirstRelation = parameterBoundsString.find("<=");
                STORM_LOG_THROW(positionOfFirstRelation!=std::string::npos, storm::exceptions::InvalidArgumentException, "When parsing the region" << parameterBoundsString << " I could not find  a '<=' after the first number");
                std::string::size_type positionOfSecondRelation = parameterBoundsString.find("<=", positionOfFirstRelation+2);
                STORM_LOG_THROW(positionOfSecondRelation!=std::string::npos, storm::exceptions::InvalidArgumentException, "When parsing the region" << parameterBoundsString << " I could not find  a '<=' after the parameter");
                
                std::string parameter=parameterBoundsString.substr(positionOfFirstRelation+2,positionOfSecondRelation-(positionOfFirstRelation+2));
                //removes all whitespaces from the parameter string:
                parameter.erase(std::remove_if(parameter.begin(), parameter.end(), isspace), parameter.end());
                STORM_LOG_THROW(parameter.length()>0, storm::exceptions::InvalidArgumentException, "When parsing the region" << parameterBoundsString << " I could not find a parameter");
                double lowerBound, upperBound;
                try{
                    lowerBound=std::stod(parameterBoundsString.substr(0,positionOfFirstRelation));
                    upperBound=std::stod(parameterBoundsString.substr(positionOfSecondRelation+2));
                }
                catch (std::exception const& exception) {
                    STORM_LOG_ERROR("Failed to parse the region: " << parameterBoundsString << ". The correct format for regions is lowerBound<=parameter<=upperbound");
                    throw exception;
                }
                
                VariableType var = getVariableFromString<VariableType>(parameter);
                BoundType lb = convertNumber<double, BoundType>(lowerBound, true, actualPrecision);
                BoundType ub = convertNumber<double, BoundType>(upperBound, false, actualPrecision);
                lowerBounds.emplace(std::make_pair(var, lb));  
                upperBounds.emplace(std::make_pair(var, ub));
                std::cout << "parsed bounds " << parameterBoundsString << ": lb=" << lowerBound << " ub=" << upperBound << " param='" << parameter << "' precision=" << actualPrecision << std::endl;
            }
            
            template<typename ParametricType, typename ConstantType>
            typename RegionParser<ParametricType, ConstantType>::ParameterRegion RegionParser<ParametricType, ConstantType>::parseRegion(std::string const& regionString, double precision){
                double actualPrecision = (precision==0.0 ? storm::settings::generalSettings().getPrecision() : precision);
                std::map<VariableType, BoundType> lowerBounds;
                std::map<VariableType, BoundType> upperBounds;
                std::vector<std::string> parameterBounds;
                boost::split(parameterBounds, regionString, boost::is_any_of(","));
                for(auto const& parameterBound : parameterBounds){
                    std::cout << "parsing a parameter bound" << std::endl;
                    RegionParser<ParametricType, ConstantType>::parseParameterBounds(lowerBounds, upperBounds, parameterBound, actualPrecision);
                    std::cout << "created a parameter bound. lower bound has size" << lowerBounds.size() << std::endl;
                    std::cout << "parsing a param bound is done" << std::endl;
                }
                return ParameterRegion(lowerBounds, upperBounds);
            }
            
            template<typename ParametricType, typename ConstantType>
            std::vector<typename RegionParser<ParametricType, ConstantType>::ParameterRegion> RegionParser<ParametricType, ConstantType>::parseMultipleRegions(std::string const& regionsString, double precision){
                double actualPrecision = (precision==0.0 ? storm::settings::generalSettings().getPrecision() : precision);
                std::vector<ParameterRegion> result;
                std::vector<std::string> regionsStrVec;
                boost::split(regionsStrVec, regionsString, boost::is_any_of(";"));
                for(auto const& regionStr : regionsStrVec){
                    std::cout << "parsing a region" << std::endl;
                    result.emplace_back(RegionParser<ParametricType, ConstantType>::parseRegion(regionStr, actualPrecision));
                    std::cout << "parsing a region is done" << std::endl;
                }
                return result;
            }
            
            template<>
            storm::RationalFunction::CoeffType convertNumber<double, storm::RationalFunction::CoeffType>(double const& number, bool const& roundDown, double const& precision){
                double actualPrecision = (precision==0.0 ? storm::settings::generalSettings().getPrecision() : precision);
                uint_fast64_t denominator = 1.0/actualPrecision;
                uint_fast64_t numerator;
                if(roundDown){
                    numerator= number*denominator; //this will always round down if necessary
                } else{
                    numerator = number*denominator*10; //*10 to look whether we have to round up
                    if(numerator%10>0){
                        numerator+=10;
                    }
                    numerator/=10;
                }
                storm::RationalFunction::CoeffType result=numerator;
                result = result/denominator;
                return result;
            }
            
            template<>
            storm::RationalFunction convertNumber<double, storm::RationalFunction>(double const& number, bool const& roundDown, double const& precision){
                return storm::RationalFunction(convertNumber<double, storm::RationalFunction::CoeffType>(number, roundDown, precision));
            }
            
            template<>
            double convertNumber<cln::cl_RA, double>(cln::cl_RA const& number, bool const& roundDown, double const& precision){
                return cln::double_approx(number);
            }
            
            template<typename SourceType, typename TargetType>
            TargetType convertNumber(SourceType const& number, bool const& roundDown, double const& precision){
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "number conversion between the given types not implemented");
            }
            
            template<>
            storm::Variable getVariableFromString<storm::Variable>(std::string variableString){
                storm::Variable const& var = carl::VariablePool::getInstance().findVariableWithName(variableString);
                STORM_LOG_THROW(var!=carl::Variable::NO_VARIABLE, storm::exceptions::InvalidArgumentException, "Variable '" + variableString + "' could not be found.");
                return var;
            }
            
            template<typename VariableType>
            VariableType getVariableFromString(std::string variableString){
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "Variable from String not implemented for this Type");
            }
            
            //explicit instantiations
#ifdef STORM_HAVE_CARL
       template class RegionParser<storm::RationalFunction, double>;
       
       template storm::RationalFunction convertNumber<double, storm::RationalFunction>(double const& number, bool const& roundDown, double const& precision);
       template storm::RationalFunction::CoeffType convertNumber<double, storm::RationalFunction::CoeffType>(double const& number, bool const& roundDown, double const& precision);
       template double convertNumber<cln::cl_RA, double>(storm::RationalFunction::CoeffType const& number, bool const& roundDown, double const& precision);
       
       template storm::Variable getVariableFromString<storm::Variable>(std::string variableString);
#endif 
        }
    }
}
