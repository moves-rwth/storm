#include "src/builder/ExplicitDFTModelBuilder.h"

namespace storm {
    namespace builder {

        void ExplicitDFTModelBuilder::exploreStateSuccessors(storm::storage::DFTState const &state) {
            size_t smallest = 0;

            while(smallest < state.nrFailableBEs()) {
                //std::cout << "exploring from: " << mDft.getStateString(state) << std::endl;

                storm::storage::DFTState newState(state);
                std::pair<std::shared_ptr<storm::storage::DFTBE<double>>, bool> nextBE = newState.letNextBEFail(smallest++);
                if(nextBE.first == nullptr) {
                    //std::cout << "break" << std::endl;
                    break;

                }
                //std::cout << "with the failure of: " << nextBE.first->name() << " [" << nextBE.first->id() << "]" << std::endl;

                storm::storage::DFTStateSpaceGenerationQueues queues;

                for(std::shared_ptr<storm::storage::DFTGate> parent : nextBE.first->parents()) {
                    if(newState.isOperational(parent->id())) {
                        queues.propagateFailure(parent);
                    }
                }

                while(!queues.failurePropagationDone()) {
                    std::shared_ptr<storm::storage::DFTGate> next = queues.nextFailurePropagation();
                    next->checkFails(newState, queues);
                }

                while(!queues.failsafePropagationDone()) {
                    std::shared_ptr<storm::storage::DFTGate> next = queues.nextFailsafePropagation();
                    next->checkFailsafe(newState, queues);
                }

                while(!queues.dontCarePropagationDone()) {
                    std::shared_ptr<storm::storage::DFTElement> next = queues.nextDontCarePropagation();
                    next->checkDontCareAnymore(newState, queues);
                }

                auto it = mStates.find(newState);
                if (it == mStates.end()) {
                    // New state
                    newState.setId(newIndex++);
                    mStates.insert(newState);
                    //std::cout << "New state " << mDft.getStateString(newState) << std::endl;

                    // Recursive call
                    if(!mDft.hasFailed(newState) && !mDft.isFailsafe(newState)) {
                        exploreStateSuccessors(newState);
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
            }
        }

    } // namespace builder
} // namespace storm


