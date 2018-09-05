#include "storm/storage/jani/traverser/ArrayEliminator.h"

#include <unordered_map>

#include "storm/storage/expressions/ExpressionVisitor.h"
#include "storm/storage/jani/expressions/JaniExpressionVisitor.h"
#include "storm/storage/jani/Variable.h"
#include "storm/storage/expressions/Expressions.h"
#include "storm/storage/jani/expressions/JaniExpressions.h"
#include "storm/storage/expressions/ExpressionManager.h"

#include "storm/exceptions/NotSupportedException.h"

namespace storm {
    namespace jani {
        namespace detail {
            
            class MaxArraySizeExpressionVisitor : public storm::expressions::ExpressionVisitor, public storm::expressions::JaniExpressionVisitor {
            public:
                MaxArraySizeExpressionVisitor() = default;
                virtual ~MaxArraySizeExpressionVisitor() = default;
    
                std::size_t getMaxSize(storm::expressions::Expression const& expression, std::unordered_map<storm::expressions::Variable, std::size_t> arrayVariableSizeMap) {
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
                    std::unordered_map<storm::expressions::Variable, std::size_t>* arrayVariableSizeMap = boost::any_cast<std::unordered_map<storm::expressions::Variable, std::size_t>*>(data);
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
                
                virtual boost::any visit(storm::expressions::BooleanLiteralExpression const& expression, boost::any const& data) override {
                    return 0;
                }
                
                virtual boost::any visit(storm::expressions::IntegerLiteralExpression const& expression, boost::any const& data) override {
                    return 0;
                }
                
                virtual boost::any visit(storm::expressions::RationalLiteralExpression const& expression, boost::any const& data) override {
                    return 0;
                }
                
                virtual boost::any visit(storm::expressions::ValueArrayExpression const& expression, boost::any const& data) override {
                    STORM_LOG_ASSERT(expression.size()->isIntegerLiteralExpression(), "unexpected kind of size expression of ValueArrayExpression (" << expression.size()->toExpression() << ").");
                    return static_cast<std::size_t>(expression.size()->evaluateAsInt());
                }
                
                virtual boost::any visit(storm::expressions::ConstructorArrayExpression const& expression, boost::any const& data) override {
                    if (!expression.size()->containsVariables()) {
                        return static_cast<std::size_t>(expression.size()->evaluateAsInt());
                    } else {
                        auto vars = expression.toExpression().getVariables();
                        std::string variables = "";
                        for (auto const& v : vars) {
                            if (variables != "") {
                                variables += ", ";
                            }
                            variables += v.getName();
                        }
                        if (vars.size() == 1) {
                            variables = "variable ";
                        } else {
                            variables = "variables ";
                        }
                        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Unable to get determine array size: Size of ConstructorArrayExpression still contains the " << variables << ".");
                    }
                }
                
                virtual boost::any visit(storm::expressions::ArrayAccessExpression const& expression, boost::any const& data) override {
                    STORM_LOG_WARN("Array access expression on rhs of array assignment. This is not expected since nested arrays are currently not supported.");
                    return 0;
                }
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
                
                virtual void traverse(Assignment const& assignment, boost::any const& data) const override {
                    if (assignment.lValueIsVariable() && assignment.getExpressionVariable().getType().isArrayType()) {
                        auto& map = *boost::any_cast<MapPtr>(data);
                        std::size_t newSize = MaxArraySizeExpressionVisitor().getMaxSize(assignment.getAssignedExpression(), map);
                        auto insertionRes = map.emplace(assignment.getExpressionVariable(), newSize);
                        if (!insertionRes.second) {
                            insertionRes.first->second = std::max(newSize, insertionRes.first->second);
                        }
                    }
                }
                
                virtual void traverse(ArrayVariable const& variable, boost::any const& data) const override {
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
                
                ArrayVariableReplacer(storm::expressions::ExpressionManager& expressionManager, bool keepNonTrivialArrayAccess) : expressionManager(expressionManager) , keepNonTrivialArrayAccess(keepNonTrivialArrayAccess) {}
                
                virtual ~ArrayVariableReplacer() = default;
                ResultType replace(Model& model, std::unordered_map<storm::expressions::Variable, std::size_t> const& arrayVarToSizesMap) {
                    ResultType result;
                    for (auto const& arraySize : arrayVarToSizesMap) {
                        result.replacements.emplace(arraySize.first, std::vector<storm::jani::Variable const*>(arraySize.second, nullptr));
                    }
                    traverse(model, &result);
                    return result;
                }

                virtual void traverse(Model& model, boost::any const& data) const override {
                    
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
                    
                    for (auto& aut : model.getAutomata()) {
                        traverse(aut, data);
                    }
                    
                    // traverse remaining components
                    for (auto& c : model.getConstants()) {
                        traverse(c, data);
                    }
                    if (model.hasInitialStatesRestriction()) {
                        //model.setInitialStatesRestriction();
                    }
                }
                
                virtual void traverse(Automaton& automaton, boost::any const& data) const override {
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

                    for (auto& loc : automaton.getLocations()) {
                        JaniTraverser::traverse(loc, data);
                    }
                    JaniTraverser::traverse(automaton.getEdgeContainer(), data);
                }
                
                void traverse(Constant& constant, boost::any const& data) const {
                    if (constant.isDefined()) {
                        // todo:
                        //traverse(constant.getExpression(), data);
                    }
                }

                virtual void traverse(OrderedAssignments& orderedAssignments, boost::any const& data) const override {
                    auto const& replacements = boost::any_cast<ResultType*>(data)->replacements;

                    // Replace array occurrences in LValues.
                    std::vector<Assignment> newAssignments;
                    uint64_t level = 0;
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
                                continue;
                            }
                        } else if (assignment.getLValue().isVariable() && assignment.getVariable().isArrayVariable()) {
                            STORM_LOG_ASSERT(assignment.getAssignedExpression().getType().isArrayType(), "Assigning a non-array expression to an array variable...");
                            std::vector<storm::jani::Variable const*> const& arrayVariableReplacements = replacements.at(assignment.getExpressionVariable());
                            for (uint64_t index = 0; index < arrayVariableReplacements.size(); ++index) {
                                auto arrayAccessExpression = std::make_shared<storm::expressions::ArrayAccessExpression>(expressionManager, assignment.getAssignedExpression().getType().getElementType(), assignment.getAssignedExpression().getBaseExpressionPointer(), expressionManager.integer(index).getBaseExpressionPointer());
                                newAssignments.emplace_back(LValue(*arrayVariableReplacements[index]), arrayAccessExpression->toExpression(), level);
                            }
                            continue;
                        }
                        newAssignments.push_back(assignment);
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
                
            private:
                
                std::shared_ptr<Variable> getBasicVariable(ArrayVariable const& arrayVariable, uint64_t index) const {
                    std::string name = arrayVariable.getExpressionVariable().getName() + "_at_" + std::to_string(index);
                    storm::expressions::Expression initValue;
                    if (arrayVariable.hasInitExpression()) {
                        initValue = std::make_shared<storm::expressions::ArrayAccessExpression>(expressionManager, arrayVariable.getExpressionVariable().getType().getElementType(), arrayVariable.getInitExpression().getBaseExpressionPointer(), expressionManager.integer(index).getBaseExpressionPointer())->toExpression();
                    }
                    if (arrayVariable.getElementType() == ArrayVariable::ElementType::Int) {
                        storm::expressions::Variable exprVariable = expressionManager.declareIntegerVariable(name);
                        if (arrayVariable.hasElementTypeBounds()) {
                            if (initValue.isInitialized()) {
                                return std::make_shared<BoundedIntegerVariable>(name, exprVariable, initValue, arrayVariable.isTransient(), arrayVariable.getElementTypeBounds().first, arrayVariable.getElementTypeBounds().second);
                            } else {
                                return std::make_shared<BoundedIntegerVariable>(name, exprVariable, arrayVariable.getElementTypeBounds().first, arrayVariable.getElementTypeBounds().second);
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
                
                void insertLValueArrayAccessReplacements(std::vector<Assignment const*> const& arrayAccesses, std::vector<storm::jani::Variable const*> const& arrayVariableReplacements, uint64_t level, std::vector<Assignment>& newAssignments) const {
                    storm::expressions::Variable const& arrayVariable = arrayAccesses.front()->getLValue().getArray().getExpressionVariable();
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
                            newAssignments.emplace_back(lvalue, aa->getAssignedExpression(), level);
                        }
                    } else {
                        for (uint64_t index = 0; index < arrayVariableReplacements.size(); ++index) {
                            storm::expressions::Expression assignedExpression = arrayVariableReplacements[index]->getExpressionVariable().getExpression();
                            auto indexExpression = expressionManager.integer(index);
                            for (auto const& aa : arrayAccesses) {
                                assignedExpression = storm::expressions::ite(aa->getLValue().getArrayIndex() == indexExpression, aa->getAssignedExpression(), assignedExpression);
                                newAssignments.emplace_back(LValue(*arrayVariableReplacements[index]), assignedExpression, level);
                            }
                        }
                    }
                }
                
                storm::expressions::ExpressionManager& expressionManager;
                bool const keepNonTrivialArrayAccess;
            };
            
            
        
        } // namespace detail
        
        ArrayEliminatorData ArrayEliminator::eliminate(Model& model, bool keepNonTrivialArrayAccess) {
            auto sizes = detail::MaxArraySizeDeterminer().getMaxSizes(model);
            ArrayEliminatorData result = detail::ArrayVariableReplacer(model.getExpressionManager(), keepNonTrivialArrayAccess).replace(model, sizes);
            
            model.finalize();
            return result;
        }
    }
}

