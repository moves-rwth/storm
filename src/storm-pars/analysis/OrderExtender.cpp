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
        OrderExtender<ValueType, ConstantType>::OrderExtender(std::shared_ptr<models::sparse::Model<ValueType>> model, std::shared_ptr<logic::Formula const> formula) {
            this->model = model;
            this->matrix = model->getTransitionMatrix();
            this->numberOfStates = this->model->getNumberOfStates();
            this->formula = formula;
            this->assumptionMaker = new analysis::AssumptionMaker<ValueType, ConstantType>(matrix);
        }

        template <typename ValueType, typename ConstantType>
        OrderExtender<ValueType, ConstantType>::OrderExtender(storm::storage::BitVector* topStates,  storm::storage::BitVector* bottomStates, storm::storage::SparseMatrix<ValueType> matrix) {
            this->matrix = matrix;
            this->model = nullptr;


            storm::storage::StronglyConnectedComponentDecompositionOptions options;
            options.forceTopologicalSort();

            this->numberOfStates = matrix.getColumnCount();
            std::vector<uint64_t> firstStates;

            storm::storage::BitVector subStates (topStates->size(), true);
            for (auto state : *topStates) {
                firstStates.push_back(state);
                subStates.set(state, false);
            }
            for (auto state : *bottomStates) {
                firstStates.push_back(state);
                subStates.set(state, false);
            }
            cyclic = storm::utility::graph::hasCycle(matrix, subStates);
            storm::storage::StronglyConnectedComponentDecomposition<ValueType> decomposition;
            if (cyclic) {
                decomposition = storm::storage::StronglyConnectedComponentDecomposition<ValueType>(matrix, options);
            }

            auto statesSorted = storm::utility::graph::getTopologicalSort(matrix.transpose(), firstStates);
            this->bottomTopOrder = std::shared_ptr<Order>(new Order(topStates, bottomStates, numberOfStates, std::move(decomposition), std::move(statesSorted)));

            // Build stateMap
            auto rowCount = 0;
            auto currentOption = 0;
            auto numberOfOptionsForState = 0;
            for (uint_fast64_t state = 0; state < numberOfStates; ++state) {
                stateMap[state] = std::vector<std::vector<uint_fast64_t>>();
                std::set<VariableType> occurringVariables;
                numberOfOptionsForState = matrix.getRowGroupSize(state);
                while (currentOption < numberOfOptionsForState) {
                    auto row = matrix.getRow(rowCount);
                    stateMap[state].push_back(std::vector<uint64_t>());
                    for (auto& entry : row) {
                        // ignore self-loops when there are more transitions
                        if (state != entry.getColumn() || row.getNumberOfEntries() == 1) {
                            if (!subStates[entry.getColumn()] && !bottomTopOrder->contains(state)) {
                                bottomTopOrder->add(state);
                            }
                            stateMap[state][currentOption].push_back(entry.getColumn());
                        }
                        storm::utility::parametric::gatherOccurringVariables(entry.getValue(), occurringVariables);

                    }

                    currentOption++;
                    rowCount++;
                }

                if (occurringVariables.empty()) {
                    nonParametricStates.insert(state);
                }

                for (auto& var : occurringVariables) {
                    occuringStatesAtVariable[var].push_back(state);
                }
                occuringVariablesAtState.push_back(std::move(occurringVariables));

                currentOption = 0;
            }

            this->assumptionMaker = new analysis::AssumptionMaker<ValueType, ConstantType>(matrix);
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
            auto const& val1 = std::stoul(var1.getName(), nullptr, 0);
            auto const& val2 = std::stoul(var2.getName(), nullptr, 0);

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
        Order::NodeComparison OrderExtender<ValueType, ConstantType>::addStatesBasedOnMinMax(std::shared_ptr<Order> order, uint_fast64_t state1, uint_fast64_t state2) const {
            assert (order->compareFast(state1, state2) == Order::UNKNOWN);
            assert (minValues.find(order) != minValues.end());
            std::vector<ConstantType> const& mins = minValues.at(order);
            std::vector<ConstantType> const& maxs = maxValues.at(order);
            if (mins[state1] == maxs[state1]
                && mins[state2] == maxs[state2]
                   && mins[state1] == mins[state2]) {
                if (order->contains(state1)) {
                    if (order->contains(state2)) {
                        order->merge(state1, state2);
                        assert (!order->isInvalid());
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
        void OrderExtender<ValueType, ConstantType>::initializeMinMaxValues(storage::ParameterRegion<ValueType> region) {
            if (model != nullptr) {
                // Use parameter lifting modelchecker to get initial min/max values for order creation
                modelchecker::SparseDtmcParameterLiftingModelChecker<models::sparse::Dtmc<ValueType>, ConstantType> plaModelChecker;
                std::unique_ptr<modelchecker::CheckResult> checkResult;
                auto env = Environment();
                boost::optional<modelchecker::CheckTask<logic::Formula, ValueType>> checkTask;
                if (this->formula->hasQuantitativeResult()) {
                    checkTask = modelchecker::CheckTask<logic::Formula, ValueType>(*formula);
                } else {
                    storm::logic::OperatorInformation opInfo(boost::none, boost::none);
                    auto newFormula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(
                            formula->asProbabilityOperatorFormula().getSubformula().asSharedPointer(), opInfo);
                    checkTask = modelchecker::CheckTask<logic::Formula, ValueType>(*newFormula);
                }
                STORM_LOG_THROW(plaModelChecker.canHandle(model, checkTask.get()), exceptions::NotSupportedException, "Cannot handle this formula");
                plaModelChecker.specify(env, model, checkTask.get(), false, false);

                modelchecker::ExplicitQuantitativeCheckResult<ConstantType> minCheck = plaModelChecker.check(env, region, solver::OptimizationDirection::Minimize)->template asExplicitQuantitativeCheckResult<ConstantType>();
                modelchecker::ExplicitQuantitativeCheckResult<ConstantType> maxCheck = plaModelChecker.check(env, region, solver::OptimizationDirection::Maximize)->template asExplicitQuantitativeCheckResult<ConstantType>();
                minValuesInit = minCheck.getValueVector();
                maxValuesInit = maxCheck.getValueVector();
                assert (minValuesInit->size() == numberOfStates);
                assert (maxValuesInit->size() == numberOfStates);
            }
        }

        template <typename ValueType, typename ConstantType>
        void OrderExtender<ValueType, ConstantType>::setMinMaxValues(std::shared_ptr<Order> order, std::vector<ConstantType>& minValues, std::vector<ConstantType>& maxValues) {
            assert (minValues.size() == numberOfStates);
            assert (maxValues.size() == numberOfStates);
            usePLA[order] = true;
            if (unknownStatesMap.find(order) != unknownStatesMap.end()) {
                auto& unknownStates = unknownStatesMap[order];
                if (unknownStates.first != numberOfStates) {
                    continueExtending[order] = minValues[unknownStates.first] >= maxValues[unknownStates.second] ||  minValues[unknownStates.second] >= maxValues[unknownStates.first];
                } else {
                    continueExtending[order] = true;
                }
            } else {
                continueExtending[order] = true;
            }
            this->minValues[order] = std::move(minValues);
            this->maxValues[order] = std::move(maxValues);
        }

        template <typename ValueType, typename ConstantType>
        void OrderExtender<ValueType, ConstantType>::setMinValues(std::shared_ptr<Order> order, std::vector<ConstantType>& minValues) {
            assert (minValues.size() == numberOfStates);
            auto& maxValues = this->maxValues[order];
            usePLA[order] = this->maxValues.find(order) != this->maxValues.end();
            if (maxValues.size() == 0) {
                continueExtending[order] = false;
            } else if (unknownStatesMap.find(order) != unknownStatesMap.end()) {
                auto& unknownStates = unknownStatesMap[order];
                if (unknownStates.first != numberOfStates) {
                    continueExtending[order] = minValues[unknownStates.first] >= maxValues[unknownStates.second] ||  minValues[unknownStates.second] >= maxValues[unknownStates.first];
                } else {
                    continueExtending[order] = true;
                }
            } else {
                continueExtending[order] = true;
            }
            this->minValues[order] = std::move(minValues);
        }

        template <typename ValueType, typename ConstantType>
        void OrderExtender<ValueType, ConstantType>::setMaxValues(std::shared_ptr<Order> order, std::vector<ConstantType>& maxValues) {
            assert (maxValues.size() == numberOfStates);
            usePLA[order] = this->minValues.find(order) != this->minValues.end();
            auto& minValues = this->minValues[order];
            if (minValues.size() == 0) {
                continueExtending[order] = false;
            } else  if (unknownStatesMap.find(order) != unknownStatesMap.end()) {
                auto& unknownStates = unknownStatesMap[order];
                if (unknownStates.first != numberOfStates) {
                    continueExtending[order] =
                            minValues[unknownStates.first] >= maxValues[unknownStates.second] ||
                            minValues[unknownStates.second] >= maxValues[unknownStates.first];
                } else {
                    continueExtending[order] = true;
                }
            } else {
                continueExtending[order] = true;
            }
            this->maxValues[order] = std::move(maxValues);//maxCheck->asExplicitQuantitativeCheckResult<ConstantType>().getValueVector();

        }
        template <typename ValueType, typename ConstantType>
        void OrderExtender<ValueType, ConstantType>::setMinValuesInit(std::vector<ConstantType>& minValues) {
            assert (minValues.size() == numberOfStates);
            this->minValuesInit = std::move(minValues);
        }

        template <typename ValueType, typename ConstantType>
        void OrderExtender<ValueType, ConstantType>::setMaxValuesInit(std::vector<ConstantType>& maxValues) {
            assert (maxValues.size() == numberOfStates);
            this->maxValuesInit = std::move(maxValues);//maxCheck->asExplicitQuantitativeCheckResult<ConstantType>().getValueVector();
        }

        template<typename ValueType, typename ConstantType>
        void OrderExtender<ValueType, ConstantType>::setUnknownStates(std::shared_ptr<Order> order, uint_fast64_t state1, uint_fast64_t state2) {
            assert (state1 != numberOfStates && state2 != numberOfStates);
            unknownStatesMap[order] = {state1, state2};
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
            unknownStatesMap.insert({orderCopy,{unknownStatesMap[orderOriginal].first, unknownStatesMap[orderOriginal].second}});
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
        std::pair<uint_fast64_t, bool> OrderExtender<ValueType, ConstantType>::getNextState(std::shared_ptr<Order> order, uint_fast64_t currentState, bool done) {
            if (done && currentState != numberOfStates) {
                order->setDoneState(currentState);
            }
            if (cyclic && order->existsStateToHandle()) {
                return order->getStateToHandle();
            }
            if (currentState == numberOfStates) {
                return order->getNextStateNumber();
            }
            if (currentState != numberOfStates) {
                return order->getNextStateNumber();
            }
            return {numberOfStates, true};
        }

        template<typename ValueType, typename ConstantType>
        bool OrderExtender<ValueType, ConstantType>::isHope(std::shared_ptr<Order> order, storage::ParameterRegion<ValueType> region) {
            assert (unknownStatesMap.find(order) != unknownStatesMap.end());
            assert (!order->getDoneBuilding());
            // First check if bounds helped us
            bool yesThereIsHope = continueExtending[order];
            // TODO: maybe extend this
            return yesThereIsHope;
        }

        template class OrderExtender<RationalFunction, double>;
        template class OrderExtender<RationalFunction, RationalNumber>;
    }
}
