#include "src/builder/ExplicitDFTModelBuilder.h"

namespace storm {
    namespace builder {

        template<typename ValueType, typename RewardModelType, typename IndexType>
        void ExplicitDFTModelBuilder<ValueType, RewardModelType, IndexType>::buildCTMC() {
            // Initialize
            storm::storage::DFTState state(mDft, newIndex++);
            mStates.insert(state);

            std::queue<storm::storage::DFTState> stateQueue;
            stateQueue.push(state);

            bool deterministicModel = true;
            storm::storage::SparseMatrixBuilder<ValueType> transitionMatrixBuilder(0, 0, 0, false, !deterministicModel, 0);

            // Begin model generation
            exploreStates(stateQueue, transitionMatrixBuilder);
            //std::cout << "Generated " << mStates.size() << " states" << std::endl;

            // Build CTMC
            transitionMatrix = transitionMatrixBuilder.build();
            //std::cout << "TransitionMatrix: " << std::endl;
            //std::cout << transitionMatrix << std::endl;
            // TODO Matthias: build CTMC
        }

        template<typename ValueType, typename RewardModelType, typename IndexType>
        void ExplicitDFTModelBuilder<ValueType, RewardModelType, IndexType>::exploreStates(std::queue<storm::storage::DFTState>& stateQueue, storm::storage::SparseMatrixBuilder<ValueType>& transitionMatrixBuilder) {

            std::vector<std::pair<size_t, ValueType>> outgoingTransitions;

            while (!stateQueue.empty()) {
                // Initialization
                outgoingTransitions.clear();
                ValueType sum = 0;

                // Consider next state
                storm::storage::DFTState state = stateQueue.front();
                stateQueue.pop();

                size_t smallest = 0;

                // Let BE fail
                while (smallest < state.nrFailableBEs()) {
                    //std::cout << "exploring from: " << mDft.getStateString(state) << std::endl;

                    storm::storage::DFTState newState(state);
                    std::pair<std::shared_ptr<storm::storage::DFTBE<ValueType>>, bool> nextBE = newState.letNextBEFail(smallest++);
                    if (nextBE.first == nullptr) {
                        std::cout << "break" << std::endl;
                        break;

                    }
                    //std::cout << "with the failure of: " << nextBE.first->name() << " [" << nextBE.first->id() << "]" << std::endl;

                    storm::storage::DFTStateSpaceGenerationQueues queues;

                    for (std::shared_ptr<storm::storage::DFTGate> parent : nextBE.first->parents()) {
                        if (newState.isOperational(parent->id())) {
                            queues.propagateFailure(parent);
                        }
                    }

                    while (!queues.failurePropagationDone()) {
                        std::shared_ptr<storm::storage::DFTGate> next = queues.nextFailurePropagation();
                        next->checkFails(newState, queues);
                    }

                    while (!queues.failsafePropagationDone()) {
                        std::shared_ptr<storm::storage::DFTGate> next = queues.nextFailsafePropagation();
                        next->checkFailsafe(newState, queues);
                    }

                    while (!queues.dontCarePropagationDone()) {
                        std::shared_ptr<storm::storage::DFTElement> next = queues.nextDontCarePropagation();
                        next->checkDontCareAnymore(newState, queues);
                    }

                    auto it = mStates.find(newState);
                    if (it == mStates.end()) {
                        // New state
                        newState.setId(newIndex++);
                        auto itInsert = mStates.insert(newState);
                        assert(itInsert.second);
                        it = itInsert.first;
                        //std::cout << "New state " << mDft.getStateString(newState) << std::endl;

                        // Recursive call
                        if (!mDft.hasFailed(newState) && !mDft.isFailsafe(newState)) {
                            stateQueue.push(newState);
                        } else {
                            if (mDft.hasFailed(newState)) {
                                //std::cout << "Failed " << mDft.getStateString(newState) << std::endl;
                            } else {
                                assert(mDft.isFailsafe(newState));
                                //std::cout << "Failsafe" << mDft.getStateString(newState) << std::endl;
                            }
                        }
                    } else {
                        // State already exists
                        //std::cout << "State " << mDft.getStateString(*it) << " already exists" << std::endl;
                    }

                    // Set transition
                    ValueType prob = nextBE.first->activeFailureRate();
                    outgoingTransitions.push_back(std::make_pair(it->getId(), prob));
                    sum += prob;
                } // end while failing BE

                // Add all transitions
                for (auto it = outgoingTransitions.begin(); it != outgoingTransitions.end(); ++it)
                {
                    ValueType rate = it->second / sum;
                    transitionMatrixBuilder.addNextValue(state.getId(), it->first, rate);
                    //std::cout << "Added transition from " << state.getId() << " to " << it->first << " with " << rate << std::endl;
                }

            } // end while queue
        }

        // Explicitly instantiate the class.
        template
        class ExplicitDFTModelBuilder<double, storm::models::sparse::StandardRewardModel<double>, uint32_t>;

#ifdef STORM_HAVE_CARL
        template class ExplicitDFTModelBuilder<double, storm::models::sparse::StandardRewardModel<storm::Interval>, uint32_t>;
        template class ExplicitDFTModelBuilder<RationalFunction, storm::models::sparse::StandardRewardModel<RationalFunction>, uint32_t>;
#endif

    } // namespace builder
} // namespace storm


