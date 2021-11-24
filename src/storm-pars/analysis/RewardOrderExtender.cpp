#include <queue>
#include <storage/StronglyConnectedComponentDecomposition.h>
#include "storm-pars/analysis/RewardOrderExtender.h"


namespace storm {
    namespace analysis {
        template<typename ValueType, typename ConstantType>
        RewardOrderExtender<ValueType, ConstantType>::RewardOrderExtender(std::shared_ptr<models::sparse::Model<ValueType>> model, std::shared_ptr<logic::Formula const> formula) : OrderExtender<ValueType, ConstantType>(model, formula) {
            this->rewardModel = this->model->getUniqueRewardModel();
            this->assumptionMaker = new analysis::AssumptionMaker<ValueType, ConstantType>(this->matrix, std::make_shared<storm::models::sparse::StandardRewardModel<ValueType>>(this->model->getUniqueRewardModel()));
        }

        template<typename ValueType, typename ConstantType>
        RewardOrderExtender<ValueType, ConstantType>::RewardOrderExtender(storm::storage::BitVector* topStates,  storm::storage::BitVector* bottomStates, storm::storage::SparseMatrix<ValueType> matrix) : OrderExtender<ValueType, ConstantType>(topStates, bottomStates, matrix) {
            this->rewardModel = this->model->getUniqueRewardModel();
            this->assumptionMaker = new analysis::AssumptionMaker<ValueType, ConstantType>(this->matrix, std::make_shared<storm::models::sparse::StandardRewardModel<ValueType>>(this->model->getUniqueRewardModel()));
        }

        template<typename ValueType, typename ConstantType>
        std::pair<uint_fast64_t, uint_fast64_t> RewardOrderExtender<ValueType, ConstantType>::extendByBackwardReasoning(std::shared_ptr<Order> order, uint_fast64_t currentState, std::vector<uint_fast64_t> const& successors, bool allowMerge) {
            bool addedSomething = false;
            // We sort the states, this also adds states to the order if they are not yet sorted, but can be sorted based on min/max values
            auto sortedSuccStates = this->sortStatesUnorderedPair(&successors, order);
            for (auto& succ: successors) {
                STORM_LOG_ASSERT(order->contains(succ), "Expecting order to contain all successors, otherwise backwards reasoning is not possible");
                auto compare = this->compareStatesBasedOnMinMax(order, currentState, succ);
                if (compare == Order::NodeComparison::ABOVE) {
                    order->addAbove(currentState, order->getNode(succ));
                    addedSomething = true;
                } else if (compare == Order::NodeComparison::SAME) {
                    addedSomething = true;
                    order->addToNode(currentState, order->getNode(succ));
                    STORM_LOG_ASSERT (sortedSuccStates.first.first == this->numberOfStates, "Expecting all successor states to be sorted (even to be at the same node)");
                    break;
                }
            }

            if (!addedSomething && sortedSuccStates.first.first != this->numberOfStates) {
                return sortedSuccStates.first;
            } else if (!addedSomething) {
                // We are considering rewards, so our current state is always above the lowest one of all our successor states
                order->addAbove(currentState, order->getNode(sortedSuccStates.second.back()));
            }
            STORM_LOG_ASSERT (order->contains(currentState) && order->compare(order->getNode(currentState), order->getBottom()) == Order::ABOVE, "Expecting order to contain state");
            return std::make_pair(this->numberOfStates, this->numberOfStates);
        }

        template <typename ValueType, typename ConstantType>
        std::shared_ptr<Order> RewardOrderExtender<ValueType, ConstantType>::getInitialOrder() {
            if (this->initialOrder == nullptr) {
                assert (this->model != nullptr);
                STORM_LOG_THROW(this->matrix.getRowCount() == this->matrix.getColumnCount(), exceptions::NotSupportedException,"Creating order not supported for non-square matrix");
                modelchecker::SparsePropositionalModelChecker<models::sparse::Model<ValueType>> propositionalChecker(*(this->model));

                // TODO check if eventually formula is true for all initial states?
                // TODO Add UNTIL formulas
                assert (this->formula->asRewardOperatorFormula().getSubformula().isEventuallyFormula());
                storage::BitVector topStates = storage::BitVector(this->numberOfStates, false);
                storage::BitVector bottomStates = propositionalChecker.check(this->formula->asRewardOperatorFormula().getSubformula().asEventuallyFormula().getSubformula())->asExplicitQualitativeCheckResult().getTruthValuesVector();

                auto& matrix = this->model->getTransitionMatrix();
                std::vector<uint64_t> firstStates;
                storm::storage::BitVector subStates (topStates.size(), true);

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

                // Create Order
                this->initialOrder = std::shared_ptr<Order>(new Order(&topStates, &bottomStates, this->numberOfStates, std::move(decomposition), std::move(statesSorted)));

                // Build stateMap
                for (uint_fast64_t state = 0; state < this->numberOfStates; ++state) {
                    auto const& row = matrix.getRow(state);
                    this->stateMap[state] = std::vector<uint_fast64_t>();
                    std::set<VariableType> occurringVariables;

                    for (auto& entry : matrix.getRow(state)) {

                        // ignore self-loops when there are more transitions
                        if (state != entry.getColumn() || row.getNumberOfEntries() == 1) {
                            //                            if (!subStates[entry.getColumn()] && !initialOrder->contains(state)) {
                            //                                initialOrder->add(state);
                            //                            }
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
                this->usePLA[this->initialOrder] = true;
            } else {
                this->usePLA[this->initialOrder] = false;
            }
            return this->initialOrder;
        }

        template<typename ValueType, typename ConstantType>
        std::pair<uint_fast64_t, uint_fast64_t> RewardOrderExtender<ValueType, ConstantType>::extendByForwardReasoning(std::shared_ptr<Order> order, uint_fast64_t currentState, std::vector<uint_fast64_t> const& successors, bool allowMerge) {
            STORM_LOG_THROW(false, exceptions::NotImplementedException, "The function is not (yet) implemented in this class");
        }

        template<typename ValueType, typename ConstantType>
        void RewardOrderExtender<ValueType, ConstantType>::handleOneSuccessor(std::shared_ptr<Order> order, uint_fast64_t currentState, uint_fast64_t successor) {
            if (rewardModel.getStateActionReward(currentState) == ValueType(0)) {
                order->addToNode(currentState, order->getNode(successor));
            } else {
                assert(!(rewardModel.getStateActionReward(currentState) < ValueType(0)));
                order->addAbove(currentState, order->getNode(successor));
            }
        }


        template class RewardOrderExtender<RationalFunction, double>;
        template class RewardOrderExtender<RationalFunction, RationalNumber>;
    }
}