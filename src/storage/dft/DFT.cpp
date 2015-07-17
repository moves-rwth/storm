#include "DFT.h"

namespace storm {
    namespace storage {
        
        DFT::DFT(std::vector<std::shared_ptr<DFTElement>> const& elements, std::shared_ptr<DFTElement> const& tle) : mElements(elements), mNrOfBEs(0), mNrOfSpares(0)
        {
            assert(elementIndicesCorrect());

            size_t stateIndex = 0;
            mUsageInfoBits = storm::utility::math::uint64_log2(mElements.size()-1)+1;

            for(auto& elem : mElements) {
                mIdToFailureIndex.push_back(stateIndex);
                stateIndex += 2;
                if(elem->isBasicElement()) {
                    ++mNrOfBEs;
                }
                else if(elem->isSpareGate()) {
                    ++mNrOfSpares;
                    for(auto const& spareReprs : std::static_pointer_cast<DFTSpare>(elem)->children()) {
                        if(mActivationIndex.count(spareReprs->id()) == 0) {
                            mActivationIndex[spareReprs->id()] =  stateIndex++;
                        }
                        std::set<size_t> module = {spareReprs->id()};
                        spareReprs->extendSpareModule(module);
                        std::vector<size_t> sparesAndBes;
                        for(auto const& modelem : module) {
                            if(mElements[modelem]->isSpareGate() || mElements[modelem]->isBasicElement()) {
                                sparesAndBes.push_back(modelem);
                            }
                        }
                        mSpareModules.insert(std::make_pair(spareReprs->id(), sparesAndBes));

                    }
                    std::static_pointer_cast<DFTSpare>(elem)->setUseIndex(stateIndex);
                    mUsageIndex.insert(std::make_pair(elem->id(), stateIndex));
                    stateIndex += mUsageInfoBits;

                }
            }

            // For the top module, we assume, contrary to [Jun15], that we have all spare gates and basic elements which are not in another module.
            std::set<size_t> topModuleSet;
            // Initialize with all ids.
            for(auto const& elem : mElements) {
                if (elem->isBasicElement() || elem->isSpareGate()) {
                    topModuleSet.insert(elem->id());
                }
            }

            for(auto const& module : mSpareModules) {
                for(auto const& index : module.second) {
                    topModuleSet.erase(index);
                }
            }
            mTopModule = std::vector<size_t>(topModuleSet.begin(), topModuleSet.end());

            mStateSize = stateIndex;
            mTopLevelIndex = tle->id();

        }
        
        void DFT::printElements(std::ostream& os) const {
            for (auto const& elem : mElements) {
                elem->print(os);
                os << std::endl;
            }
        }
        
        

        void DFT::printInfo(std::ostream& os) const {
            os << "Top level index: " << mTopLevelIndex << std::endl << "Nr BEs" << mNrOfBEs << std::endl;
        }

        void DFT::printSpareModules(std::ostream& os) const {
            std::cout << "[" << mElements[mTopLevelIndex] << "] {";
            std::vector<size_t>::const_iterator it = mTopModule.begin();
            assert(it != mTopModule.end());
            os << mElements[(*it)]->name();
            ++it;
            while(it != mTopModule.end()) {
                os <<  ", " << mElements[(*it)]->name();
                ++it;
            }
            os << "}" << std::endl;

            for(auto const& spareModule : mSpareModules) {
                std::cout << "[" << mElements[spareModule.first]->name() << "] = {";
                os.flush();
                std::vector<size_t>::const_iterator it = spareModule.second.begin();
                assert(it != spareModule.second.end());
                os << mElements[(*it)]->name();
                ++it;
                while(it != spareModule.second.end()) {
                    os <<  ", " << mElements[(*it)]->name();
                    os.flush();
                    ++it;
                }
                os << "}" << std::endl;
            }
        }

        void DFT::printElementsWithState(DFTState const& state, std::ostream& os) const{
            for (auto const& elem : mElements) {
                os << "[" << elem->id() << "]";
                elem->print(os);
                os << "\t** " << state.getElementState(elem->id());
                if(elem->isSpareGate()) {
                    if(state.isActiveSpare(elem->id())) {
                        os << " actively";
                    }
                    os << " using " << state.uses(elem->id());
                } 
                std::cout << std::endl;


            }
        }
    }
}
