#include "src/builder/ExplicitDFTModelBuilder.h"
#include <src/models/sparse/MarkovAutomaton.h>
#include <src/models/sparse/CTMC.h>
#include <src/utility/constants.h>
#include <src/exceptions/UnexpectedException.h>
#include <map>

namespace storm {
    namespace builder {

        template <typename ValueType>
        ExplicitDFTModelBuilder<ValueType>::ModelComponents::ModelComponents() : transitionMatrix(), stateLabeling(), markovianStates(), exitRates(), choiceLabeling() {
            // Intentionally left empty.
        }

        template <typename ValueType>
        std::shared_ptr<storm::models::sparse::Model<ValueType>> ExplicitDFTModelBuilder<ValueType>::buildModel() {
            // Initialize
            DFTStatePointer state = std::make_shared<storm::storage::DFTState<ValueType>>(mDft, newIndex++);
            mStates.findOrAdd(state->status(), state);

            std::queue<DFTStatePointer> stateQueue;
            stateQueue.push(state);

            bool deterministicModel = false;
            ModelComponents modelComponents;
            std::vector<uint_fast64_t> tmpMarkovianStates;
            storm::storage::SparseMatrixBuilder<ValueType> transitionMatrixBuilder(0, 0, 0, false, !deterministicModel, 0);

            // Begin model generation
            bool deterministic = exploreStates(stateQueue, transitionMatrixBuilder, tmpMarkovianStates, modelComponents.exitRates);
            STORM_LOG_DEBUG("Generated " << mStates.size() << " states");
            STORM_LOG_DEBUG("Model is " << (deterministic ? "deterministic" : "non-deterministic"));

            // Build Markov Automaton
            modelComponents.markovianStates = storm::storage::BitVector(mStates.size(), tmpMarkovianStates);
            // Build transition matrix
            modelComponents.transitionMatrix = transitionMatrixBuilder.build();
            STORM_LOG_DEBUG("Transition matrix: " << std::endl << modelComponents.transitionMatrix);
            // TODO: easier output for vectors
            std::stringstream stream;
            for (uint_fast64_t i = 0; i < modelComponents.exitRates.size() - 1; ++i) {
                stream << modelComponents.exitRates[i] << ", ";
            }
            stream << modelComponents.exitRates.back();
            STORM_LOG_DEBUG("Exit rates: " << stream.str());
            STORM_LOG_DEBUG("Markovian states: " << modelComponents.markovianStates);
            assert(modelComponents.transitionMatrix.getRowCount() == modelComponents.transitionMatrix.getColumnCount());

            // Build state labeling
            modelComponents.stateLabeling = storm::models::sparse::StateLabeling(mStates.size());
            // Initial state is always first state without any failure
            modelComponents.stateLabeling.addLabel("init");
            modelComponents.stateLabeling.addLabelToState("init", 0);
            // Label all states corresponding to their status (failed, failsafe, failed BE)
            modelComponents.stateLabeling.addLabel("failed");
            modelComponents.stateLabeling.addLabel("failsafe");
            // Collect labels for all BE
            std::vector<std::shared_ptr<storage::DFTBE<ValueType>>> basicElements = mDft.getBasicElements();
            for (std::shared_ptr<storage::DFTBE<ValueType>> elem : basicElements) {
                modelComponents.stateLabeling.addLabel(elem->name() + "_fail");
            }

            for (auto const& stateVectorPair : mStates) {
                DFTStatePointer state = stateVectorPair.second;
                if (mDft.hasFailed(state)) {
                    modelComponents.stateLabeling.addLabelToState("failed", state->getId());
                }
                if (mDft.isFailsafe(state)) {
                    modelComponents.stateLabeling.addLabelToState("failsafe", state->getId());
                };
                // Set fail status for each BE
                for (std::shared_ptr<storage::DFTBE<ValueType>> elem : basicElements) {
                    if (state->hasFailed(elem->id())) {
                        modelComponents.stateLabeling.addLabelToState(elem->name() + "_fail", state->getId());
                    }
                }
            }

            std::shared_ptr<storm::models::sparse::Model<ValueType>> model;
            if (deterministic) {
                // Turn the probabilities into rates by multiplying each row with the exit rate of the state.
                // TODO Matthias: avoid transforming back and forth
                storm::storage::SparseMatrix<ValueType> rateMatrix(modelComponents.transitionMatrix);
                for (uint_fast64_t row = 0; row < rateMatrix.getRowCount(); ++row) {
                    for (auto& entry : rateMatrix.getRow(row)) {
                        entry.setValue(entry.getValue() * modelComponents.exitRates[row]);
                    }
                }

                model = std::make_shared<storm::models::sparse::Ctmc<ValueType>>(std::move(rateMatrix), std::move(modelComponents.stateLabeling));
            } else {
                model = std::make_shared<storm::models::sparse::MarkovAutomaton<ValueType>>(std::move(modelComponents.transitionMatrix), std::move(modelComponents.stateLabeling), std::move(modelComponents.markovianStates), std::move(modelComponents.exitRates));

            }
            model->printModelInformationToStream(std::cout);
            return model;
        }

        template <typename ValueType>
        bool ExplicitDFTModelBuilder<ValueType>::exploreStates(std::queue<DFTStatePointer>& stateQueue, storm::storage::SparseMatrixBuilder<ValueType>& transitionMatrixBuilder, std::vector<uint_fast64_t>& markovianStates, std::vector<ValueType>& exitRates) {

            assert(exitRates.empty());
            assert(markovianStates.empty());
            // TODO Matthias: set Markovian states directly as bitvector?
            std::map<size_t, ValueType> outgoingTransitions;
            size_t rowOffset = 0; // Captures number of non-deterministic choices
            ValueType exitRate;
            bool deterministic = true;
            //TODO Matthias: Handle dependencies!

            while (!stateQueue.empty()) {
                // Initialization
                outgoingTransitions.clear();
                exitRate = storm::utility::zero<ValueType>();

                // Consider next state
                DFTStatePointer state = stateQueue.front();
                stateQueue.pop();

                size_t smallest = 0;

                // Add self loop for target states
                if (mDft.hasFailed(state) || mDft.isFailsafe(state)) {
                    transitionMatrixBuilder.newRowGroup(state->getId() + rowOffset);
                    transitionMatrixBuilder.addNextValue(state->getId() + rowOffset, state->getId(), storm::utility::one<ValueType>());
                    STORM_LOG_TRACE("Added self loop for " << state->getId());
                    exitRates.push_back(storm::utility::one<ValueType>());
                    assert(exitRates.size()-1 == state->getId());
                    markovianStates.push_back(state->getId());
                    // No further exploration required
                    continue;
                } else {
                    STORM_LOG_THROW(state->nrFailableBEs() > 0, storm::exceptions::UnexpectedException, "State " << state->getId() << " is no target state but behaves like one");
                }

                // Let BE fail
                while (smallest < state->nrFailableBEs()) {
                    STORM_LOG_TRACE("exploring from: " << mDft.getStateString(state));

                    // Construct new state as copy from original one
                    DFTStatePointer newState = std::make_shared<storm::storage::DFTState<ValueType>>(*state);
                    std::pair<std::shared_ptr<storm::storage::DFTBE<ValueType>>, bool> nextBEPair = newState->letNextBEFail(smallest++);
                    std::shared_ptr<storm::storage::DFTBE<ValueType>> nextBE = nextBEPair.first;
                    if (nextBE == nullptr) {
                        break;
                    }
                    STORM_LOG_TRACE("with the failure of: " << nextBE->name() << " [" << nextBE->id() << "]");

                    storm::storage::DFTStateSpaceGenerationQueues<ValueType> queues;

                    for (DFTGatePointer parent : nextBE->parents()) {
                        if (newState->isOperational(parent->id())) {
                            queues.propagateFailure(parent);
                        }
                    }

                    while (!queues.failurePropagationDone()) {
                        DFTGatePointer next = queues.nextFailurePropagation();
                        next->checkFails(*newState, queues);
                    }

                    while (!queues.failsafePropagationDone()) {
                        DFTGatePointer next = queues.nextFailsafePropagation();
                        next->checkFailsafe(*newState, queues);
                    }

                    while (!queues.dontCarePropagationDone()) {
                        DFTElementPointer next = queues.nextDontCarePropagation();
                        next->checkDontCareAnymore(*newState, queues);
                    }

                    if (mStates.contains(newState->status())) {
                        // State already exists
                        newState = mStates.findOrAdd(newState->status(), newState);
                        STORM_LOG_TRACE("State " << mDft.getStateString(newState) << " already exists");
                    } else {
                        // New state
                        newState->setId(newIndex++);
                        mStates.findOrAdd(newState->status(), newState);
                        STORM_LOG_TRACE("New state " << mDft.getStateString(newState));

                        // Add state to search
                        stateQueue.push(newState);
                    }

                    // Set failure rate according to usage
                    bool isUsed = true;
                    if (mDft.hasRepresentant(nextBE->id())) {
                        DFTElementPointer representant = mDft.getRepresentant(nextBE->id());
                        // Used must be checked for the state we are coming from as this state is responsible for the
                        // rate and not the new state we are going to
                        isUsed = state->isUsed(representant->id());
                    }
                    STORM_LOG_TRACE("BE " << nextBE->name() << " is " << (isUsed ? "used" : "not used"));
                    ValueType rate = isUsed ? nextBE->activeFailureRate() : nextBE->passiveFailureRate();
                    auto resultFind = outgoingTransitions.find(newState->getId());
                    if (resultFind != outgoingTransitions.end()) {
                        // Add to existing transition
                        resultFind->second += rate;
                        STORM_LOG_TRACE("Updated transition from " << state->getId() << " to " << resultFind->first << " with " << rate << " to " << resultFind->second);
                    } else {
                        // Insert new transition
                        outgoingTransitions.insert(std::make_pair(newState->getId(), rate));
                        STORM_LOG_TRACE("Added transition from " << state->getId() << " to " << newState->getId() << " with " << rate);
                    }
                    exitRate += rate;

                } // end while failing BE

                // Add all transitions
                transitionMatrixBuilder.newRowGroup(state->getId() + rowOffset);
                for (auto it = outgoingTransitions.begin(); it != outgoingTransitions.end(); ++it)
                {
                    ValueType probability = it->second / exitRate; // Transform rate to probability
                    transitionMatrixBuilder.addNextValue(state->getId() + rowOffset, it->first, probability);
                }
                exitRates.push_back(exitRate);
                assert(exitRates.size()-1 == state->getId());
                markovianStates.push_back(state->getId());

            } // end while queue
            return deterministic;
        }

        // Explicitly instantiate the class.
        template class ExplicitDFTModelBuilder<double>;

#ifdef STORM_HAVE_CARL
        template class ExplicitDFTModelBuilder<storm::RationalFunction>;
#endif

    } // namespace builder
} // namespace storm


