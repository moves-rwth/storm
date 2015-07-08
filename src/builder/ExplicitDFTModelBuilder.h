#ifndef EXPLICITDFTMODELBUILDER_H
#define	EXPLICITDFTMODELBUILDER_H

#include "../storage/dft/DFT.h"

namespace storm {
    namespace builder {
        class ExplicitDFTModelBuilder {
            storm::storage::DFT const& mDft;
            std::unordered_map<storm::storage::DFTState, size_t> mStateIndices;
            size_t newIndex = 0;
            //std::stack<std::shared_ptr<storm::storage::DFTState>> mStack;
            
        public:
            ExplicitDFTModelBuilder(storm::storage::DFT const& dft) : mDft(dft)
            {
                
            }
            
            
            void exploreStateSuccessors(storm::storage::DFTState const& state) {
                size_t smallest = 0; 
                
                while(smallest < state.nrFailableBEs()) {
                    //std::cout << "exploring from :" << std::endl;
                    //mDft.printElementsWithState(state);
                    //std::cout << "***************" << std::endl;
                    storm::storage::DFTState newState(state);
                    std::pair<std::shared_ptr<storm::storage::DFTBE<double>>, bool> nextBE = newState.letNextBEFail(smallest++);
                    if(nextBE.first == nullptr) {
                        //std::cout << "break" << std::endl;
                        break;
                        
                    }
                    //std::cout << "with the failure of: " << nextBE.first->name() << std::endl;
                        
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
                    
                    
                    
                    if(!mDft.hasFailed(newState) && !mDft.isFailsafe(newState)) {
                        auto it = mStateIndices.find(newState);
                        if(it == mStateIndices.end()) {
                            exploreStateSuccessors(newState);
                            mStateIndices.insert(std::make_pair(newState, newIndex++));
                            if(newIndex % 16384 == 0) std::cout << newIndex << std::endl;
                        } 
                        
                    }
                    else {
                        //std::cout << "done." << std::endl;
                    }

                    
                
                }
            }
            
            void buildCtmc() {
                storm::storage::DFTState state(mDft);
                exploreStateSuccessors(state);
                std::cout << mStateIndices.size() << std::endl;
            }
            
        };
    }
}


#endif	/* EXPLICITDFTMODELBUILDER_H */

