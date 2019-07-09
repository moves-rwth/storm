#include <queue>

#include "NonMarkovianChainTransformer.h"

#include "storm/storage/sparse/ModelComponents.h"
#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/utility/constants.h"
#include "storm/utility/ConstantsComparator.h"
#include "storm/utility/vector.h"
#include "storm/utility/macros.h"
#include "storm/utility/graph.h"

namespace storm {
    namespace transformer {

        template<typename ValueType, typename RewardModelType>
        std::shared_ptr<models::sparse::Model<ValueType, RewardModelType>>
        NonMarkovianChainTransformer<ValueType, RewardModelType>::eliminateNonmarkovianStates(
                std::shared_ptr<models::sparse::MarkovAutomaton<ValueType, RewardModelType>> ma,
                bool preserveLabels) {
            // TODO reward models

            STORM_LOG_WARN("State elimination is currently not label preserving!");
            STORM_LOG_WARN("Reward Models and Choice Labelings are ignored!");
            if (ma->isClosed() && ma->getMarkovianStates().full()) {
                storm::storage::sparse::ModelComponents<ValueType, RewardModelType> components(
                        ma->getTransitionMatrix(), ma->getStateLabeling(), ma->getRewardModels(), false);
                components.exitRates = ma->getExitRates();
                if (ma->hasChoiceLabeling()) {
                    components.choiceLabeling = ma->getChoiceLabeling();
                }
                if (ma->hasStateValuations()) {
                    components.stateValuations = ma->getStateValuations();
                }
                if (ma->hasChoiceOrigins()) {
                    components.choiceOrigins = ma->getChoiceOrigins();
                }
                return std::make_shared<storm::models::sparse::MarkovAutomaton<ValueType, RewardModelType>>(
                        std::move(components));
            }

            std::map<uint_fast64_t, uint_fast64_t> eliminationMapping;
            std::set<uint_fast64_t> statesToKeep;
            std::queue<uint_fast64_t> changedStates;
            std::queue<uint_fast64_t> queue;

            storm::storage::SparseMatrix<ValueType> backwards = ma->getBackwardTransitions();

            // Determine the state remapping
            // TODO Consider state labels
            for (uint_fast64_t base_state = 0; base_state < ma->getNumberOfStates(); ++base_state) {
                STORM_LOG_ASSERT(!ma->isHybridState(base_state), "Base state is hybrid.");
                if (ma->isMarkovianState(base_state)) {
                    queue.push(base_state);

                    while (!queue.empty()) {
                        auto currState = queue.front();
                        queue.pop();
                        // Get predecessors from matrix
                        typename storm::storage::SparseMatrix<ValueType>::rows entriesInRow = backwards.getRow(
                                currState);
                        for (auto entryIt = entriesInRow.begin(), entryIte = entriesInRow.end();
                             entryIt != entryIte; ++entryIt) {
                            uint_fast64_t predecessor = entryIt->getColumn();
                            if (!ma->isMarkovianState(predecessor) && !statesToKeep.count(predecessor)) {
                                if (!eliminationMapping.count(predecessor)) {
                                    eliminationMapping[predecessor] = base_state;
                                    queue.push(predecessor);
                                } else if (eliminationMapping[predecessor] != base_state) {
                                    eliminationMapping.erase(predecessor);
                                    statesToKeep.insert(predecessor);
                                    changedStates.push(predecessor);
                                }
                            }
                        }
                    }
                }
            }

            // Correct the mapping with the states which have to be kept
            while (!changedStates.empty()) {
                uint_fast64_t base_state = changedStates.front();
                queue.push(base_state);

                while (!queue.empty()) {
                    auto currState = queue.front();
                    queue.pop();
                    // Get predecessors from matrix
                    typename storm::storage::SparseMatrix<ValueType>::rows entriesInRow = backwards.getRow(
                            currState);
                    for (auto entryIt = entriesInRow.begin(), entryIte = entriesInRow.end();
                         entryIt != entryIte; ++entryIt) {
                        uint_fast64_t predecessor = entryIt->getColumn();
                        if (!ma->isMarkovianState(predecessor) && !statesToKeep.count(predecessor)) {
                            if (!eliminationMapping.count(predecessor)) {
                                eliminationMapping[predecessor] = base_state;
                                queue.push(predecessor);
                            } else if (eliminationMapping[predecessor] != base_state) {
                                eliminationMapping.erase(predecessor);
                                statesToKeep.insert(predecessor);
                                changedStates.push(predecessor);
                            }
                        }
                    }
                }

                changedStates.pop();
            }

            // At ma point, we hopefully have a valid mapping which eliminates a lot of states

            /*STORM_PRINT("Elimination Mapping" << std::endl)
            for (auto entry : eliminationMapping) {
                STORM_PRINT(std::to_string(entry.first) << " -> " << std::to_string(entry.second) << std::endl)
            }*/
            STORM_PRINT("Eliminating " << eliminationMapping.size() << " states" << std::endl)

            // TODO explore if one can construct elimination mapping and state remapping in one step

            // Construct a mapping of old state space to new one
            std::vector<uint_fast64_t> stateRemapping(ma->getNumberOfStates(), -1);
            uint_fast64_t currentNewState = 0;
            for (uint_fast64_t state = 0; state < ma->getNumberOfStates(); ++state) {
                if (eliminationMapping.count(state) > 0) {
                    if (stateRemapping[eliminationMapping[state]] == uint_fast64_t(-1)) {
                        stateRemapping[eliminationMapping[state]] = currentNewState;
                        stateRemapping[state] = currentNewState;
                        ++currentNewState;
                        queue.push(eliminationMapping[state]);
                    } else {
                        stateRemapping[state] = stateRemapping[eliminationMapping[state]];
                    }
                } else if (stateRemapping[state] == uint_fast64_t(-1)) {
                    stateRemapping[state] = currentNewState;
                    queue.push(state);
                    ++currentNewState;
                }
            }

            // Build the new MA
            storm::storage::SparseMatrix<ValueType> newTransitionMatrix;
            storm::models::sparse::StateLabeling newStateLabeling(
                    ma->getNumberOfStates() - eliminationMapping.size());
            storm::storage::BitVector newMarkovianStates(ma->getNumberOfStates() - eliminationMapping.size(),
                                                         false);
            std::vector<ValueType> newExitRates;
            //TODO choice labeling
            boost::optional<storm::models::sparse::ChoiceLabeling> choiceLabeling;

            // Initialize the matrix builder and helper variables
            storm::storage::SparseMatrixBuilder<ValueType> matrixBuilder = storm::storage::SparseMatrixBuilder<ValueType>(
                    0, 0, 0, false, true, 0);
            uint_fast64_t currentRow = 0;
            uint_fast64_t state = 0;
            while (!queue.empty()) {
                state = queue.front();
                queue.pop();

                for (auto const &label : ma->getLabelsOfState(state)) {
                    if (!newStateLabeling.containsLabel(label)) {
                        newStateLabeling.addLabel(label);
                    }

                    newStateLabeling.addLabelToState(label, stateRemapping[state]);
                }

                // Use a set to not include redundant rows
                std::set<std::map<uint_fast64_t, ValueType>> rowSet;
                for (uint_fast64_t row = 0; row < ma->getTransitionMatrix().getRowGroupSize(state); ++row) {
                    std::map<uint_fast64_t, ValueType> transitions;
                    for (typename storm::storage::SparseMatrix<ValueType>::const_iterator itEntry = ma->getTransitionMatrix().getRow(
                            state, row).begin();
                         itEntry != ma->getTransitionMatrix().getRow(state, row).end(); ++itEntry) {
                        uint_fast64_t newId = stateRemapping[itEntry->getColumn()];
                        if (transitions.count(newId) == 0) {
                            transitions[newId] = itEntry->getValue();
                        } else {
                            transitions[newId] += itEntry->getValue();
                        }
                    }
                    rowSet.insert(transitions);
                }

                // correctly set rates
                auto rate = storm::utility::zero<ValueType>();

                if (ma->isMarkovianState(state)) {
                    newMarkovianStates.set(stateRemapping[state], true);
                    rate = ma->getExitRates().at(state);
                }

                newExitRates.push_back(rate);
                // Build matrix
                matrixBuilder.newRowGroup(currentRow);
                for (auto const &row : rowSet) {
                    for (auto const &transition : row) {
                        matrixBuilder.addNextValue(currentRow, transition.first, transition.second);
                        //STORM_PRINT(stateRemapping[state] << "->" << transition.first << " : " << transition.second << std::endl)
                    }
                    ++currentRow;
                }
            }
            newTransitionMatrix = matrixBuilder.build();

            storm::storage::sparse::ModelComponents<ValueType, RewardModelType> newComponents = storm::storage::sparse::ModelComponents<ValueType, RewardModelType>(
                    std::move(newTransitionMatrix), std::move(newStateLabeling));

            newComponents.rateTransitions = false;
            newComponents.markovianStates = std::move(newMarkovianStates);
            newComponents.exitRates = std::move(newExitRates);
            auto model = std::make_shared<storm::models::sparse::MarkovAutomaton<ValueType, RewardModelType >>(
                    std::move(newComponents));
            if (model->isConvertibleToCtmc()) {
                return model->convertToCtmc();
            } else {
                return model;
            }
        }


        template
        class NonMarkovianChainTransformer<double>;

#ifdef STORM_HAVE_CARL

        template
        class NonMarkovianChainTransformer<storm::RationalFunction>;

#endif
    }
}

