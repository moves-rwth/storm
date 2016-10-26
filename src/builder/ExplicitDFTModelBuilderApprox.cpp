#include "src/builder/ExplicitDFTModelBuilderApprox.h"
#include <src/models/sparse/MarkovAutomaton.h>
#include <src/models/sparse/Ctmc.h>
#include <src/utility/constants.h>
#include <src/utility/vector.h>
#include "src/utility/bitoperations.h"
#include <src/exceptions/UnexpectedException.h>
#include "src/settings/modules/DFTSettings.h"
#include "src/settings/SettingsManager.h"
#include <map>

namespace storm {
    namespace builder {

        template<typename ValueType, typename StateType>
        ExplicitDFTModelBuilderApprox<ValueType, StateType>::ModelComponents::ModelComponents() : transitionMatrix(), stateLabeling(), markovianStates(), exitRates(), choiceLabeling() {
            // Intentionally left empty.
        }

        template<typename ValueType, typename StateType>
        ExplicitDFTModelBuilderApprox<ValueType, StateType>::MatrixBuilder::MatrixBuilder(bool canHaveNondeterminism) : mappingOffset(0), stateRemapping(), currentRowGroup(0), currentRow(0), canHaveNondeterminism((canHaveNondeterminism)) {
            // Create matrix builder
            builder = storm::storage::SparseMatrixBuilder<ValueType>(0, 0, 0, false, canHaveNondeterminism, 0);
        }

        template<typename ValueType, typename StateType>
        ExplicitDFTModelBuilderApprox<ValueType, StateType>::ExplicitDFTModelBuilderApprox(storm::storage::DFT<ValueType> const& dft, storm::storage::DFTIndependentSymmetries const& symmetries, bool enableDC) :
                dft(dft),
                stateGenerationInfo(std::make_shared<storm::storage::DFTStateGenerationInfo>(dft.buildStateGenerationInfo(symmetries))),
                enableDC(enableDC),
                heuristic(storm::settings::getModule<storm::settings::modules::DFTSettings>().getApproximationHeuristic()),
                generator(dft, *stateGenerationInfo, enableDC, mergeFailedStates),
                matrixBuilder(!generator.isDeterministicModel()),
                stateStorage(((dft.stateVectorSize() / 64) + 1) * 64),
                // TODO Matthias: make choosable
                //explorationQueue(dft.nrElements()+1, 0, 1)
                explorationQueue(1001, 0, 0.001)
        {
            // Intentionally left empty.
            // TODO Matthias: remove again
            heuristic = storm::builder::ApproximationHeuristic::PROBABILITY;
        }

        template<typename ValueType, typename StateType>
        void ExplicitDFTModelBuilderApprox<ValueType, StateType>::buildModel(LabelOptions const& labelOpts, size_t iteration, double approximationThreshold) {
            STORM_LOG_TRACE("Generating DFT state space");

            if (iteration < 1) {
                // Initialize
                modelComponents.markovianStates = storm::storage::BitVector(INITIAL_BITVECTOR_SIZE);

                if(mergeFailedStates) {
                    // Introduce explicit fail state
                    storm::generator::StateBehavior<ValueType, StateType> behavior = generator.createMergeFailedState([this] (DFTStatePointer const& state) {
                        this->failedStateId = newIndex++;
                        matrixBuilder.stateRemapping.push_back(0);
                        return this->failedStateId;
                    } );

                    matrixBuilder.setRemapping(failedStateId);
                    STORM_LOG_ASSERT(!behavior.empty(), "Behavior is empty.");
                    matrixBuilder.newRowGroup();
                    setMarkovian(behavior.begin()->isMarkovian());

                    // Now add self loop.
                    // TODO Matthias: maybe use general method.
                    STORM_LOG_ASSERT(behavior.getNumberOfChoices() == 1, "Wrong number of choices for failed state.");
                    STORM_LOG_ASSERT(behavior.begin()->size() == 1, "Wrong number of transitions for failed state.");
                    std::pair<StateType, ValueType> stateProbabilityPair = *(behavior.begin()->begin());
                    STORM_LOG_ASSERT(stateProbabilityPair.first == failedStateId, "No self loop for failed state.");
                    STORM_LOG_ASSERT(storm::utility::isOne<ValueType>(stateProbabilityPair.second), "Probability for failed state != 1.");
                    matrixBuilder.addTransition(stateProbabilityPair.first, stateProbabilityPair.second);
                    matrixBuilder.finishRow();
                }

                // Build initial state
                this->stateStorage.initialStateIndices = generator.getInitialStates(std::bind(&ExplicitDFTModelBuilderApprox::getOrAddStateIndex, this, std::placeholders::_1));
                STORM_LOG_ASSERT(stateStorage.initialStateIndices.size() == 1, "Only one initial state assumed.");
                initialStateIndex = stateStorage.initialStateIndices[0];
                STORM_LOG_TRACE("Initial state: " << initialStateIndex);
                // Initialize heuristic values for inital state
                STORM_LOG_ASSERT(!statesNotExplored.at(initialStateIndex).second, "Heuristic for initial state is already initialized");
                ExplorationHeuristicPointer heuristic = std::make_shared<ExplorationHeuristic>(initialStateIndex);
                heuristic->markExpand();
                statesNotExplored[initialStateIndex].second = heuristic;
                explorationQueue.push(heuristic);
            } else {
                initializeNextIteration();
            }

            switch (heuristic) {
                case storm::builder::ApproximationHeuristic::NONE:
                    // Do not change anything
                    approximationThreshold = dft.nrElements()+10;
                    break;
                case storm::builder::ApproximationHeuristic::DEPTH:
                    approximationThreshold = iteration;
                    break;
                case storm::builder::ApproximationHeuristic::PROBABILITY:
                    approximationThreshold = 10 * std::pow(2, iteration);
                    break;
            }
            exploreStateSpace(approximationThreshold);

            size_t stateSize = stateStorage.getNumberOfStates() + (mergeFailedStates ? 1 : 0);
            modelComponents.markovianStates.resize(stateSize);
            modelComponents.deterministicModel = generator.isDeterministicModel();

            // Fix the entries in the transition matrix according to the mapping of ids to row group indices
            STORM_LOG_ASSERT(matrixBuilder.stateRemapping[initialStateIndex] == initialStateIndex, "Initial state should not be remapped.");
            // TODO Matthias: do not consider all rows?
            STORM_LOG_TRACE("Remap matrix: " << matrixBuilder.stateRemapping << ", offset: " << matrixBuilder.mappingOffset);
            matrixBuilder.remap();

            STORM_LOG_TRACE("State remapping: " << matrixBuilder.stateRemapping);
            STORM_LOG_TRACE("Markovian states: " << modelComponents.markovianStates);
            STORM_LOG_DEBUG("Model has " << stateSize << " states");
            STORM_LOG_DEBUG("Model is " << (generator.isDeterministicModel() ? "deterministic" : "non-deterministic"));

            // Build transition matrix
            modelComponents.transitionMatrix = matrixBuilder.builder.build(stateSize, stateSize);
            if (stateSize <= 15) {
                STORM_LOG_TRACE("Transition matrix: " << std::endl << modelComponents.transitionMatrix);
            } else {
                STORM_LOG_TRACE("Transition matrix: too big to print");
            }

            buildLabeling(labelOpts);
        }

        template<typename ValueType, typename StateType>
        void ExplicitDFTModelBuilderApprox<ValueType, StateType>::initializeNextIteration() {
            STORM_LOG_TRACE("Refining DFT state space");

            // TODO Matthias: should be easier now as skipped states all are at the end of the matrix
            // Push skipped states to explore queue
            // TODO Matthias: remove
            for (auto const& skippedState : skippedStates) {
                statesNotExplored[skippedState.second.first->getId()] = skippedState.second;
                explorationQueue.push(skippedState.second.second);
            }

            // Initialize matrix builder again
            // TODO Matthias: avoid copy
            std::vector<uint_fast64_t> copyRemapping = matrixBuilder.stateRemapping;
            matrixBuilder = MatrixBuilder(!generator.isDeterministicModel());
            matrixBuilder.stateRemapping = copyRemapping;
            StateType nrStates = modelComponents.transitionMatrix.getRowGroupCount();
            STORM_LOG_ASSERT(nrStates == matrixBuilder.stateRemapping.size(), "No. of states does not coincide with mapping size.");

            // Start by creating a remapping from the old indices to the new indices
            std::vector<StateType> indexRemapping = std::vector<StateType>(nrStates, 0);
            auto iterSkipped = skippedStates.begin();
            size_t skippedBefore = 0;
            for (size_t i = 0; i < indexRemapping.size(); ++i) {
                while (iterSkipped->first <= i) {
                    ++skippedBefore;
                    ++iterSkipped;
                }
                indexRemapping[i] = i - skippedBefore;
            }

            // Set remapping
            size_t nrExpandedStates = nrStates - skippedBefore;
            matrixBuilder.mappingOffset = nrStates;
            STORM_LOG_TRACE("# expanded states: " << nrExpandedStates);
            StateType skippedIndex = nrExpandedStates;
            std::map<StateType, std::pair<DFTStatePointer, ExplorationHeuristicPointer>> skippedStatesNew;
            for (size_t id = 0; id < matrixBuilder.stateRemapping.size(); ++id) {
                StateType index = matrixBuilder.stateRemapping[id];
                auto itFind = skippedStates.find(index);
                if (itFind != skippedStates.end()) {
                    // Set new mapping for skipped state
                    matrixBuilder.stateRemapping[id] = skippedIndex;
                    skippedStatesNew[skippedIndex] = itFind->second;
                    indexRemapping[index] = skippedIndex;
                    ++skippedIndex;
                } else {
                    // Set new mapping for expanded state
                    matrixBuilder.stateRemapping[id] = indexRemapping[index];
                }
            }
            STORM_LOG_TRACE("New state remapping: " << matrixBuilder.stateRemapping);
            std::stringstream ss;
            ss << "Index remapping:" << std::endl;
            for (auto tmp : indexRemapping) {
                ss << tmp << " ";
            }
            STORM_LOG_TRACE(ss.str());

            // Remap markovian states
            storm::storage::BitVector markovianStatesNew = storm::storage::BitVector(modelComponents.markovianStates.size(), true);
            // Iterate over all not set bits
            modelComponents.markovianStates.complement();
            size_t index = modelComponents.markovianStates.getNextSetIndex(0);
            while (index < modelComponents.markovianStates.size()) {
                markovianStatesNew.set(indexRemapping[index], false);
                index = modelComponents.markovianStates.getNextSetIndex(index+1);
            }
            STORM_LOG_ASSERT(modelComponents.markovianStates.size() - modelComponents.markovianStates.getNumberOfSetBits() == markovianStatesNew.getNumberOfSetBits(), "Remapping of markovian states is wrong.");
            STORM_LOG_ASSERT(markovianStatesNew.size() == nrStates, "No. of states does not coincide with markovian size.");
            modelComponents.markovianStates = markovianStatesNew;

            // Build submatrix for expanded states
            // TODO Matthias: only use row groups when necessary
            for (StateType oldRowGroup = 0; oldRowGroup < modelComponents.transitionMatrix.getRowGroupCount(); ++oldRowGroup) {
                if (indexRemapping[oldRowGroup] < nrExpandedStates) {
                    // State is expanded -> copy to new matrix
                    matrixBuilder.newRowGroup();
                    for (StateType oldRow = modelComponents.transitionMatrix.getRowGroupIndices()[oldRowGroup]; oldRow < modelComponents.transitionMatrix.getRowGroupIndices()[oldRowGroup+1]; ++oldRow) {
                        for (typename storm::storage::SparseMatrix<ValueType>::const_iterator itEntry = modelComponents.transitionMatrix.begin(oldRow); itEntry != modelComponents.transitionMatrix.end(oldRow); ++itEntry) {
                            auto itFind = skippedStates.find(itEntry->getColumn());
                            if (itFind != skippedStates.end()) {
                                // Set id for skipped states as we remap it later
                                matrixBuilder.addTransition(matrixBuilder.mappingOffset + itFind->second.first->getId(), itEntry->getValue());
                            } else {
                                // Set newly remapped index for expanded states
                                matrixBuilder.addTransition(indexRemapping[itEntry->getColumn()], itEntry->getValue());
                            }
                        }
                        matrixBuilder.finishRow();
                    }
                }
            }

            skippedStates = skippedStatesNew;

            STORM_LOG_ASSERT(matrixBuilder.getCurrentRowGroup() == nrExpandedStates, "Row group size does not match.");
            skippedStates.clear();
        }

        template<typename ValueType, typename StateType>
        void ExplicitDFTModelBuilderApprox<ValueType, StateType>::exploreStateSpace(double approximationThreshold) {
            size_t nrExpandedStates = 0;
            size_t nrSkippedStates = 0;
            // TODO Matthias: do not empty queue every time but break before
            while (!explorationQueue.empty()) {
                explorationQueue.fix();
                //explorationQueue.print(std::cout);
                //printNotExplored();
                // Get the first state in the queue
                ExplorationHeuristicPointer currentExplorationHeuristic = explorationQueue.popTop();
                StateType currentId = currentExplorationHeuristic->getId();
                auto itFind = statesNotExplored.find(currentId);
                STORM_LOG_ASSERT(itFind != statesNotExplored.end(), "Id " << currentId << " not found");
                DFTStatePointer currentState = itFind->second.first;
                STORM_LOG_ASSERT(currentExplorationHeuristic == itFind->second.second, "Exploration heuristics do not match");
                STORM_LOG_ASSERT(currentState->getId() == currentId, "Ids do not match");
                // Remove it from the list of not explored states
                statesNotExplored.erase(itFind);
                STORM_LOG_ASSERT(stateStorage.stateToId.contains(currentState->status()), "State is not contained in state storage.");
                STORM_LOG_ASSERT(stateStorage.stateToId.getValue(currentState->status()) == currentId, "Ids of states do not coincide.");

                // Get concrete state if necessary
                if (currentState->isPseudoState()) {
                    // Create concrete state from pseudo state
                    currentState->construct();
                }
                STORM_LOG_ASSERT(!currentState->isPseudoState(), "State is pseudo state.");

                // Remember that the current row group was actually filled with the transitions of a different state
                matrixBuilder.setRemapping(currentId);

                matrixBuilder.newRowGroup();

                // Try to explore the next state
                generator.load(currentState);

                if (nrExpandedStates > approximationThreshold && !currentExplorationHeuristic->isExpand()) {
                //if (currentExplorationHeuristic->isSkip(approximationThreshold)) {
                    // Skip the current state
                    ++nrSkippedStates;
                    STORM_LOG_TRACE("Skip expansion of state: " << dft.getStateString(currentState));
                    setMarkovian(true);
                    // Add transition to target state with temporary value 0
                    // TODO Matthias: what to do when there is no unique target state?
                    matrixBuilder.addTransition(failedStateId, storm::utility::zero<ValueType>());
                    // Remember skipped state
                    skippedStates[matrixBuilder.getCurrentRowGroup() - 1] = std::make_pair(currentState, currentExplorationHeuristic);
                    matrixBuilder.finishRow();
                } else {
                    // Explore the current state
                    ++nrExpandedStates;
                    storm::generator::StateBehavior<ValueType, StateType> behavior = generator.expand(std::bind(&ExplicitDFTModelBuilderApprox::getOrAddStateIndex, this, std::placeholders::_1));
                    STORM_LOG_ASSERT(!behavior.empty(), "Behavior is empty.");
                    setMarkovian(behavior.begin()->isMarkovian());

                    // Now add all choices.
                    for (auto const& choice : behavior) {
                        // Add the probabilistic behavior to the matrix.
                        for (auto const& stateProbabilityPair : choice) {
                            STORM_LOG_ASSERT(!storm::utility::isZero(stateProbabilityPair.second), "Probability zero.");
                            // Set transition to state id + offset. This helps in only remapping all previously skipped states.
                            matrixBuilder.addTransition(matrixBuilder.mappingOffset + stateProbabilityPair.first, stateProbabilityPair.second);
                            // Set heuristic values for reached states
                            auto iter = statesNotExplored.find(stateProbabilityPair.first);
                            if (iter != statesNotExplored.end()) {
                                // Update heuristic values
                                DFTStatePointer state = iter->second.first;
                                if (!iter->second.second) {
                                    // Initialize heuristic values
                                    ExplorationHeuristicPointer heuristic = std::make_shared<ExplorationHeuristic>(stateProbabilityPair.first, *currentExplorationHeuristic, stateProbabilityPair.second, choice.getTotalMass());
                                    iter->second.second = heuristic;
                                    if (state->hasFailed(dft.getTopLevelIndex()) || state->isFailsafe(dft.getTopLevelIndex()) || state->nrFailableDependencies() > 0 || (state->nrFailableDependencies() == 0 && state->nrFailableBEs() == 0)) {
                                        // Do not skip absorbing state or if reached by dependencies
                                        iter->second.second->markExpand();
                                    }
                                    explorationQueue.push(heuristic);
                                } else if (!iter->second.second->isExpand()) {
                                    double oldPriority = iter->second.second->getPriority();
                                    if (iter->second.second->updateHeuristicValues(*currentExplorationHeuristic, stateProbabilityPair.second, choice.getTotalMass())) {
                                        // Update priority queue
                                        explorationQueue.update(iter->second.second, oldPriority);
                                    }
                                }
                            }
                        }
                        matrixBuilder.finishRow();
                    }
                }
            } // end exploration

            STORM_LOG_INFO("Expanded " << nrExpandedStates << " states");
            STORM_LOG_INFO("Skipped " << nrSkippedStates << " states");
            STORM_LOG_ASSERT(nrSkippedStates == skippedStates.size(), "Nr skipped states is wrong");
        }

        template<typename ValueType, typename StateType>
        void ExplicitDFTModelBuilderApprox<ValueType, StateType>::buildLabeling(LabelOptions const& labelOpts) {
            // Build state labeling
            modelComponents.stateLabeling = storm::models::sparse::StateLabeling(modelComponents.transitionMatrix.getRowGroupCount());
            // Initial state
            modelComponents.stateLabeling.addLabel("init");
            modelComponents.stateLabeling.addLabelToState("init", initialStateIndex);
            // Label all states corresponding to their status (failed, failsafe, failed BE)
            if(labelOpts.buildFailLabel) {
                modelComponents.stateLabeling.addLabel("failed");
            }
            if(labelOpts.buildFailSafeLabel) {
                modelComponents.stateLabeling.addLabel("failsafe");
            }

            // Collect labels for all BE
            std::vector<std::shared_ptr<storage::DFTBE<ValueType>>> basicElements = dft.getBasicElements();
            for (std::shared_ptr<storage::DFTBE<ValueType>> elem : basicElements) {
                if(labelOpts.beLabels.count(elem->name()) > 0) {
                    modelComponents.stateLabeling.addLabel(elem->name() + "_fail");
                }
            }

            // Set labels to states
            if(mergeFailedStates) {
                modelComponents.stateLabeling.addLabelToState("failed", failedStateId);
            }
            for (auto const& stateIdPair : stateStorage.stateToId) {
                storm::storage::BitVector state = stateIdPair.first;
                size_t stateId = stateIdPair.second;
                if (!mergeFailedStates && labelOpts.buildFailLabel && dft.hasFailed(state, *stateGenerationInfo)) {
                    modelComponents.stateLabeling.addLabelToState("failed", stateId);
                }
                if (labelOpts.buildFailSafeLabel && dft.isFailsafe(state, *stateGenerationInfo)) {
                    modelComponents.stateLabeling.addLabelToState("failsafe", stateId);
                };
                // Set fail status for each BE
                for (std::shared_ptr<storage::DFTBE<ValueType>> elem : basicElements) {
                    if (labelOpts.beLabels.count(elem->name()) > 0 && storm::storage::DFTState<ValueType>::hasFailed(state, stateGenerationInfo->getStateIndex(elem->id())) ) {
                        modelComponents.stateLabeling.addLabelToState(elem->name() + "_fail", stateId);
                    }
                }
            }
        }

        template<typename ValueType, typename StateType>
        std::shared_ptr<storm::models::sparse::Model<ValueType>> ExplicitDFTModelBuilderApprox<ValueType, StateType>::getModel() {
            STORM_LOG_ASSERT(skippedStates.size() == 0, "Concrete model has skipped states");
            return createModel(false);
        }

        template<typename ValueType, typename StateType>
        std::shared_ptr<storm::models::sparse::Model<ValueType>> ExplicitDFTModelBuilderApprox<ValueType, StateType>::getModelApproximation(bool lowerBound) {
            // TODO Matthias: handle case with no skipped states
            changeMatrixBound(modelComponents.transitionMatrix, lowerBound);
            return createModel(true);
        }

        template<typename ValueType, typename StateType>
        std::shared_ptr<storm::models::sparse::Model<ValueType>> ExplicitDFTModelBuilderApprox<ValueType, StateType>::createModel(bool copy) {
            std::shared_ptr<storm::models::sparse::Model<ValueType>> model;

            if (modelComponents.deterministicModel) {
                // Build CTMC
                if (copy) {
                    model = std::make_shared<storm::models::sparse::Ctmc<ValueType>>(modelComponents.transitionMatrix, modelComponents.stateLabeling);
                } else {
                    model = std::make_shared<storm::models::sparse::Ctmc<ValueType>>(std::move(modelComponents.transitionMatrix), std::move(modelComponents.stateLabeling));
                }
            } else {
                // Build MA
                // Compute exit rates
                // TODO Matthias: avoid computing multiple times
                modelComponents.exitRates = std::vector<ValueType>(modelComponents.markovianStates.size());
                std::vector<typename storm::storage::SparseMatrix<ValueType>::index_type> indices = modelComponents.transitionMatrix.getRowGroupIndices();
                for (StateType stateIndex = 0; stateIndex < modelComponents.markovianStates.size(); ++stateIndex) {
                    if (modelComponents.markovianStates[stateIndex]) {
                        modelComponents.exitRates[stateIndex] = modelComponents.transitionMatrix.getRowSum(indices[stateIndex]);
                    } else {
                        modelComponents.exitRates[stateIndex] = storm::utility::zero<ValueType>();
                    }
                }
                STORM_LOG_TRACE("Exit rates: " << modelComponents.exitRates);

                std::shared_ptr<storm::models::sparse::MarkovAutomaton<ValueType>> ma;
                if (copy) {
                    ma = std::make_shared<storm::models::sparse::MarkovAutomaton<ValueType>>(modelComponents.transitionMatrix, modelComponents.stateLabeling, modelComponents.markovianStates, modelComponents.exitRates);
                } else {
                    ma = std::make_shared<storm::models::sparse::MarkovAutomaton<ValueType>>(std::move(modelComponents.transitionMatrix), std::move(modelComponents.stateLabeling), std::move(modelComponents.markovianStates), std::move(modelComponents.exitRates));
                }
                if (ma->hasOnlyTrivialNondeterminism()) {
                    // Markov automaton can be converted into CTMC
                    // TODO Matthias: change components which were not moved accordingly
                    model = ma->convertToCTMC();
                } else {
                    model = ma;
                }
            }

            STORM_LOG_DEBUG("No. states: " << model->getNumberOfStates());
            STORM_LOG_DEBUG("No. transitions: " << model->getNumberOfTransitions());
            if (model->getNumberOfStates() <= 15) {
                STORM_LOG_TRACE("Transition matrix: " << std::endl << model->getTransitionMatrix());
            } else {
                STORM_LOG_TRACE("Transition matrix: too big to print");
            }
            return model;
        }

        template<typename ValueType, typename StateType>
        void ExplicitDFTModelBuilderApprox<ValueType, StateType>::changeMatrixBound(storm::storage::SparseMatrix<ValueType> & matrix, bool lowerBound) const {
            // Set lower bound for skipped states
            for (auto it = skippedStates.begin(); it != skippedStates.end(); ++it) {
                auto matrixEntry = matrix.getRow(it->first, 0).begin();
                STORM_LOG_ASSERT(matrixEntry->getColumn() == failedStateId, "Transition has wrong target state.");
                STORM_LOG_ASSERT(!it->second.first->isPseudoState(), "State is still pseudo state.");

                ExplorationHeuristicPointer heuristic = it->second.second;
                if (storm::utility::isZero(heuristic->getUpperBound())) {
                    // Initialize bounds
                    ValueType lowerBound = getLowerBound(it->second.first);
                    ValueType upperBound = getUpperBound(it->second.first);
                    heuristic->setBounds(lowerBound, upperBound);
                }

                // Change bound
                if (lowerBound) {
                    matrixEntry->setValue(it->second.second->getLowerBound());
                } else {
                    matrixEntry->setValue(it->second.second->getUpperBound());
                }
            }
        }

        template<typename ValueType, typename StateType>
        ValueType ExplicitDFTModelBuilderApprox<ValueType, StateType>::getLowerBound(DFTStatePointer const& state) const {
            // Get the lower bound by considering the failure of all possible BEs
            ValueType lowerBound = storm::utility::zero<ValueType>();
            for (size_t index = 0; index < state->nrFailableBEs(); ++index) {
                lowerBound += state->getFailableBERate(index);
            }
            for (size_t index = 0; index < state->nrNotFailableBEs(); ++index) {
                lowerBound += state->getNotFailableBERate(index);
            }
            return lowerBound;
        }

        template<typename ValueType, typename StateType>
        ValueType ExplicitDFTModelBuilderApprox<ValueType, StateType>::getUpperBound(DFTStatePointer const& state) const {
            // Get the upper bound by considering the failure of all BEs
            // The used formula for the rate is 1/( 1/a + 1/b + ...)
            // TODO Matthias: improve by using closed formula for AND of all BEs
            ValueType upperBound = storm::utility::zero<ValueType>();

            // Get all possible rates
            std::vector<ValueType> rates(state->nrFailableBEs() + state->nrNotFailableBEs());
            for (size_t index = 0; index < state->nrFailableBEs(); ++index) {
                rates[index] = state->getFailableBERate(index);
            }
            for (size_t index = 0; index < state->nrNotFailableBEs(); ++index) {
                rates[index + state->nrFailableBEs()] = state->getNotFailableBERate(index);
            }
            STORM_LOG_ASSERT(rates.size() > 0, "State is absorbing");

            // TODO Matthias: works only for <64 BEs!
            for (size_t i = 1; i < 4 && i <= rates.size(); ++i) {
                size_t permutation = smallestIntWithNBitsSet(static_cast<size_t>(i));
                ValueType sum = storm::utility::zero<ValueType>();
                do {
                    ValueType permResult = storm::utility::zero<ValueType>();
                    for(size_t j = 0; j < rates.size(); ++j) {
                        if(permutation & (1 << j)) {
                            permResult += rates[j];
                        }
                    }
                    permutation = nextBitPermutation(permutation);
                    STORM_LOG_ASSERT(!storm::utility::isZero(permResult), "PermResult is 0");
                    sum += storm::utility::one<ValueType>() / permResult;
                } while(permutation < (1 << rates.size()) && permutation != 0);
                if (i % 2 == 0) {
                    upperBound -= sum;
                } else {
                    upperBound += sum;
                }
            }
            STORM_LOG_ASSERT(!storm::utility::isZero(upperBound), "UpperBound is 0");
            return storm::utility::one<ValueType>() / upperBound;
        }

        template<typename ValueType, typename StateType>
        StateType ExplicitDFTModelBuilderApprox<ValueType, StateType>::getOrAddStateIndex(DFTStatePointer const& state) {
            StateType stateId;
            bool changed = false;

            if (stateGenerationInfo->hasSymmetries()) {
                // Order state by symmetry
                STORM_LOG_TRACE("Check for symmetry: " << dft.getStateString(state));
                changed = state->orderBySymmetry();
                STORM_LOG_TRACE("State " << (changed ? "changed to " : "did not change") << (changed ? dft.getStateString(state) : ""));
            }

            if (stateStorage.stateToId.contains(state->status())) {
                // State already exists
                stateId = stateStorage.stateToId.getValue(state->status());
                STORM_LOG_TRACE("State " << dft.getStateString(state) << " with id " << stateId << " already exists");
                if (!changed) {
                    // Check if state is pseudo state
                    // If state is explored already the possible pseudo state was already constructed
                    auto iter = statesNotExplored.find(stateId);
                    if (iter != statesNotExplored.end() && iter->second.first->isPseudoState()) {
                        // Create pseudo state now
                        STORM_LOG_ASSERT(iter->second.first->getId() == stateId, "Ids do not match.");
                        STORM_LOG_ASSERT(iter->second.first->status() == state->status(), "Pseudo states do not coincide.");
                        state->setId(stateId);
                        // Update mapping to map to concrete state now
                        // TODO Matthias: just change pointer?
                        statesNotExplored[stateId] = std::make_pair(state, iter->second.second);
                        // We do not push the new state on the exploration queue as the pseudo state was already pushed
                        STORM_LOG_TRACE("Created pseudo state " << dft.getStateString(state));
                    }
                }
            } else {
                // State does not exist yet
                STORM_LOG_ASSERT(state->isPseudoState() == changed, "State type (pseudo/concrete) wrong.");
                // Create new state
                state->setId(newIndex++);
                stateId = stateStorage.stateToId.findOrAdd(state->status(), state->getId());
                STORM_LOG_ASSERT(stateId == state->getId(), "Ids do not match.");
                // Insert state as not yet explored
                ExplorationHeuristicPointer nullHeuristic;
                statesNotExplored[stateId] = std::make_pair(state, nullHeuristic);
                // Reserve one slot for the new state in the remapping
                matrixBuilder.stateRemapping.push_back(0);
                STORM_LOG_TRACE("New " << (state->isPseudoState() ? "pseudo" : "concrete") << " state: " << dft.getStateString(state));
            }
            return stateId;
        }

        template<typename ValueType, typename StateType>
        void ExplicitDFTModelBuilderApprox<ValueType, StateType>::setMarkovian(bool markovian) {
            if (matrixBuilder.getCurrentRowGroup() > modelComponents.markovianStates.size()) {
                // Resize BitVector
                modelComponents.markovianStates.resize(modelComponents.markovianStates.size() + INITIAL_BITVECTOR_SIZE);
            }
            modelComponents.markovianStates.set(matrixBuilder.getCurrentRowGroup() - 1, markovian);
        }

        template<typename ValueType, typename StateType>
        void ExplicitDFTModelBuilderApprox<ValueType, StateType>::printNotExplored() const {
            std::cout << "states not explored:" << std::endl;
            for (auto it : statesNotExplored) {
                std::cout << it.first << " -> " << dft.getStateString(it.second.first) << std::endl;
            }
        }


        // Explicitly instantiate the class.
        template class ExplicitDFTModelBuilderApprox<double>;

#ifdef STORM_HAVE_CARL
        template class ExplicitDFTModelBuilderApprox<storm::RationalFunction>;
#endif

    } // namespace builder
} // namespace storm


