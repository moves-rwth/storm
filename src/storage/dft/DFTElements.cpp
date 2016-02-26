#include <src/exceptions/NotImplementedException.h>
#include <src/utility/macros.h>
#include "DFTElements.h"
#include "elements/DFTRestriction.h"

namespace storm {
    namespace storage {

        template<typename ValueType>
        bool DFTElement<ValueType>::checkDontCareAnymore(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const {
            if (state.dontCare(mId)) {
                return false;
            }
            
            // Check that no outgoing dependencies can be triggered anymore
            for (DFTDependencyPointer dependency : mOutgoingDependencies) {
                if (state.isOperational(dependency->dependentEvent()->id()) && state.isOperational(dependency->triggerEvent()->id())) {
                    return false;
                }
            }
            
            bool hasParentSpare = false;

            // Check that no parent can fail anymore
            for(DFTGatePointer const& parent : mParents) {
                if(state.isOperational(parent->id())) {
                    return false;
                }
                if (parent->isSpareGate()) {
                    hasParentSpare = true;
                }
            }
            
            state.setDontCare(mId);
            if (hasParentSpare && !state.isActive(mId)) {
                // Activate child for consistency in failed spares
                state.activate(mId);
            }
            return true;
        }

        template<typename ValueType>
        void DFTElement<ValueType>::extendSpareModule(std::set<size_t>& elementsInModule) const {
            for(auto const& parent : mParents) {
                if(elementsInModule.count(parent->id()) == 0 && !parent->isSpareGate()) {
                    elementsInModule.insert(parent->id());
                    parent->extendSpareModule(elementsInModule);
                }
            }
        }

        template<typename ValueType>
        std::vector<size_t> DFTElement<ValueType>::independentUnit() const {
            std::vector<size_t> res;
            res.push_back(this->id());
            // Extend for pdeps.
            return res;
        }

        template<typename ValueType>
        void DFTElement<ValueType>::extendUnit(std::set<size_t>& unit) const {
            unit.insert(mId);
        }

        template<typename ValueType>
        std::vector<size_t> DFTElement<ValueType>::independentSubDft(bool blockParents) const {
            //std::cout << "INDEPENDENT SUBTREE CALL " << this->id() << std::endl;
            std::vector<size_t> res;
            res.push_back(this->id());
            return res;
        }

        template<typename ValueType>
        void DFTElement<ValueType>::extendSubDft(std::set<size_t>& elemsInSubtree, std::vector<size_t> const& parentsOfSubRoot, bool blockParents) const {
            if(elemsInSubtree.count(this->id()) > 0) return;
            if(std::find(parentsOfSubRoot.begin(), parentsOfSubRoot.end(), mId) != parentsOfSubRoot.end()) {
                // This is a parent of the suspected root, thus it is not a subdft.
                elemsInSubtree.clear();
                return;
            }
            elemsInSubtree.insert(mId);
            for(auto const& parent : mParents) {
                if(blockParents && std::find(parentsOfSubRoot.begin(), parentsOfSubRoot.end(), parent->id()) != parentsOfSubRoot.end()) {
                    continue;
                }
                parent->extendSubDft(elemsInSubtree, parentsOfSubRoot, blockParents);
                if(elemsInSubtree.empty()) {
                    return;
                }
            }
            for(auto const& dep : mOutgoingDependencies) {
                dep->extendSubDft(elemsInSubtree, parentsOfSubRoot, blockParents);
                if(elemsInSubtree.empty()) {
                    return;
                }

            }
            
            for(auto const& restr : mRestrictions) {
                restr->extendSubDft(elemsInSubtree, parentsOfSubRoot, blockParents);
                if(elemsInSubtree.empty()) {
                    return;
                }
            }
            
        }


        template<typename ValueType>
        bool DFTBE<ValueType>::checkDontCareAnymore(storm::storage::DFTState<ValueType>& state, DFTStateSpaceGenerationQueues<ValueType>& queues) const {
            if(DFTElement<ValueType>::checkDontCareAnymore(state, queues)) {
                state.beNoLongerFailable(this->mId);
                return true;
            }
            return false;
        }


        // Explicitly instantiate the class.
        template class DFTBE<double>;

#ifdef STORM_HAVE_CARL
        template class DFTBE<RationalFunction>;
#endif
        
    }
}
