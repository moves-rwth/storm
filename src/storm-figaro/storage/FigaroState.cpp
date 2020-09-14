#include "FigaroState.h"
#include "storm-dft/storage/dft/DFTElements.h"
#include "storm-dft/storage/dft/DFT.h"
#include "storm/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace figaro {
        namespace storage {

//            template<typename ValueType>
//            FigaroState<ValueType>::FigaroState(DFT <ValueType> const &dft, FigaroStateGenerationInfo const &stateGenerationInfo,
//                                          size_t id) : mStatus(dft.stateBitVectorSize()), mId(id),
//                                                       failableElements(dft.nrElements()), indexRelevant(0),
//                                                       mPseudoState(false), mDft(dft),
//                                                       mStateGenerationInfo(stateGenerationInfo) {
//                // TODO: use construct()
//
//                // Initialize uses
//                for (size_t spareId : mDft.getSpareIndices()) {
//                    std::shared_ptr<DFTGate < ValueType> const> elem = mDft.getGate(spareId);
//                    STORM_LOG_ASSERT(elem->isSpareGate(), "Element is no spare gate.");
//                    STORM_LOG_ASSERT(elem->nrChildren() > 0, "Element has no child.");
//                    this->setUses(spareId, elem->children()[0]->id());
//                }
//
//                // Initialize activation
//                propagateActivation(mDft.getTopLevelIndex());
//
//                // Initialize currently failable BEs
//                for (size_t id : mDft.nonColdBEs()) {
//                    // Check if restriction might prevent failure
//                    if (!isEventDisabledViaRestriction(id)) {
//                        failableElements.addBE(id);
//                    } else {
//                        STORM_LOG_TRACE("BE " << id << " is disabled due to a restriction.");
//                    }
//                }
//            }

//            template<typename ValueType>
//            FigaroState<ValueType>::FigaroState(storm::storage::BitVector const &status, DFT <ValueType> const &dft,
//                                          FigaroStateGenerationInfo const &stateGenerationInfo, size_t id) : mStatus(
//                    status), mId(id), failableElements(dft.nrElements()), indexRelevant(0), mPseudoState(true),
//                                                                                                          mDft(dft),
//                                                                                                          mStateGenerationInfo(
//                                                                                                                  stateGenerationInfo) {
//                // Intentionally left empty
//            }

            template<typename ValueType>
            FigaroState<ValueType>::FigaroState(storm::storage::BitVector const &status, size_t id):
            mStatus(status), mId(id),mPseudoState(false){ //shahid: we are not coonsidering any pseudo state
                std::cout<<"new state created";
            }


//            template<typename ValueType>
//            void FigaroState<ValueType>::construct() {
//                STORM_LOG_TRACE("Construct concrete state from pseudo state "
//                                        << mDft.getStateString(mStatus, mStateGenerationInfo, mId));
//                // Clear information from pseudo state
//                failableElements.clear();
//                mUsedRepresentants.clear();
//                STORM_LOG_ASSERT(mPseudoState, "Only pseudo states can be constructed.");
//                for (size_t index = 0; index < mDft.nrElements(); ++index) {
//                    // Initialize currently failable BE
//                    if (mDft.isBasicElement(index) && isOperational(index) && !isEventDisabledViaRestriction(index)) {
//                        std::shared_ptr<const DFTBE <ValueType>> be = mDft.getBasicElement(index);
//                        if (be->canFail()) {
//                            switch (be->beType()) {
//                                case storm::storage::BEType::CONSTANT:
//                                    failableElements.addBE(index);
//                                    STORM_LOG_TRACE("Currently failable: " << *be);
//                                    break;
//                                case storm::storage::BEType::EXPONENTIAL: {
//                                    auto beExp = std::static_pointer_cast<BEExponential < ValueType> const>(be);
//                                    if (!beExp->isColdBasicElement() || !mDft.hasRepresentant(index) ||
//                                        isActive(mDft.getRepresentant(index))) {
//                                        failableElements.addBE(index);
//                                        STORM_LOG_TRACE("Currently failable: " << *beExp);
//                                    }
//                                    break;
//                                }
//                                default:
//                                    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException,
//                                                    "BE type '" << be->type() << "' is not supported.");
//                                    break;
//                            }
//                        }
//                    } else if (mDft.getElement(index)->isSpareGate()) {
//                        // Initialize used representants
//                        uint_fast64_t useId = uses(index);
//                        mUsedRepresentants.push_back(useId);
//                        STORM_LOG_TRACE("Spare " << index << " uses " << useId);
//                    }
//                }
//
//                // Initialize failable dependencies
//                for (size_t dependencyId : mDft.getDependencies()) {
//                    std::shared_ptr<DFTDependency < ValueType> const> dependency = mDft.getDependency(dependencyId);
//                    STORM_LOG_ASSERT(dependencyId == dependency->id(), "Ids do not match.");
//                    assert(dependency->dependentEvents().size() == 1);
//                    if (hasFailed(dependency->triggerEvent()->id()) &&
//                        getElementState(dependency->dependentEvents()[0]->id()) == DFTElementState::Operational) {
//                        failableElements.addDependency(dependency->id(), mDft.isDependencyInConflict(dependency->id()));
//                        STORM_LOG_TRACE("New dependency failure: " << *dependency);
//                    }
//                }
//
//                mPseudoState = false;
//            }
//
//            template<typename ValueType>
//            std::shared_ptr<FigaroState < ValueType>>
//
//            FigaroState<ValueType>::copy() const {
//                return std::make_shared<storm::storage::FigaroState<ValueType>>(*this);
//            }
//
//            template<typename ValueType>
//            DFTElementState FigaroState<ValueType>::getElementState(size_t id) const {
//                return static_cast<DFTElementState>(getElementStateInt(id));
//            }
//
//            template<typename ValueType>
//            DFTElementState FigaroState<ValueType>::getElementState(storm::storage::BitVector const &state,
//                                                                 FigaroStateGenerationInfo const &stateGenerationInfo,
//                                                                 size_t id) {
//                return static_cast<DFTElementState>(FigaroState<ValueType>::getElementStateInt(state, stateGenerationInfo,
//                                                                                            id));
//            }
//
//
//
//            template<typename ValueType>
//            int FigaroState<ValueType>::getElementStateInt(size_t id) const {
//                return mStatus.getAsInt(mStateGenerationInfo.getStateIndex(id), 2);
//            }
//
//            template<typename ValueType>
//            int FigaroState<ValueType>::getElementStateInt(storm::storage::BitVector const &state,
//                                                        FigaroStateGenerationInfo const &stateGenerationInfo, size_t id) {
//                return state.getAsInt(stateGenerationInfo.getStateIndex(id), 2);
//            }
//
            template<typename ValueType>
            size_t FigaroState<ValueType>::getId() const {
                return mId;
            }

            template<typename ValueType>
            void FigaroState<ValueType>::setId(size_t id) {
                mId = id;
            }
//
//
//
//            template<typename ValueType>
//            bool FigaroState<ValueType>::hasFailed(size_t id) const {
//                return mStatus[mStateGenerationInfo.getStateIndex(id)];
//            }
//
//            template<typename ValueType>
//            bool FigaroState<ValueType>::hasFailed(storm::storage::BitVector const &state, size_t indexId) {
//                return state[indexId];
//            }

    // Explicitly instantiate the class.
    template
    class FigaroState<double>;
}
}
}
