#include "storm/modelchecker/region/ParameterRegion.h"

#include "storm/utility/region.h"
#include "storm/utility/macros.h"
#include "storm/parser/MappedFile.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/RegionSettings.h"
#include "storm/exceptions/InvalidSettingsException.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/utility/constants.h"
#include "storm/utility/file.h"

namespace storm {
    namespace modelchecker {
        namespace region {

            template<typename ParametricType>
            ParameterRegion<ParametricType>::ParameterRegion(VariableSubstitutionType const& lowerBoundaries, VariableSubstitutionType const& upperBoundaries) : lowerBoundaries(lowerBoundaries), upperBoundaries(upperBoundaries), checkResult(RegionCheckResult::UNKNOWN) {
                init();
            }

            template<typename ParametricType>
            ParameterRegion<ParametricType>::ParameterRegion(VariableSubstitutionType&& lowerBoundaries, VariableSubstitutionType&& upperBoundaries) : lowerBoundaries(std::move(lowerBoundaries)), upperBoundaries(std::move(upperBoundaries)), checkResult(RegionCheckResult::UNKNOWN) {
                init();
            }

            template<typename ParametricType>
            void ParameterRegion<ParametricType>::init() {
                //check whether both mappings map the same variables, check that lowerboundary <= upper boundary,  and pre-compute the set of variables
                for (auto const& variableWithLowerBoundary : this->lowerBoundaries) {
                    auto variableWithUpperBoundary = this->upperBoundaries.find(variableWithLowerBoundary.first);
                    STORM_LOG_THROW((variableWithUpperBoundary != upperBoundaries.end()), storm::exceptions::InvalidArgumentException, "Couldn't create region. No upper boundary specified for Variable " << variableWithLowerBoundary.first);
                    STORM_LOG_THROW((variableWithLowerBoundary.second<=variableWithUpperBoundary->second), storm::exceptions::InvalidArgumentException, "Couldn't create region. The lower boundary for " << variableWithLowerBoundary.first << " is larger then the upper boundary");
                    this->variables.insert(variableWithLowerBoundary.first);
                }
                for (auto const& variableWithBoundary : this->upperBoundaries) {
                    STORM_LOG_THROW((this->variables.find(variableWithBoundary.first) != this->variables.end()), storm::exceptions::InvalidArgumentException, "Couldn't create region. No lower boundary specified for Variable " << variableWithBoundary.first);
                }
            }

            template<typename ParametricType>
            ParameterRegion<ParametricType>::~ParameterRegion() {
                //Intentionally left empty
            }

            template<typename ParametricType>
            std::set<typename ParameterRegion<ParametricType>::VariableType> ParameterRegion<ParametricType>::getVariables() const {
                return this->variables;
            }

            template<typename ParametricType>
            typename ParameterRegion<ParametricType>::CoefficientType const& ParameterRegion<ParametricType>::getLowerBoundary(VariableType const& variable) const {
                auto const& result = lowerBoundaries.find(variable);
                STORM_LOG_THROW(result != lowerBoundaries.end(), storm::exceptions::InvalidArgumentException, "tried to find a lower boundary for variable " << variable << " which is not specified by this region");
                return (*result).second;
            }

            template<typename ParametricType>
            typename ParameterRegion<ParametricType>::CoefficientType const& ParameterRegion<ParametricType>::getUpperBoundary(VariableType const& variable) const {
                auto const& result = upperBoundaries.find(variable);
                STORM_LOG_THROW(result != upperBoundaries.end(), storm::exceptions::InvalidArgumentException, "tried to find an upper boundary for variable " << variable << " which is not specified by this region");
                return (*result).second;
            }

            template<typename ParametricType>
            const typename ParameterRegion<ParametricType>::VariableSubstitutionType ParameterRegion<ParametricType>::getUpperBoundaries() const {
                return upperBoundaries;
            }

            template<typename ParametricType>
            const typename ParameterRegion<ParametricType>::VariableSubstitutionType ParameterRegion<ParametricType>::getLowerBoundaries() const {
                return lowerBoundaries;
            }

            template<typename ParametricType>
            std::vector<typename ParameterRegion<ParametricType>::VariableSubstitutionType> ParameterRegion<ParametricType>::getVerticesOfRegion(std::set<VariableType> const& consideredVariables) const {
                std::size_t const numOfVariables = consideredVariables.size();
                std::size_t const numOfVertices = std::pow(2, numOfVariables);
                std::vector<VariableSubstitutionType> resultingVector(numOfVertices, VariableSubstitutionType());
                if (numOfVertices == 1) {
                    //no variables are given, the returned vector should still contain an empty map
                    return resultingVector;
                }

                for (uint_fast64_t vertexId = 0; vertexId < numOfVertices; ++vertexId) {
                    //interprete vertexId as a bit sequence
                    //the consideredVariables.size() least significant bits of vertex will always represent the next vertex
                    //(00...0 = lower boundaries for all variables, 11...1 = upper boundaries for all variables)
                    std::size_t variableIndex = 0;
                    for (auto const& variable : consideredVariables) {
                        if ((vertexId >> variableIndex) % 2 == 0) {
                            resultingVector[vertexId].insert(std::pair<VariableType, CoefficientType>(variable, getLowerBoundary(variable)));
                        } else {
                            resultingVector[vertexId].insert(std::pair<VariableType, CoefficientType>(variable, getUpperBoundary(variable)));
                        }
                        ++variableIndex;
                    }
                }
                return resultingVector;
            }

            template<typename ParametricType>
            typename ParameterRegion<ParametricType>::VariableSubstitutionType ParameterRegion<ParametricType>::getSomePoint() const {
                return this->getLowerBoundaries();
            }
            
            template<typename ParametricType>
            typename ParameterRegion<ParametricType>::VariableSubstitutionType ParameterRegion<ParametricType>::getCenterPoint() const {
                VariableSubstitutionType result;
                for (auto const& variable : this->variables) {
                    result.insert(typename VariableSubstitutionType::value_type(variable, (this->getLowerBoundary(variable) + this->getUpperBoundary(variable))/2));
                }
                return result;
            }
            
            template<typename ParametricType>
            typename ParameterRegion<ParametricType>::CoefficientType ParameterRegion<ParametricType>::area() const{
                CoefficientType result = storm::utility::one<CoefficientType>();
                for( auto const& variable : this->variables){
                    result *= (this->getUpperBoundary(variable) - this->getLowerBoundary(variable));
                }
                return result;
            }

            template<typename ParametricType>
            void ParameterRegion<ParametricType>::split(VariableSubstitutionType const& splittingPoint, std::vector<ParameterRegion<ParametricType> >& regionVector) const{
                //Check if splittingPoint is valid.
                STORM_LOG_THROW(splittingPoint.size() == this->variables.size(), storm::exceptions::InvalidArgumentException, "Tried to split a region w.r.t. a point, but the point considers a different number of variables.");
                for(auto const& variable : this->variables){
                    auto splittingPointEntry=splittingPoint.find(variable);
                    STORM_LOG_THROW(splittingPointEntry != splittingPoint.end(), storm::exceptions::InvalidArgumentException, "tried to split a region but a variable of this region is not defined by the splitting point.");
                    STORM_LOG_THROW(this->getLowerBoundary(variable) <=splittingPointEntry->second, storm::exceptions::InvalidArgumentException, "tried to split a region but the splitting point is not contained in the region.");
                    STORM_LOG_THROW(this->getUpperBoundary(variable) >=splittingPointEntry->second, storm::exceptions::InvalidArgumentException, "tried to split a region but the splitting point is not contained in the region.");
                }
                
                //Now compute the subregions.
                std::vector<VariableSubstitutionType> vertices(this->getVerticesOfRegion(this->variables));
                for(auto const& vertex : vertices){
                    //The resulting subregion is the smallest region containing vertex and splittingPoint.
                    VariableSubstitutionType subLower, subUpper;
                    for(auto variableBound : this->lowerBoundaries){
                        VariableType variable = variableBound.first;
                        auto vertexEntry=vertex.find(variable);
                        auto splittingPointEntry=splittingPoint.find(variable);
                        subLower.insert(typename VariableSubstitutionType::value_type(variable, std::min(vertexEntry->second, splittingPointEntry->second)));
                        subUpper.insert(typename VariableSubstitutionType::value_type(variable, std::max(vertexEntry->second, splittingPointEntry->second)));
                    }
                    ParameterRegion<ParametricType> subRegion((subLower), (subUpper));
                    if(subRegion.area() != storm::utility::zero<CoefficientType>()){
                        regionVector.push_back((subRegion));
                    }
                }
            }
            
            template<typename ParametricType>
            RegionCheckResult ParameterRegion<ParametricType>::getCheckResult() const {
                return checkResult;
            }

            template<typename ParametricType>
            void ParameterRegion<ParametricType>::setCheckResult(RegionCheckResult checkResult) {
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

            template<typename ParametricType>
            typename ParameterRegion<ParametricType>::VariableSubstitutionType ParameterRegion<ParametricType>::getViolatedPoint() const {
                return violatedPoint;
            }

            template<typename ParametricType>
            void ParameterRegion<ParametricType>::setViolatedPoint(VariableSubstitutionType const& violatedPoint) {
                this->violatedPoint = violatedPoint;
            }

            template<typename ParametricType>
            typename ParameterRegion<ParametricType>::VariableSubstitutionType ParameterRegion<ParametricType>::getSatPoint() const {
                return satPoint;
            }

            template<typename ParametricType>
            void ParameterRegion<ParametricType>::setSatPoint(VariableSubstitutionType const& satPoint) {
                this->satPoint = satPoint;
            }

            template<typename ParametricType>
            void ParameterRegion<ParametricType>::fixVariables(std::map<VariableType, RegionBoundary> const& fixedVariables) {
                this->fixedVariables = fixedVariables;
            }

            template<typename ParametricType>
            std::map<typename ParameterRegion<ParametricType>::VariableType, RegionBoundary> ParameterRegion<ParametricType>::getFixedVariables() const {
                return fixedVariables;
            }

            template<typename ParametricType>
            std::string ParameterRegion<ParametricType>::toString() const {
                std::stringstream regionstringstream;
                for (auto var : this->getVariables()) {
                    regionstringstream << storm::utility::convertNumber<double>(this->getLowerBoundary(var));
                    regionstringstream << "<=";
                    regionstringstream << storm::utility::region::getVariableName(var);
                    regionstringstream << "<=";
                    regionstringstream << storm::utility::convertNumber<double>(this->getUpperBoundary(var));
                    regionstringstream << ",";
                }
                std::string regionstring = regionstringstream.str();
                //the last comma should actually be a semicolon
                regionstring = regionstring.substr(0, regionstring.length() - 1) + ";";
                return regionstring;
            }



                  template<typename ParametricType>
                void ParameterRegion<ParametricType>::parseParameterBoundaries(
                        VariableSubstitutionType& lowerBoundaries,
                        VariableSubstitutionType& upperBoundaries,
                        std::string const& parameterBoundariesString){

                    std::string::size_type positionOfFirstRelation = parameterBoundariesString.find("<=");
                    STORM_LOG_THROW(positionOfFirstRelation!=std::string::npos, storm::exceptions::InvalidArgumentException, "When parsing the region" << parameterBoundariesString << " I could not find  a '<=' after the first number");
                    std::string::size_type positionOfSecondRelation = parameterBoundariesString.find("<=", positionOfFirstRelation+2);
                    STORM_LOG_THROW(positionOfSecondRelation!=std::string::npos, storm::exceptions::InvalidArgumentException, "When parsing the region" << parameterBoundariesString << " I could not find  a '<=' after the parameter");

                    std::string parameter=parameterBoundariesString.substr(positionOfFirstRelation+2,positionOfSecondRelation-(positionOfFirstRelation+2));
                    //removes all whitespaces from the parameter string:
                    parameter.erase(std::remove_if(parameter.begin(), parameter.end(), ::isspace), parameter.end());
                    STORM_LOG_THROW(parameter.length()>0, storm::exceptions::InvalidArgumentException, "When parsing the region" << parameterBoundariesString << " I could not find a parameter");

                    VariableType var = storm::utility::region::getVariableFromString<VariableType>(parameter);
                    CoefficientType lb = storm::utility::convertNumber<CoefficientType>(parameterBoundariesString.substr(0,positionOfFirstRelation));
                    CoefficientType ub = storm::utility::convertNumber<CoefficientType>(parameterBoundariesString.substr(positionOfSecondRelation+2));
                    lowerBoundaries.emplace(std::make_pair(var, lb));  
                    upperBoundaries.emplace(std::make_pair(var, ub));
                }

                template<typename ParametricType>
                ParameterRegion<ParametricType> ParameterRegion<ParametricType>::parseRegion(
                        std::string const& regionString){
                    VariableSubstitutionType lowerBoundaries;
                    VariableSubstitutionType upperBoundaries;
                    std::vector<std::string> parameterBoundaries;
                    boost::split(parameterBoundaries, regionString, boost::is_any_of(","));
                    for(auto const& parameterBoundary : parameterBoundaries){
                        if(!std::all_of(parameterBoundary.begin(),parameterBoundary.end(), ::isspace)){ //skip this string if it only consists of space
                            parseParameterBoundaries(lowerBoundaries, upperBoundaries, parameterBoundary);
                        }
                    }
                    return ParameterRegion(std::move(lowerBoundaries), std::move(upperBoundaries));
                }

                template<typename ParametricType>
                std::vector<ParameterRegion<ParametricType>> ParameterRegion<ParametricType>::parseMultipleRegions(
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

                template<typename ParametricType>
                std::vector<ParameterRegion<ParametricType>> ParameterRegion<ParametricType>::getRegionsFromSettings(){
                    STORM_LOG_THROW(storm::settings::getModule<storm::settings::modules::RegionSettings>().isRegionsSet() ||storm::settings::getModule<storm::settings::modules::RegionSettings>().isRegionFileSet(), storm::exceptions::InvalidSettingsException, "Tried to obtain regions from the settings but no regions are specified.");
                    STORM_LOG_THROW(!(storm::settings::getModule<storm::settings::modules::RegionSettings>().isRegionsSet() && storm::settings::getModule<storm::settings::modules::RegionSettings>().isRegionFileSet()), storm::exceptions::InvalidSettingsException, "Regions are specified via file AND cmd line. Only one option is allowed.");

                    std::string regionsString;
                    if(storm::settings::getModule<storm::settings::modules::RegionSettings>().isRegionsSet()){
                        regionsString = storm::settings::getModule<storm::settings::modules::RegionSettings>().getRegionsFromCmdLine();
                    }
                    else{
                        //if we reach this point we can assume that the region is given as a file.
                        STORM_LOG_THROW(storm::utility::fileExistsAndIsReadable(storm::settings::getModule<storm::settings::modules::RegionSettings>().getRegionFilePath()), storm::exceptions::InvalidSettingsException, "The path to the file in which the regions are specified is not valid.");
                        storm::parser::MappedFile mf(storm::settings::getModule<storm::settings::modules::RegionSettings>().getRegionFilePath().c_str());
                        regionsString = std::string(mf.getData(),mf.getDataSize());
                    }
                    return parseMultipleRegions(regionsString);
                }
#ifdef STORM_HAVE_CARL
            template class ParameterRegion<storm::RationalFunction>;
#endif
        } //namespace region
    }
}

