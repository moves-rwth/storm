#include "DFTJsonParser.h"

#include <iostream>
#include <fstream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>
#include "storm/exceptions/NotImplementedException.h"
#include "storm/exceptions/FileIoException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/utility/macros.h"
#include "storm/utility/file.h"

namespace storm {
    namespace parser {

        template<typename ValueType>
        storm::storage::DFT<ValueType> DFTJsonParser<ValueType>::parseJsonFromFile(std::string const& filename) {
            STORM_LOG_DEBUG("Parsing from JSON file");
            std::ifstream file;
            storm::utility::openFile(filename, file);
            json jsonInput;
            jsonInput << file;
            storm::utility::closeFile(file);
            return parseJson(jsonInput);
        }

        template<typename ValueType>
        storm::storage::DFT<ValueType> DFTJsonParser<ValueType>::parseJsonFromString(std::string const& jsonString) {
            STORM_LOG_DEBUG("Parsing from JSON string");
            json jsonInput = json::parse(jsonString);
            return parseJson(jsonInput);
        }

        template<typename ValueType>
        storm::storage::DFT<ValueType> DFTJsonParser<ValueType>::parseJson(json const& jsonInput) {
            // Parse parameters
            json parameters = jsonInput.at("parameters");
            for (auto it = parameters.begin(); it != parameters.end(); ++it) {
            STORM_LOG_THROW((std::is_same<ValueType, storm::RationalFunction>::value), storm::exceptions::NotSupportedException, "Parameters only allowed when using rational functions.");
                std::string parameter = it.key();
                storm::expressions::Variable var = manager->declareRationalVariable(parameter);
                identifierMapping.emplace(var.getName(), var);
                parser.setIdentifierMapping(identifierMapping);
                STORM_LOG_TRACE("Added parameter: " << var.getName());
            }

            json nodes = jsonInput.at("nodes");
            // Start by building mapping from ids to their unique names
            std::map<std::string, std::string> nameMapping;
            for (auto& element : nodes) {
                json data = element.at("data");
                std::string id = data.at("id");
                nameMapping[id] = generateUniqueName(id, data.at("name"));
            }

            // Parse nodes
            for (auto& element : nodes) {
                STORM_LOG_TRACE("Parsing: " << element);
                bool success = true;
                json data = element.at("data");
                std::string name = generateUniqueName(data.at("id"), data.at("name"));
                std::vector<std::string> childNames;
                if (data.count("children") > 0) {
                    for (std::string const& child : data.at("children")) {
                        childNames.push_back(nameMapping[child]);
                    }
                }

                std::string type = data.at("type");

                if(type == "and") {
                    success = builder.addAndElement(name, childNames);
                } else if (type == "or") {
                    success = builder.addOrElement(name, childNames);
                } else if (type == "vot") {
                    std::string votThreshold = parseJsonNumber(data.at("voting"));
                    success = builder.addVotElement(name, boost::lexical_cast<unsigned>(votThreshold), childNames);
                } else if (type == "pand") {
                    success = builder.addPandElement(name, childNames);
                } else if (type == "por") {
                    success = builder.addPorElement(name, childNames);
                } else if (type == "spare") {
                    success = builder.addSpareElement(name, childNames);
                } else if (type == "seq") {
                    success = builder.addSequenceEnforcer(name, childNames);
                } else if (type== "fdep") {
                    success = builder.addDepElement(name, childNames, storm::utility::one<ValueType>());
                } else if (type== "pdep") {
                    ValueType probability = parseRationalExpression(parseJsonNumber(data.at("prob")));
                    success = builder.addDepElement(name, childNames, probability);
                } else if (type == "be") {
                    ValueType failureRate = parseRationalExpression(parseJsonNumber(data.at("rate")));
                    ValueType dormancyFactor = parseRationalExpression(parseJsonNumber(data.at("dorm")));
                    bool transient = false;
                    if (data.count("transient") > 0) {
                        transient = data.at("transient");
                    }
                    success = builder.addBasicElement(name, failureRate, dormancyFactor, transient);
                } else {
                    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Type name: " << type << "  not recognized.");
                    success = false;
                }

                // Do not set layout for dependencies
                // This does not work because dependencies might be splitted
                // TODO: do splitting later in rewriting step
                if (type != "fdep" && type != "pdep") {
                    // Set layout positions
                    json position = element.at("position");
                    double x = position.at("x");
                    double y = position.at("y");
                    builder.addLayoutInfo(name, x / 7, y / 7);
                }
                STORM_LOG_THROW(success, storm::exceptions::FileIoException, "Error while adding element '" << element << "'.");
            }

            std::string toplevelName = nameMapping[parseJsonNumber(jsonInput.at("toplevel"))];
            if(!builder.setTopLevel(toplevelName)) {
                STORM_LOG_THROW(false, storm::exceptions::FileIoException, "Top level id unknown.");
            }

            // Build DFT
            storm::storage::DFT<ValueType> dft = builder.build();
            STORM_LOG_DEBUG("Elements:" << std::endl << dft.getElementsString());
            STORM_LOG_DEBUG("Spare Modules:" << std::endl << dft.getSpareModulesString());
            return dft;

        }

        template<typename ValueType>
        std::string DFTJsonParser<ValueType>::generateUniqueName(std::string const& id, std::string const& name) {
            std::string newId = name;
            std::replace(newId.begin(), newId.end(), ' ', '_');
            std::replace(newId.begin(), newId.end(), '-', '_');
            return newId + "_" + id;
        }

        template<typename ValueType>
        std::string DFTJsonParser<ValueType>::parseJsonNumber(json number) {
            if (number.is_string()) {
                return number.get<std::string>();
            } else {
                std::stringstream stream;
                stream << number;
                return stream.str();
            }
        }

        template<typename ValueType>
        ValueType DFTJsonParser<ValueType>::parseRationalExpression(std::string const& expr) {
            STORM_LOG_ASSERT(false, "Specialized method should be called.");
            return 0;
        }

        template<>
        double DFTJsonParser<double>::parseRationalExpression(std::string const& expr) {
            return boost::lexical_cast<double>(expr);
        }

        template<>
        storm::RationalFunction DFTJsonParser<storm::RationalFunction>::parseRationalExpression(std::string const& expr) {
            STORM_LOG_TRACE("Translating expression: " << expr);
            storm::expressions::Expression expression = parser.parseFromString(expr);
            STORM_LOG_TRACE("Expression: " << expression);
            storm::RationalFunction rationalFunction = evaluator.asRational(expression);
            STORM_LOG_TRACE("Parsed expression: " << rationalFunction);
            return rationalFunction;
        }


        // Explicitly instantiate the class.
        template class DFTJsonParser<double>;
        template class DFTJsonParser<RationalFunction>;
    }
}
