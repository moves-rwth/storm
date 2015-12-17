#include "DFTGalileoParser.h"

#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "../exceptions/FileIoException.h"
#include "../exceptions/NotSupportedException.h"
#include "src/utility/macros.h"

namespace storm {
    namespace parser {

        template<typename ValueType>
        storm::storage::DFT DFTGalileoParser<ValueType>::parseDFT(const std::string& filename) {
            if(readFile(filename)) {
                storm::storage::DFT dft = mBuilder.build();
                STORM_LOG_DEBUG("Elements:" << std::endl << dft.getElementsString());
                STORM_LOG_DEBUG("Spare Modules:" << std::endl << dft.getSpareModulesString());
                return dft;
            } else {
                throw storm::exceptions::FileIoException();
            }
        }

        template<typename ValueType>
        std::string DFTGalileoParser<ValueType>::stripQuotsFromName(std::string const& name) {
            size_t firstQuots = name.find("\"");
            size_t secondQuots = name.find("\"", firstQuots+1);
            
            if(firstQuots == std::string::npos) {
                return name;
            } else if (secondQuots == std::string::npos) {
                STORM_LOG_THROW(false, storm::exceptions::FileIoException, "No ending quotation mark found in " << name);
            } else {
                return name.substr(firstQuots+1,secondQuots-1);
            }
        }

        template<typename ValueType>
        bool DFTGalileoParser<ValueType>::readFile(const std::string& filename) {
            // constants
            std::string topleveltoken = "toplevel";
            std::string toplevelId;

            std::ifstream file;
            file.exceptions ( std::ifstream::failbit );
            try {
                file.open(filename);
            }
            catch (std::ifstream::failure e) {
                STORM_LOG_THROW(false, storm::exceptions::FileIoException, "Exception during file opening on " << filename << ".");
                return false;
            }
            file.exceptions( 0 );

            std::string line;
            bool generalSuccess = true;
            while(std::getline(file, line))
            {
                bool success = true;
                STORM_LOG_TRACE("Parsing: " << line);
                size_t commentstarts = line.find("//");
                line = line.substr(0, commentstarts);
                size_t firstsemicolon = line.find(";");
                line = line.substr(0, firstsemicolon);
                if (line.find_first_not_of(' ') == std::string::npos)
                {
                    // Only whitespace
                    continue;
                }

                // Top level indicator.
                if(boost::starts_with(line, topleveltoken)) {
                    toplevelId = stripQuotsFromName(line.substr(topleveltoken.size() + 1));
                }
                else
                {
                    std::vector<std::string> tokens;
                    boost::split(tokens, line, boost::is_any_of(" "));
                    std::string name(stripQuotsFromName(tokens[0]));

                    std::vector<std::string> childNames;
                    for(unsigned i = 2; i < tokens.size(); ++i) {
                        childNames.push_back(stripQuotsFromName(tokens[i]));
                    }
                    if(tokens[1] == "and") {
                        success = mBuilder.addAndElement(name, childNames);
                    } else if(tokens[1] == "or") {
                        success = mBuilder.addOrElement(name, childNames);
                    } else if(boost::starts_with(tokens[1], "vot")) {
                        success = mBuilder.addVotElement(name, boost::lexical_cast<unsigned>(tokens[1].substr(3)), childNames);
                    } else if(tokens[1] == "pand") {
                        success = mBuilder.addPandElement(name, childNames);
                    } else if(tokens[1] == "wsp" || tokens[1] == "csp") {
                        success = mBuilder.addSpareElement(name, childNames);
                    } else if(boost::starts_with(tokens[1], "lambda=")) {
                        //TODO Matthias: Use ValueType instead of fixed double
                        success = mBuilder.addBasicElement(name, boost::lexical_cast<double>(tokens[1].substr(7)), boost::lexical_cast<double>(tokens[2].substr(5)));
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Type name: " + tokens[1] + "  not recognized.");
                        success = false;
                    }
                }
                if (generalSuccess) {
                    generalSuccess = success;
                }
            }
            if(!mBuilder.setTopLevel(toplevelId)) {
                STORM_LOG_THROW(false, storm::exceptions::FileIoException, "Top level id unknown.");
            }
            file.close();
            return generalSuccess;
        }

        // Explicitly instantiate the class.
        template class DFTGalileoParser<double>;

#ifdef STORM_HAVE_CARL
        template class DFTGalileoParser<RationalFunction>;
#endif
        
    }
}