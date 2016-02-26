
#ifndef DFT_H
#define	DFT_H

#include <memory>
#include <unordered_map>
#include <list>
#include <map>

#include <boost/iterator/counting_iterator.hpp>

#include "DFTElements.h"
#include "elements/DFTRestriction.h"
#include "../BitVector.h"
#include "SymmetricUnits.h"
#include "../../utility/math.h"
#include "src/utility/macros.h"
#include "DFTStateGenerationInfo.h"

namespace storm {
    namespace storage {

        template<typename ValueType>
        struct DFTElementSort {
            bool operator()(std::shared_ptr<DFTElement<ValueType>> const& a, std::shared_ptr<DFTElement<ValueType>> const& b)  const {
                if (a->rank() == 0 && b->rank() == 0) {
                    return a->isConstant();
                } else {
                    return a->rank() < b->rank();
                }
            }
        };


        // Forward declarations
        template<typename T> class DFTColouring;

        
        

        /**
         * Represents a Dynamic Fault Tree
         */
        template<typename ValueType>
        class DFT {

            using DFTElementPointer = std::shared_ptr<DFTElement<ValueType>>;
            using DFTElementCPointer = std::shared_ptr<DFTElement<ValueType> const>;
            using DFTElementVector = std::vector<DFTElementPointer>;
            using DFTGatePointer = std::shared_ptr<DFTGate<ValueType>>;
            using DFTGateVector = std::vector<DFTGatePointer>;
            using DFTStatePointer = std::shared_ptr<DFTState<ValueType>>;

        private:
            DFTElementVector mElements;
            size_t mNrOfBEs;
            size_t mNrOfSpares;
            size_t mTopLevelIndex;
            size_t mStateVectorSize;
            size_t mMaxSpareChildCount;
            std::map<size_t, std::vector<size_t>> mSpareModules;
            std::vector<size_t> mDependencies;
            std::vector<size_t> mTopModule;
            std::map<size_t, size_t> mRepresentants; // id element -> id representative
            std::vector<std::vector<size_t>> mSymmetries;
            
        public:
            DFT(DFTElementVector const& elements, DFTElementPointer const& tle);
            
            DFTStateGenerationInfo buildStateGenerationInfo(storm::storage::DFTIndependentSymmetries const& symmetries) const;
            
            size_t generateStateInfo(DFTStateGenerationInfo& generationInfo, size_t id, storm::storage::BitVector& visited, size_t stateIndex) const;

            size_t performStateGenerationInfoDFS(DFTStateGenerationInfo& generationInfo, std::queue<size_t>& visitQueue, storm::storage::BitVector& visited, size_t stateIndex) const;
            
            size_t stateVectorSize() const {
                return mStateVectorSize;
            }
            
            size_t nrElements() const {
                return mElements.size();
            }
            
            size_t nrBasicElements() const {
                return mNrOfBEs;
            }
            
            size_t getTopLevelIndex() const {
                return mTopLevelIndex;
            }
            
            size_t getMaxSpareChildCount() const {
                return mMaxSpareChildCount;
            }
            
            std::vector<size_t> getSpareIndices() const {
                std::vector<size_t> indices;
                for(auto const& elem : mElements) {
                    if(elem->isSpareGate()) {
                        indices.push_back(elem->id());
                    }
                }
                return indices;
            }
            
            std::vector<size_t> const& module(size_t representativeId) const {
                if(representativeId == mTopLevelIndex) {
                    return mTopModule;
                } else {
                    assert(mSpareModules.count(representativeId)>0);
                    return mSpareModules.find(representativeId)->second;
                }
            }
            
            std::vector<size_t> const& getDependencies() const {
                return mDependencies;
            }

            std::vector<size_t> nonColdBEs() const {
                std::vector<size_t> result;
                for(DFTElementPointer elem : mElements) {
                    if(elem->isBasicElement() && !elem->isColdBasicElement()) {
                        result.push_back(elem->id());
                    }
                }
                return result;
            }

            /**
             *  Get a pointer to an element in the DFT
             *  @param index The id of the element
             */
            DFTElementCPointer getElement(size_t index) const {
                assert(index < nrElements());
                return mElements[index];
            }

            bool isBasicElement(size_t index) const {
                return getElement(index)->isBasicElement();
            }

            bool isGate(size_t index) const {
                return getElement(index)->isGate();
            }

            bool isDependency(size_t index) const {
                return getElement(index)->isDependency();
            }
            
            bool isRestriction(size_t index) const {
                return getElement(index)->isRestriction();
            }

            std::shared_ptr<DFTBE<ValueType> const> getBasicElement(size_t index) const {
                assert(isBasicElement(index));
                return std::static_pointer_cast<DFTBE<ValueType> const>(mElements[index]);
            }

            std::shared_ptr<DFTGate<ValueType> const> getGate(size_t index) const {
                assert(isGate(index));
                return std::static_pointer_cast<DFTGate<ValueType> const>(mElements[index]);
            }

            std::shared_ptr<DFTDependency<ValueType> const> getDependency(size_t index) const {
                assert(isDependency(index));
                return std::static_pointer_cast<DFTDependency<ValueType> const>(mElements[index]);
            }

            std::vector<std::shared_ptr<DFTBE<ValueType>>> getBasicElements() const {
                std::vector<std::shared_ptr<DFTBE<ValueType>>> elements;
                for (DFTElementPointer elem : mElements) {
                    if (elem->isBasicElement()) {
                        elements.push_back(std::static_pointer_cast<DFTBE<ValueType>>(elem));
                    }
                }
                return elements;
            }
            
            bool isRepresentative(size_t id) const {
                for (auto const& parent : getElement(id)->parents()) {
                    if (parent->isSpareGate()) {
                        return true;
                    }
                }
                return false;
            }

            bool hasRepresentant(size_t id) const {
                return mRepresentants.find(id) != mRepresentants.end();
            }

            DFTElementCPointer getRepresentant(size_t id) const {
                assert(hasRepresentant(id));
                return getElement(mRepresentants.find(id)->second);
            }

            bool hasFailed(DFTStatePointer const& state) const {
                return state->hasFailed(mTopLevelIndex);
            }
            
            bool hasFailed(storm::storage::BitVector const& state, DFTStateGenerationInfo const& stateGenerationInfo) const {
                return storm::storage::DFTState<ValueType>::hasFailed(state, stateGenerationInfo.getStateIndex(mTopLevelIndex));
            }
            
            bool isFailsafe(DFTStatePointer const& state) const {
                return state->isFailsafe(mTopLevelIndex);
            }
            
            bool isFailsafe(storm::storage::BitVector const& state, DFTStateGenerationInfo const& stateGenerationInfo) const {
                return storm::storage::DFTState<ValueType>::isFailsafe(state, stateGenerationInfo.getStateIndex(mTopLevelIndex));
            }
            
            size_t getChild(size_t spareId, size_t nrUsedChild) const;
            
            size_t getNrChild(size_t spareId, size_t childId) const;
            
            std::string getElementsString() const;

            std::string getInfoString() const;

            std::string getSpareModulesString() const;

            std::string getElementsWithStateString(DFTStatePointer const& state) const;

            std::string getStateString(DFTStatePointer const& state) const;

            std::vector<size_t> getIndependentSubDftRoots(size_t index) const;

            DFTColouring<ValueType> colourDFT() const;

            DFTIndependentSymmetries findSymmetries(DFTColouring<ValueType> const& colouring) const;

            std::vector<size_t> immediateFailureCauses(size_t index) const;
        private:
            std::pair<std::vector<size_t>, std::vector<size_t>> getSortedParentAndOutDepIds(size_t index) const;
            
            bool elementIndicesCorrect() const {
                for(size_t i = 0; i < mElements.size(); ++i) {
                    if(mElements[i]->id() != i) return false;
                }
                return true;
            }

        };
       
    }
}
#endif	/* DFT_H */

