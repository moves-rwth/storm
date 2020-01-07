#include <queue>

#include "NonMarkovianChainTransformer.h"

#include "storm/logic/Formulas.h"
#include "storm/logic/FragmentSpecification.h"

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
                EliminationLabelBehavior labelBehavior) {
            // TODO reward models

            STORM_LOG_WARN_COND(labelBehavior == EliminationLabelBehavior::KeepLabels, "Labels are not preserved! Results may be incorrect. Continue at your own caution.");
            if (labelBehavior == EliminationLabelBehavior::DeleteLabels) {
                STORM_PRINT("Use Label Deletion" << std::endl)
            }
            if (labelBehavior == EliminationLabelBehavior::MergeLabels) {
                STORM_PRINT("Use Label Merging" << std::endl)
            }
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
            for (uint_fast64_t base_state = 0; base_state < ma->getNumberOfStates(); ++base_state) {
                STORM_LOG_ASSERT(!ma->isHybridState(base_state), "Base state is hybrid.");
                if (ma->isMarkovianState(base_state)) {
                    queue.push(base_state);

                    while (!queue.empty()) {
                        auto currState = queue.front();
                        queue.pop();

                        auto currLabels = ma->getLabelsOfState(currState);

                        // Get predecessors from matrix
                        typename storm::storage::SparseMatrix<ValueType>::rows entriesInRow = backwards.getRow(
                                currState);
                        for (auto entryIt = entriesInRow.begin(), entryIte = entriesInRow.end();
                             entryIt != entryIte; ++entryIt) {
                            uint_fast64_t predecessor = entryIt->getColumn();
                            if (!ma->isMarkovianState(predecessor) && !statesToKeep.count(predecessor)) {
                                if (labelBehavior == EliminationLabelBehavior::DeleteLabels || labelBehavior == EliminationLabelBehavior::MergeLabels ||
                                    currLabels == ma->getLabelsOfState(predecessor)) {
                                    // If labels are not to be preserved or states are labeled the same
                                    if (!eliminationMapping.count(predecessor)) {
                                        eliminationMapping[predecessor] = base_state;
                                        queue.push(predecessor);
                                    } else if (eliminationMapping[predecessor] != base_state) {
                                        eliminationMapping.erase(predecessor);
                                        statesToKeep.insert(predecessor);
                                        changedStates.push(predecessor);
                                    }
                                } else {
                                    // Labels are to be preserved and states have different labels
                                    if (eliminationMapping.count(predecessor)) {
                                        eliminationMapping.erase(predecessor);
                                    }
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

                    auto currLabels = ma->getLabelsOfState(currState);

                    // Get predecessors from matrix
                    typename storm::storage::SparseMatrix<ValueType>::rows entriesInRow = backwards.getRow(
                            currState);
                    for (auto entryIt = entriesInRow.begin(), entryIte = entriesInRow.end();
                         entryIt != entryIte; ++entryIt) {
                        uint_fast64_t predecessor = entryIt->getColumn();
                        if (!ma->isMarkovianState(predecessor) && !statesToKeep.count(predecessor)) {
                            if (labelBehavior == EliminationLabelBehavior::DeleteLabels || labelBehavior == EliminationLabelBehavior::MergeLabels ||
                                currLabels == ma->getLabelsOfState(predecessor)) {
                                // If labels are not to be preserved or states are labeled the same
                                if (!eliminationMapping.count(predecessor)) {
                                    eliminationMapping[predecessor] = base_state;
                                    queue.push(predecessor);
                                } else if (eliminationMapping[predecessor] != base_state) {
                                    eliminationMapping.erase(predecessor);
                                    statesToKeep.insert(predecessor);
                                    changedStates.push(predecessor);
                                }
                            } else {
                                // Labels are to be preserved and states have different labels
                                if (eliminationMapping.count(predecessor)) {
                                    eliminationMapping.erase(predecessor);
                                }
                                statesToKeep.insert(predecessor);
                                changedStates.push(predecessor);
                            }
                        }
                    }
                }

                changedStates.pop();
            }

            // At this point, we hopefully have a valid mapping which eliminates a lot of states

            STORM_LOG_TRACE("Elimination Mapping" << std::endl);
            for (auto entry : eliminationMapping) {
                STORM_LOG_TRACE(std::to_string(entry.first) << " -> " << std::to_string(entry.second) << std::endl);
            }
            STORM_LOG_INFO("Eliminating " << eliminationMapping.size() << " states" << std::endl);

            // TODO explore if one can construct elimination mapping and state remapping in one step

            uint64_t newStateCount = ma->getNumberOfStates() - eliminationMapping.size();

            std::vector<std::set<std::string>> labelMap(newStateCount, std::set<std::string>());

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
                    if (labelBehavior == EliminationLabelBehavior::MergeLabels) {
                        //add all labels to the label set for the representative
                        for (auto const &label : ma->getLabelsOfState(state)) {
                            labelMap[stateRemapping[eliminationMapping[state]]].insert(label);
                        }
                    }
                } else {
                    if (stateRemapping[state] == uint_fast64_t(-1)) {
                        stateRemapping[state] = currentNewState;
                        queue.push(state);
                        ++currentNewState;
                    }
                    if (labelBehavior == EliminationLabelBehavior::MergeLabels) {
                        for (auto const &label : ma->getLabelsOfState(state)) {
                            labelMap[stateRemapping[state]].insert(label);
                        }
                    }
                }
            }

            // Build the new MA
            storm::storage::SparseMatrix<ValueType> newTransitionMatrix;
            storm::models::sparse::StateLabeling newStateLabeling(
                    newStateCount);
            storm::storage::BitVector newMarkovianStates(ma->getNumberOfStates() - eliminationMapping.size(),
                                                         false);
            std::vector<ValueType> newExitRates;
            //TODO choice labeling
            boost::optional<storm::models::sparse::ChoiceLabeling> newChoiceLabeling;

            // Initialize the matrix builder and helper variables
            storm::storage::SparseMatrixBuilder<ValueType> matrixBuilder = storm::storage::SparseMatrixBuilder<ValueType>(
                    0, 0, 0, false, true, 0);

            for (auto const &label : ma->getStateLabeling().getLabels()) {
                if (!newStateLabeling.containsLabel(label)) {
                    newStateLabeling.addLabel(label);
                }
            }
            uint_fast64_t currentRow = 0;
            uint_fast64_t state = 0;
            while (!queue.empty()) {
                state = queue.front();
                queue.pop();

                std::set<std::string> labelSet = ma->getLabelsOfState(state);
                if (labelBehavior == EliminationLabelBehavior::MergeLabels) {
                    labelSet = labelMap[stateRemapping[state]];
                }

                for (auto const &label : labelSet) {
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
                        STORM_LOG_TRACE(stateRemapping[state] << "->" << transition.first << " : " << transition.second
                                                              << std::endl);
                    }
                    ++currentRow;
                }
            }
            // explicitly force dimensions of the matrix in case a column is missing
            newTransitionMatrix = matrixBuilder.build(newStateCount, newStateCount, newStateCount);

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

        template<typename ValueType, typename RewardModelType>
        bool NonMarkovianChainTransformer<ValueType, RewardModelType>::preservesFormula(
                storm::logic::Formula const &formula) {
            storm::logic::FragmentSpecification fragment = storm::logic::propositional();

            fragment.setProbabilityOperatorsAllowed(true);
            fragment.setGloballyFormulasAllowed(true);
            fragment.setReachabilityProbabilityFormulasAllowed(true);
            fragment.setUntilFormulasAllowed(true);
            fragment.setTimeBoundedUntilFormulasAllowed(true);

            return formula.isInFragment(fragment);
        }

        template<typename ValueType, typename RewardModelType>
        std::vector<std::shared_ptr<storm::logic::Formula const>>
        NonMarkovianChainTransformer<ValueType, RewardModelType>::checkAndTransformFormulas(
                std::vector<std::shared_ptr<storm::logic::Formula const>> const &formulas) {
            std::vector<std::shared_ptr<storm::logic::Formula const>> result;

            for (auto const &f : formulas) {
                if (preservesFormula(*f)) {
                    result.push_back(f);
                } else {
                    STORM_LOG_INFO("Non-Markovian chain elimination does not preserve formula " << *f);
                }
            }
            return result;
        }


        template
        class NonMarkovianChainTransformer<double>;

        template
        class NonMarkovianChainTransformer<double, storm::models::sparse::StandardRewardModel<storm::Interval>>;
#ifdef STORM_HAVE_CARL

        template
        class NonMarkovianChainTransformer<storm::RationalFunction>;

        template
        class NonMarkovianChainTransformer<storm::RationalNumber>;

#endif
    }
}

