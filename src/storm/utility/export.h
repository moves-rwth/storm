#ifndef STORM_UTILITY_EXPORT_H_
#define STORM_UTILITY_EXPORT_H_

#include <iostream>
#include <boost/optional.hpp>

#include "storm/utility/macros.h"
#include "storm/utility/file.h"



namespace storm {
    namespace utility {


        
        template <typename ValueType>
        inline void exportDataToCSVFile(std::string filepath, std::vector<std::vector<ValueType>> const& data, boost::optional<std::vector<std::string>> const& columnHeaders) {
            std::ofstream filestream;
            storm::utility::openFile(filepath, filestream);
            
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
            storm::utility::closeFile(filestream);
        }
    }
}

#endif
