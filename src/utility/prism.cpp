#include "src/utility/prism.h"

#include "src/adapters/CarlAdapter.h"

#include "src/storage/expressions/ExpressionManager.h"
#include "src/storage/prism/Program.h"

#include "src/utility/macros.h"

#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace utility {
        namespace prism {
            
            template<typename ValueType>
            storm::prism::Program preprocessProgram(storm::prism::Program const& program, boost::optional<std::map<storm::expressions::Variable, storm::expressions::Expression>> const& constantDefinitions, boost::optional<std::set<std::string>> const& restrictedLabelSet, boost::optional<std::vector<storm::expressions::Expression>> const& expressionLabels) {
                storm::prism::Program result;
                
                // Start by defining the undefined constants in the model.
                if (constantDefinitions) {
                    result = program.defineUndefinedConstants(constantDefinitions.get());
                } else {
                    result = program;
                }
                
                // If the program still contains undefined constants and we are not in a parametric setting, assemble an appropriate error message.
                if (!std::is_same<ValueType, storm::RationalFunction>::value && result.hasUndefinedConstants()) {
                    std::vector<std::reference_wrapper<storm::prism::Constant const>> undefinedConstants = result.getUndefinedConstants();
                    std::stringstream stream;
                    bool printComma = false;
                    for (auto const& constant : undefinedConstants) {
                        if (printComma) {
                            stream << ", ";
                        } else {
                            printComma = true;
                        }
                        stream << constant.get().getName() << " (" << constant.get().getType() << ")";
                    }
                    stream << ".";
                    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Program still contains these undefined constants: " + stream.str());
                } else if (std::is_same<ValueType, storm::RationalFunction>::value && !result.hasUndefinedConstantsOnlyInUpdateProbabilitiesAndRewards()) {
                    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "The program contains undefined constants that appear in some places other than update probabilities and reward value expressions, which is not admitted.");
                }
                
                // If the set of labels we are supposed to built is restricted, we need to remove the other labels from the program.
                if (restrictedLabelSet) {
                    result.filterLabels(restrictedLabelSet.get());
                }
                
                // Build new labels.
                if (expressionLabels) {
                    std::map<storm::expressions::Variable, storm::expressions::Expression> constantsSubstitution = result.getConstantsSubstitution();
                    
                    for (auto const& expression : expressionLabels.get()) {
                        std::stringstream stream;
                        stream << expression.substitute(constantsSubstitution);
                        std::string name = stream.str();
                        if (!result.hasLabel(name)) {
                            result.addLabel(name, expression);
                        }
                    }
                }
                
                // Now that the program is fixed, we we need to substitute all constants with their concrete value.
                result = result.substituteConstants();
                return result;
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
            
            template storm::prism::Program preprocessProgram<double>(storm::prism::Program const& program, boost::optional<std::map<storm::expressions::Variable, storm::expressions::Expression>> const& constantDefinitions, boost::optional<std::set<std::string>> const& restrictedLabelSet, boost::optional<std::vector<storm::expressions::Expression>> const& expressionLabels);

            template storm::prism::Program preprocessProgram<storm::RationalFunction>(storm::prism::Program const& program, boost::optional<std::map<storm::expressions::Variable, storm::expressions::Expression>> const& constantDefinitions, boost::optional<std::set<std::string>> const& restrictedLabelSet, boost::optional<std::vector<storm::expressions::Expression>> const& expressionLabels);

        }
    }
}