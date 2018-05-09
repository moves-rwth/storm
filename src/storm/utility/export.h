#ifndef STORM_UTILITY_EXPORT_H_
#define STORM_UTILITY_EXPORT_H_

#include <iostream>
#include <boost/optional.hpp>

#include "storm/utility/macros.h"
#include "storm/utility/file.h"



namespace storm {
    namespace utility {


        
        template <typename DataType, typename Header1Type = DataType, typename Header2Type = DataType>
        inline void exportDataToCSVFile(std::string filepath, std::vector<std::vector<DataType>> const& data, boost::optional<std::vector<Header1Type>> const& header1 = boost::none, boost::optional<std::vector<Header2Type>> const& header2 = boost::none) {
            std::ofstream filestream;
            storm::utility::openFile(filepath, filestream);
            
            if (header1) {
                for(auto columnIt = header1->begin(); columnIt != header1->end(); ++columnIt) {
                    if(columnIt != header1->begin()) {
                        filestream << ",";
                    }
                    filestream << *columnIt;
                }
                filestream << std::endl;
            }
            
            if (header2) {
                for(auto columnIt = header2->begin(); columnIt != header2->end(); ++columnIt) {
                    if(columnIt != header2->begin()) {
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
