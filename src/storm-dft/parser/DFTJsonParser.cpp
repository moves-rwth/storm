#include "DFTJsonParser.h"

#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/lexical_cast.hpp>

#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/FileIoException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/utility/macros.h"
#include "storm/io/file.h"
#include "storm-parsers/parser/ValueParser.h"

namespace storm {
    namespace parser {

        template<typename ValueType>
        storm::storage::DFT<ValueType> DFTJsonParser<ValueType>::parseJsonFromFile(std::string const& filename) {
            STORM_LOG_DEBUG("Parsing from JSON file");
            std::ifstream file;
            storm::utility::openFile(filename, file);
            Json jsonInput;
            jsonInput << file;
            storm::utility::closeFile(file);
            return parseJson(jsonInput);
        }

        template<typename ValueType>
        storm::storage::DFT<ValueType> DFTJsonParser<ValueType>::parseJsonFromString(std::string const& jsonString) {
            STORM_LOG_DEBUG("Parsing from JSON string");
            Json jsonInput = Json::parse(jsonString);
            return parseJson(jsonInput);
        }

        template<typename ValueType>
        storm::storage::DFT<ValueType> DFTJsonParser<ValueType>::parseJson(Json const& jsonInput) {
            // Init DFT builder and value parser
            storm::builder::DFTBuilder<ValueType> builder;
            ValueParser<ValueType> valueParser;

            // Try to parse parameters
            if (jsonInput.find("parameters") != jsonInput.end()) {
                Json parameters = jsonInput.at("parameters");
                STORM_LOG_THROW(parameters.empty() || (std::is_same<ValueType, storm::RationalFunction>::value), storm::exceptions::NotSupportedException,
                                "Parameters only allowed when using rational functions.");
                for (auto it = parameters.begin(); it != parameters.end(); ++it) {
                    std::string parameter = it.key();
                    valueParser.addParameter(parameter);
                    STORM_LOG_TRACE("Added parameter: " << parameter);
                }
            }

            Json nodes = jsonInput.at("nodes");
            // Start by building mapping from ids to their unique names
            std::map<std::string, std::string> nameMapping;
            std::set<std::string> names;
            for (auto& element : nodes) {
                Json data = element.at("data");
                std::string id = data.at("id");
                std::string uniqueName = generateUniqueName(data.at("name"));
                STORM_LOG_THROW(names.find(uniqueName) == names.end(), storm::exceptions::WrongFormatException, "Element '" << uniqueName << "' was already declared.");
                names.insert(uniqueName);
                nameMapping[id] = uniqueName;
            }

            // Parse nodes
            for (auto& element : nodes) {
                STORM_LOG_TRACE("Parsing: " << element);
                bool success = true;
                Json data = element.at("data");
                std::string name = generateUniqueName(data.at("name"));
                std::vector<std::string> childNames;
                if (data.count("children") > 0) {
                    for (std::string const& child : data.at("children")) {
                        STORM_LOG_THROW(nameMapping.find(child) != nameMapping.end(), storm::exceptions::WrongFormatException, "Child '" << child << "' was not defined.");
                        childNames.push_back(nameMapping.at(child));
                    }
                }

                std::string type = data.at("type");

                if (type == "and") {
                    success = builder.addAndElement(name, childNames);
                } else if (type == "or") {
                    success = builder.addOrElement(name, childNames);
                } else if (type == "vot") {
                    std::string votThreshold = parseJsonNumber(data.at("voting"));
                    success = builder.addVotElement(name, storm::parser::parseNumber<size_t>(votThreshold), childNames);
                } else if (type == "pand") {
                    success = builder.addPandElement(name, childNames);
                } else if (type == "por") {
                    success = builder.addPorElement(name, childNames);
                } else if (type == "spare") {
                    success = builder.addSpareElement(name, childNames);
                } else if (type == "seq") {
                    success = builder.addSequenceEnforcer(name, childNames);
                } else if (type == "mutex") {
                    success = builder.addMutex(name, childNames);
                } else if (type == "fdep") {
                    success = builder.addDepElement(name, childNames, storm::utility::one<ValueType>());
                } else if (type == "pdep") {
                    ValueType probability = valueParser.parseValue(parseJsonNumber(data.at("probability")));
                    success = builder.addDepElement(name, childNames, probability);
                } else if (boost::starts_with(type, "be")) {
                    std::string distribution = "exp"; // Set default of exponential distribution
                    if (data.count("distribution") > 0) {
                        distribution = data.at("distribution");
                    }
                    STORM_LOG_THROW(type == "be" || "be_" + distribution == type, storm::exceptions::WrongFormatException,
                                    "BE type '" << type << "' and distribution '" << distribution << " do not agree.");
                    if (distribution == "exp") {
                        ValueType failureRate = valueParser.parseValue(parseJsonNumber(data.at("rate")));
                        ValueType dormancyFactor = valueParser.parseValue(parseJsonNumber(data.at("dorm")));
                        bool transient = false;
                        if (data.count("transient") > 0) {
                            transient = data.at("transient");
                        }
                        success = builder.addBasicElementExponential(name, failureRate, dormancyFactor, transient);
                    } else if (distribution == "const") {
                        bool failed = data.at("failed");
                        success = builder.addBasicElementConst(name, failed);
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Distribution: " << distribution << " not supported.");
                        success = false;
                    }
                } else if (type == "compound") {
                    STORM_LOG_TRACE("Ignoring compound node '" << name << "'.");

                } else {
                    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Type name: " << type << " not recognized.");
                    success = false;
                }

                // Try to set layout information
                if (element.find("position") != element.end()) {
                    // Do not set layout for dependencies
                    // This does not work because dependencies might be splitted
                    // TODO: do splitting later in rewriting step
                    if (type != "fdep" && type != "pdep") {
                        // Set layout positions
                        Json position = element.at("position");
                        double x = position.at("x");
                        double y = position.at("y");
                        builder.addLayoutInfo(name, x / 7, y / 7);
                    }
                }
                STORM_LOG_THROW(success, storm::exceptions::FileIoException, "Error while adding element '" << element << "'.");
            }

            std::string topLevelId = parseJsonNumber(jsonInput.at("toplevel"));
            STORM_LOG_THROW(nameMapping.find(topLevelId) != nameMapping.end(), storm::exceptions::WrongFormatException,
                            "Top level element with id '" << topLevelId << "' was not defined.");
            std::string toplevelName = nameMapping.at(topLevelId);
            if (!builder.setTopLevel(toplevelName)) {
                STORM_LOG_THROW(false, storm::exceptions::FileIoException, "Top level id unknown.");
            }

            // Build DFT
            storm::storage::DFT<ValueType> dft = builder.build();
            STORM_LOG_DEBUG("Elements:" << std::endl << dft.getElementsString());
            STORM_LOG_DEBUG("Spare Modules:" << std::endl << dft.getSpareModulesString());
            return dft;

        }

        template<typename ValueType>
        std::string DFTJsonParser<ValueType>::generateUniqueName(std::string const& name) {
            std::string newName = name;
            std::replace(newName.begin(), newName.end(), ' ', '_');
            std::replace(newName.begin(), newName.end(), '-', '_');
            return newName;
        }

        template<typename ValueType>
        std::string DFTJsonParser<ValueType>::parseJsonNumber(Json number) {
            if (number.is_string()) {
                return number.get<std::string>();
            } else {
                std::stringstream stream;
                stream << number;
                return stream.str();
            }
        }

        // Explicitly instantiate the class.
        template class DFTJsonParser<double>;
        template class DFTJsonParser<RationalFunction>;
    }
}
