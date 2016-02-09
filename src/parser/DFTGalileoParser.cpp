#include "DFTGalileoParser.h"

#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include "src/storage/expressions/ExpressionManager.h"
#include "src/exceptions/NotImplementedException.h"
#include "src/exceptions/FileIoException.h"
#include "src/exceptions/NotSupportedException.h"
#include "src/utility/macros.h"

namespace storm {
    namespace parser {

        template<typename ValueType>
        storm::storage::DFT<ValueType> DFTGalileoParser<ValueType>::parseDFT(const std::string& filename) {
            if(readFile(filename)) {
                storm::storage::DFT<ValueType> dft = builder.build();
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
            } else {
                STORM_LOG_THROW(secondQuots != std::string::npos, storm::exceptions::FileIoException, "No ending quotation mark found in " << name);
                return name.substr(firstQuots+1,secondQuots-1);
            }
        }

        template<typename ValueType>
        bool DFTGalileoParser<ValueType>::readFile(const std::string& filename) {
            // constants
            std::string toplevelToken = "toplevel";
            std::string toplevelId;
            std::string parametricToken = "param";

            std::ifstream file;
            file.exceptions ( std::ifstream::failbit );
            try {
                file.open(filename);
            }
            catch (std::ifstream::failure e) {
                STORM_LOG_THROW(false, storm::exceptions::FileIoException, "Exception during file opening on " << filename << ".");
                return false;
            }
            file.exceptions( std::ifstream::goodbit );

            std::string line;
            bool generalSuccess = true;
            while(std::getline(file, line)) {
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
                    storm::expressions::Variable var = manager->declareRationalVariable(parameter);
                    identifierMapping.emplace(var.getName(), var);
                    parser.setIdentifierMapping(identifierMapping);
                    STORM_LOG_TRACE("Added parameter: " << var.getName());
                } else {
                    std::vector<std::string> tokens;
                    boost::split(tokens, line, boost::is_any_of(" "));
                    std::string name(stripQuotsFromName(tokens[0]));

                    std::vector<std::string> childNames;
                    for(unsigned i = 2; i < tokens.size(); ++i) {
                        childNames.push_back(stripQuotsFromName(tokens[i]));
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
                    } else if (tokens[1] == "wsp" || tokens[1] == "csp") {
                        success = builder.addSpareElement(name, childNames);
                    } else if (tokens[1] == "fdep") {
                        success = builder.addDepElement(name, childNames, storm::utility::one<ValueType>());
                    } else if (boost::starts_with(tokens[1], "pdep=")) {
                        ValueType probability = parseRationalExpression(tokens[1].substr(5));
                        success = builder.addDepElement(name, childNames, probability);
                    } else if (boost::starts_with(tokens[1], "lambda=")) {
                        ValueType failureRate = parseRationalExpression(tokens[1].substr(7));
                        ValueType dormancyFactor = parseRationalExpression(tokens[2].substr(5));
                        success = builder.addBasicElement(name, failureRate, dormancyFactor);
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Type name: " << tokens[1] << "  not recognized.");
                        success = false;
                    }
                }
                if (generalSuccess) {
                    generalSuccess = success;
                }
            }
            if(!builder.setTopLevel(toplevelId)) {
                STORM_LOG_THROW(false, storm::exceptions::FileIoException, "Top level id unknown.");
            }
            file.close();
            return generalSuccess;
        }

        template<typename ValueType>
        ValueType DFTGalileoParser<ValueType>::parseRationalExpression(std::string const& expr) {
            assert(false);
        }

        template<>
        double DFTGalileoParser<double>::parseRationalExpression(std::string const& expr) {
            return boost::lexical_cast<double>(expr);
        }

        // Explicitly instantiate the class.
        template class DFTGalileoParser<double>;

#ifdef STORM_HAVE_CARL
        template<>
        storm::RationalFunction DFTGalileoParser<storm::RationalFunction>::parseRationalExpression(std::string const& expr) {
            STORM_LOG_TRACE("Translating expression: " << expr);
            storm::expressions::Expression expression = parser.parseFromString(expr);
            STORM_LOG_TRACE("Expression: " << expression);
            storm::RationalFunction rationalFunction = evaluator.asRational(expression);
            STORM_LOG_TRACE("Parsed expression: " << rationalFunction);
            return rationalFunction;
        }

        template class DFTGalileoParser<RationalFunction>;
#endif
        
    }
}
