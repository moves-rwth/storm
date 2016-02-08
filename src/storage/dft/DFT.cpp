#include "DFT.h"

namespace storm {
    namespace storage {

        template<typename ValueType>
        DFT<ValueType>::DFT(DFTElementVector const& elements, DFTElementPointer const& tle) : mElements(elements), mNrOfBEs(0), mNrOfSpares(0)
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
                    for(auto const& spareReprs : std::static_pointer_cast<DFTSpare<ValueType>>(elem)->children()) {
                        if(mActivationIndex.count(spareReprs->id()) == 0) {
                            mActivationIndex[spareReprs->id()] =  stateIndex++;
                        }
                        std::set<size_t> module = {spareReprs->id()};
                        spareReprs->extendSpareModule(module);
                        std::vector<size_t> sparesAndBes;
                        for(auto const& modelem : module) {
                            if(mElements[modelem]->isSpareGate() || mElements[modelem]->isBasicElement()) {
                                sparesAndBes.push_back(modelem);
                                mRepresentants.insert(std::make_pair(modelem, spareReprs->id()));
                            }
                        }
                        mSpareModules.insert(std::make_pair(spareReprs->id(), sparesAndBes));

                    }
                    std::static_pointer_cast<DFTSpare<ValueType>>(elem)->setUseIndex(stateIndex);
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

        template<typename ValueType>
        std::string DFT<ValueType>::getElementsString() const {
            std::stringstream stream;
            for (auto const& elem : mElements) {
                stream << "[" << elem->id() << "]" << elem->toString() << std::endl;
            }
            return stream.str();
        }

        template<typename ValueType>
        std::string DFT<ValueType>::getInfoString() const {
            std::stringstream stream;
            stream << "Top level index: " << mTopLevelIndex << ", Nr BEs" << mNrOfBEs;
            return stream.str();
        }

        template<typename ValueType>
        std::string DFT<ValueType>::getSpareModulesString() const {
            std::stringstream stream;
            stream << "[" << mElements[mTopLevelIndex]->id() << "] {";
            std::vector<size_t>::const_iterator it = mTopModule.begin();
            assert(it != mTopModule.end());
            stream << mElements[(*it)]->name();
            ++it;
            while(it != mTopModule.end()) {
                stream <<  ", " << mElements[(*it)]->name();
                ++it;
            }
            stream << "}" << std::endl;

            for(auto const& spareModule : mSpareModules) {
                stream << "[" << mElements[spareModule.first]->name() << "] = {";
                std::vector<size_t>::const_iterator it = spareModule.second.begin();
                assert(it != spareModule.second.end());
                stream << mElements[(*it)]->name();
                ++it;
                while(it != spareModule.second.end()) {
                    stream <<  ", " << mElements[(*it)]->name();
                    ++it;
                }
                stream << "}" << std::endl;
            }
            return stream.str();
        }

        template<typename ValueType>
        std::string DFT<ValueType>::getElementsWithStateString(DFTStatePointer const& state) const{
            std::stringstream stream;
            for (auto const& elem : mElements) {
                stream << "[" << elem->id() << "]";
                stream << elem->toString();
                stream << "\t** " << state->getElementState(elem->id());
                if(elem->isSpareGate()) {
                    if(state->isActiveSpare(elem->id())) {
                        stream << " actively";
                    }
                    stream << " using " << state->uses(elem->id());
                } 
                stream << std::endl;
            }
            return stream.str();
        }

        template<typename ValueType>
        std::string DFT<ValueType>::getStateString(DFTStatePointer const& state) const{
            std::stringstream stream;
            stream << "(" << state->getId() << ") ";
            for (auto const& elem : mElements) {
                stream << state->getElementStateInt(elem->id());
                if(elem->isSpareGate()) {
                    stream << "[";
                    if(state->isActiveSpare(elem->id())) {
                        stream << "actively ";
                    }
                    stream << "using " << state->uses(elem->id()) << "]";
                }
            }
            return stream.str();
        }

        // Explicitly instantiate the class.
        template class DFT<double>;

#ifdef STORM_HAVE_CARL
        template class DFT<RationalFunction>;
#endif

    }
}
