#include "DFTState.h"
#include "storm-dft/storage/dft/DFTElements.h"
#include "storm-dft/storage/dft/DFT.h"

namespace storm {
    namespace storage {

        template<typename ValueType>
        DFTState<ValueType>::DFTState(DFT<ValueType> const& dft, DFTStateGenerationInfo const& stateGenerationInfo, size_t id) : mStatus(dft.stateBitVectorSize()), mId(id), mPseudoState(false), mDft(dft), mStateGenerationInfo(stateGenerationInfo) {
            // TODO Matthias: use construct()
            
            // Initialize uses
            for(size_t spareId : mDft.getSpareIndices()) {
                std::shared_ptr<DFTGate<ValueType> const> elem = mDft.getGate(spareId);
                STORM_LOG_ASSERT(elem->isSpareGate(), "Element is no spare gate.");
                STORM_LOG_ASSERT(elem->nrChildren() > 0, "Element has no child.");
                this->setUses(spareId, elem->children()[0]->id());
            }

            // Initialize activation
            propagateActivation(mDft.getTopLevelIndex());

            std::vector<size_t> alwaysActiveBEs = mDft.nonColdBEs();
            mCurrentlyFailableBE.insert(mCurrentlyFailableBE.end(), alwaysActiveBEs.begin(), alwaysActiveBEs.end());
        }

        template<typename ValueType>
        DFTState<ValueType>::DFTState(storm::storage::BitVector const& status, DFT<ValueType> const& dft, DFTStateGenerationInfo const& stateGenerationInfo, size_t id) : mStatus(status), mId(id), mPseudoState(true), mDft(dft), mStateGenerationInfo(stateGenerationInfo) {
            // Intentionally left empty
        }
        
        template<typename ValueType>
        void DFTState<ValueType>::construct() {
            STORM_LOG_TRACE("Construct concrete state from pseudo state " << mDft.getStateString(mStatus, mStateGenerationInfo, mId));
            // Clear information from pseudo state
            mCurrentlyFailableBE.clear();
            mFailableDependencies.clear();
            mUsedRepresentants.clear();
            STORM_LOG_ASSERT(mPseudoState, "Only pseudo states can be constructed.");
            for(size_t index = 0; index < mDft.nrElements(); ++index) {
                // Initialize currently failable BE
                if (mDft.isBasicElement(index) && isOperational(index)) {
                    std::shared_ptr<const DFTBE<ValueType>> be = mDft.getBasicElement(index);
                    if (be->canFail() && (!be->isColdBasicElement() || !mDft.hasRepresentant(index) || isActive(mDft.getRepresentant(index)))) {
                        mCurrentlyFailableBE.push_back(index);
                        STORM_LOG_TRACE("Currently failable: " << be->toString());
                    }
                } else if (mDft.getElement(index)->isSpareGate()) {
                    // Initialize used representants
                    uint_fast64_t useId = uses(index);
                    mUsedRepresentants.push_back(useId);
                    STORM_LOG_TRACE("Spare " << index << " uses " << useId);
                }
            }

            // Initialize failable dependencies
            for (size_t dependencyId : mDft.getDependencies()) {
                std::shared_ptr<DFTDependency<ValueType> const> dependency = mDft.getDependency(dependencyId);
                STORM_LOG_ASSERT(dependencyId == dependency->id(), "Ids do not match.");
                assert(dependency->dependentEvents().size() == 1);
                if (hasFailed(dependency->triggerEvent()->id()) && getElementState(dependency->dependentEvents()[0]->id()) == DFTElementState::Operational) {
                    mFailableDependencies.push_back(dependencyId);
                    STORM_LOG_TRACE("New dependency failure: " << dependency->toString());
                }
            }
            mPseudoState = false;
        }

        template<typename ValueType>
        std::shared_ptr<DFTState<ValueType>> DFTState<ValueType>::copy() const {
            return std::make_shared<storm::storage::DFTState<ValueType>>(*this);
        }

        template<typename ValueType>
        DFTElementState DFTState<ValueType>::getElementState(size_t id) const {
            return static_cast<DFTElementState>(getElementStateInt(id));
        }
        
        template<typename ValueType>
        DFTDependencyState DFTState<ValueType>::getDependencyState(size_t id) const {
            return static_cast<DFTDependencyState>(getElementStateInt(id));
        }

        template<typename ValueType>
        int DFTState<ValueType>::getElementStateInt(size_t id) const {
            return mStatus.getAsInt(mStateGenerationInfo.getStateIndex(id), 2);
        }
        
        template<typename ValueType>
        int DFTState<ValueType>::getElementStateInt(storm::storage::BitVector const& state, size_t indexId) {
            return state.getAsInt(indexId, 2);
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
            return mStatus[mStateGenerationInfo.getStateIndex(id)];
        }
        
        template<typename ValueType>
        bool DFTState<ValueType>::hasFailed(storm::storage::BitVector const& state, size_t indexId) {
            return state[indexId];
        }

        template<typename ValueType>
        bool DFTState<ValueType>::isFailsafe(size_t id) const {
            return mStatus[mStateGenerationInfo.getStateIndex(id)+1];
        }
        
        template<typename ValueType>
        bool DFTState<ValueType>::isFailsafe(storm::storage::BitVector const& state, size_t indexId) {
            return state[indexId+1];
        }

        template<typename ValueType>
        bool DFTState<ValueType>::dontCare(size_t id) const {
            return getElementState(id) == DFTElementState::DontCare;
        }
        
        template<typename ValueType>
        bool DFTState<ValueType>::dependencyTriggered(size_t id) const {
            return getElementStateInt(id) > 0;
        }
        
        template<typename ValueType>
        bool DFTState<ValueType>::dependencySuccessful(size_t id) const {
            return mStatus[mStateGenerationInfo.getStateIndex(id)];
        }
        template<typename ValueType>
        bool DFTState<ValueType>::dependencyUnsuccessful(size_t id) const {
            return mStatus[mStateGenerationInfo.getStateIndex(id)+1];
        }

        template<typename ValueType>
        void DFTState<ValueType>::setFailed(size_t id) {
            mStatus.set(mStateGenerationInfo.getStateIndex(id));
        }

        template<typename ValueType>
        void DFTState<ValueType>::setFailsafe(size_t id) {
            mStatus.set(mStateGenerationInfo.getStateIndex(id)+1);
        }

        template<typename ValueType>
        void DFTState<ValueType>::setDontCare(size_t id) {
            if (mDft.isRepresentative(id)) {
                // Activate dont care element
                activate(id);
            }
            mStatus.setFromInt(mStateGenerationInfo.getStateIndex(id), 2, static_cast<uint_fast64_t>(DFTElementState::DontCare) );
        }
        
        template<typename ValueType>
        void DFTState<ValueType>::setDependencySuccessful(size_t id) {
            // Only distinguish between passive and dont care dependencies
            //mStatus.set(mStateGenerationInfo.getStateIndex(id));
            setDependencyDontCare(id);
        }

        template<typename ValueType>
        void DFTState<ValueType>::setDependencyUnsuccessful(size_t id) {
            // Only distinguish between passive and dont care dependencies
            //mStatus.set(mStateGenerationInfo.getStateIndex(id)+1);
            setDependencyDontCare(id);
        }
        
        template<typename ValueType>
        void DFTState<ValueType>::setDependencyDontCare(size_t id) {
            mStatus.setFromInt(mStateGenerationInfo.getStateIndex(id), 2, static_cast<uint_fast64_t>(DFTDependencyState::DontCare));
        }

        template<typename ValueType>
        void DFTState<ValueType>::beNoLongerFailable(size_t id) {
            auto it = std::find(mCurrentlyFailableBE.begin(), mCurrentlyFailableBE.end(), id);
            if (it != mCurrentlyFailableBE.end()) {
                mCurrentlyFailableBE.erase(it);
            }
        }

        template<typename ValueType>
        bool DFTState<ValueType>::updateFailableDependencies(size_t id) {
            if (!hasFailed(id)) {
                return false;
            }
            
            for (auto dependency : mDft.getElement(id)->outgoingDependencies()) {
                STORM_LOG_ASSERT(dependency->triggerEvent()->id() == id, "Ids do not match.");
                assert(dependency->dependentEvents().size() == 1);
                if (getElementState(dependency->dependentEvents()[0]->id()) == DFTElementState::Operational) {
                    STORM_LOG_ASSERT(!isFailsafe(dependency->dependentEvents()[0]->id()), "Dependent event is failsafe.");
                    mFailableDependencies.push_back(dependency->id());
                    STORM_LOG_TRACE("New dependency failure: " << dependency->toString());
                }
            }
            return nrFailableDependencies() > 0;
        }
        
        template<typename ValueType>
        void DFTState<ValueType>::updateDontCareDependencies(size_t id) {
            STORM_LOG_ASSERT(mDft.isBasicElement(id), "Element is no BE.");
            STORM_LOG_ASSERT(hasFailed(id), "Element has not failed.");
            
            for (auto dependency : mDft.getBasicElement(id)->ingoingDependencies()) {
                assert(dependency->dependentEvents().size() == 1);
                STORM_LOG_ASSERT(dependency->dependentEvents()[0]->id() == id, "Ids do not match.");
                setDependencyDontCare(dependency->id());
            }
        }

        template<typename ValueType>
        ValueType DFTState<ValueType>::getBERate(size_t id) const {
            STORM_LOG_ASSERT(mDft.isBasicElement(id), "Element is no BE.");

            if (mDft.hasRepresentant(id) && !isActive(mDft.getRepresentant(id))) {
                // Return passive failure rate
                return mDft.getBasicElement(id)->passiveFailureRate();
            } else {
                // Return active failure rate
                return mDft.getBasicElement(id)->activeFailureRate();
            }
        }

        template<typename ValueType>
        ValueType DFTState<ValueType>::getFailableBERate(size_t index) const {
            STORM_LOG_ASSERT(index < nrFailableBEs(), "Index invalid.");
            return getBERate(mCurrentlyFailableBE[index]);
        }

        template<typename ValueType>
        std::pair<std::shared_ptr<DFTBE<ValueType> const>, bool> DFTState<ValueType>::letNextBEFail(size_t index) {
            STORM_LOG_TRACE("currently failable: " << getCurrentlyFailableString());
            if (nrFailableDependencies() > 0) {
                // Consider failure due to dependency
                STORM_LOG_ASSERT(index < nrFailableDependencies(), "Index invalid.");
                std::shared_ptr<DFTDependency<ValueType> const> dependency = mDft.getDependency(mFailableDependencies[index]);
                assert(dependency->dependentEvents().size() == 1);
                std::pair<std::shared_ptr<DFTBE<ValueType> const>,bool> res(mDft.getBasicElement(dependency->dependentEvents()[0]->id()), true);
                mFailableDependencies.erase(mFailableDependencies.begin() + index);
                setFailed(res.first->id());
                setDependencySuccessful(dependency->id());
                return res;
            } else {
                // Consider "normal" failure
                STORM_LOG_ASSERT(index < nrFailableBEs(), "Index invalid.");
                std::pair<std::shared_ptr<DFTBE<ValueType> const>,bool> res(mDft.getBasicElement(mCurrentlyFailableBE[index]), false);
                STORM_LOG_ASSERT(res.first->canFail(), "Element " << *(res.first) << " cannot fail.");
                mCurrentlyFailableBE.erase(mCurrentlyFailableBE.begin() + index);
                setFailed(res.first->id());
                return res;
            }
        }
 
        template<typename ValueType>
        void DFTState<ValueType>::letDependencyBeUnsuccessful(size_t index) {
            STORM_LOG_ASSERT(nrFailableDependencies() > 0 && index < nrFailableDependencies(), "Index invalid.");
            std::shared_ptr<DFTDependency<ValueType> const> dependency = mDft.getDependency(getDependencyId(index));
            mFailableDependencies.erase(mFailableDependencies.begin() + index);
            setDependencyUnsuccessful(dependency->id());
        }

        template<typename ValueType>
        void DFTState<ValueType>::activate(size_t repr) {
            size_t activationIndex = mStateGenerationInfo.getSpareActivationIndex(repr);
            mStatus.set(activationIndex);
        }

        template<typename ValueType>
        bool DFTState<ValueType>::isActive(size_t id) const {
            STORM_LOG_ASSERT(mDft.isRepresentative(id), "Element " << mDft.getElement(id)->toString() << " is no representative.");
            return mStatus[mStateGenerationInfo.getSpareActivationIndex(id)];
        }
            
        template<typename ValueType>
        void DFTState<ValueType>::propagateActivation(size_t representativeId) {
            if (representativeId != mDft.getTopLevelIndex()) {
                activate(representativeId);
            }
            for(size_t elem : mDft.module(representativeId)) {
                if(mDft.isBasicElement(elem) && isOperational(elem)) {
                    std::shared_ptr<const DFTBE<ValueType>> be = mDft.getBasicElement(elem);
                    if (be->isColdBasicElement() && be->canFail()) {
                        // Add to failable BEs
                        mCurrentlyFailableBE.push_back(elem);
                    }
                } else if (mDft.getElement(elem)->isSpareGate() && !isActive(uses(elem))) {
                    propagateActivation(uses(elem));
                }
            }
        }

        template<typename ValueType>
        uint_fast64_t DFTState<ValueType>::uses(size_t id) const {
            size_t nrUsedChild = extractUses(mStateGenerationInfo.getSpareUsageIndex(id));
            if (nrUsedChild == mDft.getMaxSpareChildCount()) {
                return id;
            } else {
                return mDft.getChild(id, nrUsedChild);
            }
        }

        template<typename ValueType>
        uint_fast64_t DFTState<ValueType>::extractUses(size_t from) const {
            STORM_LOG_ASSERT(mStateGenerationInfo.usageInfoBits() < 64, "UsageInfoBit size too large.");
            return mStatus.getAsInt(from, mStateGenerationInfo.usageInfoBits());
        }

        template<typename ValueType>
        bool DFTState<ValueType>::isUsed(size_t child) const {
            return (std::find(mUsedRepresentants.begin(), mUsedRepresentants.end(), child) != mUsedRepresentants.end());
        }

        template<typename ValueType>
        void DFTState<ValueType>::setUses(size_t spareId, size_t child) {
            mStatus.setFromInt(mStateGenerationInfo.getSpareUsageIndex(spareId), mStateGenerationInfo.usageInfoBits(), mDft.getNrChild(spareId, child));
            mUsedRepresentants.push_back(child);
        }
        
        template<typename ValueType>
        void DFTState<ValueType>::finalizeUses(size_t spareId) {
            STORM_LOG_ASSERT(hasFailed(spareId), "Spare has not failed.");
            mStatus.setFromInt(mStateGenerationInfo.getSpareUsageIndex(spareId), mStateGenerationInfo.usageInfoBits(), mDft.getMaxSpareChildCount());
        }
        
        template<typename ValueType>
        bool DFTState<ValueType>::hasOperationalPostSeqElements(size_t id) const {
            STORM_LOG_ASSERT(!mDft.isDependency(id), "Element is dependency.");
            STORM_LOG_ASSERT(!mDft.isRestriction(id), "Element is restriction.");
            auto const& postIds =  mStateGenerationInfo.seqRestrictionPostElements(id);
            for(size_t id : postIds) {
                if(isOperational(id)) {
                    return true;
                }
            }
            return false;
        }

        template<typename ValueType>
        bool DFTState<ValueType>::claimNew(size_t spareId, size_t currentlyUses, std::vector<std::shared_ptr<DFTElement<ValueType>>> const& children) {
            auto it = children.begin();
            while ((*it)->id() != currentlyUses) {
                STORM_LOG_ASSERT(it != children.end(), "Currently used element not found.");
                ++it;
            }
            ++it;
            while(it != children.end()) {
                size_t childId = (*it)->id();
                if(!hasFailed(childId) && !isUsed(childId)) {
                    setUses(spareId, childId);
                    if(isActive(currentlyUses)) {
                        propagateActivation(childId);
                    }
                    return true;
                }
                ++it;
            }
            return false;
        }
        
        template<typename ValueType>
        bool DFTState<ValueType>::orderBySymmetry() {
            bool changed = false;
            for (size_t pos = 0; pos < mStateGenerationInfo.getSymmetrySize(); ++pos) {
                // Check each symmetry
                size_t length = mStateGenerationInfo.getSymmetryLength(pos);
                std::vector<size_t> symmetryIndices = mStateGenerationInfo.getSymmetryIndices(pos);
                // Sort symmetry group in decreasing order by bubble sort
                // TODO use better algorithm?
                size_t tmp;
                size_t n = symmetryIndices.size();
                do {
                    tmp = 0;
                    for (size_t i = 1; i < n; ++i) {
                        if (mStatus.compareAndSwap(symmetryIndices[i-1], symmetryIndices[i], length)) {
                            tmp = i;
                            changed = true;
                        }
                    }
                    n = tmp;
                } while (n > 0);
            }
            if (changed) {
                mPseudoState = true;
            }
            return changed;
        }


        // Explicitly instantiate the class.
        template class DFTState<double>;

#ifdef STORM_HAVE_CARL
        template class DFTState<RationalFunction>;
#endif

    }
}
