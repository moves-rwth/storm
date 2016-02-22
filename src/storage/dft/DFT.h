
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

        
        class DFTStateGenerationInfo {
        private:
            const size_t mUsageInfoBits;
            std::map<size_t, size_t> mSpareUsageIndex; // id spare -> index first bit in state
            std::map<size_t, size_t> mSpareActivationIndex; // id spare representative -> index in state
            std::vector<size_t> mIdToStateIndex; // id -> index first bit in state
            
        public:
            
            DFTStateGenerationInfo(size_t nrElements) : mUsageInfoBits(nrElements > 1 ? storm::utility::math::uint64_log2(nrElements-1) + 1 : 1), mIdToStateIndex(nrElements) {
            }

            size_t usageInfoBits() const {
                return mUsageInfoBits;
            }
            
            void addStateIndex(size_t id, size_t index) {
                assert(id < mIdToStateIndex.size());
                mIdToStateIndex[id] = index;
            }

            void addSpareActivationIndex(size_t id, size_t index) {
                mSpareActivationIndex[id] = index;
            }

            void addSpareUsageIndex(size_t id, size_t index) {
                mSpareUsageIndex[id] = index;
            }

            size_t getStateIndex(size_t id) const {
                assert(id < mIdToStateIndex.size());
                return mIdToStateIndex[id];
            }
            
            size_t getSpareUsageIndex(size_t id) const {
                assert(mSpareUsageIndex.count(id) > 0);
                return mSpareUsageIndex.at(id);
            }
            
            size_t getSpareActivationIndex(size_t id) const {
                assert(mSpareActivationIndex.count(id) > 0);
                return mSpareActivationIndex.at(id);
            }
            
            friend std::ostream& operator<<(std::ostream& os, DFTStateGenerationInfo const& info) {
                os << "Id to state index:" << std::endl;
                for (size_t id = 0; id < info.mIdToStateIndex.size(); ++id) {
                    os << id << " -> " << info.getStateIndex(id) << std::endl;
                }
                os << "Spare usage index with usage InfoBits of size " << info.mUsageInfoBits << ":" << std::endl;
                for (auto pair : info.mSpareUsageIndex) {
                    os << pair.first << " -> " << pair.second << std::endl;
                }
                os << "Spare activation index:" << std::endl;
                for (auto pair : info.mSpareActivationIndex) {
                    os << pair.first << " -> " << pair.second << std::endl;
                }
                return os;
            }
            
        };
        

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
            std::map<size_t, std::vector<size_t>> mSpareModules;
            std::vector<size_t> mDependencies;
            std::vector<size_t> mTopModule;
            std::map<size_t, size_t> mRepresentants; // id element -> id representative
            std::vector<std::vector<size_t>> mSymmetries;
            
        public:
            DFT(DFTElementVector const& elements, DFTElementPointer const& tle);
            
            DFTStateGenerationInfo buildStateGenerationInfo(storm::storage::DFTIndependentSymmetries const& symmetries) const;

            size_t performStateGenerationInfoDFS(DFTStateGenerationInfo& generationInfo, std::queue<size_t>& visitQueue, std::set<size_t>& visited, size_t stateIndex) const;
            
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

