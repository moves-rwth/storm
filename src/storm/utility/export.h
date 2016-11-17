/** 
 * @file:   export.h
 * @author: Sebastian Junges
 *
 * @since October 7, 2014
 */

#ifndef STORM_UTILITY_EXPORT_H_
#define STORM_UTILITY_EXPORT_H_

#include <iostream>
#include <boost/optional.hpp>

#include "storm/utility/macros.h"
#include "storm/exceptions/FileIoException.h"

//#include "storm/storage/parameters.h"
//#include "storm/settings/modules/ParametricSettings.h"
//#include "storm/modelchecker/reachability/CollectConstraints.h"

namespace storm {
    namespace utility {

            /* TODO Fix me
        template<typename ValueType>	
        void exportParametricMcResult(const ValueType& mcresult, storm::modelchecker::reachability::CollectConstraints<storm::RationalFunction> const& constraintCollector) {
            std::string path = storm::settings::getModule<storm::settings::modules::ParametricSettings>().exportResultPath();
            std::ofstream filestream;
            filestream.open(path);
            // todo add checks.
            filestream << "!Parameters: ";
            std::set<storm::RationalFunctionVariable> vars = mcresult.gatherVariables();
            std::copy(vars.begin(), vars.end(), std::ostream_iterator<storm::RationalFunctionVariable>(filestream, ", "));
            filestream << std::endl;
            filestream << "!Result: " << mcresult << std::endl;
            filestream << "!Well-formed Constraints: " << std::endl;
            std::copy(constraintCollector.wellformedConstraints().begin(), constraintCollector.wellformedConstraints().end(), std::ostream_iterator<carl::Constraint<ValueType>>(filestream, "\n"));
            filestream << "!Graph-preserving Constraints: " << std::endl;
            std::copy(constraintCollector.graphPreservingConstraints().begin(), constraintCollector.graphPreservingConstraints().end(), std::ostream_iterator<carl::Constraint<ValueType>>(filestream, "\n"));
            filestream.close();
        }
        */
            
        inline void exportStringToFile(std::string const& str, std::string filepath) {
            std::ofstream filestream;
            filestream.open(filepath);
            STORM_LOG_THROW(filestream.is_open(), storm::exceptions::FileIoException , "Could not open file " << filepath << ".");
            filestream << str;
        }
        
        template <typename ValueType>
        inline void exportDataToCSVFile(std::string filepath, std::vector<std::vector<ValueType>> const& data, boost::optional<std::vector<std::string>> const& columnHeaders) {
            std::ofstream filestream;
            filestream.open(filepath);
            STORM_LOG_THROW(filestream.is_open(), storm::exceptions::FileIoException , "Could not open file " << filepath << ".");
            
            if(columnHeaders) {
                for(auto columnIt = columnHeaders->begin(); columnIt != columnHeaders->end(); ++columnIt) {
                    if(columnIt != columnHeaders->begin()) {
                        filestream << ",";
                    }
                    filestream << *columnIt;
                }
                filestream << std::endl;
            }
            
            for (auto const& row : data) {
                for(auto columnIt = row.begin(); columnIt != row.end(); ++columnIt) {
                    if(columnIt != row.begin()) {
                        filestream << ",";
                    }
                    filestream << *columnIt;
                }
                filestream << std::endl;
            }
        }
    }
}



#endif
