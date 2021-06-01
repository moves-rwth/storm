#include "DFTGalileoParser.h"

#include <iostream>
#include <fstream>
#include <regex>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>
#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/FileIoException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/WrongFormatException.h"
#include "storm/utility/macros.h"
#include "storm/io/file.h"

namespace storm {
    namespace parser {

        template<typename ValueType>
        std::string DFTGalileoParser<ValueType>::parseName(std::string const& name) {
            size_t firstQuots = name.find("\"");
            size_t secondQuots = name.find("\"", firstQuots+1);
            std::string parsedName;
            
            if (firstQuots == std::string::npos) {
                parsedName = name;
            } else {
                STORM_LOG_THROW(secondQuots != std::string::npos, storm::exceptions::WrongFormatException, "No ending quotation mark found in " << name);
                parsedName = name.substr(firstQuots+1,secondQuots-1);
            }
            return boost::replace_all_copy(parsedName, "'", "__prime__");
        }

        template<typename ValueType>
        storm::storage::DFT<ValueType>
        DFTGalileoParser<ValueType>::parseDFT(const std::string &filename, bool defaultInclusive) {
            storm::builder::DFTBuilder<ValueType> builder(defaultInclusive);
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
            try {
                while (storm::utility::getline(file, line)) {
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
                        toplevelId = parseName(tokens[1]);
                    } else if (tokens[0] == "param") {
                        // Parameters
                        STORM_LOG_THROW(tokens.size() == 2, storm::exceptions::WrongFormatException, "Expected parameter name after 'param' in line " << lineNo << ".");
                        STORM_LOG_THROW((std::is_same<ValueType, storm::RationalFunction>::value), storm::exceptions::NotSupportedException, "Parameters only allowed when using rational functions.");
                        valueParser.addParameter(parseName(tokens[1]));
                    } else {
                        // DFT element
                        std::string name = parseName(tokens[0]);

                        std::vector<std::string> childNames;
                        for(size_t i = 2; i < tokens.size(); ++i) {
                            childNames.push_back(parseName(tokens[i]));
                        }
                        bool success = true;

                        // Add element according to type
                        std::string type = tokens[1];
                        if (type == "and") {
                            success = builder.addAndElement(name, childNames);
                        } else if (type == "or") {
                            success = builder.addOrElement(name, childNames);
                        } else if (boost::starts_with(type, "vot")) {
                            size_t threshold = storm::parser::parseNumber<size_t>(type.substr(3));
                            success = builder.addVotElement(name, threshold, childNames);
                        } else if (type.find("of") != std::string::npos) {
                            size_t pos = type.find("of");
                            size_t threshold = storm::parser::parseNumber<size_t>(type.substr(0, pos));
                            size_t count = storm::parser::parseNumber<size_t>(type.substr(pos + 2));
                            STORM_LOG_THROW(count == childNames.size(), storm::exceptions::WrongFormatException, "Voting gate number " << count << " does not correspond to number of children " << childNames.size() << " in line " << lineNo << ".");
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
                        } else if (type == "mutex") {
                            success = builder.addMutex(name, childNames);
                        } else if (type == "fdep") {
                            STORM_LOG_THROW(childNames.size() >= 2, storm::exceptions::WrongFormatException, "FDEP gate needs at least two children in line " << lineNo << ".");
                            success = builder.addDepElement(name, childNames, storm::utility::one<ValueType>());
                        } else if (boost::starts_with(type, "pdep=")) {
                            STORM_LOG_THROW(childNames.size() >= 2, storm::exceptions::WrongFormatException, "PDEP gate needs at least two children in line " << lineNo << ".");
                            ValueType probability = valueParser.parseValue(type.substr(5));
                            success = builder.addDepElement(name, childNames, probability);
                        } else if (type.find("=") != std::string::npos) {
                            success = parseBasicElement(tokens[0], line, lineNo, builder, valueParser);
                        } else if (type.find("insp") != std::string::npos) {
                            // Inspection as defined by DFTCalc
                            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Inspections (defined in line " << lineNo << ") are not supported.");
                        } else {
                            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Type name: " << type << " in line " << lineNo << " not recognized.");
                            success = false;
                        }
                        STORM_LOG_THROW(success, storm::exceptions::FileIoException, "Error while adding element '" << name << "' in line " << lineNo << ".");
                    }
                }
            } catch (storm::exceptions::BaseException const& exception) {
                STORM_LOG_THROW(false, storm::exceptions::FileIoException, "A parsing exception occurred in line " << lineNo << ": " << exception.what());
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

        template<typename ValueType>
        std::pair<bool, ValueType> DFTGalileoParser<ValueType>::parseValue(std::string name, std::string& line, ValueParser<ValueType>& valueParser) {
            // Build regex for: name=(value)
            std::regex nameRegex(name + "\\s*=\\s*([^\\s]*)");
            std::smatch match;
            if (std::regex_search(line, match, nameRegex)) {
                std::string value = match.str(1);
                // Remove matched part
                line = std::regex_replace(line, nameRegex, "");
                return std::make_pair(true, valueParser.parseValue(value));
            } else {
                // No match found
                return std::make_pair(false, storm::utility::zero<ValueType>());
            }
        }

        template<typename ValueType>
        std::pair<bool, size_t> DFTGalileoParser<ValueType>::parseNumber(std::string name, std::string& line) {
            // Build regex for: name=(number)
            std::regex nameRegex(name + "\\s*=\\s*([[:digit:]]+)");
            std::smatch match;
            if (std::regex_search(line, match, nameRegex)) {
                std::string value = match.str(1);
                // Remove matched part
                line = std::regex_replace(line, nameRegex, "");
                return std::make_pair(true, storm::parser::parseNumber<size_t>(value));
            } else {
                // No match found
                return std::make_pair(false, 0);
            }
        }

        template<typename ValueType>
        bool DFTGalileoParser<ValueType>::parseBasicElement(std::string const& name, std::string const& input, size_t lineNo, storm::builder::DFTBuilder<ValueType>& builder, ValueParser<ValueType>& valueParser) {
            // Default values
            Distribution distribution = Distribution::None;
            ValueType firstValDistribution = storm::utility::zero<ValueType>();
            ValueType secondValDistribution = storm::utility::zero<ValueType>();
            ValueType dormancyFactor = storm::utility::one<ValueType>();
            size_t replication = 1;
            size_t erlangPhases = 1;
            // Remove name from input
            std::regex regexName("\"?" + name + "\"?");
            std::string line = std::regex_replace(input, regexName, "");

            // Parse properties and determine distribution
            // Constant distribution
            std::pair<bool, ValueType> result = parseValue("prob", line, valueParser);
            if (result.first) {
                STORM_LOG_THROW(distribution == Distribution::None, storm::exceptions::WrongFormatException, "A different distribution was already defined for basic element '" << name << "' in line " << lineNo << ".");
                firstValDistribution = result.second;
                distribution = Distribution::Constant;
            }
            // Exponential distribution
            result = parseValue("lambda", line, valueParser);
            if (result.first) {
                STORM_LOG_THROW(distribution == Distribution::None || distribution == Distribution::Erlang, storm::exceptions::WrongFormatException, "A different distribution was already defined for basic element '" << name << "' in line " << lineNo << ".");
                firstValDistribution = result.second;
                if (distribution == Distribution::None) {
                    distribution = Distribution::Exponential;
                }
            }
            // Erlang distribution
            std::pair<bool, size_t> resultNum = parseNumber("phases", line);
            if (resultNum.first) {
                STORM_LOG_THROW(distribution == Distribution::None || distribution == Distribution::Exponential, storm::exceptions::WrongFormatException, "A different distribution was already defined for basic element '" << name << "' in line " << lineNo << ".");
                erlangPhases = resultNum.second;
                distribution = Distribution::Erlang;
            }
            // Weibull distribution
            result = parseValue("rate", line, valueParser);
            if (result.first) {
                STORM_LOG_THROW(distribution == Distribution::None || distribution == Distribution::Weibull, storm::exceptions::WrongFormatException, "A different distribution was already defined for basic element '" << name << "' in line " << lineNo << ".");
                firstValDistribution = result.second;
                distribution = Distribution::Weibull;
            }
            result = parseValue("shape", line, valueParser);
            if (result.first) {
                STORM_LOG_THROW(distribution == Distribution::None || distribution == Distribution::Weibull, storm::exceptions::WrongFormatException, "A different distribution was already defined for basic element '" << name << "' in line " << lineNo << ".");
                secondValDistribution = result.second;
                distribution = Distribution::Weibull;
            }
            // Lognormal distribution
            result = parseValue("mean", line, valueParser);
            if (result.first) {
                STORM_LOG_THROW(distribution == Distribution::None || distribution == Distribution::LogNormal, storm::exceptions::WrongFormatException, "A different distribution was already defined for basic element '" << name << "' in line " << lineNo << ".");
                firstValDistribution = result.second;
                distribution = Distribution::LogNormal;
            }
            result = parseValue("stddev", line, valueParser);
            if (result.first) {
                STORM_LOG_THROW(distribution == Distribution::None || distribution == Distribution::LogNormal, storm::exceptions::WrongFormatException, "A different distribution was already defined for basic element '" << name << "' in line " << lineNo << ".");
                secondValDistribution = result.second;
                distribution = Distribution::LogNormal;
            }
            // Additional arguments
            result = parseValue("cov", line, valueParser);
            if (result.first) {
                STORM_LOG_WARN("Coverage is not supported and will be ignored for basic element '" << name << "' in line " << lineNo << ".");
            }
            result = parseValue("res", line, valueParser);
            if (result.first) {
                STORM_LOG_WARN("Restoration is not supported and will be ignored for basic element '" << name << "' in line " << lineNo << ".");
            }
            resultNum = parseNumber("repl", line);
            if (resultNum.first) {
                replication = resultNum.second;
                STORM_LOG_THROW(replication == 1, storm::exceptions::NotSupportedException, "Replication > 1 is not supported for basic element '" << name << "' in line " << lineNo << ".");
            }
            result = parseValue("interval", line, valueParser);
            if (result.first) {
                STORM_LOG_WARN("Interval is not supported and will be ignored for basic element '" << name << "' in line " << lineNo << ".");
            }
            result = parseValue("dorm", line, valueParser);
            if (result.first) {
                dormancyFactor = result.second;
            } else {
                STORM_LOG_WARN("No dormancy factor was provided for basic element '" << name << "' in line " << lineNo << ". Assuming dormancy factor of 1.");
            }
            boost::trim(line);
            if (line != "") {
                STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "Unknown arguments for basic element '" << name << "' in line " << lineNo << ": " << line);
            }

            switch (distribution) {
                case Constant:
                    if (storm::utility::isZero(firstValDistribution) || storm::utility::isOne(firstValDistribution)) {
                        return builder.addBasicElementProbability(parseName(name), firstValDistribution, dormancyFactor, false); // TODO set transient BEs
                    } else {
                        // Model constant BEs with probability 0 < p < 1
                        bool success = true;
                        if (!builder.nameInUse("constantBeTrigger")) {
                            // Use a unique constantly failed element that triggers failsafe elements probabilistically
                            success = success && builder.addBasicElementProbability("constantBeTrigger",
                                                                                    storm::utility::one<ValueType>(),
                                                                                    storm::utility::one<ValueType>(),
                                                                                    false);
                        }
                        std::vector<std::string> childNames;
                        childNames.push_back("constantBeTrigger");
                        success = success &&
                                  builder.addBasicElementProbability(parseName(name), storm::utility::zero<ValueType>(),
                                                                     storm::utility::one<ValueType>(), false);
                        childNames.push_back(parseName(name));
                        return success &&
                               builder.addDepElement(parseName(name) + "_pdep", childNames, firstValDistribution);
                    }
                    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Constant distribution is not supported for basic element '" << name << "' in line " << lineNo << ".");
                    break;
                case Exponential:
                    return builder.addBasicElementExponential(parseName(name), firstValDistribution, dormancyFactor, false); // TODO set transient BEs
                    break;
                case Erlang:
                    if (erlangPhases == 1) {
                        // Erlang distribution reduces to exponential distribution
                        return builder.addBasicElementExponential(parseName(name), firstValDistribution, dormancyFactor, false); // TODO set transient BEs
                    } else {
                        // Model Erlang distribution by using SEQ over BEs instead.
                        // For each phase a BE is added, then the SEQ ensures the ordered failure.
                        STORM_LOG_WARN("Erlang distribution for basic element '" << name << "' in line " << lineNo << " is modelled by SEQ gate and BEs.");
                        std::string origName = parseName(name);
                        std::vector<std::string> childNames;
                        bool success = builder.addBasicElementExponential(origName, firstValDistribution, dormancyFactor, false); // TODO set transient BEs
                        for (size_t i = 0; i < erlangPhases - 1; ++i) {
                            std::string beName = origName + "_" + std::to_string(i);
                            childNames.push_back(beName);
                            success = success && builder.addBasicElementExponential(beName, firstValDistribution, dormancyFactor, false); // TODO set transient BEs
                        }
                        childNames.push_back(origName);
                        return success && builder.addSequenceEnforcer(origName + "_seq", childNames);
                    }
                    break;
                case Weibull:
                    if (storm::utility::isOne<ValueType>(secondValDistribution)) {
                        // Weibull distribution reduces to exponential distribution
                        return builder.addBasicElementExponential(parseName(name), firstValDistribution, dormancyFactor, false); // TODO set transient BEs
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Weibull distribution is not supported for basic element '" << name << "' in line " << lineNo << ".");
                    }
                    break;
                case LogNormal:
                    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "LogNormal distribution is not supported for basic element '" << name << "' in line " << lineNo << ".");
                    break;
                case None:
                    // go-through
                default:
                    STORM_LOG_THROW(false, storm::exceptions::WrongFormatException, "No distribution defined for basic element '" << name << "' in line " << lineNo << ".");
                    break;
            }
            return false;

        }

        // Explicitly instantiate the class.
        template class DFTGalileoParser<double>;
        template class DFTGalileoParser<RationalFunction>;

    }
}
