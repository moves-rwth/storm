#include "storm/storage/jani/ArrayEliminator.h"

#include <unordered_map>

#include "storm/storage/expressions/ExpressionVisitor.h"
#include "storm/storage/jani/expressions/JaniExpressionVisitor.h"
#include "storm/storage/jani/Variable.h"
#include "storm/storage/jani/Model.h"
#include "storm/storage/jani/Property.h"
#include "storm/storage/jani/traverser/JaniTraverser.h"
#include "storm/storage/jani/traverser/ArrayExpressionFinder.h"

#include "storm/storage/expressions/Expressions.h"
#include "storm/storage/jani/expressions/JaniExpressions.h"
#include "storm/storage/expressions/ExpressionManager.h"

#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/UnexpectedException.h"
#include "storm/exceptions/OutOfRangeException.h"

namespace storm {
    
    
    
    namespace jani {
        namespace detail {
            
            class MaxArraySizeExpressionVisitor : public storm::expressions::ExpressionVisitor, public storm::expressions::JaniExpressionVisitor {
            public:
                MaxArraySizeExpressionVisitor() = default;
                virtual ~MaxArraySizeExpressionVisitor() = default;
    
                std::size_t getMaxSize(storm::expressions::Expression const& expression, std::unordered_map<storm::expressions::Variable, std::size_t> const& arrayVariableSizeMap) {
                    return boost::any_cast<std::size_t>(expression.accept(*this, &arrayVariableSizeMap));
                }
     
                virtual boost::any visit(storm::expressions::IfThenElseExpression const& expression, boost::any const& data) override {
                    if (expression.getCondition()->containsVariables()) {
                        return std::max<std::size_t>(boost::any_cast<std::size_t>(expression.getThenExpression()->accept(*this, data)), boost::any_cast<std::size_t>(expression.getElseExpression()->accept(*this, data)));
                    } else {
                        if (expression.getCondition()->evaluateAsBool()) {
                            return boost::any_cast<std::size_t>(expression.getThenExpression()->accept(*this, data));
                        }
                            return boost::any_cast<std::size_t>(expression.getElseExpression()->accept(*this, data));
                    }
                }
                
                virtual boost::any visit(storm::expressions::BinaryBooleanFunctionExpression const& expression, boost::any const& data) override {
                    return std::max<std::size_t>(boost::any_cast<std::size_t>(expression.getFirstOperand()->accept(*this, data)), boost::any_cast<std::size_t>(expression.getSecondOperand()->accept(*this, data)));
                }
                
                virtual boost::any visit(storm::expressions::BinaryNumericalFunctionExpression const& expression, boost::any const& data) override {
                    return std::max<std::size_t>(boost::any_cast<std::size_t>(expression.getFirstOperand()->accept(*this, data)), boost::any_cast<std::size_t>(expression.getSecondOperand()->accept(*this, data)));
                }
                
                virtual boost::any visit(storm::expressions::BinaryRelationExpression const& expression, boost::any const& data) override {
                    return std::max<std::size_t>(boost::any_cast<std::size_t>(expression.getFirstOperand()->accept(*this, data)), boost::any_cast<std::size_t>(expression.getSecondOperand()->accept(*this, data)));
                }
                
                virtual boost::any visit(storm::expressions::VariableExpression const& expression, boost::any const& data) override {
                    std::unordered_map<storm::expressions::Variable, std::size_t> const* arrayVariableSizeMap = boost::any_cast<std::unordered_map<storm::expressions::Variable, std::size_t> const*>(data);
                    if (expression.getType().isArrayType()) {
                        auto varIt = arrayVariableSizeMap->find(expression.getVariable());
                        if (varIt != arrayVariableSizeMap->end()) {
                            return varIt->second;
                        }
                    }
                    return static_cast<std::size_t>(0);
                }
                
                virtual boost::any visit(storm::expressions::UnaryBooleanFunctionExpression const& expression, boost::any const& data) override {
                    return boost::any_cast<std::size_t>(expression.getOperand()->accept(*this, data));
                }
                
                virtual boost::any visit(storm::expressions::UnaryNumericalFunctionExpression const& expression, boost::any const& data) override {
                    return boost::any_cast<std::size_t>(expression.getOperand()->accept(*this, data));
                }
                
                virtual boost::any visit(storm::expressions::BooleanLiteralExpression const&, boost::any const&) override {
                    return 0;
                }
                
                virtual boost::any visit(storm::expressions::IntegerLiteralExpression const&, boost::any const&) override {
                    return 0;
                }
                
                virtual boost::any visit(storm::expressions::RationalLiteralExpression const&, boost::any const&) override {
                    return 0;
                }
                
                virtual boost::any visit(storm::expressions::ValueArrayExpression const& expression, boost::any const&) override {
                    STORM_LOG_ASSERT(expression.size()->isIntegerLiteralExpression(), "unexpected kind of size expression of ValueArrayExpression (" << expression.size()->toExpression() << ").");
                    return static_cast<std::size_t>(expression.size()->evaluateAsInt());
                }
                
                virtual boost::any visit(storm::expressions::ConstructorArrayExpression const& expression, boost::any const&) override {
                    if (!expression.size()->containsVariables()) {
                        return static_cast<std::size_t>(expression.size()->evaluateAsInt());
                    } else {
                        auto vars = expression.size()->toExpression().getVariables();
                        std::string variables = "";
                        for (auto const& v : vars) {
                            if (variables != "") {
                                variables += ", ";
                            }
                            variables += v.getName();
                        }
                        if (vars.size() == 1) {
                            variables = "variable " + variables;
                        } else {
                            variables = "variables " + variables;
                        }
                        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Unable to determine array size: Size of ConstructorArrayExpression '" << expression << "' still contains the " << variables << ".");
                    }
                }
                
                virtual boost::any visit(storm::expressions::ArrayAccessExpression const&, boost::any const&) override {
                    STORM_LOG_WARN("Found Array access expression within an array expression. This is not expected since nested arrays are currently not supported.");
                    return 0;
                }
                
                virtual boost::any visit(storm::expressions::FunctionCallExpression const&, boost::any const&) override {
                    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Found Function call expression within an array expression. This is not expected since functions are expected to be eliminated at this point.");
                    return 0;
                }
                
            };
            
            class ArrayExpressionEliminationVisitor : public storm::expressions::ExpressionVisitor, public storm::expressions::JaniExpressionVisitor {
            public:
                
                typedef std::shared_ptr<storm::expressions::BaseExpression const> BaseExprPtr;
                class ResultType {
                public:
                    ResultType(ResultType const& other) = default;
                    ResultType(BaseExprPtr expression) : expression(expression), arrayOutOfBoundsMessage("") {}
                    ResultType(std::string arrayOutOfBoundsMessage) : expression(nullptr), arrayOutOfBoundsMessage(arrayOutOfBoundsMessage) {}
                    BaseExprPtr& expr() {
                        STORM_LOG_ASSERT(!isArrayOutOfBounds(), "Tried to get the result expression, but " << arrayOutOfBoundsMessage);
                        return expression;
                    };
                    bool isArrayOutOfBounds() { return arrayOutOfBoundsMessage != ""; };
                    std::string const& outOfBoundsMessage() const { return arrayOutOfBoundsMessage; }
                private:
                    BaseExprPtr expression;
                    std::string arrayOutOfBoundsMessage;
                };
                
                ArrayExpressionEliminationVisitor(std::unordered_map<storm::expressions::Variable, std::vector<storm::jani::Variable const*>> const& replacements, std::unordered_map<storm::expressions::Variable, std::size_t> const& sizes) : replacements(replacements), arraySizes(sizes) {}
                virtual ~ArrayExpressionEliminationVisitor() = default;
    
                storm::expressions::Expression eliminate(storm::expressions::Expression const& expression) {
                    // here, data is the accessed index of the most recent array access expression. Initially, there is none.
                    auto res = boost::any_cast<ResultType>(expression.accept(*this, boost::any()));
                    STORM_LOG_THROW(!res.isArrayOutOfBounds(), storm::exceptions::OutOfRangeException, res.outOfBoundsMessage());
                    STORM_LOG_ASSERT(!containsArrayExpression(res.expr()->toExpression()), "Expression still contains array expressions. Before: " << std::endl << expression << std::endl << "After:" << std::endl << res.expr()->toExpression());
                    return res.expr()->simplify();
                }
                
                virtual boost::any visit(storm::expressions::IfThenElseExpression const& expression, boost::any const& data) override {
                    // for the condition expression, outer array accesses should not matter.
                    ResultType conditionResult = boost::any_cast<ResultType>(expression.getCondition()->accept(*this, boost::any()));
                    if (conditionResult.isArrayOutOfBounds()) {
                        return conditionResult;
                    }
                    
                    // We need to handle expressions of the kind '42<size : A[42] : 0', where size is a variable and A[42] might be out of bounds.
                    ResultType thenResult = boost::any_cast<ResultType>(expression.getThenExpression()->accept(*this, data));
                    ResultType elseResult = boost::any_cast<ResultType>(expression.getElseExpression()->accept(*this, data));
                    if (thenResult.isArrayOutOfBounds()) {
                        if (elseResult.isArrayOutOfBounds()) {
                            return ResultType(thenResult.outOfBoundsMessage() + " and " + elseResult.outOfBoundsMessage());
                        } else {
                            // Assume the else expression
                            return elseResult;
                        }
                    } else if (elseResult.isArrayOutOfBounds()) {
                        // Assume the then expression
                        return thenResult;
                    } else {
                        // If the arguments did not change, we simply push the expression itself.
                        if (conditionResult.expr().get() == expression.getCondition().get() && thenResult.expr().get() == expression.getThenExpression().get() && elseResult.expr().get() == expression.getElseExpression().get()) {
                            return ResultType(expression.getSharedPointer());
                        } else {
                            return ResultType(std::const_pointer_cast<storm::expressions::BaseExpression const>(std::shared_ptr<storm::expressions::BaseExpression>(new storm::expressions::IfThenElseExpression(expression.getManager(), thenResult.expr()->getType(), conditionResult.expr(), thenResult.expr(), elseResult.expr()))));
                        }
                    }
                }
        
                virtual boost::any visit(storm::expressions::BinaryBooleanFunctionExpression const& expression, boost::any const& data) override {
                    STORM_LOG_ASSERT(data.empty(), "BinaryBooleanFunctionExpressions should not be direct subexpressions of array access expressions. However, the expression " << expression << " is.");
                    ResultType firstResult = boost::any_cast<ResultType>(expression.getFirstOperand()->accept(*this, data));
                    ResultType secondResult = boost::any_cast<ResultType>(expression.getSecondOperand()->accept(*this, data));
                    
                    if (firstResult.isArrayOutOfBounds()) {
                        return firstResult;
                    } else if (secondResult.isArrayOutOfBounds()) {
                        return secondResult;
                    }
        
                    // If the arguments did not change, we simply push the expression itself.
                    if (firstResult.expr().get() == expression.getFirstOperand().get() && secondResult.expr().get() == expression.getSecondOperand().get()) {
                        return ResultType(expression.getSharedPointer());
                    } else {
                        return ResultType(std::const_pointer_cast<storm::expressions::BaseExpression const>(std::shared_ptr<storm::expressions::BaseExpression>(new storm::expressions::BinaryBooleanFunctionExpression(expression.getManager(), expression.getType(), firstResult.expr(), secondResult.expr(), expression.getOperatorType()))));
                    }
                }
        
                virtual boost::any visit(storm::expressions::BinaryNumericalFunctionExpression const& expression, boost::any const& data) override {
                    STORM_LOG_ASSERT(data.empty(), "BinaryNumericalFunctionExpression should not be direct subexpressions of array access expressions. However, the expression " << expression << " is.");
                    ResultType firstResult = boost::any_cast<ResultType>(expression.getFirstOperand()->accept(*this, data));
                    ResultType secondResult = boost::any_cast<ResultType>(expression.getSecondOperand()->accept(*this, data));
                    
                    if (firstResult.isArrayOutOfBounds()) {
                        return firstResult;
                    } else if (secondResult.isArrayOutOfBounds()) {
                        return secondResult;
                    }
                    
                    // If the arguments did not change, we simply push the expression itself.
                    if (firstResult.expr().get() == expression.getFirstOperand().get() && secondResult.expr().get() == expression.getSecondOperand().get()) {
                        return ResultType(expression.getSharedPointer());
                    } else {
                        return ResultType(std::const_pointer_cast<storm::expressions::BaseExpression const>(std::shared_ptr<storm::expressions::BaseExpression>(new storm::expressions::BinaryNumericalFunctionExpression(expression.getManager(), expression.getType(), firstResult.expr(), secondResult.expr(), expression.getOperatorType()))));
                    }
                }
        
                virtual boost::any visit(storm::expressions::BinaryRelationExpression const& expression, boost::any const& data) override {
                    STORM_LOG_ASSERT(data.empty(), "BinaryRelationExpression should not be direct subexpressions of array access expressions. However, the expression " << expression << " is.");
                    ResultType firstResult = boost::any_cast<ResultType>(expression.getFirstOperand()->accept(*this, data));
                    ResultType secondResult = boost::any_cast<ResultType>(expression.getSecondOperand()->accept(*this, data));
                    
                    if (firstResult.isArrayOutOfBounds()) {
                        return firstResult;
                    } else if (secondResult.isArrayOutOfBounds()) {
                        return secondResult;
                    }
                    
                    // If the arguments did not change, we simply push the expression itself.
                    if (firstResult.expr().get() == expression.getFirstOperand().get() && secondResult.expr().get() == expression.getSecondOperand().get()) {
                        return ResultType(expression.getSharedPointer());
                    } else {
                        return ResultType(std::const_pointer_cast<storm::expressions::BaseExpression const>(std::shared_ptr<storm::expressions::BaseExpression>(new storm::expressions::BinaryRelationExpression(expression.getManager(), expression.getType(), firstResult.expr(), secondResult.expr(), expression.getRelationType()))));
                    }
                }
                
                virtual boost::any visit(storm::expressions::VariableExpression const& expression, boost::any const& data) override {
                    if (expression.getType().isArrayType()) {
                        STORM_LOG_THROW(!data.empty(), storm::exceptions::NotSupportedException, "Unable to translate array variable to basic variable, since it does not seem to be within an array access expression.");
                        uint64_t index = boost::any_cast<uint64_t>(data);
                        STORM_LOG_ASSERT(replacements.find(expression.getVariable()) != replacements.end(), "Unable to find array variable " << expression << " in array replacements.");
                        auto const& arrayVarReplacements = replacements.at(expression.getVariable());
                        if (index >= arrayVarReplacements.size()) {
                            return ResultType("Array index " + std::to_string(index) + " for variable " + expression.getVariableName() + " is out of bounds.");
                        }
                        return ResultType(arrayVarReplacements[index]->getExpressionVariable().getExpression().getBaseExpressionPointer());
                    } else {
                        STORM_LOG_ASSERT(data.empty(), "VariableExpression of non-array variable should not be a subexpressions of array access expressions. However, the expression " << expression << " is.");
                        return ResultType(expression.getSharedPointer());
                    }
                }
        
                virtual boost::any visit(storm::expressions::UnaryBooleanFunctionExpression const& expression, boost::any const& data) override {
                    STORM_LOG_ASSERT(data.empty(), "UnaryBooleanFunctionExpression should not be direct subexpressions of array access expressions. However, the expression " << expression << " is.");
                    ResultType operandResult = boost::any_cast<ResultType>(expression.getOperand()->accept(*this, data));
                    
                    if (operandResult.isArrayOutOfBounds()) {
                        return operandResult;
                    }
                    
                    // If the argument did not change, we simply push the expression itself.
                    if (operandResult.expr().get() == expression.getOperand().get()) {
                        return ResultType(expression.getSharedPointer());
                    } else {
                        return ResultType(std::const_pointer_cast<storm::expressions::BaseExpression const>(std::shared_ptr<storm::expressions::BaseExpression>(new storm::expressions::UnaryBooleanFunctionExpression(expression.getManager(), expression.getType(), operandResult.expr(), expression.getOperatorType()))));
                    }
                }
        
                virtual boost::any visit(storm::expressions::UnaryNumericalFunctionExpression const& expression, boost::any const& data) override {
                    STORM_LOG_ASSERT(data.empty(), "UnaryBooleanFunctionExpression should not be direct subexpressions of array access expressions. However, the expression " << expression << " is.");
        
                    ResultType operandResult = boost::any_cast<ResultType>(expression.getOperand()->accept(*this, data));
                    
                    if (operandResult.isArrayOutOfBounds()) {
                        return operandResult;
                    }
                   
                    // If the argument did not change, we simply push the expression itself.
                    if (operandResult.expr().get() == expression.getOperand().get()) {
                        return ResultType(expression.getSharedPointer());
                    } else {
                        return ResultType(std::const_pointer_cast<storm::expressions::BaseExpression const>(std::shared_ptr<storm::expressions::BaseExpression>(new storm::expressions::UnaryNumericalFunctionExpression(expression.getManager(), expression.getType(), operandResult.expr(), expression.getOperatorType()))));
                    }
                }
                
                virtual boost::any visit(storm::expressions::BooleanLiteralExpression const& expression, boost::any const&) override {
                    return ResultType(expression.getSharedPointer());
                }
                
                virtual boost::any visit(storm::expressions::IntegerLiteralExpression const& expression, boost::any const&) override {
                    return ResultType(expression.getSharedPointer());
                }
                
                virtual boost::any visit(storm::expressions::RationalLiteralExpression const& expression, boost::any const&) override {
                    return ResultType(expression.getSharedPointer());
                }
                
                virtual boost::any visit(storm::expressions::ValueArrayExpression const& expression, boost::any const& data) override {
                    STORM_LOG_THROW(!data.empty(), storm::exceptions::NotSupportedException, "Unable to translate ValueArrayExpression to element expression since it does not seem to be within an array access expression.");
                    uint64_t index = boost::any_cast<uint64_t>(data);
                    STORM_LOG_ASSERT(expression.size()->isIntegerLiteralExpression(), "unexpected kind of size expression of ValueArrayExpression (" << expression.size()->toExpression() << ").");
                    if (index >= static_cast<uint64_t>(expression.size()->evaluateAsInt())) {
                        return ResultType("Array index " + std::to_string(index) + " for ValueArrayExpression " + expression.toExpression().toString() + " is out of bounds.");
                    }
                    return ResultType(boost::any_cast<ResultType>(expression.at(index)->accept(*this, boost::any())));
                }
                
                virtual boost::any visit(storm::expressions::ConstructorArrayExpression const& expression, boost::any const& data) override {
                    STORM_LOG_THROW(!data.empty(), storm::exceptions::NotSupportedException, "Unable to translate ValueArrayExpression to element expression since it does not seem to be within an array access expression.");
                    uint64_t index = boost::any_cast<uint64_t>(data);
                    if (expression.size()->containsVariables()) {
                        STORM_LOG_WARN("Ignoring length of constructorArrayExpression " << expression << " as it still contains variables.");
                    } else {
                        if (index >= static_cast<uint64_t>(expression.size()->evaluateAsInt())) {
                            return ResultType("Array index " + std::to_string(index) + " for ConstructorArrayExpression " + expression.toExpression().toString() + " is out of bounds.");
                        }
                    }
                    return ResultType(boost::any_cast<ResultType>(expression.at(index)->accept(*this, boost::any())));
                }
                
                virtual boost::any visit(storm::expressions::ArrayAccessExpression const& expression, boost::any const&) override {
                    if (expression.getSecondOperand()->containsVariables()) {
                        //get the size of the array expression
                        uint64_t size = MaxArraySizeExpressionVisitor().getMaxSize(expression.getFirstOperand()->toExpression(), arraySizes);
                        STORM_LOG_THROW(size > 0, storm::exceptions::NotSupportedException, "Unable to get size of array expression for array access " << expression << ".");
                        uint64_t index = size - 1;
                        storm::expressions::Expression result = boost::any_cast<ResultType>(expression.getFirstOperand()->accept(*this, index)).expr()->toExpression();
                        while (index > 0) {
                            --index;
                            storm::expressions::Expression isCurrentIndex = boost::any_cast<ResultType>(expression.getSecondOperand()->accept(*this, boost::any())).expr()->toExpression() == expression.getManager().integer(index);
                            result = storm::expressions::ite(isCurrentIndex,
                                    boost::any_cast<ResultType>(expression.getFirstOperand()->accept(*this, index)).expr()->toExpression(),
                                    result);
                        }
                        return ResultType(result.getBaseExpressionPointer());
                    } else {
                        uint64_t index = expression.getSecondOperand()->evaluateAsInt();
                        return boost::any_cast<ResultType>(expression.getFirstOperand()->accept(*this, index));
                    }
                }
                
                virtual boost::any visit(storm::expressions::FunctionCallExpression const&, boost::any const&) override {
                    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Found Function call expression while eliminating array expressions. This is not expected since functions are expected to be eliminated at this point.");
                    return false;
                }
                
            private:
                std::unordered_map<storm::expressions::Variable, std::vector<storm::jani::Variable const*>> const& replacements;
                std::unordered_map<storm::expressions::Variable, std::size_t> const& arraySizes;
            };
            
            class MaxArraySizeDeterminer : public ConstJaniTraverser {
            public:
                
                typedef std::unordered_map<storm::expressions::Variable, std::size_t>* MapPtr;
                
                MaxArraySizeDeterminer() = default;
                virtual ~MaxArraySizeDeterminer() = default;
                std::unordered_map<storm::expressions::Variable, std::size_t> getMaxSizes(Model const& model) {
                    // We repeatedly determine the max array sizes until convergence. This is to cover assignments of one array variable to another (A := B)
                    std::unordered_map<storm::expressions::Variable, std::size_t> result, previousResult;
                    do {
                        previousResult = result;
                        ConstJaniTraverser::traverse(model, &result);
                    } while (previousResult != result);
                    return result;
                }
                
                virtual void traverse(Assignment const& assignment, boost::any const& data) override {
                    if (assignment.lValueIsVariable() && assignment.getExpressionVariable().getType().isArrayType()) {
                        auto& map = *boost::any_cast<MapPtr>(data);
                        std::size_t newSize = MaxArraySizeExpressionVisitor().getMaxSize(assignment.getAssignedExpression(), map);
                        auto insertionRes = map.emplace(assignment.getExpressionVariable(), newSize);
                        if (!insertionRes.second) {
                            insertionRes.first->second = std::max(newSize, insertionRes.first->second);
                        }
                    }
                }
                
                virtual void traverse(ArrayVariable const& variable, boost::any const& data) override {
                    if (variable.hasInitExpression()) {
                        auto& map = *boost::any_cast<MapPtr>(data);
                        std::size_t newSize = MaxArraySizeExpressionVisitor().getMaxSize(variable.getInitExpression(), map);
                        auto insertionRes = map.emplace(variable.getExpressionVariable(), newSize);
                        if (!insertionRes.second) {
                            insertionRes.first->second = std::max(newSize, insertionRes.first->second);
                        }
                    }
                }
            };
            
        
            class ArrayVariableReplacer : public JaniTraverser {
            public:

                typedef ArrayEliminatorData ResultType;
                using JaniTraverser::traverse;
                
                ArrayVariableReplacer(storm::expressions::ExpressionManager& expressionManager, bool keepNonTrivialArrayAccess, std::unordered_map<storm::expressions::Variable, std::size_t> const& arrayVarToSizesMap) : expressionManager(expressionManager) , keepNonTrivialArrayAccess(keepNonTrivialArrayAccess), arraySizes(arrayVarToSizesMap) {}
                
                virtual ~ArrayVariableReplacer() = default;
                ResultType replace(Model& model) {

                    ResultType result;
                    arrayExprEliminator = std::make_unique<ArrayExpressionEliminationVisitor>(result.replacements, arraySizes);
                    for (auto const& arraySize : arraySizes) {
                        result.replacements.emplace(arraySize.first, std::vector<storm::jani::Variable const*>(arraySize.second, nullptr));
                    }
                    traverse(model, &result);
                    return result;
                }

                virtual void traverse(Model& model, boost::any const& data) override {
                    
                    // Insert fresh basic variables for global array variables
                    auto& replacements = boost::any_cast<ResultType*>(data)->replacements;
                    for (storm::jani::ArrayVariable const& arrayVariable : model.getGlobalVariables().getArrayVariables()) {
                        std::vector<storm::jani::Variable const*>& basicVars = replacements.at(arrayVariable.getExpressionVariable());
                        for (uint64_t index = 0; index < basicVars.size(); ++index) {
                            basicVars[index] = &model.addVariable(*getBasicVariable(arrayVariable, index));
                        }
                    }
                    // drop all occuring array variables
                    auto elVars = model.getGlobalVariables().dropAllArrayVariables();
                    auto& eliminatedArrayVariables = boost::any_cast<ResultType*>(data)->eliminatedArrayVariables;
                    eliminatedArrayVariables.insert(eliminatedArrayVariables.end(), elVars.begin(), elVars.end());
                    
                    // Make new variable replacements known to the expression eliminator
                    arrayExprEliminator = std::make_unique<ArrayExpressionEliminationVisitor>(replacements, arraySizes);
                    
                    for (auto& aut : model.getAutomata()) {
                        traverse(aut, data);
                    }
                    
                    // traversal of remaining components
                    if (model.hasInitialStatesRestriction()) {
                        model.setInitialStatesRestriction(arrayExprEliminator->eliminate(model.getInitialStatesRestriction()));
                    }
                    for (auto& nonTrivRew : model.getNonTrivialRewardExpressions()) {
                        nonTrivRew.second = arrayExprEliminator->eliminate(nonTrivRew.second);
                    }
                }
                
                virtual void traverse(Automaton& automaton, boost::any const& data) override {
                    // No need to traverse the init restriction.
                    
                    // Insert fresh basic variables for local array variables
                    auto& replacements = boost::any_cast<ResultType*>(data)->replacements;
                    for (storm::jani::ArrayVariable const& arrayVariable : automaton.getVariables().getArrayVariables()) {
                        std::vector<storm::jani::Variable const*>& basicVars = replacements.at(arrayVariable.getExpressionVariable());
                        for (uint64_t index = 0; index < basicVars.size(); ++index) {
                            basicVars[index] = &automaton.addVariable(*getBasicVariable(arrayVariable, index));
                        }
                    }
                    // drop all occuring array variables
                    auto elVars = automaton.getVariables().dropAllArrayVariables();
                    auto& eliminatedArrayVariables = boost::any_cast<ResultType*>(data)->eliminatedArrayVariables;
                    eliminatedArrayVariables.insert(eliminatedArrayVariables.end(), elVars.begin(), elVars.end());

                    // Make new variable replacements known to the expression eliminator
                    arrayExprEliminator = std::make_unique<ArrayExpressionEliminationVisitor>(replacements, arraySizes);
                    
                    for (auto& loc : automaton.getLocations()) {
                        traverse(loc, data);
                    }
                    traverse(automaton.getEdgeContainer(), data);
                    
                    if (automaton.hasInitialStatesRestriction()) {
                        automaton.setInitialStatesRestriction(arrayExprEliminator->eliminate(automaton.getInitialStatesRestriction()));
                    }
                }

                virtual void traverse(Location& location, boost::any const& data) override {
                    traverse(location.getAssignments(), data);
                    if (location.hasTimeProgressInvariant()) {
                        location.setTimeProgressInvariant(arrayExprEliminator->eliminate(location.getTimeProgressInvariant()));
                        traverse(location.getTimeProgressInvariant(), data);
                    }
                }

                void traverse(TemplateEdge& templateEdge, boost::any const& data) override {
                    templateEdge.setGuard(arrayExprEliminator->eliminate(templateEdge.getGuard()));
                    for (auto& dest : templateEdge.getDestinations()) {
                        traverse(dest, data);
                    }
                    traverse(templateEdge.getAssignments(), data);
                }

                
                void traverse(Edge& edge, boost::any const& data) override {
                    if (edge.hasRate()) {
                        edge.setRate(arrayExprEliminator->eliminate(edge.getRate()));
                    }
                    for (auto& dest : edge.getDestinations()) {
                        traverse(dest, data);
                    }
                }
                
                void traverse(EdgeDestination& edgeDestination, boost::any const&) override {
                    edgeDestination.setProbability(arrayExprEliminator->eliminate(edgeDestination.getProbability()));
                }
                
                virtual void traverse(OrderedAssignments& orderedAssignments, boost::any const& data) override {
                    auto const& replacements = boost::any_cast<ResultType*>(data)->replacements;

                    // Replace array occurrences in LValues and assigned expressions.
                    std::vector<Assignment> newAssignments;
                    if (!orderedAssignments.empty()) {
                        
                        int64_t level = orderedAssignments.getLowestLevel();
                        std::unordered_map<storm::expressions::Variable, std::vector<Assignment const*>> collectedArrayAccessAssignments;
                        for (Assignment const& assignment : orderedAssignments) {
                            if (assignment.getLevel() != level) {
                                STORM_LOG_ASSERT(assignment.getLevel() > level, "Ordered Assignment does not have the expected order.");
                                for (auto const& arrayAssignments : collectedArrayAccessAssignments) {
                                    insertLValueArrayAccessReplacements(arrayAssignments.second, replacements.at(arrayAssignments.first), level, newAssignments);
                                }
                                collectedArrayAccessAssignments.clear();
                                level = assignment.getLevel();
                            }
                            if (assignment.getLValue().isArrayAccess()) {
                                if (!keepNonTrivialArrayAccess || !assignment.getLValue().getArrayIndex().containsVariables()) {
                                    auto insertionRes = collectedArrayAccessAssignments.emplace(assignment.getLValue().getArray().getExpressionVariable(), std::vector<Assignment const*>({&assignment}));
                                    if (!insertionRes.second) {
                                        insertionRes.first->second.push_back(&assignment);
                                    }
                                } else {
                                    // Keeping array access LValue
                                    LValue newLValue(LValue(assignment.getLValue().getArray()), arrayExprEliminator->eliminate(assignment.getLValue().getArrayIndex()));
                                    newAssignments.emplace_back(newLValue, arrayExprEliminator->eliminate(assignment.getAssignedExpression()), assignment.getLevel());
                                }
                            } else if (assignment.getLValue().isVariable() && assignment.getVariable().isArrayVariable()) {
                                STORM_LOG_ASSERT(assignment.getAssignedExpression().getType().isArrayType(), "Assigning a non-array expression to an array variable...");
                                std::vector<storm::jani::Variable const*> const& arrayVariableReplacements = replacements.at(assignment.getExpressionVariable());
                                // Get the maximum size of the array expression on the rhs
                                uint64_t rhsSize = MaxArraySizeExpressionVisitor().getMaxSize(assignment.getAssignedExpression(), arraySizes);
                                STORM_LOG_ASSERT(arrayVariableReplacements.size() >= rhsSize, "Array size too small.");
                                for (uint64_t index = 0; index < arrayVariableReplacements.size(); ++index) {
                                    auto const& replacement = *arrayVariableReplacements[index];
                                    storm::expressions::Expression newRhs;
                                    if (index < rhsSize) {
                                        newRhs = std::make_shared<storm::expressions::ArrayAccessExpression>(expressionManager, assignment.getAssignedExpression().getType().getElementType(), assignment.getAssignedExpression().getBaseExpressionPointer(), expressionManager.integer(index).getBaseExpressionPointer())->toExpression();
                                    } else {
                                        newRhs = getOutOfBoundsValue(replacement);
                                    }
                                    newRhs = arrayExprEliminator->eliminate(newRhs);
                                    newAssignments.emplace_back(LValue(replacement), newRhs, level);
                                }
                            } else {
                                newAssignments.emplace_back(assignment.getLValue(), arrayExprEliminator->eliminate(assignment.getAssignedExpression()), assignment.getLevel());
                            }
                        }
                        for (auto const& arrayAssignments : collectedArrayAccessAssignments) {
                            insertLValueArrayAccessReplacements(arrayAssignments.second, replacements.at(arrayAssignments.first), level, newAssignments);
                        }
                        collectedArrayAccessAssignments.clear();
                        orderedAssignments.clear();
                        for (auto const& assignment : newAssignments) {
                            orderedAssignments.add(assignment);
                        }
                    }
                }
                
            private:
                
                std::shared_ptr<Variable> getBasicVariable(ArrayVariable const& arrayVariable, uint64_t index) const {
                    std::string name = arrayVariable.getExpressionVariable().getName() + "_at_" + std::to_string(index);
                    storm::expressions::Expression initValue;
                    if (arrayVariable.hasInitExpression()) {
                        initValue = arrayExprEliminator->eliminate(std::make_shared<storm::expressions::ArrayAccessExpression>(expressionManager, arrayVariable.getExpressionVariable().getType().getElementType(), arrayVariable.getInitExpression().getBaseExpressionPointer(), expressionManager.integer(index).getBaseExpressionPointer())->toExpression());
                    }
                    if (arrayVariable.getElementType() == ArrayVariable::ElementType::Int) {
                        storm::expressions::Variable exprVariable = expressionManager.declareIntegerVariable(name);
                        if (arrayVariable.hasElementTypeBound()) {
                            if (initValue.isInitialized()) {
                                return std::make_shared<BoundedIntegerVariable>(name, exprVariable, initValue, arrayVariable.isTransient(), arrayVariable.getLowerElementTypeBound(), arrayVariable.getUpperElementTypeBound());
                            } else {
                                return std::make_shared<BoundedIntegerVariable>(name, exprVariable, arrayVariable.getLowerElementTypeBound(), arrayVariable.getUpperElementTypeBound());
                            }
                        } else {
                            if (initValue.isInitialized()) {
                                return std::make_shared<UnboundedIntegerVariable>(name, exprVariable, initValue, arrayVariable.isTransient());
                            } else {
                                return std::make_shared<UnboundedIntegerVariable>(name, exprVariable);
                            }
                        }
                    } else if (arrayVariable.getElementType() == ArrayVariable::ElementType::Real) {
                        storm::expressions::Variable exprVariable = expressionManager.declareRationalVariable(name);
                        if (initValue.isInitialized()) {
                            return std::make_shared<RealVariable>(name, exprVariable, initValue, arrayVariable.isTransient());
                        } else {
                            return std::make_shared<RealVariable>(name, exprVariable);
                        }
                    } else if (arrayVariable.getElementType() == ArrayVariable::ElementType::Bool) {
                        storm::expressions::Variable exprVariable = expressionManager.declareBooleanVariable(name);
                        if (initValue.isInitialized()) {
                            return std::make_shared<BooleanVariable>(name, exprVariable, initValue, arrayVariable.isTransient());
                        } else {
                            return std::make_shared<BooleanVariable>(name, exprVariable);
                        }
                    }
                    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Unhandled array base type.");
                    return nullptr;
                }
                
                void insertLValueArrayAccessReplacements(std::vector<Assignment const*> const& arrayAccesses, std::vector<storm::jani::Variable const*> const& arrayVariableReplacements, int64_t level, std::vector<Assignment>& newAssignments) const {
                    bool onlyConstantIndices = true;
                    for (auto const& aa : arrayAccesses) {
                        if (aa->getLValue().getArrayIndex().containsVariables()) {
                            onlyConstantIndices = false;
                            break;
                        }
                    }
                    if (onlyConstantIndices) {
                        for (auto const& aa : arrayAccesses) {
                            LValue lvalue(*arrayVariableReplacements.at(aa->getLValue().getArrayIndex().evaluateAsInt()));
                            newAssignments.emplace_back(lvalue, arrayExprEliminator->eliminate(aa->getAssignedExpression()), level);
                        }
                    } else {
                        for (uint64_t index = 0; index < arrayVariableReplacements.size(); ++index) {
                            storm::expressions::Expression assignedExpression = arrayVariableReplacements[index]->getExpressionVariable().getExpression();
                            auto indexExpression = expressionManager.integer(index);
                            for (auto const& aa : arrayAccesses) {
                                assignedExpression = storm::expressions::ite(arrayExprEliminator->eliminate(aa->getLValue().getArrayIndex()) == indexExpression, arrayExprEliminator->eliminate(aa->getAssignedExpression()), assignedExpression);
                            }
                            newAssignments.emplace_back(LValue(*arrayVariableReplacements[index]), assignedExpression, level);
                        }
                    }
                }
                
                storm::expressions::Expression getOutOfBoundsValue(Variable const& var) const {
                    if (var.hasInitExpression()) {
                        return var.getInitExpression();
                    }
                    if (var.isBooleanVariable()) {
                        return expressionManager.boolean(false);
                    }
                    if (var.isBoundedIntegerVariable()) {
                        return var.asBoundedIntegerVariable().getLowerBound();
                    }
                    if (var.isUnboundedIntegerVariable()) {
                        return expressionManager.integer(0);
                    }
                    if (var.isRealVariable()) {
                        return expressionManager.rational(0.0);
                    }
                    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "unhandled variabe type");
                    return storm::expressions::Expression();
                }
                
                std::unique_ptr<ArrayExpressionEliminationVisitor> arrayExprEliminator;
                storm::expressions::ExpressionManager& expressionManager;
                bool const keepNonTrivialArrayAccess;
                std::unordered_map<storm::expressions::Variable, std::size_t> const& arraySizes;
            };
        } // namespace detail
        
        storm::expressions::Expression ArrayEliminatorData::transformExpression(storm::expressions::Expression const& arrayExpression) const {
            std::unordered_map<storm::expressions::Variable, std::size_t> arraySizes;
            for (auto const& r : replacements) {
                arraySizes.emplace(r.first, r.second.size());
            }
            detail::ArrayExpressionEliminationVisitor eliminator(replacements, arraySizes);
            return eliminator.eliminate(arrayExpression);
        }
        
        void ArrayEliminatorData::transformProperty(storm::jani::Property& property) const {
            property = property.substitute([this](storm::expressions::Expression const& exp) {return transformExpression(exp);});
        }
        
        ArrayEliminatorData ArrayEliminator::eliminate(Model& model, bool keepNonTrivialArrayAccess) {
            ArrayEliminatorData result;
            // Only perform actions if there actually are arrays.
            if (model.getModelFeatures().hasArrays()) {
                auto sizes = detail::MaxArraySizeDeterminer().getMaxSizes(model);
                result = detail::ArrayVariableReplacer(model.getExpressionManager(), keepNonTrivialArrayAccess, sizes).replace(model);
                if (!keepNonTrivialArrayAccess) {
                    model.getModelFeatures().remove(ModelFeature::Arrays);
                }
                model.finalize();
            }
            STORM_LOG_ASSERT(!containsArrayExpression(model), "the model still contains array expressions.");
            return result;
        }
    }
}

