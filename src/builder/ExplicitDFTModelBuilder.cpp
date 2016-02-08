#include "src/builder/ExplicitDFTModelBuilder.h"
#include <src/models/sparse/Ctmc.h>
#include <src/utility/constants.h>
#include <src/exceptions/UnexpectedException.h>
#include <map>

namespace storm {
    namespace builder {

        template <typename ValueType, typename RewardModelType, typename IndexType>
        ExplicitDFTModelBuilder<ValueType, RewardModelType, IndexType>::ModelComponents::ModelComponents() : transitionMatrix(), stateLabeling(), rewardModels(), choiceLabeling() {
            // Intentionally left empty.
        }

        template<typename ValueType, typename RewardModelType, typename IndexType>
        std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>> ExplicitDFTModelBuilder<ValueType, RewardModelType, IndexType>::buildCTMC() {
            // Initialize
            DFTStatePointer state = std::make_shared<storm::storage::DFTState<ValueType>>(mDft, newIndex++);
            mStates.findOrAdd(state->status(), state);

            std::queue<DFTStatePointer> stateQueue;
            stateQueue.push(state);

            bool deterministicModel = true;
            storm::storage::SparseMatrixBuilder<ValueType> transitionMatrixBuilder(0, 0, 0, false, !deterministicModel, 0);

            // Begin model generation
            exploreStates(stateQueue, transitionMatrixBuilder);
            STORM_LOG_DEBUG("Generated " << mStates.size() << " states");

            // Build CTMC
            ModelComponents modelComponents;
            // Build transition matrix
            modelComponents.transitionMatrix = transitionMatrixBuilder.build();
            STORM_LOG_DEBUG("TransitionMatrix: " << std::endl << modelComponents.transitionMatrix);
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

            std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>> model;
            model = std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>>(new storm::models::sparse::Ctmc<ValueType, RewardModelType>(std::move(modelComponents.transitionMatrix), std::move(modelComponents.stateLabeling)));
            model->printModelInformationToStream(std::cout);
            return model;
        }

        template<typename ValueType, typename RewardModelType, typename IndexType>
        void ExplicitDFTModelBuilder<ValueType, RewardModelType, IndexType>::exploreStates(std::queue<DFTStatePointer>& stateQueue, storm::storage::SparseMatrixBuilder<ValueType>& transitionMatrixBuilder) {

            std::map<size_t, ValueType> outgoingTransitions;

            while (!stateQueue.empty()) {
                // Initialization
                outgoingTransitions.clear();

                // Consider next state
                DFTStatePointer state = stateQueue.front();
                stateQueue.pop();

                size_t smallest = 0;

                // Add self loop for target states
                if (mDft.hasFailed(state) || mDft.isFailsafe(state)) {
                    transitionMatrixBuilder.addNextValue(state->getId(), state->getId(), storm::utility::one<ValueType>());
                    STORM_LOG_TRACE("Added self loop for " << state->getId());
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
                } // end while failing BE

                // Add all transitions
                for (auto it = outgoingTransitions.begin(); it != outgoingTransitions.end(); ++it)
                {
                    transitionMatrixBuilder.addNextValue(state->getId(), it->first, it->second);
                }

            } // end while queue
        }

        // Explicitly instantiate the class.
        template class ExplicitDFTModelBuilder<double, storm::models::sparse::StandardRewardModel<double>, uint32_t>;

#ifdef STORM_HAVE_CARL
        template class ExplicitDFTModelBuilder<double, storm::models::sparse::StandardRewardModel<storm::Interval>, uint32_t>;
        template class ExplicitDFTModelBuilder<RationalFunction, storm::models::sparse::StandardRewardModel<RationalFunction>, uint32_t>;
#endif

    } // namespace builder
} // namespace storm


