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
#include "exceptions/InvalidSettingsException.h"
#include "parser/MappedFile.h"

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
                STORM_LOG_WARN_COND((lowerBound==convertNumber<BoundType, double>(lb, true, actualPrecision)), "The lower bound of '"<< parameterBoundsString << "' could not be parsed accurately. Increase precision?");
                BoundType ub = convertNumber<double, BoundType>(upperBound, false, actualPrecision);
                STORM_LOG_WARN_COND((upperBound==convertNumber<BoundType, double>(ub, true, actualPrecision)), "The upper bound of '"<< parameterBoundsString << "' could not be parsed accurately. Increase precision?");
                lowerBounds.emplace(std::make_pair(var, lb));  
                upperBounds.emplace(std::make_pair(var, ub));
               // std::cout << "parsed bounds " << parameterBoundsString << ": lb=" << lowerBound << " ub=" << upperBound << " param='" << parameter << "' precision=" << actualPrecision << std::endl;
            }
            
            template<typename ParametricType, typename ConstantType>
            typename RegionParser<ParametricType, ConstantType>::ParameterRegion RegionParser<ParametricType, ConstantType>::parseRegion(std::string const& regionString, double precision){
                double actualPrecision = (precision==0.0 ? storm::settings::generalSettings().getPrecision() : precision);
                std::map<VariableType, BoundType> lowerBounds;
                std::map<VariableType, BoundType> upperBounds;
                std::vector<std::string> parameterBounds;
                boost::split(parameterBounds, regionString, boost::is_any_of(","));
                for(auto const& parameterBound : parameterBounds){
                    RegionParser<ParametricType, ConstantType>::parseParameterBounds(lowerBounds, upperBounds, parameterBound, actualPrecision);
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
                    if(!std::all_of(regionStr.begin(),regionStr.end(),isspace)){ //skip this string if it only consists of space
                    result.emplace_back(RegionParser<ParametricType, ConstantType>::parseRegion(regionStr, actualPrecision));
                    }
                }
                return result;
            }
            
            template<typename ParametricType, typename ConstantType>
            std::vector<typename RegionParser<ParametricType, ConstantType>::ParameterRegion> RegionParser<ParametricType, ConstantType>::getRegionsFromSettings(double precision){
                STORM_LOG_THROW(storm::settings::regionSettings().isRegionsSet() || storm::settings::regionSettings().isRegionFileSet(), storm::exceptions::InvalidSettingsException, "Tried to obtain regions from the settings but no regions are specified.");
                STORM_LOG_THROW(!(storm::settings::regionSettings().isRegionsSet() && storm::settings::regionSettings().isRegionFileSet()), storm::exceptions::InvalidSettingsException, "Regions are specified via file AND cmd line. Only one option is allowed.");
                
                std::string regionsString;
                if(storm::settings::regionSettings().isRegionsSet()){
                    regionsString = storm::settings::regionSettings().getRegionsFromCmdLine();
                }
                else{
                    //if we reach this point we can assume that the region is given as a file.
                    STORM_LOG_THROW(storm::parser::MappedFile::fileExistsAndIsReadable(storm::settings::regionSettings().getRegionFilePath().c_str()), storm::exceptions::InvalidSettingsException, "The path to the file in which the regions are specified is not valid.");
                    storm::parser::MappedFile mf(storm::settings::regionSettings().getRegionFilePath().c_str());
                    regionsString = std::string(mf.getData(),mf.getDataSize());
                }
                return RegionParser<ParametricType, ConstantType>::parseMultipleRegions(regionsString,precision);
            }
            
            
            template<>
            storm::RationalFunction::CoeffType convertNumber<double, storm::RationalFunction::CoeffType>(double const& number, bool const& roundDown, double const& precision){
                double actualPrecision = (precision==0.0 ? storm::settings::generalSettings().getPrecision() : precision);
                uint_fast64_t denominator = 1.0/actualPrecision;
                uint_fast64_t numerator;
                if(roundDown){
                    numerator= number*denominator; //this will always round down
                } else{
                    numerator = std::ceil(number*denominator);
                }
                storm::RationalFunction::CoeffType result(numerator);
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
            
            template<>
            double convertNumber<double, double>(double const& number, bool const& roundDown, double const& precision){
                return number;
            }
            
            template<>
            cln::cl_RA convertNumber<cln::cl_RA, cln::cl_RA>(cln::cl_RA const& number, bool const& roundDown, double const& precision){
                return number;
            }
                       
            
            template<>
            storm::Variable getVariableFromString<storm::Variable>(std::string variableString){
                storm::Variable const& var = carl::VariablePool::getInstance().findVariableWithName(variableString);
                STORM_LOG_THROW(var!=carl::Variable::NO_VARIABLE, storm::exceptions::InvalidArgumentException, "Variable '" + variableString + "' could not be found.");
                return var;
            }
                      
            template<>
            std::string getVariableName<storm::Variable>(storm::Variable variable){
                return carl::VariablePool::getInstance().getName(variable);
            }
                        
            template<>
            typename storm::modelchecker::SparseDtmcRegionModelChecker<storm::RationalFunction,double>::BoundType evaluateFunction<storm::RationalFunction, double>(
                    storm::RationalFunction const& function, 
                    std::map<typename storm::modelchecker::SparseDtmcRegionModelChecker<storm::RationalFunction,double>::VariableType,
                             typename storm::modelchecker::SparseDtmcRegionModelChecker<storm::RationalFunction,double>::BoundType> const& point){
                return function.evaluate(point);
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
            
            //explicit instantiations
       template double convertNumber<double, double>(double const& number, bool const& roundDown, double const& precision);
       
#ifdef STORM_HAVE_CARL
       template class RegionParser<storm::RationalFunction, double>;
       
       template storm::RationalFunction convertNumber<double, storm::RationalFunction>(double const& number, bool const& roundDown, double const& precision);
       template storm::RationalFunction::CoeffType convertNumber<double, storm::RationalFunction::CoeffType>(double const& number, bool const& roundDown, double const& precision);
       template double convertNumber<cln::cl_RA, double>(storm::RationalFunction::CoeffType const& number, bool const& roundDown, double const& precision);
       template cln::cl_RA convertNumber<cln::cl_RA, cln::cl_RA>(cln::cl_RA const& number, bool const& roundDown, double const& precision);
       
       template storm::Variable getVariableFromString<storm::Variable>(std::string variableString);
       template std::string getVariableName<storm::Variable>(storm::Variable variable);
       
       template bool functionIsLinear<storm::RationalFunction>(storm::RationalFunction const& function);
#endif 
        }
    }
}
