
#ifndef DFT_H
#define	DFT_H


#include "DFTElements.h"
#include "../BitVector.h"
#include <memory>
#include <unordered_map>
#include <list>
#include <map>

#include "../../utility/math.h"
#include <boost/iterator/counting_iterator.hpp>

namespace storm {
    namespace storage {

        struct DFTElementSort {
            bool operator()(std::shared_ptr<DFTElement> const& a, std::shared_ptr<DFTElement> const& b)  const {
                if (a->rank() == 0 && b->rank() == 0) {
                    return a->isConstant();
                } else {
                    return a->rank() < b->rank();
                }
            }
        };
        
        class DFT {

        private:
            std::vector<std::shared_ptr<DFTElement>> mElements;
            size_t mNrOfBEs;
            size_t mNrOfSpares;
            size_t mTopLevelIndex;
            size_t mUsageInfoBits;
            size_t mStateSize;
            std::map<size_t, size_t> mActivationIndex;
            std::map<size_t, std::vector<size_t>> mSpareModules;
            std::vector<size_t> mTopModule;
            std::vector<size_t> mIdToFailureIndex;
            std::map<size_t, size_t> mUsageIndex;
            
        public:
            DFT(std::vector<std::shared_ptr<DFTElement>> const& elements, std::shared_ptr<DFTElement> const& tle);
            
            
            size_t stateSize() const {
                return mStateSize;
            }
            
            size_t nrElements() const {
                return mElements.size();
            }
            
            size_t nrBasicElements() const {
                return mNrOfBEs;
            }
            
            size_t usageInfoBits() const {
                return mUsageInfoBits;
            }
            
            size_t usageIndex(size_t id) const {
                assert(mUsageIndex.find(id) != mUsageIndex.end());
                return mUsageIndex.find(id)->second;
            }
            
            size_t failureIndex(size_t id) const {
                return mIdToFailureIndex[id];
            }
            
            void initializeUses(DFTState& state) const {
                for(auto const& elem : mElements) {
                    if(elem->isSpareGate()) {
                        std::static_pointer_cast<DFTSpare>(elem)->initializeUses(state);
                    }
                }
            }
            
            void initializeActivation(DFTState& state) const {
                state.activate(mTopLevelIndex);
                for(auto const& elem : mTopModule) {
                    if(mElements[elem]->isSpareGate()) {
                        propagateActivation(state, state.uses(elem));
                    }
                }
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
            
            
            void propagateActivation(DFTState& state, size_t representativeId) const {
                state.activate(representativeId);
                for(size_t id : module(representativeId)) {
                    if(mElements[id]->isSpareGate()) {
                        propagateActivation(state, state.uses(id));
                    }
                }
            }
            
            std::vector<size_t> nonColdBEs() const {
                std::vector<size_t> result;
                for(std::shared_ptr<DFTElement> elem : mElements) {
                    if(elem->isBasicElement() && !elem->isColdBasicElement()) {
                        result.push_back(elem->id());
                    }
                }
                return result;
            }
            
            std::shared_ptr<DFTElement> const& getElement(size_t index) const {
                assert(index < nrElements());
                return mElements[index];
            }
            
            std::shared_ptr<DFTBE<double>> getBasicElement(size_t index) const {
                assert(mElements[index]->isBasicElement());
                return std::static_pointer_cast<DFTBE<double>>(mElements[index]);
            }
            
            bool hasFailed(DFTState const& state) const {
                return state.hasFailed(mTopLevelIndex);
            }
            
            bool isFailsafe(DFTState const& state) const {
                return state.isFailsafe(mTopLevelIndex);
            }
                    
            
            void printElements(std::ostream& os = std::cout) const;
            
            void printInfo(std::ostream& os = std::cout) const;
            
            void printSpareModules(std::ostream& os = std::cout) const;
            
            void printElementsWithState(DFTState const& state, std::ostream& os = std::cout) const; 
            
        private:
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

