/* 
 * File:   ParameterRegion.cpp
 * Author: tim
 * 
 * Created on August 10, 2015, 1:51 PM
 */

#include "src/modelchecker/region/ParameterRegion.h"

#include "src/utility/regions.h"
#include "src/settings/SettingsManager.h"
#include "src/settings/modules/RegionSettings.h"

#include "src/exceptions/UnexpectedException.h"
#include "exceptions/InvalidSettingsException.h"
#include "exceptions/InvalidArgumentException.h"
#include "parser/MappedFile.h"

namespace storm {
    namespace modelchecker {

        template<typename ParametricType, typename ConstantType>
        SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ParameterRegion::ParameterRegion(std::map<VariableType, CoefficientType> lowerBounds, std::map<VariableType, CoefficientType> upperBounds) : lowerBounds(lowerBounds), upperBounds(upperBounds), checkResult(RegionCheckResult::UNKNOWN) {
            //check whether both mappings map the same variables and precompute the set of variables
            for (auto const& variableWithBound : lowerBounds) {
                STORM_LOG_THROW((upperBounds.find(variableWithBound.first) != upperBounds.end()), storm::exceptions::InvalidArgumentException, "Couldn't create region. No upper bound specified for Variable " << variableWithBound.first);
                this->variables.insert(variableWithBound.first);
            }
            for (auto const& variableWithBound : upperBounds) {
                STORM_LOG_THROW((this->variables.find(variableWithBound.first) != this->variables.end()), storm::exceptions::InvalidArgumentException, "Couldn't create region. No lower bound specified for Variable " << variableWithBound.first);
            }
        }

        template<typename ParametricType, typename ConstantType>
        SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ParameterRegion::~ParameterRegion() {
            //Intentionally left empty
        }

        template<typename ParametricType, typename ConstantType>
        std::set<typename SparseDtmcRegionModelChecker<ParametricType, ConstantType>::VariableType> SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ParameterRegion::getVariables() const {
            return this->variables;
        }

        template<typename ParametricType, typename ConstantType>
        typename SparseDtmcRegionModelChecker<ParametricType, ConstantType>::CoefficientType const& SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ParameterRegion::getLowerBound(VariableType const& variable) const {
            auto const& result = lowerBounds.find(variable);
            STORM_LOG_THROW(result != lowerBounds.end(), storm::exceptions::InvalidArgumentException, "tried to find a lower bound of a variable that is not specified by this region");
            return (*result).second;
        }

        template<typename ParametricType, typename ConstantType>
        typename SparseDtmcRegionModelChecker<ParametricType, ConstantType>::CoefficientType const& SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ParameterRegion::getUpperBound(VariableType const& variable) const {
            auto const& result = upperBounds.find(variable);
            STORM_LOG_THROW(result != upperBounds.end(), storm::exceptions::InvalidArgumentException, "tried to find an upper bound of a variable that is not specified by this region");
            return (*result).second;
        }

        template<typename ParametricType, typename ConstantType>
        const std::map<typename SparseDtmcRegionModelChecker<ParametricType, ConstantType>::VariableType, typename SparseDtmcRegionModelChecker<ParametricType, ConstantType>::CoefficientType> SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ParameterRegion::getUpperBounds() const {
            return upperBounds;
        }

        template<typename ParametricType, typename ConstantType>
        const std::map<typename SparseDtmcRegionModelChecker<ParametricType, ConstantType>::VariableType, typename SparseDtmcRegionModelChecker<ParametricType, ConstantType>::CoefficientType> SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ParameterRegion::getLowerBounds() const {
            return lowerBounds;
        }
        
        template<typename ParametricType, typename ConstantType>
        std::vector<std::map<typename SparseDtmcRegionModelChecker<ParametricType, ConstantType>::VariableType, typename SparseDtmcRegionModelChecker<ParametricType, ConstantType>::CoefficientType>> SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ParameterRegion::getVerticesOfRegion(std::set<VariableType> const& consideredVariables) const {
            std::size_t const numOfVariables = consideredVariables.size();
            std::size_t const numOfVertices = std::pow(2, numOfVariables);
            std::vector<std::map<VariableType, CoefficientType >> resultingVector(numOfVertices, std::map<VariableType, CoefficientType>());
            if (numOfVertices == 1) {
                //no variables are given, the returned vector should still contain an empty map
                return resultingVector;
            }

            for (uint_fast64_t vertexId = 0; vertexId < numOfVertices; ++vertexId) {
                //interprete vertexId as a bit sequence
                //the consideredVariables.size() least significant bits of vertex will always represent the next vertex
                //(00...0 = lower bounds for all variables, 11...1 = upper bounds for all variables)
                std::size_t variableIndex = 0;
                for (auto const& variable : consideredVariables) {
                    if ((vertexId >> variableIndex) % 2 == 0) {
                        resultingVector[vertexId].insert(std::pair<VariableType, CoefficientType>(variable, getLowerBound(variable)));
                    } else {
                        resultingVector[vertexId].insert(std::pair<VariableType, CoefficientType>(variable, getUpperBound(variable)));
                    }
                    ++variableIndex;
                }
            }
            return resultingVector;
        }
        
        template<typename ParametricType, typename ConstantType>
        typename SparseDtmcRegionModelChecker<ParametricType, ConstantType>::RegionCheckResult SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ParameterRegion::getCheckResult() const {
            return checkResult;
        }

        template<typename ParametricType, typename ConstantType>
        void SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ParameterRegion::setCheckResult(RegionCheckResult checkResult) {
            //a few sanity checks
            STORM_LOG_THROW((this->checkResult == RegionCheckResult::UNKNOWN || checkResult != RegionCheckResult::UNKNOWN), storm::exceptions::InvalidArgumentException, "Tried to change the check result of a region from something known to UNKNOWN ");
            STORM_LOG_THROW((this->checkResult != RegionCheckResult::EXISTSSAT || checkResult != RegionCheckResult::EXISTSVIOLATED), storm::exceptions::InvalidArgumentException, "Tried to change the check result of a region from EXISTSSAT to EXISTSVIOLATED");
            STORM_LOG_THROW((this->checkResult != RegionCheckResult::EXISTSSAT || checkResult != RegionCheckResult::ALLVIOLATED), storm::exceptions::InvalidArgumentException, "Tried to change the check result of a region from EXISTSSAT to ALLVIOLATED");
            STORM_LOG_THROW((this->checkResult != RegionCheckResult::EXISTSVIOLATED || checkResult != RegionCheckResult::EXISTSSAT), storm::exceptions::InvalidArgumentException, "Tried to change the check result of a region from EXISTSVIOLATED to EXISTSSAT");
            STORM_LOG_THROW((this->checkResult != RegionCheckResult::EXISTSVIOLATED || checkResult != RegionCheckResult::ALLSAT), storm::exceptions::InvalidArgumentException, "Tried to change the check result of a region from EXISTSVIOLATED to ALLSAT");
            STORM_LOG_THROW((this->checkResult != RegionCheckResult::EXISTSBOTH || checkResult != RegionCheckResult::ALLSAT), storm::exceptions::InvalidArgumentException, "Tried to change the check result of a region from EXISTSBOTH to ALLSAT");
            STORM_LOG_THROW((this->checkResult != RegionCheckResult::EXISTSBOTH || checkResult != RegionCheckResult::ALLVIOLATED), storm::exceptions::InvalidArgumentException, "Tried to change the check result of a region from EXISTSBOTH to ALLVIOLATED");
            STORM_LOG_THROW((this->checkResult != RegionCheckResult::ALLSAT || checkResult == RegionCheckResult::ALLSAT), storm::exceptions::InvalidArgumentException, "Tried to change the check result of a region from ALLSAT to something else");
            STORM_LOG_THROW((this->checkResult != RegionCheckResult::ALLVIOLATED || checkResult == RegionCheckResult::ALLVIOLATED), storm::exceptions::InvalidArgumentException, "Tried to change the check result of a region from ALLVIOLATED to something else");
            this->checkResult = checkResult;
        }

        template<typename ParametricType, typename ConstantType>
        std::map<typename SparseDtmcRegionModelChecker<ParametricType, ConstantType>::VariableType, typename SparseDtmcRegionModelChecker<ParametricType, ConstantType>::CoefficientType> SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ParameterRegion::getViolatedPoint() const {
            return violatedPoint;
        }
        
        template<typename ParametricType, typename ConstantType>
        void SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ParameterRegion::setViolatedPoint(std::map<VariableType, CoefficientType> const& violatedPoint) {
            this->violatedPoint = violatedPoint;
        }

        template<typename ParametricType, typename ConstantType>
        std::map<typename SparseDtmcRegionModelChecker<ParametricType, ConstantType>::VariableType, typename SparseDtmcRegionModelChecker<ParametricType, ConstantType>::CoefficientType> SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ParameterRegion::getSatPoint() const {
            return satPoint;
        }

        template<typename ParametricType, typename ConstantType>
        void SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ParameterRegion::setSatPoint(std::map<VariableType, CoefficientType> const& satPoint) {
            this->satPoint = satPoint;
        }

        template<typename ParametricType, typename ConstantType>
        std::string SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ParameterRegion::checkResultToString() const {
            switch (this->checkResult) {
                case RegionCheckResult::UNKNOWN:
                    return "unknown";
                case RegionCheckResult::EXISTSSAT:
                    return "ExistsSat";
                case RegionCheckResult::EXISTSVIOLATED:
                    return "ExistsViolated";
                case RegionCheckResult::EXISTSBOTH:
                    return "ExistsBoth";
                case RegionCheckResult::ALLSAT:
                    return "allSat";
                case RegionCheckResult::ALLVIOLATED:
                    return "allViolated";
            }
            STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Could not identify check result")
            return "ERROR";
        }

        template<typename ParametricType, typename ConstantType>
        std::string SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ParameterRegion::toString() const {
            std::stringstream regionstringstream;
            for (auto var : this->getVariables()) {
                regionstringstream << storm::utility::regions::convertNumber<SparseDtmcRegionModelChecker<ParametricType, ConstantType>::CoefficientType, double>(this->getLowerBound(var));
                regionstringstream << "<=";
                regionstringstream << storm::utility::regions::getVariableName(var);
                regionstringstream << "<=";
                regionstringstream << storm::utility::regions::convertNumber<SparseDtmcRegionModelChecker<ParametricType, ConstantType>::CoefficientType, double>(this->getUpperBound(var));
                regionstringstream << ",";
            }
            std::string regionstring = regionstringstream.str();
            //the last comma should actually be a semicolon
            regionstring = regionstring.substr(0, regionstring.length() - 1) + ";";
            return regionstring;
        }

        
        
              template<typename ParametricType, typename ConstantType>
            void SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ParameterRegion::parseParameterBounds(
                    std::map<VariableType, CoefficientType>& lowerBounds,
                    std::map<VariableType, CoefficientType>& upperBounds,
                    std::string const& parameterBoundsString){
                
                std::string::size_type positionOfFirstRelation = parameterBoundsString.find("<=");
                STORM_LOG_THROW(positionOfFirstRelation!=std::string::npos, storm::exceptions::InvalidArgumentException, "When parsing the region" << parameterBoundsString << " I could not find  a '<=' after the first number");
                std::string::size_type positionOfSecondRelation = parameterBoundsString.find("<=", positionOfFirstRelation+2);
                STORM_LOG_THROW(positionOfSecondRelation!=std::string::npos, storm::exceptions::InvalidArgumentException, "When parsing the region" << parameterBoundsString << " I could not find  a '<=' after the parameter");
                
                std::string parameter=parameterBoundsString.substr(positionOfFirstRelation+2,positionOfSecondRelation-(positionOfFirstRelation+2));
                //removes all whitespaces from the parameter string:
                parameter.erase(std::remove_if(parameter.begin(), parameter.end(), ::isspace), parameter.end());
                STORM_LOG_THROW(parameter.length()>0, storm::exceptions::InvalidArgumentException, "When parsing the region" << parameterBoundsString << " I could not find a parameter");
                
                VariableType var = storm::utility::regions::getVariableFromString<VariableType>(parameter);
                CoefficientType lb = storm::utility::regions::convertNumber<std::string, CoefficientType>(parameterBoundsString.substr(0,positionOfFirstRelation));
                CoefficientType ub = storm::utility::regions::convertNumber<std::string, CoefficientType>(parameterBoundsString.substr(positionOfSecondRelation+2));
                lowerBounds.emplace(std::make_pair(var, lb));  
                upperBounds.emplace(std::make_pair(var, ub));
            }
            
            template<typename ParametricType, typename ConstantType>
            typename SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ParameterRegion SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ParameterRegion::parseRegion(
                    std::string const& regionString){
                std::map<VariableType, CoefficientType> lowerBounds;
                std::map<VariableType, CoefficientType> upperBounds;
                std::vector<std::string> parameterBounds;
                boost::split(parameterBounds, regionString, boost::is_any_of(","));
                for(auto const& parameterBound : parameterBounds){
                    parseParameterBounds(lowerBounds, upperBounds, parameterBound);
                }
                return ParameterRegion(lowerBounds, upperBounds);
            }
            
            template<typename ParametricType, typename ConstantType>
            std::vector<typename SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ParameterRegion> SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ParameterRegion::parseMultipleRegions(
                    std::string const& regionsString){
                std::vector<ParameterRegion> result;
                std::vector<std::string> regionsStrVec;
                boost::split(regionsStrVec, regionsString, boost::is_any_of(";"));
                for(auto const& regionStr : regionsStrVec){
                    if(!std::all_of(regionStr.begin(),regionStr.end(), ::isspace)){ //skip this string if it only consists of space
                    result.emplace_back(parseRegion(regionStr));
                    }
                }
                return result;
            }
            
            template<typename ParametricType, typename ConstantType>
            std::vector<typename SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ParameterRegion> SparseDtmcRegionModelChecker<ParametricType, ConstantType>::ParameterRegion::getRegionsFromSettings(){
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
                return parseMultipleRegions(regionsString);
            }
#ifdef STORM_HAVE_CARL
        template class SparseDtmcRegionModelChecker<storm::RationalFunction, double>::ParameterRegion;
#endif

    }
}

