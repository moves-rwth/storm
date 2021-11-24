#include <storage/StronglyConnectedComponentDecomposition.h>
#include "storm-pars/analysis/ReachabilityOrderExtender.h"


namespace storm {
    namespace analysis {

        template<typename ValueType, typename ConstantType>
        ReachabilityOrderExtender<ValueType, ConstantType>::ReachabilityOrderExtender(std::shared_ptr<models::sparse::Model<ValueType>> model, std::shared_ptr<logic::Formula const> formula) : OrderExtender<ValueType, ConstantType>(model, formula) {
            this->assumptionMaker = new analysis::AssumptionMaker<ValueType, ConstantType>(this->matrix);
        }

        template<typename ValueType, typename ConstantType>
        ReachabilityOrderExtender<ValueType, ConstantType>::ReachabilityOrderExtender(storm::storage::BitVector* topStates,  storm::storage::BitVector* bottomStates, storm::storage::SparseMatrix<ValueType> matrix) : OrderExtender<ValueType, ConstantType>(topStates, bottomStates, matrix) {
            this->assumptionMaker = new analysis::AssumptionMaker<ValueType, ConstantType>(this->matrix);
        }

        template <typename ValueType, typename ConstantType>
        std::pair<uint_fast64_t, uint_fast64_t> ReachabilityOrderExtender<ValueType, ConstantType>::extendByBackwardReasoning(std::shared_ptr<Order> order, uint_fast64_t currentState, std::vector<uint_fast64_t> const& successors, bool allowMerge) {
            assert (!order->isOnlyInitialOrder());
            assert (successors.size() > 1);

            bool pla = (this->usePLA.find(order) != this->usePLA.end() && this->usePLA.at(order));
            std::vector<uint_fast64_t> sortedSuccs;

            auto temp = this->sortStatesUnorderedPair(&successors, order);
            if (temp.first.first != this->numberOfStates) {
                return temp.first;
            }
            sortedSuccs = std::move(temp.second);


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
                    order->addBetween(currentState, sortedSuccs[0], sortedSuccs[sortedSuccs.size()-1]);
                } else {
                    order->addRelation(sortedSuccs[0], currentState, allowMerge);
                    order->addRelation(currentState, sortedSuccs[sortedSuccs.size() - 1], allowMerge);
                }

            }
            assert (order->contains(currentState) && order->compare(order->getNode(currentState), order->getBottom()) == Order::ABOVE && order->compare(order->getNode(currentState), order->getTop()) == Order::BELOW);
            return {this->numberOfStates, this->numberOfStates};
        }

        template <typename ValueType, typename ConstantType>
        std::pair<uint_fast64_t, uint_fast64_t> ReachabilityOrderExtender<ValueType, ConstantType>::extendByForwardReasoning(std::shared_ptr<Order> order, uint_fast64_t currentState, std::vector<uint_fast64_t> const& successors, bool allowMerge)  {
            assert (successors.size() > 1);
            assert (order->contains(currentState));
            assert (this->cyclic);

            std::vector<uint_fast64_t> statesSorted;
            statesSorted.push_back(currentState);
            bool pla = (this->usePLA.find(order) != this->usePLA.end() && this->usePLA.at(order));
            // Go over all states
            bool oneUnknown = false;
            bool unknown = false;
            uint_fast64_t s1 = this->numberOfStates;
            uint_fast64_t s2 = this->numberOfStates;
            for (auto& state : successors) {
                unknown = false;
                bool added = false;
                for (auto itr = statesSorted.begin(); itr != statesSorted.end(); ++itr) {
                    auto compareRes = order->compareFast(state, (*itr));
                    if (pla && compareRes == Order::NodeComparison::UNKNOWN) {
                        compareRes = this->addStatesBasedOnMinMax(order, state, (*itr));
                    }
                    if (compareRes == Order::NodeComparison::UNKNOWN) {
                        compareRes = order->compare(state, *itr);
                    }
                    if (compareRes == Order::NodeComparison::ABOVE || compareRes == Order::NodeComparison::SAME) {
                        if (!order->contains(state) && compareRes == Order::NodeComparison::ABOVE) {
                            order->add(state);
                            order->addStateToHandle(state);
                        }
                        added = true;
                        // insert at current pointer (while keeping other values)
                        statesSorted.insert(itr, state);
                        break;
                    } else if (compareRes == Order::NodeComparison::UNKNOWN && !oneUnknown) {
                        // We miss state in the result.
                        s1 = state;
                        s2 = *itr;
                        oneUnknown = true;
                        added = true;
                        break;
                    } else if (compareRes == Order::NodeComparison::UNKNOWN && oneUnknown) {
                        unknown = true;
                        added = true;
                        break;
                    }
                }
                if (!(unknown && oneUnknown) && !added ) {
                    // State will be last in the list
                    statesSorted.push_back(state);
                }
                if (unknown && oneUnknown) {
                    break;
                }
            }
            if (!unknown && oneUnknown) {
                assert (statesSorted.size() == successors.size());
                s2 = this->numberOfStates;
            }

            if (s1 == this->numberOfStates) {
                assert (statesSorted.size() == successors.size() + 1);
                // all could be sorted, no need to do anything
            } else if (s2 == this->numberOfStates) {
                if (!order->contains(s1)) {
                    order->add(s1);
                }

                if (statesSorted[0] == currentState) {
                    order->addRelation(s1, statesSorted[0], allowMerge);
                    assert ((order->compare(s1, statesSorted[0]) == Order::ABOVE) || (allowMerge && (order->compare(s1, statesSorted[statesSorted.size() - 1]) == Order::SAME)));
                    order->addRelation(s1, statesSorted[statesSorted.size() - 1], allowMerge);
                    assert ((order->compare(s1, statesSorted[statesSorted.size() - 1]) == Order::ABOVE) || (allowMerge && (order->compare(s1, statesSorted[statesSorted.size() - 1]) == Order::SAME)));
                    order->addStateToHandle(s1);
                } else if (statesSorted[statesSorted.size() - 1] == currentState) {
                    order->addRelation(statesSorted[0], s1, allowMerge);
                    assert ((order->compare(s1, statesSorted[0]) == Order::BELOW) || (allowMerge && (order->compare(s1, statesSorted[statesSorted.size() - 1]) == Order::SAME)));
                    order->addRelation(statesSorted[statesSorted.size() - 1], s1, allowMerge);
                    assert ((order->compare(s1, statesSorted[statesSorted.size() - 1]) == Order::BELOW) || (allowMerge && (order->compare(s1, statesSorted[statesSorted.size() - 1]) == Order::SAME)));
                    order->addStateToHandle(s1);
                } else {
                    bool continueSearch = true;
                    for (auto& entry :  this->matrix.getRow(currentState)) {
                        if (entry.getColumn() == s1) {
                            if (entry.getValue().isConstant()) {
                                continueSearch = false;
                            }
                        }
                    }
                    if (continueSearch) {
                        for (auto& i : statesSorted) {
                            if (order->compare(i, s1) == Order::UNKNOWN) {
                                return {i, s1};
                            }
                        }
                    }
                }
            } else {
                return {s1, s2};
            }
            assert (order->contains(currentState) && order->compare(order->getNode(currentState), order->getBottom()) == Order::ABOVE && order->compare(order->getNode(currentState), order->getTop()) == Order::BELOW);
            return {this->numberOfStates, this->numberOfStates};
        }

        template<typename ValueType, typename ConstantType>
        void ReachabilityOrderExtender<ValueType, ConstantType>::handleOneSuccessor(std::shared_ptr<Order> order, uint_fast64_t currentState, uint_fast64_t successor) {
            assert (order->contains(successor));
            if (currentState != successor) {
                if (order->contains(currentState)) {
                    order->merge(currentState, successor);
                } else {
                    order->addToNode(currentState, order->getNode(successor));
                }
            }
        }

        template <typename ValueType, typename ConstantType>
        std::shared_ptr<Order> ReachabilityOrderExtender<ValueType, ConstantType>::getInitialOrder() {
            if (this->initialOrder == nullptr) {
                assert (this->model != nullptr);
                STORM_LOG_THROW(this->matrix.getRowCount() == this->matrix.getColumnCount(), exceptions::NotSupportedException,"Creating order not supported for non-square matrix");
                modelchecker::SparsePropositionalModelChecker<models::sparse::Model<ValueType>> propositionalChecker(*(this->model));
                storage::BitVector phiStates;
                storage::BitVector psiStates;
                assert (this->formula->isProbabilityOperatorFormula());
                if (this->formula->asProbabilityOperatorFormula().getSubformula().isUntilFormula()) {
                    phiStates = propositionalChecker.check(
                                                        this->formula->asProbabilityOperatorFormula().getSubformula().asUntilFormula().getLeftSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
                    psiStates = propositionalChecker.check(
                                                        this->formula->asProbabilityOperatorFormula().getSubformula().asUntilFormula().getRightSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
                } else {
                    assert (this->formula->asProbabilityOperatorFormula().getSubformula().isEventuallyFormula());
                    phiStates = storage::BitVector(this->numberOfStates, true);
                    psiStates = propositionalChecker.check(
                                                        this->formula->asProbabilityOperatorFormula().getSubformula().asEventuallyFormula().getSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();
                }
                // Get the maybeStates
                std::pair<storage::BitVector, storage::BitVector> statesWithProbability01 = utility::graph::performProb01(this->model->getBackwardTransitions(), phiStates, psiStates);
                storage::BitVector topStates = statesWithProbability01.second;
                storage::BitVector bottomStates = statesWithProbability01.first;

                STORM_LOG_THROW(topStates.begin() != topStates.end(), exceptions::NotSupportedException,"Formula yields to no 1 states");
                STORM_LOG_THROW(bottomStates.begin() != bottomStates.end(), exceptions::NotSupportedException,"Formula yields to no zero states");
                auto& matrix = this->model->getTransitionMatrix();
                std::vector<uint64_t> firstStates;

                storm::storage::BitVector subStates (topStates.size(), true);
                for (auto state : topStates) {
                    firstStates.push_back(state);
                    subStates.set(state, false);
                }
                for (auto state : bottomStates) {
                    firstStates.push_back(state);
                    subStates.set(state, false);
                }
                this->cyclic = storm::utility::graph::hasCycle(matrix, subStates);
                storm::storage::StronglyConnectedComponentDecomposition<ValueType> decomposition;
                if (this->cyclic) {
                    storm::storage::StronglyConnectedComponentDecompositionOptions options;
                    options.forceTopologicalSort();
                    decomposition = storm::storage::StronglyConnectedComponentDecomposition<ValueType>(matrix, options);
                }
                auto statesSorted = storm::utility::graph::getTopologicalSort(matrix.transpose(), firstStates);
                this->initialOrder = std::shared_ptr<Order>(new Order(&topStates, &bottomStates, this->numberOfStates, std::move(decomposition), std::move(statesSorted)));

                // Build stateMap
                for (uint_fast64_t state = 0; state < this->numberOfStates; ++state) {
                    auto const& row = matrix.getRow(state);
                    this->stateMap[state] = std::vector<uint_fast64_t>();
                    std::set<VariableType> occurringVariables;

                    for (auto& entry : matrix.getRow(state)) {

                        // ignore self-loops when there are more transitions
                        if (state != entry.getColumn() || row.getNumberOfEntries() == 1) {
                            this->stateMap[state].push_back(entry.getColumn());
                        }
                        storm::utility::parametric::gatherOccurringVariables(entry.getValue(), occurringVariables);

                    }
                    if (occurringVariables.empty()) {
                        this->nonParametricStates.insert(state);
                    }

                    for (auto& var : occurringVariables) {
                        this->occuringStatesAtVariable[var].push_back(state);
                    }
                    this->occuringVariablesAtState.push_back(std::move(occurringVariables));
                }

            }

            if (!this->minValues.empty() && !this->maxValues.empty()) {
                this->continueExtending[this->initialOrder] = true;
                this->usePLA[this->initialOrder] = true;
            } else {
                this->usePLA[this->initialOrder] = false;
            }
            return this->initialOrder;
        }

        template class ReachabilityOrderExtender<RationalFunction, double>;
        template class ReachabilityOrderExtender<RationalFunction, RationalNumber>;

    }
}