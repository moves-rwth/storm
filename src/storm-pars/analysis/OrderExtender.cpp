#include "OrderExtender.h"

#include "storm/exceptions/NotSupportedException.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/modelchecker/propositional/SparsePropositionalModelChecker.h"
#include "storm/storage/BitVector.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/utility/macros.h"
#include "storm/utility/graph.h"

#include "storm-pars/api/region.h"
#include "storm-pars/api/export.h"
#include "storm-pars/analysis/MonotonicityHelper.h"
#include "storm/storage/StronglyConnectedComponentDecomposition.h"

#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"

namespace storm {
    namespace analysis {

        template <typename ValueType, typename ConstantType>
        OrderExtender<ValueType, ConstantType>::OrderExtender(std::shared_ptr<models::sparse::Model<ValueType>> model, std::shared_ptr<logic::Formula const> formula,  storage::ParameterRegion<ValueType> region) : monotonicityChecker(MonotonicityChecker<ValueType>(model->getTransitionMatrix())) {
            this->model = model;
            this->matrix = model->getTransitionMatrix();
            this->numberOfStates = this->model->getNumberOfStates();
            this->params = storm::models::sparse::getProbabilityParameters(*model);

            // Build stateMap
            for (uint_fast64_t state = 0; state < numberOfStates; ++state) {
                auto row = matrix.getRow(state);
                stateMap[state] = std::vector<uint_fast64_t>();
                std::set<VariableType> occurringVariables;

                for (auto& entry : matrix.getRow(state)) {
                    // ignore self-loops when there are more transitions
                    if (state != entry.getColumn() || row.getNumberOfEntries() == 1) {
                        stateMap[state].push_back(entry.getColumn());
                    }
                    storm::utility::parametric::gatherOccurringVariables(entry.getValue(), occurringVariables);

                }
                if (occurringVariables.empty()) {
                    nonParametericStates.insert(state);
                }
                for (auto& var : occurringVariables) {
                    occuringStatesAtVariable[var].push_back(state);
                }
                occuringVariablesAtState.push_back(std::move(occurringVariables));
            }
            cyclic = storm::utility::graph::hasCycle(matrix);
            this->region = region;
            this->formula = formula;
//            usePLA = false;
            this->assumptionMaker = new analysis::AssumptionMaker<ValueType, ConstantType>(matrix);
            statesSorted = storm::utility::graph::getTopologicalSort(matrix);
            std::reverse(statesSorted.begin(), statesSorted.end());
        }

        template <typename ValueType, typename ConstantType>
        OrderExtender<ValueType, ConstantType>::OrderExtender(storm::storage::BitVector* topStates,  storm::storage::BitVector* bottomStates, storm::storage::SparseMatrix<ValueType> matrix) : monotonicityChecker(MonotonicityChecker<ValueType>(matrix)) {
            this->matrix = matrix;
            this->model = nullptr;
            this->monotonicityChecker = MonotonicityChecker<ValueType>(matrix);

            storm::storage::StronglyConnectedComponentDecompositionOptions options;
            options.forceTopologicalSort();

            this->numberOfStates = matrix.getColumnCount();

            // Build stateMap
            for (uint_fast64_t state = 0; state < numberOfStates; ++state) {
                auto row = matrix.getRow(state);
                stateMap[state] = std::vector<uint_fast64_t>();
                std::set<VariableType> occurringVariables;

                for (auto& entry : matrix.getRow(state)) {
                    // ignore self-loops when there are more transitions
                    if (state != entry.getColumn() || row.getNumberOfEntries() == 1) {
                        stateMap[state].push_back(entry.getColumn());
                    }
                    storm::utility::parametric::gatherOccurringVariables(entry.getValue(), occurringVariables);

                }
                if (occurringVariables.empty()) {
                    nonParametericStates.insert(state);
                }
                for (auto& var : occurringVariables) {
                    occuringStatesAtVariable[var].push_back(state);
                }
            }
            cyclic = storm::utility::graph::hasCycle(matrix);
            this->bottomTopOrder = std::shared_ptr<Order>(new Order(topStates, bottomStates, numberOfStates, storm::storage::StronglyConnectedComponentDecomposition<ValueType>(matrix, options)));
            this->assumptionMaker = new analysis::AssumptionMaker<ValueType, ConstantType>(matrix);
            statesSorted = storm::utility::graph::getTopologicalSort(matrix);
            std::reverse(statesSorted.begin(), statesSorted.end());
        }

        template <typename ValueType, typename ConstantType>
        std::shared_ptr<Order> OrderExtender<ValueType, ConstantType>::getBottomTopOrder() {
            if (bottomTopOrder == nullptr) {
                assert (model != nullptr);
                STORM_LOG_THROW(matrix.getRowCount() == matrix.getColumnCount(), exceptions::NotSupportedException,"Creating order not supported for non-square matrix");
                modelchecker::SparsePropositionalModelChecker<models::sparse::Model<ValueType>> propositionalChecker(*model);
                storage::BitVector phiStates;
                storage::BitVector psiStates;
                assert (formula->isProbabilityOperatorFormula());
                if (formula->asProbabilityOperatorFormula().getSubformula().isUntilFormula()) {
                    phiStates = propositionalChecker.check(
                            formula->asProbabilityOperatorFormula().getSubformula().asUntilFormula().getLeftSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
                    psiStates = propositionalChecker.check(
                            formula->asProbabilityOperatorFormula().getSubformula().asUntilFormula().getRightSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
                } else {
                    assert (formula->asProbabilityOperatorFormula().getSubformula().isEventuallyFormula());
                    phiStates = storage::BitVector(numberOfStates, true);
                    psiStates = propositionalChecker.check(
                            formula->asProbabilityOperatorFormula().getSubformula().asEventuallyFormula().getSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
                }
                // Get the maybeStates
                std::pair<storage::BitVector, storage::BitVector> statesWithProbability01 = utility::graph::performProb01(this->model->getBackwardTransitions(), phiStates, psiStates);
                storage::BitVector topStates = statesWithProbability01.second;
                storage::BitVector bottomStates = statesWithProbability01.first;

                STORM_LOG_THROW(topStates.begin() != topStates.end(), exceptions::NotSupportedException,"Formula yields to no 1 states");
                STORM_LOG_THROW(bottomStates.begin() != bottomStates.end(), exceptions::NotSupportedException,"Formula yields to no zero states");
                auto matrix = this->model->getTransitionMatrix();
                storm::storage::StronglyConnectedComponentDecompositionOptions options;
                options.forceTopologicalSort();
                bottomTopOrder = std::shared_ptr<Order>(new Order(&topStates, &bottomStates, numberOfStates, storm::storage::StronglyConnectedComponentDecomposition<ValueType>(matrix, options)));
            }

            auto transpose = matrix.transpose();
            for (auto const& bottom : bottomTopOrder->getBottom()->states) {
                auto currentStates = transpose.getRow(bottom);
                for (auto const &rowEntry : currentStates) {
                    auto currentState = rowEntry.getColumn();
                    if (currentState != bottom && stateMap[currentState].size() == 2) {
                        if (bottomTopOrder->contains(currentState)) {
                            bottomTopOrder->addAbove(currentState, bottomTopOrder->getBottom());
                        } else {
                            bottomTopOrder->add(currentState);
                        }
                        if (cyclic) {
                            bottomTopOrder->addStateToHandle(currentState);
                        }
                    }
                }
            }

            for (auto const& bottom : bottomTopOrder->getTop()->states) {
                auto currentStates = transpose.getRow(bottom);
                for (auto const &rowEntry : currentStates) {
                    auto currentState = rowEntry.getColumn();
                    if (currentState != bottom && stateMap[currentState].size() == 2) {
                        if (bottomTopOrder->contains(currentState)) {
                            // Do nothing, as this state will point at =( and =)
                        } else {
                            bottomTopOrder->add(currentState);
                            if (cyclic) {
                                bottomTopOrder->addStateToHandle(currentState);
                            }
                        }
                    }
                }
            }
            storm::storage::StronglyConnectedComponentDecompositionOptions const options;
            auto decomposition = storm::storage::StronglyConnectedComponentDecomposition<ValueType>(matrix, options);
            return bottomTopOrder;
        }

        template <typename ValueType, typename ConstantType>
        std::tuple<std::shared_ptr<Order>, uint_fast64_t, uint_fast64_t> OrderExtender<ValueType, ConstantType>::toOrder(storage::ParameterRegion<ValueType> region, std::shared_ptr<MonotonicityResult<VariableType>> monRes) {
            return this->extendOrder(nullptr, region, monRes, nullptr);
        }

        template <typename ValueType, typename ConstantType>
        void OrderExtender<ValueType, ConstantType>::handleAssumption(std::shared_ptr<Order> order, std::shared_ptr<expressions::BinaryRelationExpression> assumption) const {
            assert (assumption != nullptr);
            assert (assumption->getFirstOperand()->isVariable() && assumption->getSecondOperand()->isVariable());

            expressions::Variable var1 = assumption->getFirstOperand()->asVariableExpression().getVariable();
            expressions::Variable var2 = assumption->getSecondOperand()->asVariableExpression().getVariable();
            auto val1 = std::stoul(var1.getName(), nullptr, 0);
            auto val2 = std::stoul(var2.getName(), nullptr, 0);

            assert (order->compare(val1, val2) == Order::UNKNOWN);

            Order::Node* n1 = order->getNode(val1);
            Order::Node* n2 = order->getNode(val2);

            if (assumption->getRelationType() == expressions::BinaryRelationExpression::RelationType::Equal) {
                if (n1 != nullptr && n2 != nullptr) {
                    order->mergeNodes(n1,n2);
                } else if (n1 != nullptr) {
                    order->addToNode(val2, n1);
                } else if (n2 != nullptr) {
                    order->addToNode(val1, n2);
                } else {
                    order->add(val1);
                    order->addToNode(val2, order->getNode(val1));
                }
            } else {
                assert (assumption->getRelationType() == expressions::BinaryRelationExpression::RelationType::Greater);
                if (n1 != nullptr && n2 != nullptr) {
                    order->addRelationNodes(n1, n2);
                } else if (n1 != nullptr) {
                    order->addBetween(val2, n1, order->getBottom());
                } else if (n2 != nullptr) {
                    order->addBetween(val1, order->getTop(), n2);
                } else {
                    order->add(val1);
                    order->addBetween(val2, order->getNode(val1), order->getBottom());
                }
            }
        }


        template <typename ValueType, typename ConstantType>
        std::tuple<std::shared_ptr<Order>, uint_fast64_t, uint_fast64_t> OrderExtender<ValueType, ConstantType>::extendOrder(std::shared_ptr<Order> order, storm::storage::ParameterRegion<ValueType> region, std::shared_ptr<MonotonicityResult<VariableType>> monRes, std::shared_ptr<expressions::BinaryRelationExpression> assumption) {
            if (order == nullptr || continueExtending[order] || assumption != nullptr) {
                this->region = region;
                if (order == nullptr) {
                    order = getBottomTopOrder();
                }
                return extendOrder(order, monRes, assumption);
            } else {
                auto res = unknownStatesMap[order].first != numberOfStates ? unknownStatesMap[order] : lastUnknownStatesMap[order];
                return {order, res.first, res.second};
            }
        }

        template <typename ValueType, typename ConstantType>
        std::tuple<std::shared_ptr<Order>, uint_fast64_t, uint_fast64_t> OrderExtender<ValueType, ConstantType>::extendOrder(std::shared_ptr<Order> order, std::shared_ptr<MonotonicityResult<VariableType>> monRes, std::shared_ptr<expressions::BinaryRelationExpression> assumption) {
            if (assumption != nullptr) {
                handleAssumption(order, assumption);
            }

            std::set<uint_fast64_t> seenStates;
            auto currentStateSCC = getNextState(order, -1, seenStates);
            std::vector<uint_fast64_t> statesToHandle;
            while (currentStateSCC.first != numberOfStates) {
                assert (currentStateSCC.first < numberOfStates);
                auto currentState = currentStateSCC.first;
                auto successors = stateMap[currentState];
                std::pair<uint_fast64_t, uint_fast64_t> result =  {numberOfStates, numberOfStates};
                if (successors.size() == 1) {
                    assert (order->contains(successors[0]));
                    handleOneSuccessor(order, currentState, successors[0]);
                } else if (!successors.empty()) {
                    if (order->isOnlyBottomTopOrder()) {
                        order->add(currentState);
                    }
                    if (usePLAOnce || (usePLA.find(order) != usePLA.end() && usePLA[order])) {
                        result = extendByMinMax(order, currentState, successors, assumption != nullptr);
                        if (result.first != numberOfStates) {
                            if (currentStateSCC.second == -1) {
                                result = extendStateToHandle(order, currentState, successors, assumption != nullptr, result.first == currentState);
                            } else {
                                result = extendNormal(order, currentState, currentStateSCC.second, successors, assumption != nullptr, result.first == currentState);
                            }
                        }
                    } else {
                        if (currentStateSCC.second == -1) {
                            result = extendStateToHandle(order, currentState, successors, assumption != nullptr, false);
                        } else {
                            result = extendNormal(order, currentState, currentStateSCC.second, successors, assumption != nullptr, false);
                        }
                    }

                }

                if (result.first == numberOfStates) {
                    // We did extend the order
                    assert (result.second == numberOfStates);
                    assert (order->sortStates(&successors).size() == successors.size());
                    assert (order->contains(currentState) && order->getNode(currentState) != nullptr);
                    if (currentStateSCC.second != -1) {
                        while (!statesToHandle.empty()) {
                            order->addStateToHandle(statesToHandle.back());
                            statesToHandle.pop_back();
                        }
                    }

                    if (monRes != nullptr && currentStateSCC.second != -1) {
//                        auto succsOrdered = order->sortStates(&stateMap[currentState]);
                        for (auto param : occuringVariablesAtState[currentState]) {
//                            assert (succsOrdered[succsOrdered.size() -1] != numberOfStates);
//                            assert (succsOrdered.size() == stateMap[currentState].size());
                            checkParOnStateMonRes(currentState, order, param, monRes);
                        }
                    }
                    bool prev = currentStateSCC.second;
                    if (prev != -1) {
                        seenStates.insert(currentState);
                    }
                    assert (order->sortStates(&stateMap[currentState]).size() == stateMap[currentState].size());
                    // Get the next state
                    currentStateSCC = getNextState(order, currentStateSCC.second, seenStates);
                } else {
                    assert (result.first < numberOfStates);
                    assert (result.second < numberOfStates);
                    // Try to add states based on min/max and assumptions
                    if (currentStateSCC.second < numberOfStates && extendByAssumption(order, currentState, result.first, result.second)) {
                        continue;
                    }
                    // We couldn't extend the order
                    if (nonParametericStates.find(currentState) != nonParametericStates.end()) {
                        if (!order->contains(currentState)) {
                            // State is not parametric, so we hope that just adding it between =) and =( will help us
                            order->add(currentState);
                        }
                        seenStates.insert(currentState);
                        currentStateSCC = getNextState(order, currentStateSCC.second, seenStates);
                    } else {
                        if (currentStateSCC.second == -1) {
                            // The state was based on statesToHandle, so it is not bad if we cannot continue with this.
                            statesToHandle.push_back(currentState);
                            currentStateSCC = getNextState(order, currentStateSCC.second, seenStates);
                        } else {
                            // The state was based on the topological sorting, so we need to return, but first add all the not yet handled states to the order
                            while (!statesToHandle.empty()) {
                                order->addStateToHandle(statesToHandle.back());
                                statesToHandle.pop_back();
                            }
                            return {order, result.first, result.second};
                        }
                    }
                }
                // End while loop
            }
            order->setDoneBuilding();
            if (monRes != nullptr) {
                monRes->setDone();
            }
            return std::make_tuple(order, numberOfStates, numberOfStates);
        }

        template<typename ValueType, typename ConstantType>
        std::pair<uint_fast64_t, uint_fast64_t>
        OrderExtender<ValueType, ConstantType>::extendStateToHandle(std::shared_ptr<Order> order, uint_fast64_t currentState,
                                                                    const vector<uint_fast64_t> &successors, bool allowMerge,  bool onlyBackwards) const {
            std::pair<uint_fast64_t, uint_fast64_t> result;
            if (!onlyBackwards && order->contains(currentState)) {
                // Try to extend the order for this scc
                result = extendByForwardReasoning(order, currentState, successors, allowMerge);
                while (result.first != numberOfStates) {
                    auto res2 = extendByBackwardReasoning(order, currentState, successors, allowMerge);
                    if (res2.first != numberOfStates) {
                        if (usePLA.find(order) != usePLA.end() && usePLA.at(order)) {
                            auto check = addStatesBasedOnMinMax(order, res2.first, res2.second);
                            if (check != Order::UNKNOWN) {
                                result = extendByForwardReasoning(order, currentState, successors, allowMerge);
                            } else {
                                break;
                            }
                        } else {
                            break;
                        }
                    } else {
                        result = res2;
                    }
                }
            } else {
                result = extendByBackwardReasoning(order, currentState, successors, allowMerge);
            }
            return result;
        }

        template<typename ValueType, typename ConstantType>
        std::pair<uint_fast64_t, uint_fast64_t>
        OrderExtender<ValueType, ConstantType>::extendNormal(std::shared_ptr<Order> order, uint_fast64_t currentState, uint_fast64_t currentSCC,
                                                                    const vector<uint_fast64_t> &successors, bool allowMerge,  bool onlyBackwards) const {
            // If it is cyclic, we first do forward reasoning, when this didn't work we do backward reasoning
            std::pair<uint_fast64_t, uint_fast64_t> result;
            if (!onlyBackwards && !order->getSCC(currentSCC).isTrivial()) {
                if (order->contains(currentState)) {
                    // Try to extend the order for this scc
                    result = extendByForwardReasoning(order, currentState, successors, allowMerge);
                    if (result.first != numberOfStates ) {
                        auto backwardResult = extendByBackwardReasoning(order, currentState, successors, allowMerge);
                        if (backwardResult.first == numberOfStates) {
                            result = std::move(backwardResult);
                        }
                    }
                } else {
                    result = extendByBackwardReasoning(order, currentState, successors, allowMerge);
                }
            } else {
                // Do backward reasoning, all successor states must be in the order
                for (auto& state : successors) {
                    assert (order->contains(state));
                }
                result = extendByBackwardReasoning(order, currentState, successors, allowMerge);
            }
            return result;
        }

        template<typename ValueType, typename ConstantType>
        void OrderExtender<ValueType, ConstantType>::handleOneSuccessor(std::shared_ptr<Order> order, uint_fast64_t currentState, uint_fast64_t successor) {
            assert (order->contains(successor));
            if (currentState != successor) {
                if (order->contains(currentState)) {
                    order->merge(currentState, successor);
                } else {
                    order->addToNode(currentState, order->getNode(successor));
                }
            }
        }

        template<typename ValueType, typename ConstantType>
        std::pair<uint_fast64_t, uint_fast64_t>
        OrderExtender<ValueType, ConstantType>::extendByMinMax(std::shared_ptr<Order> order, uint_fast64_t currentState,
                                                               const vector<uint_fast64_t> &successors,
                                                               bool allowMerge) const {
            bool addedCurrent = false;
            for (auto i1 = 0; i1 < successors.size(); i1++) {
                auto state1 = successors.at(i1);
                auto state2 = currentState;
                if (order->compareFast(state1, state2) == Order::UNKNOWN) {
                    if (addStatesBasedOnMinMax(order, state1, state2) != Order::UNKNOWN) {
                        addedCurrent = true;
                    }
                } else {
                    addedCurrent = true;
                }
                for (auto i2 = i1 + 1; i2 < successors.size(); ++i2) {
                    state2 = successors.at(i2);
                    if (order->compareFast(state1, state2) == Order::UNKNOWN) {
                        if (addStatesBasedOnMinMax(order, state1, state2) == Order::UNKNOWN) {
                            return {state1, state2};
                        }
                    }
                }
            }

            if (!addedCurrent) {
                return {currentState, currentState};
            }
            return {numberOfStates, numberOfStates};
        }

        template <typename ValueType, typename ConstantType>
        std::pair<uint_fast64_t, uint_fast64_t> OrderExtender<ValueType, ConstantType>::extendByBackwardReasoning(std::shared_ptr<Order> order, uint_fast64_t currentState, std::vector<uint_fast64_t> const& successors, bool allowMerge) const{
            assert (!order->isOnlyBottomTopOrder());
            assert (successors.size() > 1);

            // temp.first = pair of unordered states, if this is numberOfStates all successor states could be sorted, so temp.second is fully sorted and contains all successors.
            auto temp = order->sortStatesUnorderedPair(&successors);
            if (temp.first.first != numberOfStates) {
                assert (temp.first.first < numberOfStates);
                assert (temp.first.second < numberOfStates);
                return temp.first;
            }
            auto sortedSuccs = temp.second;
            if (order->compare(sortedSuccs[0], sortedSuccs[sortedSuccs.size() - 1]) == Order::SAME) {
                if (!order->contains(currentState)) {
                    order->addToNode(currentState, order->getNode(sortedSuccs[0]));
                } else {
                    order->merge(currentState, sortedSuccs[0]);
                }
            } else {
                if (!order->contains(sortedSuccs[0])) {
                    assert (order->isBottomState(sortedSuccs[sortedSuccs.size() - 1]));
                    assert (sortedSuccs.size() == 2);
                    order->addAbove(sortedSuccs[0], order->getBottom());
                }
                if (!order->contains(sortedSuccs[sortedSuccs.size() - 1])) {
                    assert (order->isTopState(sortedSuccs[0]));
                    assert (sortedSuccs.size() == 2);
                    order->addBelow(sortedSuccs[sortedSuccs.size() - 1], order->getTop());
                }
                // sortedSuccs[0] is highest
                if (!order->contains(currentState)) {
                    order->addBelow(currentState, order->getNode(sortedSuccs[0]));
                } else {
                    order->addRelation(sortedSuccs[0], currentState, allowMerge);
                }
                order->addRelation(currentState, sortedSuccs[sortedSuccs.size() - 1], allowMerge);

            }
            assert (order->contains(currentState)
                    && order->compare(order->getNode(currentState), order->getBottom()) == Order::ABOVE
                    && order->compare(order->getNode(currentState), order->getTop()) == Order::BELOW);
            return {numberOfStates, numberOfStates};
        }

        template <typename ValueType, typename ConstantType>
        std::pair<uint_fast64_t, uint_fast64_t> OrderExtender<ValueType, ConstantType>::extendByForwardReasoning(std::shared_ptr<Order> order, uint_fast64_t currentState, std::vector<uint_fast64_t> const& successors, bool allowMerge) const {
            assert (successors.size() > 1);
            assert (order->contains(currentState));
            auto temp = order->sortStatesForForward(currentState, successors);
            if (temp.first.first == numberOfStates) {
                assert (temp.second.size() == successors.size() + 1);
                // all could be sorted, no need to do anything
            } else if (temp.first.second == numberOfStates) {
                if (!order->contains(temp.first.first)) {
                    order->add(temp.first.first);
                }

                if (temp.second[0] == currentState) {
                    order->addRelation(temp.first.first, temp.second[0], allowMerge);
                    assert ((order->compare(temp.first.first, temp.second[0]) == Order::ABOVE) || (allowMerge && (order->compare(temp.first.first, temp.second[temp.second.size() - 1]) == Order::SAME)));
                    order->addRelation(temp.first.first, temp.second[temp.second.size() - 1], allowMerge);
                    assert ((order->compare(temp.first.first, temp.second[temp.second.size() - 1]) == Order::ABOVE) || (allowMerge && (order->compare(temp.first.first, temp.second[temp.second.size() - 1]) == Order::SAME)));
                    order->addStateToHandle(temp.first.first);
                } else if (temp.second[temp.second.size() - 1] == currentState) {
                    order->addRelation(temp.second[0], temp.first.first, allowMerge);
                    assert ((order->compare(temp.first.first, temp.second[0]) == Order::BELOW) || (allowMerge && (order->compare(temp.first.first, temp.second[temp.second.size() - 1]) == Order::SAME)));
                    order->addRelation(temp.second[temp.second.size() - 1], temp.first.first, allowMerge);
                    assert ((order->compare(temp.first.first, temp.second[temp.second.size() - 1]) == Order::BELOW) || (allowMerge && (order->compare(temp.first.first, temp.second[temp.second.size() - 1]) == Order::SAME)));
                    order->addStateToHandle(temp.first.first);
                } else {
                    bool continueSearch = true;
                    for (auto& entry :  matrix.getRow(currentState)) {
                        if (entry.getColumn() == temp.first.first) {
                            if (entry.getValue().isConstant()) {
                                continueSearch = false;
                            }
                        }
                    }
                    if (continueSearch) {
                        for (auto &i : temp.second) {
                            if (order->compare(i, temp.first.first) == Order::UNKNOWN) {
                                return {i, temp.first.first};
                            }
                        }
                    }
                }
            } else {
                return {temp.first.first, temp.first.second};
            }
            return {numberOfStates, numberOfStates};
        }

        template<typename ValueType, typename ConstantType>
        bool OrderExtender<ValueType, ConstantType>::extendByAssumption(std::shared_ptr<Order> order, uint_fast64_t currentState, uint_fast64_t stateSucc1, uint_fast64_t stateSucc2) {
            if (usePLAOnce) {
                    auto assumptions = usePLAOnce.get() ? assumptionMaker->createAndCheckAssumptions(stateSucc1, stateSucc2,  order, region, minValuesOnce.get(), maxValuesOnce.get()) : assumptionMaker->createAndCheckAssumptions(stateSucc1, stateSucc2, order, region);
                    if (assumptions.size() == 1 && assumptions.begin()->second == AssumptionStatus::VALID) {
                        handleAssumption(order, assumptions.begin()->first);
                        // Assumptions worked, we continue
                        return true;
                    }
            } else {
                bool usePLANow = usePLA.find(order) != usePLA.end() && usePLA[order];
                    auto assumptions = usePLANow ? assumptionMaker->createAndCheckAssumptions(stateSucc1, stateSucc2,  order, region, minValues[order], maxValues[order]) : assumptionMaker->createAndCheckAssumptions(stateSucc1, stateSucc2, order, region);
                    if (assumptions.size() == 1 && assumptions.begin()->second == AssumptionStatus::VALID) {
                        handleAssumption(order, assumptions.begin()->first);
                        // Assumptions worked, we continue
                        return true;
                    }
            }
            return false;
        }

        template <typename ValueType, typename ConstantType>
        Order::NodeComparison OrderExtender<ValueType, ConstantType>::addStatesBasedOnMinMax(std::shared_ptr<Order> order, uint_fast64_t state1, uint_fast64_t state2) const {
            assert (order->compare(state1, state2) == Order::UNKNOWN);
            std::vector<ConstantType> mins, maxs;
            if (usePLAOnce) {
                assert (usePLAOnce.get());
                mins = minValuesOnce.get();
                maxs = maxValuesOnce.get();
            } else {
                assert (minValues.find(order) != minValues.end());
                assert (maxValues.find(order) != maxValues.end());
                mins = minValues.at(order);
                maxs = maxValues.at(order);

            }
            if (mins[state1] == maxs[state1]
                && mins[state2] == maxs[state2]
                   && mins[state1] == mins[state2]) {
                if (order->contains(state1)) {
                    if (order->contains(state2)) {
                        order->merge(state1, state2);
                    } else {
                        order->addToNode(state2, order->getNode(state1));
                    }
                }
                return Order::SAME;
            } else if (mins[state1] > maxs[state2]) {
                // state 1 will always be larger than state2
                if (!order->contains(state1)) {
                    order->add(state1);
                }
                if (!order->contains(state2)) {
                    order->add(state2);
                }
                assert (order->compare(state1, state2) != Order::BELOW);
                assert (order->compare(state1, state2) != Order::SAME);
                order->addRelation(state1, state2);

                return Order::ABOVE;
            } else if (mins[state2] > maxs[state1]) {
                // state2 will always be larger than state1
                if (!order->contains(state1)) {
                    order->add(state1);
                }
                if (!order->contains(state2)) {
                    order->add(state2);
                }
                assert (order->compare(state2, state1) != Order::BELOW);
                assert (order->compare(state2, state1) != Order::SAME);
                order->addRelation(state2, state1);

                return Order::BELOW;
            } else {
                // Couldn't add relation between state1 and state 2 based on min/max values;
                return Order::UNKNOWN;
            }
        }

        template <typename ValueType, typename ConstantType>
        void OrderExtender<ValueType, ConstantType>::initializeMinMaxValues() {
            if (model != nullptr) {
                this->usePLAOnce = true;
                // Use parameter lifting modelchecker to get initial min/max values for order creation
                modelchecker::SparseDtmcParameterLiftingModelChecker<models::sparse::Dtmc<ValueType>, ConstantType> plaModelChecker;
                std::unique_ptr<modelchecker::CheckResult> checkResult;
                auto env = Environment();
                const modelchecker::CheckTask<logic::Formula, ValueType> checkTask = modelchecker::CheckTask<logic::Formula, ValueType>(*formula);
                STORM_LOG_THROW(plaModelChecker.canHandle(model, checkTask), exceptions::NotSupportedException, "Cannot handle this formula");
                plaModelChecker.specify(env, model, checkTask, false, false);

                modelchecker::ExplicitQuantitativeCheckResult<ConstantType> minCheck = plaModelChecker.check(env, region, solver::OptimizationDirection::Minimize)->template asExplicitQuantitativeCheckResult<ConstantType>();
                modelchecker::ExplicitQuantitativeCheckResult<ConstantType> maxCheck = plaModelChecker.check(env, region, solver::OptimizationDirection::Maximize)->template asExplicitQuantitativeCheckResult<ConstantType>();

                minValuesOnce = minCheck.getValueVector();
                maxValuesOnce = maxCheck.getValueVector();
                assert (minValuesOnce->size() == numberOfStates);
                assert (maxValuesOnce->size() == numberOfStates);
            }
        }

        template <typename ValueType, typename ConstantType>
        void OrderExtender<ValueType, ConstantType>::setMinMaxValues(std::shared_ptr<Order> order, std::vector<ConstantType>& minValues, std::vector<ConstantType>& maxValues) {
            assert (minValues.size() == numberOfStates);
            assert (maxValues.size() == numberOfStates);
            this->minValues[order] = minValues;//minCheck->asExplicitQuantitativeCheckResult<ConstantType>().getValueVector();
            this->maxValues[order] = maxValues;//maxCheck->asExplicitQuantitativeCheckResult<ConstantType>().getValueVector();
            usePLA[order] = true;
            if (unknownStatesMap.find(order) != unknownStatesMap.end()) {
                if (unknownStatesMap[order].first != numberOfStates) {
                    continueExtending[order] = minValues[unknownStatesMap[order].first] >= maxValues[unknownStatesMap[order].second] ||  minValues[unknownStatesMap[order].second] >= maxValues[unknownStatesMap[order].first];
                } else if (lastUnknownStatesMap.find(order) != lastUnknownStatesMap.end() && lastUnknownStatesMap[order].first != numberOfStates) {
                    continueExtending[order] = minValues[lastUnknownStatesMap[order].first] >= maxValues[lastUnknownStatesMap[order].second] ||  minValues[lastUnknownStatesMap[order].second] >= maxValues[lastUnknownStatesMap[order].first];
                } else {
                    continueExtending[order] = true;
                }
            } else {
                continueExtending[order] = true;
            }
        }

        template <typename ValueType, typename ConstantType>
        void OrderExtender<ValueType, ConstantType>::setMinValues(std::shared_ptr<Order> order, std::vector<ConstantType>& minValues) {
            assert (minValues.size() == numberOfStates);
            this->minValues[order] = minValues;
            auto maxValues = this->maxValues[order];
            usePLA[order] = this->maxValues.find(order) != this->maxValues.end();
            if (maxValues.size() == 0) {
                continueExtending[order] = false;
            } else if (unknownStatesMap.find(order) != unknownStatesMap.end()) {
                if (unknownStatesMap[order].first != numberOfStates) {
                    continueExtending[order] = minValues[unknownStatesMap[order].first] >= maxValues[unknownStatesMap[order].second] ||  minValues[unknownStatesMap[order].second] >= maxValues[unknownStatesMap[order].first];
                } else if (lastUnknownStatesMap.find(order) != lastUnknownStatesMap.end() && lastUnknownStatesMap[order].first != numberOfStates) {
                    continueExtending[order] = minValues[lastUnknownStatesMap[order].first] >= maxValues[lastUnknownStatesMap[order].second] ||  minValues[lastUnknownStatesMap[order].second] >= maxValues[lastUnknownStatesMap[order].first];
                } else {
                    continueExtending[order] = true;
                }
            } else {
                continueExtending[order] = true;
            }
        }

        template <typename ValueType, typename ConstantType>
        void OrderExtender<ValueType, ConstantType>::setMaxValues(std::shared_ptr<Order> order, std::vector<ConstantType>& maxValues) {
            assert (maxValues.size() == numberOfStates);
            this->maxValues[order] = maxValues;//maxCheck->asExplicitQuantitativeCheckResult<ConstantType>().getValueVector();
            usePLA[order] = this->minValues.find(order) != this->minValues.end();
            auto minValues = this->minValues[order];
            if (minValues.size() == 0) {
                continueExtending[order] = false;
            } else  if (unknownStatesMap.find(order) != unknownStatesMap.end()) {
                if (unknownStatesMap[order].first != numberOfStates) {
                    continueExtending[order] =
                            minValues[unknownStatesMap[order].first] >= maxValues[unknownStatesMap[order].second] ||
                            minValues[unknownStatesMap[order].second] >= maxValues[unknownStatesMap[order].first];
                } else if (lastUnknownStatesMap.find(order) != lastUnknownStatesMap.end() &&
                           lastUnknownStatesMap[order].first != numberOfStates) {
                    continueExtending[order] = minValues[lastUnknownStatesMap[order].first] >=
                                               maxValues[lastUnknownStatesMap[order].second] ||
                                               minValues[lastUnknownStatesMap[order].second] >=
                                               maxValues[lastUnknownStatesMap[order].first];
                } else {
                    continueExtending[order] = true;
                }
            } else {
                continueExtending[order] = true;
            }
        }

        template <typename ValueType, typename ConstantType>
        ValueType OrderExtender<ValueType, ConstantType>::getDerivative(ValueType function, typename OrderExtender<ValueType, ConstantType>::VariableType var) {
            if (function.isConstant()) {
                return utility::zero<ValueType>();
            }
            if ((derivatives[function]).find(var) == (derivatives[function]).end()) {
                (derivatives[function])[var] = function.derivative(var);
            }
            return (derivatives[function])[var];
        }

        template <typename ValueType, typename ConstantType>
        typename OrderExtender<ValueType, ConstantType>::Monotonicity OrderExtender<ValueType, ConstantType>::checkTransitionMonRes(ValueType function, typename OrderExtender<ValueType, ConstantType>::VariableType param) {
                std::pair<bool, bool> res = MonotonicityHelper<ValueType, ConstantType>::checkDerivative(getDerivative(function, param), region);
                if (res.first && !res.second) {
                    return Monotonicity::Incr;
                } else if (!res.first && res.second) {
                    return Monotonicity::Decr;
                } else if (res.first && res.second) {
                    return Monotonicity::Constant;
                } else {
                    return Monotonicity::Not;
                }
        }

        template <typename ValueType, typename ConstantType>
        void OrderExtender<ValueType, ConstantType>::checkParOnStateMonRes(uint_fast64_t s, std::shared_ptr<Order> order, typename OrderExtender<ValueType, ConstantType>::VariableType param, std::shared_ptr<MonotonicityResult<VariableType>> monResult) {
            auto mon = monotonicityChecker.checkLocalMonotonicity(order, s, param, region);
            monResult->updateMonotonicityResult(param, mon);
        }


        template <typename ValueType, typename ConstantType>
        void OrderExtender<ValueType, ConstantType>::checkParOnStateMonRes(uint_fast64_t s, const std::vector<uint_fast64_t>& succ, typename OrderExtender<ValueType, ConstantType>::VariableType param, std::shared_ptr<MonotonicityResult<VariableType>> monResult) {
            uint_fast64_t succSize = succ.size();
            if (succSize == 2) {
                // In this case we can ignore the last entry, as this will have a probability of 1 - the other
                succSize = 1;
            }

            // Create + fill Vector containing the Monotonicity of the transitions to the succs
            std::vector<Monotonicity> succsMon(succSize);
            auto row = matrix.getRow(s);
            for (auto entry : row) {
                auto succState = entry.getColumn();
                auto function = entry.getValue();
                auto it = std::find(succ.begin(), succ.end(), succState);
                auto index = std::distance(succ.begin(), it);
                if (index != succSize) {
                    succsMon[index] = checkTransitionMonRes(function, param);
                }
            }

            uint_fast64_t index = 0;
            Monotonicity monCandidate = Monotonicity::Constant;
            Monotonicity temp;

            //go to first inc / dec
            while (index < succSize && monCandidate == Monotonicity::Constant) {
                temp = succsMon[index];
                if (temp != Monotonicity::Not) {
                    monCandidate = temp;
                } else {
                    monResult->updateMonotonicityResult(param, Monotonicity::Unknown);
                    return;
                }
                index++;
            }
            if (index == succSize) {
                monResult->updateMonotonicityResult(param, monCandidate);
                return;
            }

            //go to first non-inc / non-dec
            while (index < succSize) {
                temp = succsMon[index];
                if (temp == Monotonicity::Not) {
                    monResult->updateMonotonicityResult(param, Monotonicity::Unknown);
                    return;
                } else if (temp == Monotonicity::Constant || temp == monCandidate) {
                    index++;
                } else {
                    monCandidate = temp;
                    break;
                }
            }

            //check if it doesn't change until the end of vector
            while (index < succSize) {
                temp = succsMon[index];
                if (temp == Monotonicity::Constant || temp == monCandidate) {
                    index++;
                } else {
                    monResult->updateMonotonicityResult(param, Monotonicity::Unknown);
                    return;
                }
            }

            if (monCandidate == Monotonicity::Incr) {
                monResult->updateMonotonicityResult(param, Monotonicity::Decr);
            } else {
                monResult->updateMonotonicityResult(param, Monotonicity::Incr);
            }
        }

        template<typename ValueType, typename ConstantType>
        void OrderExtender<ValueType, ConstantType>::setUnknownStates(std::shared_ptr<Order> order, uint_fast64_t state1, uint_fast64_t state2) {
            assert (state1 != numberOfStates && state2 != numberOfStates);
            if (unknownStatesMap.find(order) == unknownStatesMap.end()) {
                unknownStatesMap.insert({order, {state1, state2}});
            } else if (lastUnknownStatesMap.find(order) == lastUnknownStatesMap.end()) {
                lastUnknownStatesMap.insert({order, {state1, state2}});
                unknownStatesMap[order] = {state1, state2};
            } else {
                if (unknownStatesMap[order].first == numberOfStates && ((lastUnknownStatesMap[order].first == state1 && lastUnknownStatesMap[order].second == state2) ||
                        (lastUnknownStatesMap[order].first == state2 && lastUnknownStatesMap[order].second == state1))) {
                    unknownStatesMap[order] = {numberOfStates, numberOfStates};
                } else if ((unknownStatesMap[order].first == state1 && unknownStatesMap[order].second == state2) ||
                           (unknownStatesMap[order].first == state2 && unknownStatesMap[order].second == state1)) {
                    unknownStatesMap[order] = {numberOfStates, numberOfStates};
                } else {
                    lastUnknownStatesMap[order] = unknownStatesMap[order];
                    unknownStatesMap[order] = {state1, state2};
                }
            }
        }

        template<typename ValueType, typename ConstantType>
        std::pair<uint_fast64_t, uint_fast64_t> OrderExtender<ValueType, ConstantType>::getUnknownStates(std::shared_ptr<Order> order) const {
            if (unknownStatesMap.find(order) != unknownStatesMap.end()) {
                return unknownStatesMap.at(order);
            }
            return {numberOfStates, numberOfStates};
        }

        template<typename ValueType, typename ConstantType>
        void OrderExtender<ValueType, ConstantType>::setUnknownStates(std::shared_ptr<Order> orderOriginal, std::shared_ptr<Order> orderCopy) {
            assert (unknownStatesMap.find(orderCopy) == unknownStatesMap.end());
            assert (lastUnknownStatesMap.find(orderCopy) == lastUnknownStatesMap.end());
            unknownStatesMap.insert({orderCopy,{unknownStatesMap[orderOriginal].first, unknownStatesMap[orderOriginal].second}});
            lastUnknownStatesMap.insert({orderCopy,{lastUnknownStatesMap[orderOriginal].first, lastUnknownStatesMap[orderOriginal].second}});
        }

        template<typename ValueType, typename ConstantType>
        void OrderExtender<ValueType, ConstantType>::copyMinMax(std::shared_ptr<Order> orderOriginal,
                                                                std::shared_ptr<Order> orderCopy) {
            usePLA[orderCopy] = usePLA[orderOriginal];
            if (usePLA[orderCopy]) {
                minValues[orderCopy] = minValues[orderOriginal];
                assert (maxValues.find(orderOriginal) != maxValues.end());
                maxValues[orderCopy] = maxValues[orderOriginal];
            }
            continueExtending[orderCopy] = continueExtending[orderOriginal];
        }

        template<typename ValueType, typename ConstantType>
        uint_fast64_t OrderExtender<ValueType, ConstantType>::getNextStateSCC(storage::StronglyConnectedComponent& scc, std::set<uint_fast64_t> const seenStates) {
            if (scc.size() == 1 && seenStates.size() == 0) {
                return *scc.begin();
            } else if (scc.size() == 1) {
                return numberOfStates;
            }
            for (auto state : statesSorted) {
                if (scc.containsState(state) && seenStates.find(state) == seenStates.end()) {
                   return state;
                }
            }
            return numberOfStates;
        }

        template<typename ValueType, typename ConstantType>
        std::pair<uint_fast64_t, uint_fast64_t>
        OrderExtender<ValueType, ConstantType>::getNextState(std::shared_ptr<Order> order, uint_fast64_t currentSCCNumber, std::set<uint_fast64_t>& seenStates, bool trick) {
            if (currentSCCNumber == -1) {
                currentSCCNumber = order->getNextSCCNumber(currentSCCNumber);
            }

            if (cyclic && order->existsStateToHandle()) {
                return {order->getStateToHandle(), -1};
            }


            storage::StronglyConnectedComponent& scc = order->getSCC(currentSCCNumber);
            if (scc.size() == 0) {
                return {numberOfStates, numberOfStates};
            } else {
                auto nextState = getNextStateSCC(scc, seenStates);
                if (nextState == numberOfStates) {
                    if (trick) {
                        currentSCCNumber = order->getNextSCCNumber(currentSCCNumber);
                        nextState = getNextStateSCC(order->getSCC(currentSCCNumber), seenStates);
                        if (nextState == numberOfStates) {
                            return {numberOfStates, numberOfStates};
                        } else {
                            assert (order->getSCC(currentSCCNumber).containsState(nextState));
                            return {nextState, currentSCCNumber};
                        }
                    } else {
                        order->setAddedSCC(currentSCCNumber);
                        currentSCCNumber = order->getNextSCCNumber(currentSCCNumber);
                        seenStates.clear();
                        if (order->getSCC(currentSCCNumber).size() == 0) {
                            return {numberOfStates, numberOfStates};
                        } else {
                            nextState = getNextStateSCC(order->getSCC(currentSCCNumber), seenStates);
                            assert (order->getSCC(currentSCCNumber).containsState(nextState));
                            return {nextState, currentSCCNumber};
                        }
                    }
                } else {
                    assert (order->getSCC(currentSCCNumber).containsState(nextState));
                    return {nextState, currentSCCNumber};
                }
            }
        }

        template class OrderExtender<RationalFunction, double>;
        template class OrderExtender<RationalFunction, RationalNumber>;
    }
}
