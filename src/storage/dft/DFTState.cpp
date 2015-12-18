#include "DFTState.h"
#include "DFTElements.h"
#include "DFT.h"

namespace storm {
    namespace storage {

        template<typename ValueType>
        DFTState<ValueType>::DFTState(DFT<ValueType> const& dft, size_t id) : mStatus(dft.stateSize()), mId(id), mDft(dft)  {
            mInactiveSpares = dft.getSpareIndices();
            dft.initializeUses(*this);
            dft.initializeActivation(*this);
            std::vector<size_t> alwaysActiveBEs = dft.nonColdBEs();
            mIsCurrentlyFailableBE.insert(mIsCurrentlyFailableBE.end(), alwaysActiveBEs.begin(), alwaysActiveBEs.end());
            
        }

        template<typename ValueType>
        DFTElementState DFTState<ValueType>::getElementState(size_t id) const {
            return static_cast<DFTElementState>(getElementStateInt(id));
        }

        template<typename ValueType>
        int DFTState<ValueType>::getElementStateInt(size_t id) const {
            return mStatus.getAsInt(mDft.failureIndex(id), 2);
        }

        template<typename ValueType>
        size_t DFTState<ValueType>::getId() const {
            return mId;
        }

        template<typename ValueType>
        void DFTState<ValueType>::setId(size_t id) {
            mId = id;
        }

        template<typename ValueType>
        bool DFTState<ValueType>::isOperational(size_t id) const {
            return getElementState(id) == DFTElementState::Operational;
        }

        template<typename ValueType>
        bool DFTState<ValueType>::hasFailed(size_t id) const {
            return mStatus[mDft.failureIndex(id)];
        }

        template<typename ValueType>
        bool DFTState<ValueType>::isFailsafe(size_t id) const {
            return mStatus[mDft.failureIndex(id)+1];
        }

        template<typename ValueType>
        bool DFTState<ValueType>::dontCare(size_t id) const {
            return getElementState(id) == DFTElementState::DontCare;
        }

        template<typename ValueType>
        void DFTState<ValueType>::setFailed(size_t id) {
            mStatus.set(mDft.failureIndex(id));
        }

        template<typename ValueType>
        void DFTState<ValueType>::setFailsafe(size_t id) {
            mStatus.set(mDft.failureIndex(id)+1);
        }

        template<typename ValueType>
        void DFTState<ValueType>::setDontCare(size_t id) {
            mStatus.setFromInt(mDft.failureIndex(id), 2, static_cast<uint_fast64_t>(DFTElementState::DontCare) );
        }

        template<typename ValueType>
        void DFTState<ValueType>::beNoLongerFailable(size_t id) {
            auto it = std::find(mIsCurrentlyFailableBE.begin(), mIsCurrentlyFailableBE.end(), id);
            if(it != mIsCurrentlyFailableBE.end()) {
                mIsCurrentlyFailableBE.erase(it);
            }
        }

        template<typename ValueType>
        std::pair<std::shared_ptr<DFTBE<ValueType>>, bool> DFTState<ValueType>::letNextBEFail(size_t index)
        {
            assert(index < mIsCurrentlyFailableBE.size());
            STORM_LOG_TRACE("currently failable: " << getCurrentlyFailableString());
            std::pair<std::shared_ptr<DFTBE<ValueType>>,bool> res(mDft.getBasicElement(mIsCurrentlyFailableBE[index]), false);
            mIsCurrentlyFailableBE.erase(mIsCurrentlyFailableBE.begin() + index);
            setFailed(res.first->id());
            return res;
        }

        template<typename ValueType>
        void DFTState<ValueType>::activate(size_t repr) {
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

        template<typename ValueType>
        bool DFTState<ValueType>::isActiveSpare(size_t id) const {
            assert(mDft.getElement(id)->isSpareGate());
            return (std::find(mInactiveSpares.begin(), mInactiveSpares.end(), id) == mInactiveSpares.end());
        }

        template<typename ValueType>
        uint_fast64_t DFTState<ValueType>::uses(size_t id) const {
            return extractUses(mDft.usageIndex(id));
        }

        template<typename ValueType>
        uint_fast64_t DFTState<ValueType>::extractUses(size_t from) const {
            assert(mDft.usageInfoBits() < 64);
            return mStatus.getAsInt(from, mDft.usageInfoBits());
        }

        template<typename ValueType>
        bool DFTState<ValueType>::isUsed(size_t child) {
            return (std::find(mUsedRepresentants.begin(), mUsedRepresentants.end(), child) != mUsedRepresentants.end());
        }

        template<typename ValueType>
        void DFTState<ValueType>::setUsesAtPosition(size_t usageIndex, size_t child) {
            mStatus.setFromInt(usageIndex, mDft.usageInfoBits(), child);
            mUsedRepresentants.push_back(child);
        }

        template<typename ValueType>
        bool DFTState<ValueType>::claimNew(size_t spareId, size_t usageIndex, size_t currentlyUses, std::vector<size_t> const& childIds) {
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

        // Explicitly instantiate the class.
        template class DFTState<double>;

#ifdef STORM_HAVE_CARL
        template class DFTState<RationalFunction>;
#endif

    }
}