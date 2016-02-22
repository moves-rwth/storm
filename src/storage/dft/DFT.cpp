#include <boost/container/flat_set.hpp>

#include <map>
#include "DFT.h"
#include "src/exceptions/NotSupportedException.h"

#include "DFTIsomorphism.h"
#include "utility/iota_n.h"

namespace storm {
    namespace storage {

        template<typename ValueType>
        DFT<ValueType>::DFT(DFTElementVector const& elements, DFTElementPointer const& tle) : mElements(elements), mNrOfBEs(0), mNrOfSpares(0), mTopLevelIndex(tle->id()) {
            assert(elementIndicesCorrect());
            size_t nrRepresentatives = 0;
            
            for (auto& elem : mElements) {
                if (isRepresentative(elem->id())) {
                    ++nrRepresentatives;
                }
                if(elem->isBasicElement()) {
                    ++mNrOfBEs;
                }
                else if (elem->isSpareGate()) {
                    ++mNrOfSpares;
                    bool firstChild = true;
                    for(auto const& spareReprs : std::static_pointer_cast<DFTSpare<ValueType>>(elem)->children()) {
                        std::set<size_t> module = {spareReprs->id()};
                        spareReprs->extendSpareModule(module);
                        std::vector<size_t> sparesAndBes;
                        for(size_t modelem : module) {
                            if (spareReprs->id() != modelem && (isRepresentative(modelem) || (!firstChild && mTopLevelIndex == modelem))) {
                                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Module for '" << spareReprs->name() << "' contains more than one representative.");
                            }
                            if(mElements[modelem]->isSpareGate() || mElements[modelem]->isBasicElement()) {
                                sparesAndBes.push_back(modelem);
                                mRepresentants.insert(std::make_pair(modelem, spareReprs->id()));
                            }
                        }
                        mSpareModules.insert(std::make_pair(spareReprs->id(), sparesAndBes));
                        firstChild = false;
                    }

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
            
            size_t usageInfoBits = mElements.size() > 1 ? storm::utility::math::uint64_log2(mElements.size()-1) + 1 : 1;
            mStateVectorSize = nrElements() * 2 + mNrOfSpares * usageInfoBits + nrRepresentatives;
        }

        template<typename ValueType>
        DFTStateGenerationInfo DFT<ValueType>::buildStateGenerationInfo(storm::storage::DFTIndependentSymmetries const& symmetries) const {
            // Use symmetry
            // Collect all elements in the first subtree
            // TODO make recursive to use for nested subtrees
            
            DFTStateGenerationInfo generationInfo(nrElements());

            // Perform DFS and insert all elements of subtree sequentially
            size_t stateIndex = 0;
            std::queue<size_t> visitQueue;
            std::set<size_t> visited;
            size_t firstRoot;
            if (symmetries.groups.empty()) {
                firstRoot = mTopLevelIndex;
            } else {
                firstRoot = symmetries.groups.begin()->first;
            }
            visitQueue.push(firstRoot);
            stateIndex = performStateGenerationInfoDFS(generationInfo, visitQueue, visited, stateIndex);
            
            // Consider dependencies
            for (size_t idDependency : getDependencies()) {
                std::shared_ptr<DFTDependency<ValueType> const> dependency = getDependency(idDependency);
                visitQueue.push(dependency->id());
                visitQueue.push(dependency->triggerEvent()->id());
                visitQueue.push(dependency->dependentEvent()->id());
            }
            stateIndex = performStateGenerationInfoDFS(generationInfo, visitQueue, visited, stateIndex);
            
            assert(stateIndex = mStateVectorSize);

            STORM_LOG_TRACE(generationInfo);
            
            return generationInfo;
        }
        
        template<typename ValueType>
        size_t DFT<ValueType>::performStateGenerationInfoDFS(DFTStateGenerationInfo& generationInfo, std::queue<size_t>& visitQueue, std::set<size_t>& visited, size_t stateIndex) const {
            while (!visitQueue.empty()) {
                size_t id = visitQueue.front();
                visitQueue.pop();
                if (visited.count(id) == 1) {
                    // Already visited
                    continue;
                }
                visited.insert(id);
                DFTElementPointer element = mElements[id];
                
                // Insert children
                if (element->isGate()) {
                    for (auto const& child : std::static_pointer_cast<DFTGate<ValueType>>(element)->children()) {
                        visitQueue.push(child->id());
                    }
                }
                
                // Reserve bits for element
                generationInfo.addStateIndex(id, stateIndex);
                stateIndex += 2;
                
                if (isRepresentative(id)) {
                    generationInfo.addSpareActivationIndex(id, stateIndex);
                    ++stateIndex;
                }
                
                if (element->isSpareGate()) {
                    generationInfo.addSpareUsageIndex(id, stateIndex);
                    stateIndex += generationInfo.usageInfoBits();
                }
                
            }
            return stateIndex;
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
                        size_t useId = state->uses(elem->id());
                        if(state->isActive(useId)) {
                            stream << " actively ";
                        }
                        stream << " using " << useId;
                    }
                }
                stream << std::endl;
            }
            return stream.str();
        }

        // TODO rewrite to only use bitvector and id
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
                        size_t useId = state->uses(elem->id());
                        if(state->isActive(useId)) {
                            stream << " actively ";
                        }
                        stream << "using " << useId << "]";
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
        std::vector<size_t> DFT<ValueType>::immediateFailureCauses(size_t index) const {
            if(isGate(index)) {

            } else {
                return {index};
            }
        }

        template<typename ValueType>
        DFTColouring<ValueType> DFT<ValueType>::colourDFT() const {
            return DFTColouring<ValueType>(*this);
        }


        template<typename ValueType>
        DFTIndependentSymmetries DFT<ValueType>::findSymmetries(DFTColouring<ValueType> const& colouring) const {
            std::vector<size_t> vec;
            vec.reserve(nrElements());
            storm::utility::iota_n(std::back_inserter(vec), nrElements(), 0);
            BijectionCandidates<ValueType> completeCategories = colouring.colourSubdft(vec);
            std::map<size_t, std::vector<std::vector<size_t>>> res;
            
            for(auto const& colourClass : completeCategories.gateCandidates) {
                if(colourClass.second.size() > 1) {
                    std::set<size_t> foundEqClassFor;
                    for(auto it1 = colourClass.second.cbegin(); it1 != colourClass.second.cend(); ++it1) {
                        std::vector<std::vector<size_t>> symClass;
                        if(foundEqClassFor.count(*it1) > 0) {
                            continue;
                        }
                        if(!getGate(*it1)->hasOnlyStaticParents()) {
                            continue;
                        }
                        
                        std::pair<std::vector<size_t>, std::vector<size_t>> influencedElem1Ids = getSortedParentAndOutDepIds(*it1);
                        auto it2 = it1;
                        for(++it2; it2 != colourClass.second.cend(); ++it2) {
                            if(!getGate(*it2)->hasOnlyStaticParents()) {
                                continue;
                            }
                            std::vector<size_t> sortedParent2Ids = getGate(*it2)->parentIds();
                            std::sort(sortedParent2Ids.begin(), sortedParent2Ids.end());
                            
                            if(influencedElem1Ids == getSortedParentAndOutDepIds(*it2)) {
                                std::cout << "Considering ids " << *it1 << ", " << *it2 << " for isomorphism." << std::endl;
                                bool isSymmetry = false;
                                std::vector<size_t> isubdft1 = getGate(*it1)->independentSubDft();
                                std::vector<size_t> isubdft2 = getGate(*it2)->independentSubDft();
                                if(isubdft1.empty() || isubdft2.empty() || isubdft1.size() != isubdft2.size()) {
                                    continue;
                                }
                                std::cout << "Checking subdfts from " << *it1 << ", " << *it2 << " for isomorphism." << std::endl;
                                auto LHS = colouring.colourSubdft(isubdft1);
                                auto RHS = colouring.colourSubdft(isubdft2);
                                auto IsoCheck = DFTIsomorphismCheck<ValueType>(LHS, RHS, *this);
                                isSymmetry = IsoCheck.findIsomorphism();
                                if(isSymmetry) {
                                    std::cout << "subdfts are symmetric" << std::endl;
                                    foundEqClassFor.insert(*it2);
                                    if(symClass.empty()) {
                                        for(auto const& i : isubdft1) {
                                            symClass.push_back(std::vector<size_t>({i}));
                                        }
                                    }
                                    auto symClassIt = symClass.begin();
                                    for(auto const& i : isubdft1) {
                                        symClassIt->emplace_back(IsoCheck.getIsomorphism().at(i));
                                        ++symClassIt;
                                        
                                    }
                                    
                                }
                            }
                        }
                        if(!symClass.empty()) {
                            res.emplace(*it1, symClass);
                        }
                    }
                    
                }
            }
            return DFTIndependentSymmetries(res);
        }

        template<typename ValueType>
        std::pair<std::vector<size_t>, std::vector<size_t>> DFT<ValueType>::getSortedParentAndOutDepIds(size_t index) const {
            std::pair<std::vector<size_t>, std::vector<size_t>> res;
            res.first = getElement(index)->parentIds();
            std::sort(res.first.begin(), res.first.end());
            for(auto const& dep : getElement(index)->outgoingDependencies()) {
                res.second.push_back(dep->id());
            }
            std::sort(res.second.begin(), res.second.end());
            return res;
        }
        

        // Explicitly instantiate the class.
        template class DFT<double>;

#ifdef STORM_HAVE_CARL
        template class DFT<RationalFunction>;
#endif

    }
}
