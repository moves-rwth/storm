#include "src/builder/ExplicitDFTModelBuilderApprox.h"
#include <src/models/sparse/MarkovAutomaton.h>
#include <src/models/sparse/Ctmc.h>
#include <src/utility/constants.h>
#include <src/utility/vector.h>
#include <src/exceptions/UnexpectedException.h>
#include "src/settings/modules/DFTSettings.h"
#include "src/settings/SettingsManager.h"
#include "src/generator/DftNextStateGenerator.h"
#include <map>

namespace storm {
    namespace builder {

        template <typename ValueType, typename StateType>
        ExplicitDFTModelBuilderApprox<ValueType, StateType>::ModelComponents::ModelComponents() : transitionMatrix(), stateLabeling(), markovianStates(), exitRates(), choiceLabeling() {
            // Intentionally left empty.
        }

        template <typename ValueType, typename StateType>
        ExplicitDFTModelBuilderApprox<ValueType, StateType>::ExplicitDFTModelBuilderApprox(storm::storage::DFT<ValueType> const& dft, storm::storage::DFTIndependentSymmetries const& symmetries, bool enableDC) : dft(dft), enableDC(enableDC), stateStorage(((dft.stateVectorSize() / 64) + 1) * 64) {
            // stateVectorSize is bound for size of bitvector

            stateGenerationInfo = std::make_shared<storm::storage::DFTStateGenerationInfo>(dft.buildStateGenerationInfo(symmetries));
        }

        template <typename ValueType, typename StateType>
        std::shared_ptr<storm::models::sparse::Model<ValueType>> ExplicitDFTModelBuilderApprox<ValueType, StateType>::buildModel(LabelOptions const& labelOpts) {
            STORM_LOG_TRACE("Generating DFT state space");

            // Initialize
            StateType currentRowGroup = 0;
            StateType currentRow = 0;
            modelComponents.markovianStates = storm::storage::BitVector(INITIAL_BITVECTOR_SIZE);
            // Create generator
            storm::generator::DftNextStateGenerator<ValueType, StateType> generator(dft, *stateGenerationInfo, enableDC, mergeFailedStates);
            // Create sparse matrix builder
            storm::storage::SparseMatrixBuilder<ValueType> transitionMatrixBuilder(0, 0, 0, false, !generator.isDeterministicModel(), 0);

            if(mergeFailedStates) {
                // Introduce explicit fail state
                storm::generator::StateBehavior<ValueType, StateType> behavior = generator.createMergeFailedState([this] (DFTStatePointer const& state) {
                        this->failedStateId = newIndex++;
                        stateRemapping.push_back(0);
                        return this->failedStateId;
                    } );

                setRemapping(failedStateId, currentRowGroup);

                STORM_LOG_ASSERT(!behavior.empty(), "Behavior is empty.");
                setMarkovian(currentRowGroup, behavior.begin()->isMarkovian());

                // If the model is nondeterministic, we need to open a row group.
                if (!generator.isDeterministicModel()) {
                    transitionMatrixBuilder.newRowGroup(currentRow);
                }

                // Now add self loop.
                // TODO Matthias: maybe use general method.
                STORM_LOG_ASSERT(behavior.getNumberOfChoices() == 1, "Wrong number of choices for failed state.");
                STORM_LOG_ASSERT(behavior.begin()->size() == 1, "Wrong number of transitions for failed state.");
                std::pair<StateType, ValueType> stateProbabilityPair = *(behavior.begin()->begin());
                STORM_LOG_ASSERT(stateProbabilityPair.first == failedStateId, "No self loop for failed state.");
                STORM_LOG_ASSERT(storm::utility::isOne<ValueType>(stateProbabilityPair.second), "Probability for failed state != 1.");
                transitionMatrixBuilder.addNextValue(currentRow, stateProbabilityPair.first, stateProbabilityPair.second);
                ++currentRow;
                ++currentRowGroup;
            }

            // Create a callback for the next-state generator to enable it to add states
            std::function<StateType (DFTStatePointer const&)> stateToIdCallback = std::bind(&ExplicitDFTModelBuilderApprox::getOrAddStateIndex, this, std::placeholders::_1);

            // Build initial states
            this->stateStorage.initialStateIndices = generator.getInitialStates(stateToIdCallback);
            STORM_LOG_ASSERT(stateStorage.initialStateIndices.size() == 1, "Only one initial state assumed.");
            StateType initialStateIndex = stateStorage.initialStateIndices[0];
            STORM_LOG_TRACE("Initial state: " << initialStateIndex);

            // Explore state space
            bool explorationFinished = false;
            while (!explorationFinished) {
                // Get the first state in the queue
                DFTStatePointer currentState = statesToExplore.front();
                STORM_LOG_ASSERT(stateStorage.stateToId.getValue(currentState->status()) == currentState->getId(), "Ids of states do not coincide.");
                statesToExplore.pop_front();

                // Remember that this row group was actually filled with the transitions of a different state
                setRemapping(currentState->getId(), currentRowGroup);

                // Explore state
                generator.load(currentState);
                storm::generator::StateBehavior<ValueType, StateType> behavior = generator.expand(stateToIdCallback);

                STORM_LOG_ASSERT(!behavior.empty(), "Behavior is empty.");
                setMarkovian(currentRowGroup, behavior.begin()->isMarkovian());

                // If the model is nondeterministic, we need to open a row group.
                if (!generator.isDeterministicModel()) {
                    transitionMatrixBuilder.newRowGroup(currentRow);
                }

                // Now add all choices.
                for (auto const& choice : behavior) {
                    // Add the probabilistic behavior to the matrix.
                    for (auto const& stateProbabilityPair : choice) {

                        // Check that pseudo state and its instantiation do not appear together
                        // TODO Matthias: prove that this is not possible and remove
                        if (stateProbabilityPair.first >= OFFSET_PSEUDO_STATE) {
                            StateType newId = stateProbabilityPair.first - OFFSET_PSEUDO_STATE;
                            STORM_LOG_ASSERT(newId < mPseudoStatesMapping.size(), "Id is not valid.");
                            if (mPseudoStatesMapping[newId].first > 0) {
                                // State exists already
                                newId = mPseudoStatesMapping[newId].first;
                                for (auto itFind = choice.begin(); itFind != choice.end(); ++itFind) {
                                    STORM_LOG_ASSERT(itFind->first != newId, "Pseudo state and instantiation occur together in a distribution.");
                                }
                            }
                        }

                        transitionMatrixBuilder.addNextValue(currentRow, stateProbabilityPair.first, stateProbabilityPair.second);
                    }
                    ++currentRow;
                }
                ++currentRowGroup;

                if (statesToExplore.empty()) {
                    explorationFinished = true;
                    // Before ending the exploration check for pseudo states which are not initialized yet
                    for ( ; pseudoStatesToCheck < mPseudoStatesMapping.size(); ++pseudoStatesToCheck) {
                        std::pair<StateType, storm::storage::BitVector> pseudoStatePair = mPseudoStatesMapping[pseudoStatesToCheck];
                        if (pseudoStatePair.first == 0) {
                            // Create state from pseudo state and explore
                            STORM_LOG_ASSERT(stateStorage.stateToId.contains(pseudoStatePair.second), "Pseudo state not contained.");
                            STORM_LOG_ASSERT(stateStorage.stateToId.getValue(pseudoStatePair.second) >= OFFSET_PSEUDO_STATE, "State is no pseudo state.");
                            STORM_LOG_TRACE("Create pseudo state from bit vector " << pseudoStatePair.second);
                            DFTStatePointer pseudoState = std::make_shared<storm::storage::DFTState<ValueType>>(pseudoStatePair.second, dft, *stateGenerationInfo, newIndex);
                            STORM_LOG_ASSERT(pseudoStatePair.second == pseudoState->status(), "Pseudo states do not coincide.");
                            STORM_LOG_TRACE("Explore pseudo state " << dft.getStateString(pseudoState) << " with id " << pseudoState->getId());

                            getOrAddStateIndex(pseudoState);
                            explorationFinished = false;
                            break;
                        }
                    }
                }

            } // end exploration

            size_t stateSize = stateStorage.getNumberOfStates() + (mergeFailedStates ? 1 : 0);
            modelComponents.markovianStates.resize(stateSize);

            // Replace pseudo states in matrix
            // TODO Matthias: avoid hack with fixed int type
            std::vector<uint_fast64_t> pseudoStatesVector;
            for (auto const& pseudoStatePair : mPseudoStatesMapping) {
                pseudoStatesVector.push_back(pseudoStatePair.first);
            }
            STORM_LOG_ASSERT(std::find(pseudoStatesVector.begin(), pseudoStatesVector.end(), 0) == pseudoStatesVector.end(), "Unexplored pseudo state still contained.");
            transitionMatrixBuilder.replaceColumns(pseudoStatesVector, OFFSET_PSEUDO_STATE);


            // Fix the entries in the matrix according to the (reversed) mapping of row groups to indices
            STORM_LOG_ASSERT(stateRemapping[initialStateIndex] == initialStateIndex, "Initial state should not be remapped.");
            // Fix the transition matrix
            transitionMatrixBuilder.replaceColumns(stateRemapping, 0);
            // Fix the hash map storing the mapping states -> ids
            this->stateStorage.stateToId.remap([this] (StateType const& state) { return this->stateRemapping[state]; } );

            STORM_LOG_TRACE("State remapping: " << stateRemapping);
            STORM_LOG_TRACE("Markovian states: " << modelComponents.markovianStates);

            STORM_LOG_DEBUG("Generated " << stateSize << " states");
            STORM_LOG_DEBUG("Model is " << (generator.isDeterministicModel() ? "deterministic" : "non-deterministic"));

            // Build transition matrix
            modelComponents.transitionMatrix = transitionMatrixBuilder.build(stateSize, stateSize);
            if (stateSize <= 15) {
                STORM_LOG_TRACE("Transition matrix: " << std::endl << modelComponents.transitionMatrix);
            } else {
                STORM_LOG_TRACE("Transition matrix: too big to print");
            }

            // Build state labeling
            modelComponents.stateLabeling = storm::models::sparse::StateLabeling(stateSize);
            // Initial state is always first state without any failure
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

            std::shared_ptr<storm::models::sparse::Model<ValueType>> model;

            if (generator.isDeterministicModel()) {
                // Build CTMC
                model = std::make_shared<storm::models::sparse::Ctmc<ValueType>>(std::move(modelComponents.transitionMatrix), std::move(modelComponents.stateLabeling));
            } else {
                // Build MA
                // Compute exit rates
                modelComponents.exitRates = std::vector<ValueType>(stateSize);
                std::vector<typename storm::storage::SparseMatrix<ValueType>::index_type> indices = modelComponents.transitionMatrix.getRowGroupIndices();
                for (StateType stateIndex = 0; stateIndex < stateSize; ++stateIndex) {
                    if (modelComponents.markovianStates[stateIndex]) {
                        modelComponents.exitRates[stateIndex] = modelComponents.transitionMatrix.getRowSum(indices[stateIndex]);
                    } else {
                        modelComponents.exitRates[stateIndex] = storm::utility::zero<ValueType>();
                    }
                }
                STORM_LOG_TRACE("Exit rates: " << modelComponents.exitRates);

                std::shared_ptr<storm::models::sparse::MarkovAutomaton<ValueType>> ma = std::make_shared<storm::models::sparse::MarkovAutomaton<ValueType>>(std::move(modelComponents.transitionMatrix), std::move(modelComponents.stateLabeling), std::move(modelComponents.markovianStates), std::move(modelComponents.exitRates));
                if (ma->hasOnlyTrivialNondeterminism()) {
                    // Markov automaton can be converted into CTMC
                    model = ma->convertToCTMC();
                } else {
                    model = ma;
                }
            }
            
            return model;

        }
        
        template<>
        bool belowThreshold(double const& number) {
            return number < 0.1;
        }

        template<>
        bool belowThreshold(storm::RationalFunction const& number) {
            storm::RationalFunction threshold = storm::utility::one<storm::RationalFunction>() / 10;
            std::cout << number << " < " << threshold << ": " << (number < threshold) << std::endl;
            std::map<storm::Variable, storm::RationalNumber> mapping;
            
            storm::RationalFunction eval(number.evaluate(mapping));
            std::cout << "Evaluated: " << eval << std::endl;
            return eval < threshold;
        }

        template <typename ValueType, typename StateType>
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

                if (!changed && stateId >= OFFSET_PSEUDO_STATE) {
                    // Pseudo state can be created now
                    STORM_LOG_ASSERT(stateId >= OFFSET_PSEUDO_STATE, "State is no pseudo state.");
                    stateId -= OFFSET_PSEUDO_STATE;
                    STORM_LOG_ASSERT(stateId < mPseudoStatesMapping.size(), "Pseudo state not known.");
                    STORM_LOG_ASSERT(mPseudoStatesMapping[stateId].first == 0, "Pseudo state already created.");
                    // Create pseudo state now
                    STORM_LOG_ASSERT(mPseudoStatesMapping[stateId].second == state->status(), "Pseudo states do not coincide.");
                    state->setId(newIndex++);
                    mPseudoStatesMapping[stateId].first = state->getId();
                    stateId = state->getId();
                    stateStorage.stateToId.setOrAdd(state->status(), stateId);
                    STORM_LOG_TRACE("Now create state " << dft.getStateString(state) << " with id " << stateId);
                    statesToExplore.push_front(state);
                }
            } else {
                // State does not exist yet
                if (changed) {
                    // Remember state for later creation
                    state->setId(mPseudoStatesMapping.size() + OFFSET_PSEUDO_STATE);
                    mPseudoStatesMapping.push_back(std::make_pair(0, state->status()));
                    stateId = stateStorage.stateToId.findOrAdd(state->status(), state->getId());
                    STORM_LOG_ASSERT(stateId == state->getId(), "Ids do not match.");
                    STORM_LOG_TRACE("Remember state for later creation: " << dft.getStateString(state));
                    // Reserve one slot for the coming state in the remapping
                    stateRemapping.push_back(0);
                } else {
                    // Create new state
                    state->setId(newIndex++);
                    stateId = stateStorage.stateToId.findOrAdd(state->status(), state->getId());
                    STORM_LOG_ASSERT(stateId == state->getId(), "Ids do not match.");
                    STORM_LOG_TRACE("New state: " << dft.getStateString(state));
                    statesToExplore.push_front(state);

                    // Reserve one slot for the new state in the remapping
                    stateRemapping.push_back(0);
                }
            }
            return stateId;
        }

        template <typename ValueType, typename StateType>
        void ExplicitDFTModelBuilderApprox<ValueType, StateType>::setMarkovian(StateType id, bool markovian) {
            if (id >= modelComponents.markovianStates.size()) {
                // Resize BitVector
                modelComponents.markovianStates.resize(modelComponents.markovianStates.size() + INITIAL_BITVECTOR_SIZE);
            }
            modelComponents.markovianStates.set(id, markovian);
        }

        template <typename ValueType, typename StateType>
        void ExplicitDFTModelBuilderApprox<ValueType, StateType>::setRemapping(StateType id, StateType mappedId) {
            STORM_LOG_ASSERT(id < stateRemapping.size(), "Invalid index for remapping.");
            stateRemapping[id] = mappedId;
        }


        // Explicitly instantiate the class.
        template class ExplicitDFTModelBuilderApprox<double>;

#ifdef STORM_HAVE_CARL
        template class ExplicitDFTModelBuilderApprox<storm::RationalFunction>;
#endif

    } // namespace builder
} // namespace storm


