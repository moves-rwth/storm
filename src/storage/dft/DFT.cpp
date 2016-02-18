#include <boost/container/flat_set.hpp>

#include "DFT.h"
#include "src/exceptions/NotSupportedException.h"

#include "DFTIsomorphism.h"
#include "utility/iota_n.h"

namespace storm {
    namespace storage {

        template<typename ValueType>
        DFT<ValueType>::DFT(DFTElementVector const& elements, DFTElementPointer const& tle) : mElements(elements), mNrOfBEs(0), mNrOfSpares(0)
        {
            assert(elementIndicesCorrect());

            size_t stateIndex = 0;
            mUsageInfoBits = storm::utility::math::uint64_log2(mElements.size()-1)+1;

            for (auto& elem : mElements) {
                mIdToFailureIndex.push_back(stateIndex);
                stateIndex += 2;
                if(elem->isBasicElement()) {
                    ++mNrOfBEs;
                }
                else if (elem->isSpareGate()) {
                    ++mNrOfSpares;
                    for(auto const& spareReprs : std::static_pointer_cast<DFTSpare<ValueType>>(elem)->children()) {
                        if(mActivationIndex.count(spareReprs->id()) == 0) {
                            mActivationIndex[spareReprs->id()] =  stateIndex++;
                        }
                        std::set<size_t> module = {spareReprs->id()};
                        spareReprs->extendSpareModule(module);
                        std::vector<size_t> sparesAndBes;
                        bool secondSpare = false;
                        for(auto const& modelem : module) {
                            if (mElements[modelem]->isSpareGate()) {
                                STORM_LOG_THROW(!secondSpare, storm::exceptions::NotSupportedException, "Module for '" << spareReprs->name() << "' contains more than one spare.");
                                secondSpare = true;
                            }
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

                } else if (elem->isDependency()) {
                    mDependencies.push_back(elem->id());
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
                if (elem->isDependency()) {
                    stream << "\t** " << storm::storage::toChar(state->getDependencyState(elem->id()));
                } else {
                    stream << "\t** " << storm::storage::toChar(state->getElementState(elem->id()));
                    if(elem->isSpareGate()) {
                        if(state->isActiveSpare(elem->id())) {
                            stream << " actively";
                        }
                        stream << " using " << state->uses(elem->id());
                    }
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
                if (elem->isDependency()) {
                    stream << storm::storage::toChar(state->getDependencyState(elem->id())) << "[dep]";
                } else {
                    stream << storm::storage::toChar(state->getElementState(elem->id()));
                    if(elem->isSpareGate()) {
                        stream << "[";
                        if(state->isActiveSpare(elem->id())) {
                            stream << "actively ";
                        }
                        stream << "using " << state->uses(elem->id()) << "]";
                    }
                }
            }
            return stream.str();
        }

        template <typename ValueType>
        std::vector<size_t> DFT<ValueType>::getIndependentSubDftRoots(size_t index) const {
            auto elem = getElement(index);
            auto ISD = elem->independentSubDft();
            return ISD;
        }

        template<typename ValueType>
        DFTColouring<ValueType> DFT<ValueType>::colourDFT() const {
            return DFTColouring<ValueType>(*this);
        }

        template<typename ValueType>
        std::vector<std::vector<size_t>> DFT<ValueType>::findSymmetries(DFTColouring<ValueType> const& colouring) const {
            std::vector<size_t> vec;
            vec.reserve(nrElements());
            storm::utility::iota_n(std::back_inserter(vec), nrElements(), 0);
            BijectionCandidates<ValueType> completeCategories = colouring.colourSubdft(vec);
            std::vector<std::vector<size_t>> res;
            
            for(auto const& colourClass : completeCategories.gateCandidates) {
                if(colourClass.second.size() > 1) {
                    std::vector<size_t> handledWithinClass;
                    for(auto it1 = colourClass.second.cbegin(); it1 != colourClass.second.cend(); ++it1) {
                        if(!getGate(*it1)->hasOnlyStaticParents()) {
                            continue;
                        }
                        std::vector<size_t> sortedParent1Ids = getGate(*it1)->parentIds();
                        std::sort(sortedParent1Ids.begin(), sortedParent1Ids.end());
                        auto it2 = it1;
                        for(++it2; it2 != colourClass.second.cend(); ++it2) {
                            if(!getGate(*it2)->hasOnlyStaticParents()) {
                                continue;
                            }
                            std::vector<size_t> sortedParent2Ids = getGate(*it2)->parentIds();
                            std::sort(sortedParent2Ids.begin(), sortedParent2Ids.end());
                            if(sortedParent1Ids == sortedParent2Ids) {
                                std::cout << "Considering ids " << *it1 << ", " << *it2 << " for isomorphism." << std::endl;
                                bool isSymmetry = false;
                                std::vector<size_t> isubdft1 = getGate(*it1)->independentSubDft();
                                std::vector<size_t> isubdft2 = getGate(*it2)->independentSubDft();
                                if(!isubdft1.empty() && !isubdft2.empty() && isubdft1.size() == isubdft2.size()) {
                                    std::cout << "Checking subdfts from " << *it1 << ", " << *it2 << " for isomorphism." << std::endl;
                                    auto LHS = colouring.colourSubdft(isubdft1);
                                    auto RHS = colouring.colourSubdft(isubdft2);
                                    auto IsoCheck = DFTIsomorphismCheck<ValueType>(LHS, RHS, *this);
                                    isSymmetry = IsoCheck.findIsomorphism();
                                }
                                if(isSymmetry) {
                                    std::cout << "subdfts are symmetric" << std::endl;
                                    
                                }
                            }
                        }
                    }
                }
            }
            return res;
        }


        // Explicitly instantiate the class.
        template class DFT<double>;

#ifdef STORM_HAVE_CARL
        template class DFT<RationalFunction>;
#endif

    }
}
