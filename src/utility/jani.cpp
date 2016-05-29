#include "src/utility/jani.h"

#include <boost/algorithm/string.hpp>

#include "src/storage/expressions/ExpressionManager.h"
#include "src/storage/jani/Model.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace utility {
        namespace jani {
            
            std::map<storm::expressions::Variable, storm::expressions::Expression> parseConstantDefinitionString(storm::jani::Model const& model, std::string const& constantDefinitionString) {
                std::map<storm::expressions::Variable, storm::expressions::Expression> constantDefinitions;
                std::set<storm::expressions::Variable> definedConstants;
                
                if (!constantDefinitionString.empty()) {
                    // Parse the string that defines the undefined constants of the model and make sure that it contains exactly
                    // one value for each undefined constant of the model.
                    std::vector<std::string> definitions;
                    boost::split(definitions, constantDefinitionString, boost::is_any_of(","));
                    for (auto& definition : definitions) {
                        boost::trim(definition);
                        
                        // Check whether the token could be a legal constant definition.
                        std::size_t positionOfAssignmentOperator = definition.find('=');
                        STORM_LOG_THROW(positionOfAssignmentOperator != std::string::npos, storm::exceptions::InvalidArgumentException, "Illegal constant definition string: syntax error.");
                        
                        // Now extract the variable name and the value from the string.
                        std::string constantName = definition.substr(0, positionOfAssignmentOperator);
                        boost::trim(constantName);
                        std::string value = definition.substr(positionOfAssignmentOperator + 1);
                        boost::trim(value);
                        
                        // Check whether the constant is a legal undefined constant of the program and if so, of what type it is.
                        if (model.hasConstant(constantName)) {
                            // Get the actual constant and check whether it's in fact undefined.
                            auto const& constant = model.getConstant(constantName);
                            storm::expressions::Variable variable = constant.getExpressionVariable();
                            STORM_LOG_THROW(!constant.isDefined(), storm::exceptions::InvalidArgumentException, "Illegally trying to define already defined constant '" << constantName <<"'.");
                            STORM_LOG_THROW(definedConstants.find(variable) == definedConstants.end(), storm::exceptions::InvalidArgumentException, "Illegally trying to define constant '" << constantName <<"' twice.");
                            definedConstants.insert(variable);
                            
                            if (constant.getType().isBooleanType()) {
                                if (value == "true") {
                                    constantDefinitions[variable] = model.getExpressionManager().boolean(true);
                                } else if (value == "false") {
                                    constantDefinitions[variable] = model.getExpressionManager().boolean(false);
                                } else {
                                    throw storm::exceptions::InvalidArgumentException() << "Illegal value for boolean constant: " << value << ".";
                                }
                            } else if (constant.getType().isIntegerType()) {
                                int_fast64_t integerValue = std::stoi(value);
                                constantDefinitions[variable] = model.getExpressionManager().integer(integerValue);
                            } else if (constant.getType().isRationalType()) {
                                double doubleValue = std::stod(value);
                                constantDefinitions[variable] = model.getExpressionManager().rational(doubleValue);
                            }
                        } else {
                            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Illegal constant definition string: unknown undefined constant '" << constantName << "'.");
                        }
                    }
                }
                
                return constantDefinitions;
            }
            
        }
    }
}