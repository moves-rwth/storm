#include "DFTGalileoParser.h"

#include <iostream>
#include <fstream>
#include <regex>

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>
#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/FileIoException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/WrongFormatException.h"
#include "storm/parser/ValueParser.h"
#include "storm/utility/macros.h"
#include "storm/utility/file.h"

namespace storm {
    namespace parser {

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
        storm::storage::DFT<ValueType> DFTGalileoParser<ValueType>::parseDFT(const std::string& filename, bool defaultInclusive, bool binaryDependencies) {
            storm::storage::DFTBuilder<ValueType> builder(defaultInclusive, binaryDependencies);
            ValueParser<ValueType> valueParser;
            // Regular expression to detect comments
            // taken from: https://stackoverflow.com/questions/9449887/removing-c-c-style-comments-using-boostregex
            const std::regex commentRegex("(/\\*([^*]|(\\*+[^*/]))*\\*+/)|(//.*)");

            std::ifstream file;
            storm::utility::openFile(filename, file);

            std::string line;
            size_t lineNo = 0;
            std::string toplevelId = "";
            bool comment = false; // Indicates whether the current line is part of a multiline comment

            while (std::getline(file, line)) {
                ++lineNo;
                // First consider comments
                if (comment) {
                    // Line is part of multiline comment -> search for end of this comment
                    size_t commentEnd = line.find("*/");
                    if (commentEnd == std::string::npos) {
                        continue;
                    } else {
                        // Remove comment
                        line = line.substr(commentEnd + 2);
                        comment = false;
                    }
                }
                // Remove comments
                line = std::regex_replace(line, commentRegex, "");
                // Check if multiline comment starts
                size_t commentStart = line.find("/*");
                if (commentStart != std::string::npos) {
                    // Remove comment
                    line = line.substr(0, commentStart);
                    comment = true;
                }

                boost::trim(line);
                if (line.empty()) {
                    // Empty line
                    continue;
                }

                // Remove semicolon
                STORM_LOG_THROW(line.back() == ';', storm::exceptions::WrongFormatException, "Semicolon expected at the end of line " << lineNo << ".");
                line.pop_back();

                // Split line into tokens
                boost::trim(line);
                std::vector<std::string> tokens;
                boost::split(tokens, line, boost::is_any_of(" \t"), boost::token_compress_on);

                if (tokens[0] == "toplevel") {
                    // Top level indicator
                    STORM_LOG_THROW(toplevelId == "", storm::exceptions::WrongFormatException, "Toplevel element already defined.");
                    STORM_LOG_THROW(tokens.size() == 2, storm::exceptions::WrongFormatException, "Expected element id after 'toplevel' in line " << lineNo << ".");
                    toplevelId = stripQuotsFromName(tokens[1]);
                } else if (tokens[0] == "param") {
                    // Parameters
                    STORM_LOG_THROW(tokens.size() == 2, storm::exceptions::WrongFormatException, "Expected parameter name after 'param' in line " << lineNo << ".");
                    STORM_LOG_THROW((std::is_same<ValueType, storm::RationalFunction>::value), storm::exceptions::NotSupportedException, "Parameters only allowed when using rational functions.");
                    valueParser.addParameter(stripQuotsFromName(tokens[1]));
                } else {
                    // DFT element
                    std::string name = parseNodeIdentifier(stripQuotsFromName(tokens[0]));

                    std::vector<std::string> childNames;
                    for(unsigned i = 2; i < tokens.size(); ++i) {
                        childNames.push_back(parseNodeIdentifier(stripQuotsFromName(tokens[i])));
                    }
                    bool success = true;

                    // Add element according to type
                    std::string type = tokens[1];
                    if (type == "and") {
                        success = builder.addAndElement(name, childNames);
                    } else if (type == "or") {
                        success = builder.addOrElement(name, childNames);
                    } else if (boost::starts_with(type, "vot")) {
                        unsigned threshold = boost::lexical_cast<unsigned>(type.substr(3));
                        success = builder.addVotElement(name, threshold, childNames);
                    } else if (type.find("of") != std::string::npos) {
                        size_t pos = type.find("of");
                        unsigned threshold = boost::lexical_cast<unsigned>(type.substr(0, pos));
                        unsigned count = boost::lexical_cast<unsigned>(type.substr(pos + 2));
                        STORM_LOG_THROW(count == childNames.size(), storm::exceptions::WrongFormatException, "Voting gate number " << count << " does not correspond to number of children " << childNames.size() << "in line " << lineNo << ".");
                        success = builder.addVotElement(name, threshold, childNames);
                    } else if (type == "pand") {
                        success = builder.addPandElement(name, childNames, defaultInclusive);
                    } else if (type == "pand-inc") {
                        success = builder.addPandElement(name, childNames, true);
                    } else if (type == "pand-ex") {
                        success = builder.addPandElement(name, childNames, false);
                    } else if (type == "por") {
                        success = builder.addPorElement(name, childNames, defaultInclusive);
                    } else if (type == "por-ex") {
                        success = builder.addPorElement(name, childNames, false);
                    } else if (type == "por-inc") {
                        success = builder.addPorElement(name, childNames, true);
                    } else if (type == "wsp" || type == "csp" || type == "hsp") {
                        success = builder.addSpareElement(name, childNames);
                    } else if (type == "seq") {
                        success = builder.addSequenceEnforcer(name, childNames);
                    } else if (type == "fdep") {
                        STORM_LOG_THROW(childNames.size() >= 2, storm::exceptions::WrongFormatException, "FDEP gate needs at least two children in line " << lineNo << ".");
                        success = builder.addDepElement(name, childNames, storm::utility::one<ValueType>());
                    } else if (boost::starts_with(type, "pdep=")) {
                        ValueType probability = valueParser.parseValue(type.substr(5));
                        success = builder.addDepElement(name, childNames, probability);
                    } else if (boost::starts_with(type, "lambda=")) {
                        ValueType failureRate = valueParser.parseValue(type.substr(7));
                        ValueType dormancyFactor = valueParser.parseValue(tokens[2].substr(5));
                        success = builder.addBasicElement(name, failureRate, dormancyFactor, false); // TODO set transient BEs
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Type name: " << type << " in line " << lineNo << " not recognized.");
                        success = false;
                    }
                    STORM_LOG_THROW(success, storm::exceptions::FileIoException, "Error while adding element '" << name << "' in line " << lineNo << ".");
                }
            }

            if (!builder.setTopLevel(toplevelId)) {
                STORM_LOG_THROW(false, storm::exceptions::FileIoException, "Top level id '" << toplevelId << "' unknown.");
            }
            storm::utility::closeFile(file);

            // Build DFT
            storm::storage::DFT<ValueType> dft = builder.build();
            STORM_LOG_DEBUG("DFT Elements:" << std::endl << dft.getElementsString());
            STORM_LOG_DEBUG("Spare Modules:" << std::endl << dft.getSpareModulesString());
            return dft;
        }

        // Explicitly instantiate the class.
        template class DFTGalileoParser<double>;
        template class DFTGalileoParser<RationalFunction>;

    }
}
