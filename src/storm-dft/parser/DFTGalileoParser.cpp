#include "DFTGalileoParser.h"

#include <iostream>
#include <fstream>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>
#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/FileIoException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/parser/ValueParser.h"
#include "storm/utility/macros.h"
#include "storm/utility/file.h"

namespace storm {
    namespace parser {

        template<typename ValueType>
        storm::storage::DFT<ValueType> DFTGalileoParser<ValueType>::parseDFT(const std::string& filename) {
            readFile(filename);
            storm::storage::DFT<ValueType> dft = builder.build();
            STORM_LOG_DEBUG("Elements:" << std::endl << dft.getElementsString());
            STORM_LOG_DEBUG("Spare Modules:" << std::endl << dft.getSpareModulesString());
            return dft;
        }

        template<typename ValueType>
        std::string DFTGalileoParser<ValueType>::stripQuotsFromName(std::string const& name) {
            size_t firstQuots = name.find("\"");
            size_t secondQuots = name.find("\"", firstQuots+1);
            
            if(firstQuots == std::string::npos) {
                return name;
            } else {
                STORM_LOG_THROW(secondQuots != std::string::npos, storm::exceptions::FileIoException, "No ending quotation mark found in " << name);
                return name.substr(firstQuots+1,secondQuots-1);
            }
        }
        
        template<typename ValueType>
        std::string DFTGalileoParser<ValueType>::parseNodeIdentifier(std::string const& name) {
            return boost::replace_all_copy(name, "'", "__prime__");
        }

        template<typename ValueType>
        void DFTGalileoParser<ValueType>::readFile(const std::string& filename) {
            // constants
            std::string toplevelToken = "toplevel";
            std::string toplevelId;
            std::string parametricToken = "param";

            std::ifstream file;
            storm::utility::openFile(filename, file);
            std::string line;

            ValueParser<ValueType> valueParser;

            while (std::getline(file, line)) {
                bool success = true;
                STORM_LOG_TRACE("Parsing: " << line);
                size_t commentstarts = line.find("//");
                line = line.substr(0, commentstarts);
                size_t firstsemicolon = line.find(";");
                line = line.substr(0, firstsemicolon);
                if (line.find_first_not_of(' ') == std::string::npos) {
                    // Only whitespace
                    continue;
                }

                // Top level indicator.
                if(boost::starts_with(line, toplevelToken)) {
                    toplevelId = stripQuotsFromName(line.substr(toplevelToken.size() + 1));
                }
                else if (boost::starts_with(line, parametricToken)) {
                    STORM_LOG_THROW((std::is_same<ValueType, storm::RationalFunction>::value), storm::exceptions::NotSupportedException, "Parameters only allowed when using rational functions.");
                    std::string parameter = stripQuotsFromName(line.substr(parametricToken.size() + 1));
                    valueParser.addParameter(parameter);
                } else {
                    std::vector<std::string> tokens;
                    boost::split(tokens, line, boost::is_any_of(" "));
                    std::string name(parseNodeIdentifier(stripQuotsFromName(tokens[0])));

                    std::vector<std::string> childNames;
                    for(unsigned i = 2; i < tokens.size(); ++i) {
                        childNames.push_back(parseNodeIdentifier(stripQuotsFromName(tokens[i])));
                    }
                    if(tokens[1] == "and") {
                        success = builder.addAndElement(name, childNames);
                    } else if (tokens[1] == "or") {
                        success = builder.addOrElement(name, childNames);
                    } else if (boost::starts_with(tokens[1], "vot")) {
                        success = builder.addVotElement(name, boost::lexical_cast<unsigned>(tokens[1].substr(3)), childNames);
                    } else if (tokens[1].find("of") != std::string::npos) {
                        size_t pos = tokens[1].find("of");
                        unsigned threshold = boost::lexical_cast<unsigned>(tokens[1].substr(0, pos));
                        unsigned count = boost::lexical_cast<unsigned>(tokens[1].substr(pos + 2));
                        STORM_LOG_THROW(count == childNames.size(), storm::exceptions::FileIoException, "Voting gate does not correspond to number of children.");
                        success = builder.addVotElement(name, threshold, childNames);
                    } else if (tokens[1] == "pand") {
                        success = builder.addPandElement(name, childNames);
                    } else if (tokens[1] == "pand-inc") {
                        success = builder.addPandElement(name, childNames, true);
                    } else if (tokens[1] == "pand-ex") {
                        success = builder.addPandElement(name, childNames, false);
                    } else if (tokens[1] == "por") {
                        success = builder.addPorElement(name, childNames);
                    } else if (tokens[1] == "por-ex") {
                        success = builder.addPorElement(name, childNames, false);
                    } else if (tokens[1] == "por-inc") {
                        success = builder.addPorElement(name, childNames, true);
                    } else if (tokens[1] == "wsp" || tokens[1] == "csp") {
                        success = builder.addSpareElement(name, childNames);
                    } else if (tokens[1] == "seq") {
                        success = builder.addSequenceEnforcer(name, childNames);
                    } else if (tokens[1] == "fdep") {
                        success = builder.addDepElement(name, childNames, storm::utility::one<ValueType>());
                    } else if (boost::starts_with(tokens[1], "pdep=")) {
                        ValueType probability = valueParser.parseValue(tokens[1].substr(5));
                        success = builder.addDepElement(name, childNames, probability);
                    } else if (boost::starts_with(tokens[1], "lambda=")) {
                        ValueType failureRate = valueParser.parseValue(tokens[1].substr(7));
                        ValueType dormancyFactor = valueParser.parseValue(tokens[2].substr(5));
                        success = builder.addBasicElement(name, failureRate, dormancyFactor, false); // TODO set transient BEs
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Type name: " << tokens[1] << "  not recognized.");
                        success = false;
                    }
                    STORM_LOG_THROW(success, storm::exceptions::FileIoException, "Error while adding element '" << name << "' of line '" << line << "'.");
                }
            }
            if(!builder.setTopLevel(toplevelId)) {
                STORM_LOG_THROW(false, storm::exceptions::FileIoException, "Top level id unknown.");
            }
            storm::utility::closeFile(file);
        }

        // Explicitly instantiate the class.
        template class DFTGalileoParser<double>;
        template class DFTGalileoParser<RationalFunction>;

    }
}
