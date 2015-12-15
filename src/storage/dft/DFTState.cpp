#include "DFTState.h"
#include "DFTElements.h"
#include "DFT.h"

namespace storm {
    namespace storage {
        
        DFTState::DFTState(DFT const& dft, size_t id) : mStatus(dft.stateSize()), mId(id), mDft(dft)  {
            mInactiveSpares = dft.getSpareIndices();
            dft.initializeUses(*this);
            dft.initializeActivation(*this);
            std::vector<size_t> alwaysActiveBEs = dft.nonColdBEs();
            mIsCurrentlyFailableBE.insert(mIsCurrentlyFailableBE.end(), alwaysActiveBEs.begin(), alwaysActiveBEs.end());
            
        }
        
        DFTElementState DFTState::getElementState(size_t id) const {
            return static_cast<DFTElementState>(getElementStateInt(id));
        }

        int DFTState::getElementStateInt(size_t id) const {
            return mStatus.getAsInt(mDft.failureIndex(id), 2);
        }

        size_t DFTState::getId() const {
            return mId;
        }

        void DFTState::setId(size_t id) {
            mId = id;
        }

        bool DFTState::isOperational(size_t id) const {
            return getElementState(id) == DFTElementState::Operational;
        }

        bool DFTState::hasFailed(size_t id) const {
            return mStatus[mDft.failureIndex(id)];
        }

        bool DFTState::isFailsafe(size_t id) const {
            return mStatus[mDft.failureIndex(id)+1];
        }

        bool DFTState::dontCare(size_t id) const {
            return getElementState(id) == DFTElementState::DontCare;
        }

        void DFTState::setFailed(size_t id) {
            mStatus.set(mDft.failureIndex(id));
        }

        void DFTState::setFailsafe(size_t id) {
            mStatus.set(mDft.failureIndex(id)+1);
        }

        void DFTState::setDontCare(size_t id) {
            mStatus.setFromInt(mDft.failureIndex(id), 2, static_cast<uint_fast64_t>(DFTElementState::DontCare) );
        }
        
        void DFTState::beNoLongerFailable(size_t id) {
            auto it = std::find(mIsCurrentlyFailableBE.begin(), mIsCurrentlyFailableBE.end(), id);
            if(it != mIsCurrentlyFailableBE.end()) {
                mIsCurrentlyFailableBE.erase(it);
            }
        }

        
        std::pair<std::shared_ptr<DFTBE<double>>, bool> DFTState::letNextBEFail(size_t index) 
        {
            assert(index < mIsCurrentlyFailableBE.size());
            //std::cout << "currently failable: ";
            //printCurrentlyFailable();
            std::pair<std::shared_ptr<DFTBE<double>>,bool> res(mDft.getBasicElement(mIsCurrentlyFailableBE[index]), false);
            mIsCurrentlyFailableBE.erase(mIsCurrentlyFailableBE.begin() + index);
            setFailed(res.first->id());
            return res;
        }
        
        void DFTState::activate(size_t repr) {
            std::vector<size_t>  const& module = mDft.module(repr);
            for(size_t elem : module) {
                if(mDft.getElement(elem)->isColdBasicElement() && isOperational(elem)) {
                    mIsCurrentlyFailableBE.push_back(elem);
                }
                else if(mDft.getElement(elem)->isSpareGate()) {
                    assert(std::find(mInactiveSpares.begin(), mInactiveSpares.end(), elem) != mInactiveSpares.end());
                    mInactiveSpares.erase(std::find(mInactiveSpares.begin(), mInactiveSpares.end(), elem));
                }
            }
        }
        
        
        bool DFTState::isActiveSpare(size_t id) const {
            assert(mDft.getElement(id)->isSpareGate());
            return (std::find(mInactiveSpares.begin(), mInactiveSpares.end(), id) == mInactiveSpares.end());
        }
        
        uint_fast64_t DFTState::uses(size_t id) const {
            return extractUses(mDft.usageIndex(id));
        }
        
        uint_fast64_t DFTState::extractUses(size_t from) const {
            assert(mDft.usageInfoBits() < 64);
            return mStatus.getAsInt(from, mDft.usageInfoBits());
        }
        
        bool DFTState::isUsed(size_t child) {
            return (std::find(mUsedRepresentants.begin(), mUsedRepresentants.end(), child) != mUsedRepresentants.end());
            
        }
        
        void DFTState::setUsesAtPosition(size_t usageIndex, size_t child) {
            mStatus.setFromInt(usageIndex, mDft.usageInfoBits(), child);
            mUsedRepresentants.push_back(child);
        }
        
        bool DFTState::claimNew(size_t spareId, size_t usageIndex, size_t currentlyUses, std::vector<size_t> const& childIds) {
            auto it = find(childIds.begin(), childIds.end(), currentlyUses);
            assert(it != childIds.end());
            ++it;
            while(it != childIds.end()) {
                if(!hasFailed(*it) && !isUsed(*it)) {
                    setUsesAtPosition(usageIndex, *it);
                    if(isActiveSpare(spareId)) {
                        mDft.propagateActivation(*this,*it);
                    }
                    return true;
                }
                ++it;
            }
            return false;
        }

    }
}
