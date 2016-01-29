#include "src/builder/ExplicitDFTModelBuilder.h"
#include <src/models/sparse/Ctmc.h>
#include <src/utility/constants.h>
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
            storm::storage::DFTState<ValueType> state(mDft, newIndex++);
            mStates.insert(state);

            std::queue<storm::storage::DFTState<ValueType>> stateQueue;
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

            for (storm::storage::DFTState<ValueType> state : mStates) {
                if (mDft.hasFailed(state)) {
                    modelComponents.stateLabeling.addLabelToState("failed", state.getId());
                }
                if (mDft.isFailsafe(state)) {
                    modelComponents.stateLabeling.addLabelToState("failsafe", state.getId());
                };
                // Set fail status for each BE
                for (std::shared_ptr<storage::DFTBE<ValueType>> elem : basicElements) {
                    if (state.hasFailed(elem->id())) {
                        modelComponents.stateLabeling.addLabelToState(elem->name() + "_fail", state.getId());
                    }
                }
            }

            std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>> model;
            model = std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>>(new storm::models::sparse::Ctmc<ValueType, RewardModelType>(std::move(modelComponents.transitionMatrix), std::move(modelComponents.stateLabeling)));
            model->printModelInformationToStream(std::cout);
            return model;
        }

        template<typename ValueType, typename RewardModelType, typename IndexType>
        void ExplicitDFTModelBuilder<ValueType, RewardModelType, IndexType>::exploreStates(std::queue<storm::storage::DFTState<ValueType>>& stateQueue, storm::storage::SparseMatrixBuilder<ValueType>& transitionMatrixBuilder) {

            std::map<size_t, ValueType> outgoingTransitions;

            while (!stateQueue.empty()) {
                // Initialization
                outgoingTransitions.clear();
                ValueType sum = storm::utility::zero<ValueType>();

                // Consider next state
                storm::storage::DFTState<ValueType> state = stateQueue.front();
                stateQueue.pop();

                size_t smallest = 0;

                // Add self loop for target states
                if (mDft.hasFailed(state) || mDft.isFailsafe(state)) {
                    transitionMatrixBuilder.addNextValue(state.getId(), state.getId(), storm::utility::one<ValueType>());
                    STORM_LOG_TRACE("Added self loop for " << state.getId());
                    // No further exploration required
                    continue;
                }

                // Let BE fail
                while (smallest < state.nrFailableBEs()) {
                    STORM_LOG_TRACE("exploring from: " << mDft.getStateString(state));

                    storm::storage::DFTState<ValueType> newState(state);
                    std::pair<std::shared_ptr<storm::storage::DFTBE<ValueType>>, bool> nextBEPair = newState.letNextBEFail(smallest++);
                    std::shared_ptr<storm::storage::DFTBE<ValueType>> nextBE = nextBEPair.first;
                    if (nextBE == nullptr) {
                        break;
                    }
                    STORM_LOG_TRACE("with the failure of: " << nextBE->name() << " [" << nextBE->id() << "]");

                    storm::storage::DFTStateSpaceGenerationQueues<ValueType> queues;

                    for (DFTGatePointer parent : nextBE->parents()) {
                        if (newState.isOperational(parent->id())) {
                            queues.propagateFailure(parent);
                        }
                    }

                    while (!queues.failurePropagationDone()) {
                        DFTGatePointer next = queues.nextFailurePropagation();
                        next->checkFails(newState, queues);
                    }

                    while (!queues.failsafePropagationDone()) {
                        DFTGatePointer next = queues.nextFailsafePropagation();
                        next->checkFailsafe(newState, queues);
                    }

                    while (!queues.dontCarePropagationDone()) {
                        DFTElementPointer next = queues.nextDontCarePropagation();
                        next->checkDontCareAnymore(newState, queues);
                    }

                    auto it = mStates.find(newState);
                    if (it == mStates.end()) {
                        // New state
                        newState.setId(newIndex++);
                        auto itInsert = mStates.insert(newState);
                        assert(itInsert.second);
                        it = itInsert.first;
                        STORM_LOG_TRACE("New state " << mDft.getStateString(newState));

                        // Add state to search
                        stateQueue.push(newState);
                    } else {
                        // State already exists
                        STORM_LOG_TRACE("State " << mDft.getStateString(*it) << " already exists");
                    }

                    // Set transition
                    ValueType rate = nextBE->activeFailureRate();
                    auto resultFind = outgoingTransitions.find(it->getId());
                    if (resultFind != outgoingTransitions.end()) {
                        // Add to existing transition
                        resultFind->second += rate;
                    } else {
                        // Insert new transition
                        outgoingTransitions.insert(std::make_pair(it->getId(), rate));
                    }
                    sum += rate;
                } // end while failing BE

                // Add all transitions
                for (auto it = outgoingTransitions.begin(); it != outgoingTransitions.end(); ++it)
                {
                    // TODO Matthias: correct?
                    ValueType rate = it->second;// / sum;
                    transitionMatrixBuilder.addNextValue(state.getId(), it->first, rate);
                    STORM_LOG_TRACE("Added transition from " << state.getId() << " to " << it->first << " with " << rate);
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


