#include "storm/utility/prism.h"

#include "storm/adapters/CarlAdapter.h"

#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/storage/prism/Program.h"

#include "storm/utility/macros.h"

#include "storm/exceptions/InvalidArgumentException.h"

#include <boost/algorithm/string.hpp>

namespace storm {
    namespace utility {
        namespace prism {
            
            storm::prism::Program preprocess(storm::prism::Program const& program, std::map<storm::expressions::Variable, storm::expressions::Expression> const& constantDefinitions) {
                storm::prism::Program result = program.defineUndefinedConstants(constantDefinitions);
                result = result.substituteConstants();
                return result;
            }
            
            storm::prism::Program preprocess(storm::prism::Program const& program, std::string const& constantDefinitionString) {
                return preprocess(program, parseConstantDefinitionString(program, constantDefinitionString));
            }
            
            std::map<storm::expressions::Variable, storm::expressions::Expression> parseConstantDefinitionString(storm::prism::Program const& program, std::string const& constantDefinitionString) {
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
                        uint_fast64_t positionOfAssignmentOperator = definition.find('=');
                        if (positionOfAssignmentOperator == std::string::npos) {
                            throw storm::exceptions::InvalidArgumentException() << "Illegal constant definition string: syntax error.";
                        }
                        
                        // Now extract the variable name and the value from the string.
                        std::string constantName = definition.substr(0, positionOfAssignmentOperator);
                        boost::trim(constantName);
                        std::string value = definition.substr(positionOfAssignmentOperator + 1);
                        boost::trim(value);
                        
                        // Check whether the constant is a legal undefined constant of the program and if so, of what type it is.
                        if (program.hasConstant(constantName)) {
                            // Get the actual constant and check whether it's in fact undefined.
                            auto const& constant = program.getConstant(constantName);
                            storm::expressions::Variable variable = constant.getExpressionVariable();
                            STORM_LOG_THROW(!constant.isDefined(), storm::exceptions::InvalidArgumentException, "Illegally trying to define already defined constant '" << constantName <<"'.");
                            STORM_LOG_THROW(definedConstants.find(variable) == definedConstants.end(), storm::exceptions::InvalidArgumentException, "Illegally trying to define constant '" << constantName <<"' twice.");
                            definedConstants.insert(variable);
                            
                            if (constant.getType().isBooleanType()) {
                                if (value == "true") {
                                    constantDefinitions[variable] = program.getManager().boolean(true);
                                } else if (value == "false") {
                                    constantDefinitions[variable] = program.getManager().boolean(false);
                                } else {
                                    throw storm::exceptions::InvalidArgumentException() << "Illegal value for boolean constant: " << value << ".";
                                }
                            } else if (constant.getType().isIntegerType()) {
                                int_fast64_t integerValue = std::stoi(value);
                                constantDefinitions[variable] = program.getManager().integer(integerValue);
                            } else if (constant.getType().isRationalType()) {
                                double doubleValue = std::stod(value);
                                constantDefinitions[variable] = program.getManager().rational(doubleValue);
                            }
                        } else {
                            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Illegal constant definition string: unknown undefined constant " << constantName << ".");
                        }
                    }
                }
                
                return constantDefinitions;
            }
            
        }
    }
}
