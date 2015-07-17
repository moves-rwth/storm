#include "DFTElements.h"

namespace storm {
    namespace storage {
        bool DFTElement::checkDontCareAnymore(storm::storage::DFTState& state, DFTStateSpaceGenerationQueues& queues) const {
            if(!state.dontCare(mId))
            {
                for(std::shared_ptr<DFTGate> const& parent : mParents) {
                    if(state.isOperational(parent->id())) {
                        return false;
                    }
                }
                state.setDontCare(mId);
                return true;

            }
            return false;
        }
        
        void DFTElement::extendSpareModule(std::set<size_t>& elementsInModule) const {
            for(auto const& parent : mParents) {
                if(elementsInModule.count(parent->id()) == 0 && !parent->isSpareGate()) {
                    elementsInModule.insert(parent->id());
                    parent->extendSpareModule(elementsInModule);
                }
            }
        }
        
        void DFTElement::extendUnit(std::set<size_t>& unit) const {
            unit.insert(mId);
            for(auto const& parent : mParents) {
                if(unit.count(parent->id()) != 0) {
                    parent->extendUnit(unit);
                }
            }
        }
        
        void DFTElement::checkForSymmetricChildren() const {
            
        }
        
        template<>
        bool DFTBE<double>::checkDontCareAnymore(storm::storage::DFTState& state, DFTStateSpaceGenerationQueues& queues) const {
            if(DFTElement::checkDontCareAnymore(state, queues)) {
                state.beNoLongerFailable(mId);
                return true;
            }
            return false;
        }
        
    }
}
