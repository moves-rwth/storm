#include "ArrayEliminator.h"

#include <unordered_map>

#include "storm/storage/expressions/ExpressionVisitor.h"
#include "storm/storage/jani/visitor/JaniExpressionVisitor.h"
#include "storm/storage/jani/Variable.h"
#include "storm/storage/jani/Model.h"
#include "storm/storage/jani/Property.h"
#include "storm/storage/jani/traverser/JaniTraverser.h"
#include "storm/storage/jani/traverser/ArrayExpressionFinder.h"

#include "storm/storage/expressions/Expressions.h"
#include "storm/storage/jani/expressions/JaniExpressions.h"
#include "storm/storage/expressions/ExpressionManager.h"

#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/InvalidOperationException.h"
#include "storm/exceptions/UnexpectedException.h"
#include "storm/exceptions/OutOfRangeException.h"

namespace storm {
    namespace jani {

    
        namespace detail {
            
            class ArrayReplacementsCollectorExpressionVisitor : public storm::expressions::ExpressionVisitor, public storm::expressions::JaniExpressionVisitor {
            public:
                using storm::expressions::ExpressionVisitor::visit;

                typedef typename ArrayEliminatorData::Replacement Replacement;
                typedef std::unordered_map<storm::expressions::Variable, Replacement> ReplMap;
                
                ArrayReplacementsCollectorExpressionVisitor( Model& model) : model(model), converged(false) {};
                virtual ~ArrayReplacementsCollectorExpressionVisitor() = default;

                /*!
                 * Adds necessary replacements for the given array variable, assuming that the given expression is assigned to the array variable.
                 * @returns true iff no replacement had to be added
                 */
                bool get(storm::expressions::Expression const& expression, storm::jani::Variable const& arrayVariable, Replacement& currentReplacement, ReplMap const& allCollectedReplacements, Automaton const* declaringAutomaton = nullptr) {
                    currentVar = &arrayVariable;
                    STORM_LOG_ASSERT(currentVar->getType().isArrayType(), "expected array variable");
                    STORM_LOG_ASSERT((declaringAutomaton == nullptr && model.hasGlobalVariable(currentVar->getName()) || declaringAutomaton != nullptr && declaringAutomaton->hasVariable(currentVar->getName())), "Cannot find variable declaration.");
                    collectedRepl = &allCollectedReplacements;
                    
                    std::vector<std::size_t> currentIndices;
                    converged = true;
                    expression.accept(*this, std::make_pair(&currentReplacement, &currentIndices));
                    return converged;
                }

                virtual boost::any visit(storm::expressions::IfThenElseExpression const& expression, boost::any const& data) override {
                    if (expression.getCondition()->containsVariables()) {
                        expression.getThenExpression()->accept(*this, data);
                        expression.getElseExpression()->accept(*this, data);
                    } else {
                        if (expression.getCondition()->evaluateAsBool()) {
                            expression.getThenExpression()->accept(*this, data);
                        } else {
                            expression.getElseExpression()->accept(*this, data);
                        }
                    }
                    return boost::any();
                }
                
                virtual boost::any visit(storm::expressions::BinaryBooleanFunctionExpression const& expression, boost::any const& data) override {
                    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unexpected type of expression.");
                    return boost::any();
                }
                
                virtual boost::any visit(storm::expressions::BinaryNumericalFunctionExpression const& expression, boost::any const& data) override {
                    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unexpected type of expression.");
                    return boost::any();
                }
                
                virtual boost::any visit(storm::expressions::BinaryRelationExpression const& expression, boost::any const& data) override {
                    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unexpected type of expression.");
                    return boost::any();
                }
                
                void varToVarAssignmentHelper(Replacement& cur, Replacement const& other, std::vector<std::size_t>& indices) {
                    if (other.isVariable()) {
                        if (!cur.isVariable()) {
                            STORM_LOG_ASSERT(cur.size() == 0, "Unexpected replacement type. Assignments incompatible?");
                            cur = Replacement(addReplacementVariable(indices));
                        }
                    } else {
                        STORM_LOG_ASSERT(!cur.isVariable(), "Unexpected replacement type. Assignments incompatible?");
                        cur.grow(other.size());
                        indices.push_back(0);
                        for (std::size_t i = 0; i < other.size(); ++i) {
                            indices.back() = i;
                            varToVarAssignmentHelper(cur.at(i), other.at(i), indices);
                        }
                        indices.pop_back();
                    }
                }
                
                virtual boost::any visit(storm::expressions::VariableExpression const& expression, boost::any const& data) override {
                    if (expression.getType().isArrayType()) {
                        auto varIt = collectedRepl->find(expression.getVariable());
                        if (varIt != collectedRepl->end()) {
                            auto const& otherRepl = varIt->second;
                            // We have to make sure that the sizes for the current replacements fit at least the number of other replacements
                            auto current = boost::any_cast<std::pair<Replacement*, std::vector<std::size_t>*>>(data);
                            varToVarAssignmentHelper(*current.first, varIt->second, *current.second);
                        }
                    }
                    return boost::any();
                }
                
                virtual boost::any visit(storm::expressions::UnaryBooleanFunctionExpression const& expression, boost::any const& data) override {
                    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unexpected type of expression.");
                    return boost::any();
                }
                
                virtual boost::any visit(storm::expressions::UnaryNumericalFunctionExpression const& expression, boost::any const& data) override {
                    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unexpected type of expression.");
                    return boost::any();
                }
                
                virtual boost::any visit(storm::expressions::BooleanLiteralExpression const&, boost::any const&) override {
                    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unexpected type of expression.");
                    return boost::any();
                }
                
                virtual boost::any visit(storm::expressions::IntegerLiteralExpression const&, boost::any const&) override {
                    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unexpected type of expression.");
                    return boost::any();
                }
                
                virtual boost::any visit(storm::expressions::RationalLiteralExpression const&, boost::any const&) override {
                    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unexpected type of expression.");
                    return boost::any();
                }
                
                void arrayExpressionHelper(storm::expressions::ArrayExpression const& expression, Replacement& curRepl, std::vector<size_t>& curIndices) {
                    STORM_LOG_ASSERT(!expression.size()->containsVariables(), "Did not expect variables in size expression.");
                    auto expSize = expression.size()->evaluateAsInt();
                    STORM_LOG_ASSERT(!curRepl.isVariable(), "Unexpected replacement type. Illegal array assignment?");
                    curRepl.grow(expSize);
                    curIndices.push_back(0);
                    for (std::size_t i = 0; i < expSize; ++i) {
                        curIndices.back() = i;
                        auto subexpr = expression.at(i);
                        if (subexpr->getType().isArrayType()) {
                            // This is a nested array.
                            subexpr->accept(*this, std::make_pair(&curRepl.at(i), &curIndices));
                        } else {
                            if (!curRepl.at(i).isVariable()) {
                                STORM_LOG_ASSERT(curRepl.size() == 0, "Unexpected replacement type. Illegal array assignment?");
                                curRepl.at(i) = Replacement(addReplacementVariable(curIndices));
                            }
                        }
                    }
                    curIndices.pop_back();
                }
                
                virtual boost::any visit(storm::expressions::ValueArrayExpression const& expression, boost::any const& data) override {
                    STORM_LOG_ASSERT(expression.size()->isIntegerLiteralExpression(), "unexpected kind of size expression of ValueArrayExpression (" << expression.size()->toExpression() << ").");
                    auto current = boost::any_cast<std::pair<Replacement*, std::vector<std::size_t>*>>(data);
                    arrayExpressionHelper(expression, *current.first, *current.second);
                    return boost::any();
                }

                virtual boost::any visit(storm::expressions::ConstructorArrayExpression const& expression, boost::any const& data) override {
                    if (!expression.size()->containsVariables()) {
                        auto current = boost::any_cast<std::pair<Replacement*, std::vector<std::size_t>*>>(data);
                        arrayExpressionHelper(expression, *current.first, *current.second);
                    } else {
                        auto vars = expression.size()->toExpression().getVariables();
                        std::string variables = "";
                        for (auto const &v : vars) {
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
                
                void arrayVariableAccessHelper(std::vector<std::shared_ptr<storm::expressions::BaseExpression const>>& indexStack, Replacement& cur, Replacement const& other, std::vector<std::size_t>& curIndices) {
                    if (indexStack.empty()) {
                        varToVarAssignmentHelper(cur, other, curIndices);
                    } else {
                        uint64_t i,size;
                        if (indexStack.back()->containsVariables()) {
                            // We have to run over all possible indices
                            i = 0;
                            size = other.size();
                        } else {
                            // only consider the accessed index
                            i = indexStack.back()->evaluateAsInt();
                            size = std::min<uint64_t>(i + 1,other.size()); // Avoid invalid access, e.g. if the sizes of the other variable aren't ready yet
                        }
                        indexStack.pop_back();
                        for (; i < size; ++i) {
                            arrayVariableAccessHelper(indexStack, cur, other.at(i), curIndices);
                        }
                    }
                }
                
                void arrayAccessHelper(std::shared_ptr<storm::expressions::BaseExpression const> const& base, std::vector<std::shared_ptr<storm::expressions::BaseExpression const>>& indexStack, boost::any const& data) {
                    // We interpret the input as expression base[i_n][i_{n-1}]...[i_1] for (potentially empty) indexStack = [i_1, ..., i_n].
                    // This deals with *nested* array accesses
                    auto baseAsArrayAccessExp = std::dynamic_pointer_cast<storm::expressions::ArrayAccessExpression>(base);
                    auto baseAsArrayExp = std::dynamic_pointer_cast<storm::expressions::ArrayExpression>(base);
                    if (indexStack.empty()) {
                        base->accept(*this, data);
                    } else if (baseAsArrayAccessExp) {
                        indexStack.push_back(baseAsArrayAccessExp->getSecondOperand());
                        arrayAccessHelper(baseAsArrayAccessExp->getFirstOperand(), indexStack, data);
                    } else if (baseAsArrayExp) {
                        // the base is an array expression.
                        uint64_t i,size;
                        if (indexStack.back()->containsVariables()) {
                            // We have to run over all possible indices
                            STORM_LOG_THROW(!baseAsArrayExp->size()->containsVariables(), storm::exceptions::NotSupportedException, "Array elimination failed because array access expression considers  array  expression '" << *baseAsArrayExp << "' of unknown size.");
                            i = 0;
                            size = baseAsArrayExp->size()->evaluateAsInt();
                        } else {
                            // only consider the accessed index
                            i = indexStack.back()->evaluateAsInt();
                            size = i + 1;
                            STORM_LOG_THROW(baseAsArrayExp->size()->containsVariables() || i < baseAsArrayExp->size()->evaluateAsInt(), storm::exceptions::InvalidOperationException, "Array access " << base << "[" << i << "] out of bounds.");
                        }
                        indexStack.pop_back();
                        for (; i < size; ++i) {
                            auto indexStackCpy = indexStack;
                            arrayAccessHelper(baseAsArrayExp->at(i), indexStackCpy, data);
                        }
                    } else if (base->isVariableExpression()) {
                        // the lhs is an array variable.
                        STORM_LOG_ASSERT(base->asVariableExpression().getVariable().getType().isArrayType(), "Unexpected variable type in array access.");
                        auto varIt = collectedRepl->find(base->asVariableExpression().getVariable());
                        if (varIt != collectedRepl->end()) {
                            auto const& otherRepl = varIt->second;
                            STORM_LOG_ASSERT(!varIt->second.isVariable(), "Unexpected replacement type. Invalid array assignment?");
                            auto current = boost::any_cast<std::pair<Replacement*, std::vector<std::size_t>*>>(data);
                            arrayVariableAccessHelper(indexStack, *current.first, varIt->second, *current.second);
                        }
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Unexpected kind of array access expression");
                    }
                }
                
                
                virtual boost::any visit(storm::expressions::ArrayAccessExpression const& expression, boost::any const& data) override {
                    std::vector<std::shared_ptr<storm::expressions::BaseExpression const>> indexStack = {expression.getSecondOperand()};
                    arrayAccessHelper(expression.getFirstOperand(), indexStack, data);
                    return boost::any();
                }
                
                virtual boost::any visit(storm::expressions::FunctionCallExpression const&, boost::any const&) override {
                    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Found Function call expression within an array expression. This is not expected since functions are expected to be eliminated at this point.");
                }

               private:
                
                storm::jani::Variable const& addReplacementVariable(std::vector<std::size_t> const& indices) {
                    auto& manager = model.getExpressionManager();
                    std::string name = currentVar->getName();
                    for (auto const& i : indices) {
                        name += "_at_" + std::to_string(i);
                    }
                    
                    storm::expressions::Expression initValue;
                    if (currentVar->hasInitExpression()) {
                        initValue = currentVar->getInitExpression();
                        for (auto const& i : indices) {
                            auto aa = std::make_shared<storm::expressions::ArrayAccessExpression>(manager, initValue.getType().getElementType(), initValue.getBaseExpressionPointer(), manager.integer(i).getBaseExpressionPointer());
                            initValue = storm::expressions::Expression(aa);
                        }
                    }
                    
                    auto const& baseType = currentVar->getType().asArrayType().getBaseTypeRecursive();
                    storm::expressions::Variable exprVariable;
                    if (baseType.isBasicType()) {
                        switch (baseType.asBasicType().get()) {
                            case BasicType::Type::Bool:
                                exprVariable = manager.declareBooleanVariable(name);
                                break;
                            case BasicType::Type::Int:
                                exprVariable = manager.declareIntegerVariable(name);
                                break;
                            case BasicType::Type::Real:
                                exprVariable = manager.declareRationalVariable(name);
                                break;
                        }
                    } else if (baseType.isBoundedType()) {
                        switch (baseType.asBoundedType().getBaseType()) {
                            case BoundedType::BaseType::Int:
                                exprVariable = manager.declareIntegerVariable(name);
                                break;
                            case BoundedType::BaseType::Real:
                                exprVariable = manager.declareRationalVariable(name);
                                break;
                        }
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Unhandled base type for array of type " << currentVar->getType());
                    }
                    
                    auto janiVar = storm::jani::Variable::makeVariable(name, baseType, exprVariable, initValue, currentVar->isTransient());
                    
                    converged = false;
                    if (declaringAutomaton) {
                        return declaringAutomaton->addVariable(*janiVar);
                    } else {
                        return model.addVariable(*janiVar);
                    }
                }
                
                Model& model;
                bool converged;
                jani::Variable const* currentVar;
                Automaton* declaringAutomaton; // nullptr if currentVar is global
                ReplMap const* collectedRepl;
            };
            
            class ArrayExpressionEliminationVisitor : public storm::expressions::ExpressionVisitor, public storm::expressions::JaniExpressionVisitor {
            public:
                using storm::expressions::ExpressionVisitor::visit;

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
    
                std::vector<storm::expressions::Expression> eliminate(std::vector<storm::expressions::Expression> const& expressions) {
                    // here, data is the accessed index of the most recent array access expression. Initially, there is none.
                    std::vector<storm::expressions::Expression> result;
                    for (auto& expression : expressions) {
                        result.push_back(eliminate(expression));
                        STORM_LOG_ASSERT(!containsArrayExpression(result.back()), "Eliminated expression already exists");
                    }
                    return result;
                }

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
                        uint64_t index;
                        if (data.type() == typeid(uint64_t)) {
                            index = boost::any_cast<uint64_t>(data);
                        } else {
                            STORM_LOG_ASSERT(data.type() == typeid(ResultType), "Expecting data to be of result type");
                            auto resultType = boost::any_cast<ResultType>(data);
                            STORM_LOG_ASSERT (!resultType.expr()->containsVariables(), "Expecting all variables to be eliminated of resulting index");
                            index = resultType.expr()->evaluateAsInt();
                        }
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

                virtual boost::any visit(storm::expressions::ValueArrayExpression::ValueArrayElements const& expression, boost::any const& data) override {
                    STORM_LOG_THROW(false, storm::exceptions::UnexpectedException, "Found value array elements expression, this is unexpected as we visit them in the value array expression");
                }

                virtual boost::any visit(storm::expressions::ConstructorArrayExpression const& expression, boost::any const& data) override {
                    STORM_LOG_THROW(!data.empty(), storm::exceptions::NotSupportedException, "Unable to translate ValueArrayExpression to element expression since it does not seem to be within an array access expression.");
                    uint64_t index;
                    if (data.type() == typeid(uint64_t)) {
                        index = boost::any_cast<uint64_t>(data);
                    } else {
                        auto resultType = boost::any_cast<ResultType>(data);
                        if (resultType.expr()->containsVariables()) {
                            auto vars = resultType.expr()->toExpression().getVariables();
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
                        index = resultType.expr()->evaluateAsInt();
                    }
                    if (expression.size()->containsVariables()) {
                            auto vars = expression.toExpression().getVariables();
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
                    } else {
                        if (index >= static_cast<uint64_t>(expression.size()->evaluateAsInt())) {
                            return ResultType("Array index " + std::to_string(index) + " for ConstructorArrayExpression " + expression.toExpression().toString() + " is out of bounds.");
                        }
                    }
                    return ResultType(boost::any_cast<ResultType>(expression.at(index)->accept(*this, boost::any())));
                }
                
                virtual boost::any visit(storm::expressions::ArrayAccessExpression const& expression, boost::any const&) override {
                    // We let the data consist of the pointer referring to the arrayAccessExpression variable
                    // and an int telling us the arrayNumber of the current arrayAccessIndex
                    // e.g. for a[1][4] the data will be a pointer to a and the arrayNumber will be 0, telling us we are considering the first part of the array
                    std::pair<std::shared_ptr<storm::expressions::BaseExpression const>, uint_fast64_t> data = {expression.getFirstOperand(), 0};
                    return expression.getSecondOperand()->accept(*this, data);
                }

                virtual boost::any visit(storm::expressions::ArrayAccessIndexExpression const& expression, boost::any const& data) override {
                    // e.g. for a[1][4] the data is a pointer to a
                    // if the arrayNumber 0 we are considering 1 in a[1]
                    // if the arrayNumber is 1 we are considering the 4 in a[4]
                    STORM_LOG_ASSERT (data.type() == typeid(std::pair<std::shared_ptr<storm::expressions::BaseExpression const>, uint_fast64_t>), "Expecting data to be the pari of a pointer to the expression and an index number");

                    auto castedData = boost::any_cast<std::pair<std::shared_ptr<storm::expressions::BaseExpression const>, uint_fast64_t>>(data);
                    auto arrayExpression = castedData.first;
                    auto arrayNumber = castedData.second;
                    // Go over other part of array, the first part of resultSecond will be a map containing of index and isCurrentIndex expressions, the second part is the size of the array so far
                    // e.g. array(4, array(5, array(3))) will give us for:
                    // - the array(3): 0
                    // - array(5, array(3)): 3
                    // - array(4, array(5, array(3))): 15
                    std::pair<std::shared_ptr<storm::expressions::BaseExpression const>, uint_fast64_t> newData = {arrayExpression, arrayNumber + 1};
                    boost::optional<std::pair<std::map<uint_fast64_t, storm::expressions::Expression>, uint_fast64_t>> resultSecond;
                    auto sizeSoFar = 1;
                    if (expression.getFirstOperand() != expression.getSecondOperand()) {
                        resultSecond = boost::any_cast<std::pair<std::map<uint_fast64_t, storm::expressions::Expression>, uint_fast64_t> >(expression.getSecondOperand()->accept(*this, newData));
                        sizeSoFar = resultSecond->second;
                    }

                    uint64_t size = MaxArraySizeExpressionVisitor().getMaxSizeAt(arrayExpression->toExpression(), arraySizes, arrayNumber);
                    STORM_LOG_THROW(size > 0, storm::exceptions::NotSupportedException, "Unable to get size of array expression for array access " << expression << ".");

                    std::map<uint_fast64_t, storm::expressions::Expression> expressions;

                    if (expression.getFirstOperand() == expression.getSecondOperand()) {
                        // Last array, so resultSecond will not be initialized
                        if (expression.getFirstOperand()->containsVariables()) {
                            for (uint_fast64_t index = 0; index < size; ++index) {
                                storm::expressions::Expression isCurrentIndex = boost::any_cast<ResultType>(expression.getFirstOperand()->accept(*this, boost::any())).expr()->toExpression() == expression.getManager().integer(index);
                                expressions[index] = std::move(isCurrentIndex);
                            }
                        } else {
                            expressions[expression.getFirstOperand()->evaluateAsInt()] = expression.getManager().boolean(true);
                        }
                    } else {
                        if (expression.getFirstOperand()->containsVariables()) {
                            for (uint_fast64_t index = 0; index < size; ++index) {
                                storm::expressions::Expression isCurrentIndex = boost::any_cast<ResultType>(expression.getFirstOperand()->accept(*this, boost::any())).expr()->toExpression() == expression.getManager().integer(index);
                                for (auto& entry : resultSecond->first) {
                                    expressions[index * sizeSoFar + entry.first] = (isCurrentIndex && entry.second).simplify();
                                }
                            }
                        } else {
                            auto index = expression.getFirstOperand()->evaluateAsInt();
                            for (auto& entry : resultSecond->first) {
                                expressions[index * sizeSoFar + entry.first] = entry.second;
                            }
                        }
                    }

                    if (arrayNumber == 0) {
                        STORM_LOG_ASSERT(expressions.size() > 1 || expressions.begin()->second.evaluateAsBool(), "Expecting either more than one expression, ir a boolean expression");
                        auto itr = expressions.begin();
                        storm::expressions::Expression result = boost::any_cast<ResultType>(arrayExpression->accept(*this, itr->first)).expr()->toExpression();
                        ++itr;
                        for (; itr != expressions.end(); ++itr) {
                            result = storm::expressions::ite(itr->second, boost::any_cast<ResultType>(arrayExpression->accept(*this, itr->first)).expr()->toExpression(), result);
                        }
                        return ResultType(result.simplify().getBaseExpressionPointer());
                    } else {
                        std::pair<std::map<uint_fast64_t, storm::expressions::Expression>, uint_fast64_t> result = {expressions, size * sizeSoFar};
                        return result;
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
            
            class ArrayEliminatorDataCollector : public ConstJaniTraverser {
            public:
                
                ArrayEliminatorDataCollector(Model& model) : model(model), exprVisitor(model), converged(false), currentAutomaton(nullptr) {}
                virtual ~ArrayEliminatorDataCollector() = default;
                
                // TODO: we are adding variables to the model while iterating over the variable set. This is dangerous. Probably should only traverse over set of array variables (which remains untouched)
                ArrayEliminatorData get() {
                    // To correctly determine the array sizes, we repeat the steps until convergence. This is to cover assignments of one array variable to another (A := B)
                    ArrayEliminatorData result;
                    converged = true;
                    do {
                        ConstJaniTraverser::traverse(model, &result);
                    } while (!converged);
                    
                    return result;
                }
                
                virtual void traverse(Automaton const& automaton, boost::any const& data) override {
                    currentAutomaton = &automaton;
                    ConstJaniTraverser::traverse(automaton, data);
                    currentAutomaton = nullptr;
                }
                
                Automaton const* declaringAutomaton(Variable const& var) const {
                    if (currentAutomaton && currentAutomaton->getVariables().hasVariable(var)) {
                        return currentAutomaton;
                    }
                    STORM_LOG_ASSERT(model.getGlobalVariables().hasVariable(var), "Variable " << var.getName() << " not found.");
                    return nullptr;
                }
                
                void gatherArrayAccessIndices(std::vector<std::vector<std::size_t>>& gatheredIndices, std::vector<std::size_t>& current, typename ArrayEliminatorData::Replacement const& repl, std::vector<storm::expressions::Expression> const& indices) {
                    STORM_LOG_ASSERT(!repl.isVariable(), "Unexpected replacement type. Assignments incompatible?");
                    if (current.size() < indices.size()) {
                        auto const& indexExp = indices[current.size()];
                        if (indexExp.containsVariables()) {
                            current.push_back(std::numeric_limits<std::size_t>::max());
                            // We have to consider all possible indices
                            for (std::size_t i = 0; i < repl.size(); ++i) {
                                current.back() = i;
                                gatherArrayAccessIndices(gatheredIndices, current, repl.at(i), indices);
                            }
                            current.pop_back();
                        } else {
                            auto i = indexExp.evaluateAsInt();
                            if (i < repl.size()) {
                                current.push_back(i);
                                gatherArrayAccessIndices(gatheredIndices, current, repl.at(i), indices);
                                current.pop_back();
                            } // The else case might either be an invalid array access or we haven't processed the corresponding assignment yet (and thus will be covered in a subsequent run)
                        }
                    } else {
                        gatheredIndices.push_back(current);
                    }
                }
                
                virtual void traverse(Assignment const& assignment, boost::any const& data) override {
                    auto const& var = assignment.getLValue().getVariable();
                    if (assignment.getLValue().isArrayAccess() && !assignment.getLValue().isFullArrayAccess()) {
                        auto& result = *boost::any_cast<ArrayEliminatorData*>(data);
                        auto& arrayVarReplacements = result.replacements[var.getExpressionVariable()]; // creates if it does not yet exist

                        // If the indices are not constant expressions, we run through every possible combination of indices
                        std::vector<std::vector<std::size_t>> gatheredIndices;
                        std::vector<std::size_t> tmp;
                        gatherArrayAccessIndices(gatheredIndices, tmp, result.replacements[var.getExpressionVariable()], assignment.getLValue().getArrayIndexVector());
                        for (auto const& indices : gatheredIndices) {
                            converged &= exprVisitor.get(assignment.getAssignedExpression(), , arrayVarReplacements.at(indices), result.replacements, declaringAutomaton(var));
                        }
                    } else if (assignment.getLValue().isArray()) {
                        auto& result = *boost::any_cast<ArrayEliminatorData*>(data);
                        converged &= exprVisitor.get(assignment.getAssignedExpression(), var, result.replacements, declaringAutomaton(var));
                    }
                }
                
                virtual void traverse(Variable const& variable, boost::any const& data) override {
                    if (variable.getType().isArrayType() && variable.hasInitExpression()) {
                        auto& result = *boost::any_cast<ArrayEliminatorData*>(data);
                        converged &= exprVisitor.get(variable.getInitExpression(), variable, result.replacements, declaringAutomaton(variable));
                    }
                }
               
               private:
                Model& model;
                Automaton const* currentAutomaton;
                ArrayReplacementsCollectorExpressionVisitor exprVisitor;
                bool converged;
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
                    for (storm::jani::Variable const& arrayVariable : model.getGlobalVariables().getArrayVariables()) {
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
                    for (storm::jani::Variable const& arrayVariable : automaton.getVariables().getArrayVariables()) {
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
                            
                            if (assignment.getLValue().isArray() && assignment.getLValue().isFullArrayAccess()) {
                                // This is the easy case, we can eliminate the variable by replacing it by its _at_... equivalent
                                if (!keepNonTrivialArrayAccess || !assignment.getLValue().arrayIndexContainsVariable()) {
                                    auto insertionRes = collectedArrayAccessAssignments.emplace(assignment.getVariable().getExpressionVariable(), std::vector<Assignment const*>({&assignment}));
                                    if (!insertionRes.second) {
                                        insertionRes.first->second.push_back(&assignment);
                                    }
                                } else {
                                    // Keeping array access LValue
                                    auto newIndex = arrayExprEliminator->eliminate(assignment.getLValue().getArrayIndex());
                                    LValue newLValue(LValue(assignment.getVariable(), newIndex, assignment.getLValue().getTotalSize()));
                                    newAssignments.emplace_back(newLValue, arrayExprEliminator->eliminate(assignment.getAssignedExpression()), assignment.getLevel());
                                }
                            } else if (assignment.getLValue().isArray()) {
                                // In this case we assign to an array to an array, there are one or more indices missing
                                STORM_LOG_ASSERT(assignment.getAssignedExpression().getType().isArrayType(), "Assigning a non-array expression to an array variable... " << assignment);
                                // We can only deal with the case where the last index missing
                                STORM_LOG_THROW((assignment.getLValue().getSizes().size() - 1 == assignment.getLValue().getArrayIndexVector().size()), storm::exceptions::NotImplementedException, "Eliminating nested arrays assigned to nested arrays not yet implemented problem occured at "<< assignment.getName());
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
                                // In this case we have a non-array variable, we don't need to eliminate anything in the lhs
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
                            STORM_LOG_ASSERT(!containsArrayExpression(assignment.getExpressionVariable()), "HELP: " << assignment);
                        }
                    }
                }
                
            private:
                
                std::shared_ptr<Variable> getBasicVariable(Variable const& arrayVariable, uint64_t index) const {
                    STORM_LOG_ASSERT(arrayVariable.getType().isArrayType(), "Unexpected variable type.");
                    std::string name = arrayVariable.getExpressionVariable().getName() + "_at_" + std::to_string(index);
                    storm::expressions::Expression initValue;
                    if (arrayVariable.hasInitExpression()) {
                        // We want to eliminate the initial value for the arrayVariable at entry index, in case of a nested_array, this index is already re-calculated.
                        // In the end we have an ArrayAccessExpression
                        //TODO: (TQ) Check how this works for nested arrays
                        auto indexExpression = std::make_shared<storm::expressions::ArrayAccessIndexExpression>(expressionManager, expressionManager.getIntegerType(), expressionManager.integer(index).getBaseExpressionPointer());
                        initValue = arrayExprEliminator->eliminate(std::make_shared<storm::expressions::ArrayAccessExpression>(expressionManager, arrayVariable.getExpressionVariable().getType().getElementType(), arrayVariable.getInitExpression().getBaseExpressionPointer(), indexExpression)->toExpression());
                        STORM_LOG_ASSERT(!containsArrayExpression(initValue), "HELP:  " << arrayVariable.getName());
                    }

                    auto const& baseType = arrayVariable.getType().asArrayType().getBaseTypeRecursive();
                    
                    storm::expressions::Variable exprVariable;
                    if (baseType.isBasicType()) {
                        switch (baseType.asBasicType().get()) {
                            case BasicType::Type::Bool:
                                exprVariable = expressionManager.declareBooleanVariable(name);
                                break;
                            case BasicType::Type::Int:
                                exprVariable = expressionManager.declareIntegerVariable(name);
                                break;
                            case BasicType::Type::Real:
                                exprVariable = expressionManager.declareRationalVariable(name);
                                break;
                        }
                    } else if (baseType.isBoundedType()) {
                        switch (baseType.asBoundedType().getBaseType()) {
                            case BoundedType::BaseType::Int:
                                exprVariable = expressionManager.declareIntegerVariable(name);
                                break;
                            case BoundedType::BaseType::Real:
                                exprVariable = expressionManager.declareRationalVariable(name);
                                break;
                        }
                    } else {
                        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Unhandled base type for array of type " << arrayVariable.getType());
                    }
                    return storm::jani::Variable::makeVariable(name, baseType, exprVariable, initValue, arrayVariable.isTransient());
                }
                
                void insertLValueArrayAccessReplacements(std::vector<Assignment const*> const& arrayAccesses, std::vector<storm::jani::Variable const*> const& arrayVariableReplacements, int64_t level, std::vector<Assignment>& newAssignments) const {
                    bool onlyConstantIndices = true;
                    for (auto const& aa : arrayAccesses) {
                        if (aa->getLValue().arrayIndexContainsVariable()) {
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
                        STORM_LOG_ASSERT(!containsArrayExpression(var.getInitExpression()), "HELP out of bounds " << var.getName());
                        return var.getInitExpression();
                    }
                    
                    if (var.getType().isBasicType()) {
                        switch (var.getType().asBasicType().get()) {
                            case BasicType::Type::Bool:
                                return expressionManager.boolean(false);
                            case BasicType::Type::Int:
                                return expressionManager.integer(0);
                            case BasicType::Type::Real:
                                return expressionManager.rational(0.0);
                        }
                    } else if (var.getType().isBoundedType()) {
                        auto const& bndType = var.getType().asBoundedType();
                        if (bndType.hasLowerBound()) {
                            return bndType.getLowerBound();
                        } else if (bndType.hasUpperBound()) {
                            return bndType.getUpperBound();
                        } else {
                            switch (bndType.getBaseType()) {
                                case BoundedType::BaseType::Int:
                                    return expressionManager.integer(0);
                                case BoundedType::BaseType::Real:
                                    return expressionManager.rational(0.0);
                            }
                        }
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
        
        
        ArrayEliminatorData::Replacement::Replacement(storm::jani::Variable const& variable) : data(&variable) {
            // Intentionally left empty
        }
        
        ArrayEliminatorData::Replacement::Replacement(std::vector<Replacement>&& replacements) : data(std::move(replacements)) {
            // Intentionally left empty
        }
        
        bool ArrayEliminatorData::Replacement::isVariable() const {
            return data.which() == 0;
        }
        
        storm::jani::Variable const& ArrayEliminatorData::Replacement::getVariable() const {
            STORM_LOG_ASSERT(isVariable(), "unexpected replacement type");
            return *boost::get<storm::jani::Variable const*>(data);
        }
        
        
        std::vector<typename ArrayEliminatorData::Replacement> const& ArrayEliminatorData::Replacement::getReplacements() const {
            STORM_LOG_ASSERT(!isVariable(), "unexpected replacement type");
            return boost::get<std::vector<Replacement>>(data);
        }
        
        typename ArrayEliminatorData::Replacement const& ArrayEliminatorData::Replacement::at(std::size_t const& index) const {
            STORM_LOG_ASSERT(!isVariable(), "unexpected replacement type");
            return getReplacements().at(index);
        }
        
        typename ArrayEliminatorData::Replacement& ArrayEliminatorData::Replacement::at(std::size_t const& index) {
            STORM_LOG_ASSERT(!isVariable(), "unexpected replacement type");
            return boost::get<std::vector<Replacement>>(data).at(index);
        }
        
        typename ArrayEliminatorData::Replacement const& ArrayEliminatorData::Replacement::at(std::vector<std::size_t> const& indices) const {
            auto const* cur = this;
            for (auto const& i : indices) {
                cur = &cur->at(i);
            }
            return *cur;
        }
        
        typename ArrayEliminatorData::Replacement & ArrayEliminatorData::Replacement::at(std::vector<std::size_t> const& indices) {
            auto* cur = this;
            for (auto const& i : indices) {
                cur = &cur->at(i);
            }
            return *cur;
        }
        
        std::size_t ArrayEliminatorData::Replacement::size() const {
            return getReplacements().size();
        }
        
        void ArrayEliminatorData::Replacement::grow(std::size_t const& minimumSize) {
            if (getReplacements().size() < minimumSize) {
                boost::get<std::vector<Replacement>>(data).resize(minimumSize);
            }
        }

        
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
                // TODO: hack
                auto size = model.getConstants().size();
                for (uint_fast64_t i = 0; i < size; ++i) {
                    auto constant = model.getConstants().at(i);
                    if (constant.isDefined() && containsArrayExpression(constant.getExpression())) {
                        // We hope we don't need this one any longer however this could break everything :D Breaking things is fun :D
                        model.removeConstant(constant.getName());
                        i--;
                        size--;
                    }
                }
                model.finalize();
            }
            STORM_LOG_ASSERT(!containsArrayExpression(model), "the model still contains array expressions.");
            return result;
        }
    }
}

