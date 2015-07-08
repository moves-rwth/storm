#ifndef DFTSTATESPACEGENERATIONQUEUES_H
#define	DFTSTATESPACEGENERATIONQUEUES_H

#include <list>
#include <queue>
#include <vector>
#include <deque>

#include "OrderDFTElementsById.h"

namespace storm {
    namespace storage {
        class DFTGate;
        class DFTElement;
        
    
        
        class DFTStateSpaceGenerationQueues {
            std::priority_queue<std::shared_ptr<DFTGate>, std::vector<std::shared_ptr<DFTGate>>, OrderElementsByRank> failurePropagation;
            std::vector<std::shared_ptr<DFTGate>> failsafePropagation;
            std::vector<std::shared_ptr<DFTElement>> dontcarePropagation;
            std::vector<std::shared_ptr<DFTElement>> activatePropagation;

        public:
            void propagateFailure(std::shared_ptr<DFTGate> const& elem) {
                failurePropagation.push(elem);
            }

            bool failurePropagationDone() const {
                return failurePropagation.empty();
            }

            std::shared_ptr<DFTGate> nextFailurePropagation() {
                std::shared_ptr<DFTGate> next= failurePropagation.top();
                failurePropagation.pop();
                return next;
            }
            
            bool failsafePropagationDone() const {
                return failsafePropagation.empty();
            }
            
            void propagateFailsafe(std::shared_ptr<DFTGate> const& gate) {
                failsafePropagation.push_back(gate);
            }
            
            std::shared_ptr<DFTGate> nextFailsafePropagation() {
                std::shared_ptr<DFTGate> next = failsafePropagation.back();
                failsafePropagation.pop_back();
                return next;
            }
            
            bool dontCarePropagationDone() const {
                return dontcarePropagation.empty();
            }
            
            void propagateDontCare(std::shared_ptr<DFTElement> const& elem) {
                dontcarePropagation.push_back(elem);
            }
            
            void propagateDontCare(std::vector<std::shared_ptr<DFTElement>> const& elems) {
                dontcarePropagation.insert(dontcarePropagation.end(), elems.begin(), elems.end());
            }
            
            std::shared_ptr<DFTElement> nextDontCarePropagation() {
                std::shared_ptr<DFTElement> next = dontcarePropagation.back();
                dontcarePropagation.pop_back();
                return next;
            }
        };
    }
    
}



#endif	/* DFTSTATESPACEGENERATIONQUEUES_H */

