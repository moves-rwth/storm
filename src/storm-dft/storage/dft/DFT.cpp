#include "DFT.h"

#include <map>

#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/WrongFormatException.h"
#include "storm/utility/iota_n.h"
#include "storm/utility/vector.h"

#include "storm-dft/builder/DFTBuilder.h"
#include "storm-dft/storage/dft/DFTIsomorphism.h"


namespace storm {
    namespace storage {

        template<typename ValueType>
        DFT<ValueType>::DFT(DFTElementVector const &elements, DFTElementPointer const &tle) :
                mElements(elements), mNrOfBEs(0), mNrOfSpares(0), mNrRepresentatives(0), mTopLevelIndex(tle->id()), mMaxSpareChildCount(0) {
            // Check that ids correspond to indices in the element vector
            STORM_LOG_ASSERT(elementIndicesCorrect(), "Ids incorrect.");

            // Initialize dynamic behavior vector with TRUE to preserve correct behavior
            // We don't directly call setDynamicBehaviorInfo to not slow down DFT generation if possible
            mDynamicBehavior = std::vector<bool>(mElements.size());
            std::fill(mDynamicBehavior.begin(), mDynamicBehavior.end(), true);

            for (auto& elem : mElements) {
                if (isRepresentative(elem->id())) {
                    ++mNrRepresentatives;
                }
                if (elem->isBasicElement()) {
                    ++mNrOfBEs;
                } else if (elem->isSpareGate()) {
                    // Build spare modules by setting representatives and representants
                    ++mNrOfSpares;
                    mMaxSpareChildCount = std::max(mMaxSpareChildCount, std::static_pointer_cast<DFTSpare<ValueType>>(elem)->children().size());
                    for (auto const& spareReprs : std::static_pointer_cast<DFTSpare<ValueType>>(elem)->children()) {
                        STORM_LOG_THROW(spareReprs->isGate() || spareReprs->isBasicElement(), storm::exceptions::WrongFormatException,
                                        "Child '" << spareReprs->name() << "' of spare '" << elem->name() << "' must be gate or BE.");
                        std::set<size_t> module = {spareReprs->id()};
                        spareReprs->extendSpareModule(module);
                        std::vector<size_t> sparesAndBes;
                        for (size_t modelem : module) {
                            if (mElements[modelem]->isSpareGate() || mElements[modelem]->isBasicElement()) {
                                sparesAndBes.push_back(modelem);
                                mRepresentants.insert(std::make_pair(modelem, spareReprs->id()));
                            }
                        }
                        mSpareModules.insert(std::make_pair(spareReprs->id(), sparesAndBes));
                    }
                } else if (elem->isDependency()) {
                    mDependencies.push_back(elem->id());
                    mDependencyInConflict.insert(std::make_pair(elem->id(), true));
                }
            }

            // For the top module, we assume, contrary to [Jun15], that we have all spare gates and basic elements which are not in another module.
            std::set<size_t> topModuleSet;
            // Initialize with all ids.
            for (auto const& elem : mElements) {
                if (elem->isBasicElement() || elem->isSpareGate()) {
                    topModuleSet.insert(elem->id());
                }
            }
            // Erase spare modules
            for (auto const& module : mSpareModules) {
                for (auto const& index : module.second) {
                    topModuleSet.erase(index);
                }
            }
            // Extend top module and insert those elements which are part of the top module and a spare module
            mElements[mTopLevelIndex]->extendSpareModule(topModuleSet);
            mTopModule = std::vector<size_t>(topModuleSet.begin(), topModuleSet.end());

            // Clear all spare modules where at least one element is also in the top module.
            // These spare modules will be activated from the beginning.
            if (!mTopModule.empty()) {
                for (auto& module : mSpareModules) {
                    if (std::find(module.second.begin(), module.second.end(), mTopModule.front()) != module.second.end()) {
                        STORM_LOG_WARN("Elements of spare module '" << getElement(module.first)->name()
                                                                    << "' also contained in top module. All elements of this spare module will be activated from the beginning on.");
                        module.second.clear();
                    }
                }
            }

            //Reserve space for failed spares
            ++mMaxSpareChildCount;
            mStateVectorSize = DFTStateGenerationInfo::getStateVectorSize(nrElements(), mNrOfSpares, mNrRepresentatives, mMaxSpareChildCount);
        }

        template<typename ValueType>
        void DFT<ValueType>::setDynamicBehaviorInfo() {
            std::vector<bool> dynamicBehaviorVector(mElements.size(), false);

            std::queue <DFTElementPointer> elementQueue;

            // deal with all dynamic elements
            for (auto const &element : mElements) {
                switch (element->type()) {
                    case storage::DFTElementType::PAND:
                    case storage::DFTElementType::POR:
                    case storage::DFTElementType::MUTEX: {
                        auto gate = std::static_pointer_cast<storm::storage::DFTChildren<ValueType>>(element);
                        dynamicBehaviorVector[gate->id()] = true;
                        for (auto const &child : gate->children()) {
                            // only enqueue static children
                            if (!dynamicBehaviorVector.at(child->id())) {
                                elementQueue.push(child);
                            }
                        }
                        break;
                    }
                        // TODO different cases
                    case storage::DFTElementType::SPARE: {
                        auto spare = std::static_pointer_cast<storm::storage::DFTSpare<ValueType>>(element);

                        // Iterate over all children (representatives of spare modules)
                        for (auto const &child : spare->children()) {
                            // Case 1: Shared Module
                            // If child only has one parent, it is this SPARE -> nothing to check
                            if (child->nrParents() > 1) {
                                // TODO make more efficient by directly setting ALL spares which share a module to be dynamic
                                for (auto const &parent : child->parents()) {
                                    if (parent->isSpareGate() and parent->id() != spare->id()) {
                                        dynamicBehaviorVector[spare->id()] = true;
                                        break; // inner loop
                                    }
                                }
                            }
                            // Case 2: Triggering outside events
                            // If the SPARE was already detected to have dynamic behavior, do not proceed
                            if (!dynamicBehaviorVector[spare->id()]) {
                                for (auto const &memberID : module(child->id())) {
                                    // Iterate over all members of the module child represents
                                    auto member = getElement(memberID);
                                    for (auto const dep : member->outgoingDependencies()) {
                                        // If the member has outgoing dependencies, check if those trigger something outside the module
                                        for (auto const depEvent : dep->dependentEvents()) {
                                            // If a dependent event is not found in the module, SPARE is dynamic
                                            if (std::find(module(child->id()).begin(), module(child->id()).end(),
                                                          depEvent->id()) == module(child->id()).end()) {
                                                dynamicBehaviorVector[spare->id()] = true;
                                                break; //depEvent-loop
                                            }
                                        }
                                        if (dynamicBehaviorVector[spare->id()]) { break; } //dependency-loop
                                    }
                                    if (dynamicBehaviorVector[spare->id()]) { break; } //module-loop
                                }

                            }
                            if (dynamicBehaviorVector[spare->id()]) { break; } //child-loop
                        }
                        // if during the computation, dynamic behavior was detected, add children to queue
                        if (dynamicBehaviorVector[spare->id()]) {
                            for (auto const &child : spare->children()) {
                                // only enqueue static children
                                if (!dynamicBehaviorVector.at(child->id())) {
                                    elementQueue.push(child);
                                }
                            }
                        }
                        break;
                    }
                    case storage::DFTElementType::SEQ: {
                        auto seq = std::static_pointer_cast<storm::storage::DFTSeq<ValueType>>(element);
                        // A SEQ only has dynamic behavior if not all children are BEs
                        if (!seq->allChildrenBEs()) {
                            dynamicBehaviorVector[seq->id()] = true;
                            for (auto const &child : seq->children()) {
                                // only enqueue static children
                                if (!dynamicBehaviorVector.at(child->id())) {
                                    elementQueue.push(child);
                                }
                            }
                        }
                        break;
                    }
                    default: {
                        break;
                    }

                }
            }
            // propagate dynamic behavior
            while (!elementQueue.empty()) {
                DFTElementPointer currentElement = elementQueue.front();
                elementQueue.pop();
                switch (currentElement->type()) {
                    // Static Gates
                    case storage::DFTElementType::AND:
                    case storage::DFTElementType::OR:
                    case storage::DFTElementType::VOT: {
                        // check all parents and if one has dynamic behavior, propagate it
                        dynamicBehaviorVector[currentElement->id()] = true;
                        auto gate = std::static_pointer_cast<storm::storage::DFTGate<ValueType>>(currentElement);
                        for (auto const &child : gate->children()) {
                            // only enqueue static children
                            if (!dynamicBehaviorVector.at(child->id())) {
                                elementQueue.push(child);
                            }
                        }
                        break;
                    }
                        //BEs
                    case storage::DFTElementType::BE_EXP:
                    case storage::DFTElementType::BE_CONST:
                    case storage::DFTElementType::BE: {
                        auto be = std::static_pointer_cast<storm::storage::DFTBE<ValueType>>(currentElement);
                        dynamicBehaviorVector[be->id()] = true;
                        // add all ingoing dependencies to queue
                        for (auto const &dep : be->ingoingDependencies()) {
                            if (!dynamicBehaviorVector.at(dep->id())) {
                                elementQueue.push(dep);
                            }
                        }
                        break;
                    }
                    case storage::DFTElementType::PDEP: {
                        auto dep = std::static_pointer_cast<storm::storage::DFTDependency<ValueType>>(currentElement);
                        dynamicBehaviorVector[dep->id()] = true;
                        // add all ingoing dependencies to queue
                        auto trigger = dep->triggerEvent();
                        if (!dynamicBehaviorVector.at(trigger->id())) {
                            elementQueue.push(trigger);
                        }
                        break;
                    }
                    default:
                        break;
                }
            }
            mDynamicBehavior = dynamicBehaviorVector;
        }

        template<typename ValueType>
        DFTStateGenerationInfo DFT<ValueType>::buildStateGenerationInfo(storm::storage::DFTIndependentSymmetries const& symmetries) const {
            DFTStateGenerationInfo generationInfo(nrElements(), mNrOfSpares, mNrRepresentatives, mMaxSpareChildCount);

            // Generate Pre and Post info for restrictions, and mutexes
            for (auto const& elem : mElements) {
                if (!elem->isDependency() && !elem->isRestriction()) {
                    generationInfo.setRestrictionPreElements(elem->id(), elem->seqRestrictionPres());
                    generationInfo.setRestrictionPostElements(elem->id(), elem->seqRestrictionPosts());
                    generationInfo.setMutexElements(elem->id(), elem->mutexRestrictionElements());
                }
            }

            size_t stateIndex = 0;
            std::queue<size_t> visitQueue;
            storm::storage::BitVector visited(nrElements(), false);

            if (symmetries.groups.empty()) {
                // Perform DFS for whole tree
                visitQueue.push(mTopLevelIndex);
                stateIndex = performStateGenerationInfoDFS(generationInfo, visitQueue, visited, stateIndex);
            } else {
                // Generate information according to symmetries
                for (size_t symmetryIndex : symmetries.sortedSymmetries) {
                    STORM_LOG_ASSERT(!visited[symmetryIndex], "Element already considered for symmetry.");
                    auto const& symmetryGroup = symmetries.groups.at(symmetryIndex);
                    STORM_LOG_ASSERT(!symmetryGroup.empty(), "No symmetry available.");

                    // Insert all elements of first subtree of each symmetry
                    size_t groupIndex = stateIndex;
                    for (std::vector<size_t> const& symmetryElement : symmetryGroup) {
                        if (visited[symmetryElement[0]]) {
                            groupIndex = std::min(groupIndex, generationInfo.getStateIndex(symmetryElement[0]));
                        } else {
                            stateIndex = generateStateInfo(generationInfo, symmetryElement[0], visited, stateIndex);
                        }
                    }
                    size_t offset = stateIndex - groupIndex;

                    // Mirror symmetries
                    size_t noSymmetricElements = symmetryGroup.front().size();
                    STORM_LOG_ASSERT(noSymmetricElements > 1, "No symmetry available.");

                    for (std::vector<size_t> symmetricElements : symmetryGroup) {
                        STORM_LOG_ASSERT(symmetricElements.size() == noSymmetricElements, "No. of symmetric elements do not coincide.");
                        if (visited[symmetricElements[1]]) {
                            // Elements already mirrored
                            for (size_t index : symmetricElements) {
                                STORM_LOG_ASSERT(visited[index], "Element not mirrored.");
                            }
                            continue;
                        }

                        // Initialize for original element
                        size_t originalElement = symmetricElements[0];
                        size_t index = generationInfo.getStateIndex(originalElement);
                        size_t activationIndex = isRepresentative(originalElement) ? generationInfo.getSpareActivationIndex(originalElement) : 0;
                        size_t usageIndex = mElements[originalElement]->isSpareGate() ? generationInfo.getSpareUsageIndex(originalElement) : 0;

                        // Mirror symmetry for each element
                        for (size_t i = 1; i < symmetricElements.size(); ++i) {
                            size_t symmetricElement = symmetricElements[i];
                            visited.set(symmetricElement);

                            generationInfo.addStateIndex(symmetricElement, index + offset * i);
                            stateIndex += 2;

                            STORM_LOG_ASSERT((activationIndex > 0) == isRepresentative(symmetricElement), "Bits for representative incorrect.");
                            if (activationIndex > 0) {
                                generationInfo.addSpareActivationIndex(symmetricElement, activationIndex + offset * i);
                                ++stateIndex;
                            }

                            STORM_LOG_ASSERT((usageIndex > 0) == mElements[symmetricElement]->isSpareGate(), "Bits for usage incorrect.");
                            if (usageIndex > 0) {
                                generationInfo.addSpareUsageIndex(symmetricElement, usageIndex + offset * i);
                                stateIndex += generationInfo.usageInfoBits();
                            }
                        }
                    }

                    // Store starting indices of symmetry groups
                    std::vector<size_t> symmetryIndices;
                    for (size_t i = 0; i < noSymmetricElements; ++i) {
                        symmetryIndices.push_back(groupIndex + i * offset);
                    }
                    generationInfo.addSymmetry(offset, symmetryIndices);
                }
            }

            // TODO symmetries in dependencies?
            // Consider dependencies
            for (size_t idDependency : getDependencies()) {
                std::shared_ptr<DFTDependency<ValueType> const> dependency = getDependency(idDependency);
                visitQueue.push(dependency->id());
                visitQueue.push(dependency->triggerEvent()->id());
                STORM_LOG_THROW(dependency->dependentEvents().size() == 1, storm::exceptions::NotSupportedException,
                                "Direct state generation does not support n-ary dependencies. Consider rewriting them by setting the binary dependency flag.");
                visitQueue.push(dependency->dependentEvents()[0]->id());
            }
            stateIndex = performStateGenerationInfoDFS(generationInfo, visitQueue, visited, stateIndex);

            // Visit all remaining states
            for (size_t i = 0; i < visited.size(); ++i) {
                if (!visited[i]) {
                    visitQueue.push(i);
                    stateIndex = performStateGenerationInfoDFS(generationInfo, visitQueue, visited, stateIndex);
                }
            }

            generationInfo.generateSymmetries(symmetries);

            STORM_LOG_TRACE(generationInfo);
            STORM_LOG_ASSERT(stateIndex == mStateVectorSize, "Id incorrect.");
            STORM_LOG_ASSERT(visited.full(), "Not all elements considered.");
            generationInfo.checkSymmetries();

            return generationInfo;
        }

        template<typename ValueType>
        size_t DFT<ValueType>::generateStateInfo(DFTStateGenerationInfo& generationInfo, size_t id, storm::storage::BitVector& visited, size_t stateIndex) const {
            STORM_LOG_ASSERT(!visited[id], "Element already visited.");
            visited.set(id);

            // Reserve bits for element
            generationInfo.addStateIndex(id, stateIndex);
            stateIndex += 2;

            if (isRepresentative(id)) {
                generationInfo.addSpareActivationIndex(id, stateIndex);
                ++stateIndex;
            }

            if (mElements[id]->isSpareGate()) {
                generationInfo.addSpareUsageIndex(id, stateIndex);
                stateIndex += generationInfo.usageInfoBits();
            }

            return stateIndex;
        }

        template<typename ValueType>
        size_t DFT<ValueType>::performStateGenerationInfoDFS(DFTStateGenerationInfo& generationInfo, std::queue<size_t>& visitQueue, storm::storage::BitVector& visited,
                                                             size_t stateIndex) const {
            while (!visitQueue.empty()) {
                size_t id = visitQueue.front();
                visitQueue.pop();
                if (visited[id]) {
                    // Already visited
                    continue;
                }
                stateIndex = generateStateInfo(generationInfo, id, visited, stateIndex);

                // Insert children
                if (mElements[id]->isGate()) {
                    for (auto const& child : std::static_pointer_cast<DFTGate<ValueType>>(mElements[id])->children()) {
                        visitQueue.push(child->id());
                    }
                }
            }
            return stateIndex;
        }

        template<typename ValueType>
        std::vector<DFT<ValueType>> DFT<ValueType>::topModularisation() const {
            STORM_LOG_ASSERT(isGate(mTopLevelIndex), "Top level element is no gate.");
            auto const& children = getGate(mTopLevelIndex)->children();
            std::map<size_t, std::vector<size_t>> subdfts;
            for (auto const& child : children) {
                std::vector<size_t> isubdft;
                if (child->nrParents() > 1 || child->hasOutgoingDependencies() || child->hasRestrictions()) {
                    STORM_LOG_TRACE("child " << child->name() << " does not allow modularisation.");
                    return {*this};
                }
                if (isGate(child->id())) {
                    isubdft = getGate(child->id())->independentSubDft(false);
                } else {
                    STORM_LOG_ASSERT(isBasicElement(child->id()), "Child is no BE.");
                    if (getBasicElement(child->id())->hasIngoingDependencies()) {
                        STORM_LOG_TRACE("child " << child->name() << " does not allow modularisation.");
                        return {*this};
                    } else {
                        isubdft = {child->id()};
                    }

                }
                if (isubdft.empty()) {
                    return {*this};
                } else {
                    subdfts[child->id()] = isubdft;
                }
            }

            std::vector<DFT<ValueType>> res;
            for (auto const& subdft : subdfts) {
                storm::builder::DFTBuilder<ValueType> builder;

                for (size_t id : subdft.second) {
                    builder.copyElement(mElements[id]);
                }
                builder.setTopLevel(mElements[subdft.first]->name());
                res.push_back(builder.build());
            }
            return res;

        }

        template<typename ValueType>
        uint64_t DFT<ValueType>::maxRank() const {
            uint64_t max = 0;
            for (auto const& e : mElements) {
                if (e->rank() > max) {
                    max = e->rank();
                }
            }
            return max;
        }

        template<typename ValueType>
        DFT<ValueType> DFT<ValueType>::optimize() const {
            std::vector<size_t> modIdea = findModularisationRewrite();
            STORM_LOG_DEBUG("Modularisation idea: " << storm::utility::vector::toString(modIdea));

            if (modIdea.empty()) {
                // No rewrite needed
                return *this;
            }

            std::vector<std::vector<size_t>> rewriteIds;
            rewriteIds.push_back(modIdea);

            storm::builder::DFTBuilder<ValueType> builder;

            // Accumulate elements which must be rewritten
            std::set<size_t> rewriteSet;
            for (std::vector<size_t> rewrites : rewriteIds) {
                rewriteSet.insert(rewrites.front());
            }
            // Copy all other elements which do not change
            for (auto elem : mElements) {
                if (rewriteSet.count(elem->id()) == 0) {
                    builder.copyElement(elem);
                }
            }

            // Add rewritten elements
            for (std::vector<size_t> rewrites : rewriteIds) {
                STORM_LOG_ASSERT(rewrites.size() > 1, "No rewritten elements.");
                STORM_LOG_ASSERT(mElements[rewrites[1]]->hasParents(), "Rewritten elements has no parents.");
                STORM_LOG_ASSERT(mElements[rewrites[1]]->parents().front()->isGate(), "Rewritten element has no parent gate.");
                DFTGatePointer originalParent = std::static_pointer_cast<DFTGate<ValueType>>(mElements[rewrites[1]]->parents().front());
                std::string newParentName = builder.getUniqueName(originalParent->name());

                // Accumulate children names
                std::vector<std::string> childrenNames;
                for (size_t i = 1; i < rewrites.size(); ++i) {
                    STORM_LOG_ASSERT(mElements[rewrites[i]]->parents().front()->id() == originalParent->id(),
                                     "Children have not the same father for rewrite " << mElements[rewrites[i]]->name());
                    childrenNames.push_back(mElements[rewrites[i]]->name());
                }

                // Add element inbetween parent and children
                switch (originalParent->type()) {
                    case DFTElementType::AND:
                        builder.addAndElement(newParentName, childrenNames);
                        break;
                    case DFTElementType::OR:
                        builder.addOrElement(newParentName, childrenNames);
                        break;
                    case DFTElementType::BE_EXP:
                    case DFTElementType::BE_CONST:
                    case DFTElementType::VOT:
                    case DFTElementType::PAND:
                    case DFTElementType::SPARE:
                    case DFTElementType::POR:
                    case DFTElementType::PDEP:
                    case DFTElementType::SEQ:
                    case DFTElementType::MUTEX:
                        // Other elements are not supported
                    default:
                        STORM_LOG_ASSERT(false, "Dft type can not be rewritten.");
                        break;
                }

                // Add parent with the new child newParent and all its remaining children
                childrenNames.clear();
                childrenNames.push_back(newParentName);
                for (auto const& child : originalParent->children()) {
                    if (std::find(rewrites.begin() + 1, rewrites.end(), child->id()) == rewrites.end()) {
                        // Child was not rewritten and must be kept
                        childrenNames.push_back(child->name());
                    }
                }
                builder.copyGate(originalParent, childrenNames);
            }

            builder.setTopLevel(mElements[mTopLevelIndex]->name());
            // TODO use reference?
            DFT<ValueType> newDft = builder.build();
            STORM_LOG_TRACE(newDft.getElementsString());
            return newDft.optimize();
        }

        template<typename ValueType>
        size_t DFT<ValueType>::nrDynamicElements() const {
            size_t noDyn = 0;
            for (auto const& elem : mElements) {
                switch (elem->type()) {
                    case DFTElementType::AND:
                    case DFTElementType::OR:
                    case DFTElementType::VOT:
                    case DFTElementType::BE_EXP:
                    case DFTElementType::BE_CONST:
                        break;
                    case DFTElementType::PAND:
                    case DFTElementType::SPARE:
                    case DFTElementType::POR:
                    case DFTElementType::SEQ:
                    case DFTElementType::MUTEX:
                    case DFTElementType::PDEP:
                        noDyn += 1;
                        break;
                    default:
                        STORM_LOG_ASSERT(false, "DFT element type " << elem->type() << " not known.");
                        break;
                }
            }
            return noDyn;
        }

        template<typename ValueType>
        size_t DFT<ValueType>::nrStaticElements() const {
            size_t noStatic = 0;
            for (auto const& elem : mElements) {
                switch (elem->type()) {
                    case DFTElementType::AND:
                    case DFTElementType::OR:
                    case DFTElementType::VOT:
                        ++noStatic;
                        break;
                    case DFTElementType::BE_EXP:
                    case DFTElementType::BE_CONST:
                    case DFTElementType::PAND:
                    case DFTElementType::SPARE:
                    case DFTElementType::POR:
                    case DFTElementType::SEQ:
                    case DFTElementType::MUTEX:
                    case DFTElementType::PDEP:
                        break;
                    default:
                        STORM_LOG_ASSERT(false, "DFT element type " << elem->type() << " not known.");
                        break;
                }
            }
            return noStatic;
        }

        template<typename ValueType>
        std::string DFT<ValueType>::getElementsString() const {
            std::stringstream stream;
            for (auto const& elem : mElements) {
                stream << "[" << elem->id() << "]" << *elem << std::endl;
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
            if (it == mTopModule.end()) {
                stream << "}" << std::endl;
                return stream.str();
            }
            STORM_LOG_ASSERT(it != mTopModule.end(), "Element not found.");
            stream << mElements[(*it)]->name();
            ++it;
            while (it != mTopModule.end()) {
                stream << ", " << mElements[(*it)]->name();
                ++it;
            }
            stream << "}" << std::endl;

            for (auto const& spareModule : mSpareModules) {
                stream << "[" << mElements[spareModule.first]->name() << "] = {";
                if (!spareModule.second.empty()) {
                    std::vector<size_t>::const_iterator it = spareModule.second.begin();
                    STORM_LOG_ASSERT(it != spareModule.second.end(), "Element not found.");
                    stream << mElements[(*it)]->name();
                    ++it;
                    while (it != spareModule.second.end()) {
                        stream << ", " << mElements[(*it)]->name();
                        ++it;
                    }
                }
                stream << "}" << std::endl;
            }
            return stream.str();
        }

        template<typename ValueType>
        std::string DFT<ValueType>::getElementsWithStateString(DFTStatePointer const& state) const {
            std::stringstream stream;
            for (auto const& elem : mElements) {
                stream << "[" << elem->id() << "]";
                stream << *elem;
                if (elem->isDependency()) {
                    stream << "\t** " << storm::storage::toChar(state->getDependencyState(elem->id())) << "[dep]";
                } else {
                    stream << "\t** " << storm::storage::toChar(state->getElementState(elem->id()));
                    if (elem->isSpareGate()) {
                        size_t useId = state->uses(elem->id());
                        if (useId == elem->id() || state->isActive(useId)) {
                            stream << "actively ";
                        }
                        stream << "using " << useId;
                    }
                }
                stream << std::endl;
            }
            return stream.str();
        }

        template<typename ValueType>
        std::string DFT<ValueType>::getStateString(DFTStatePointer const& state) const {
            std::stringstream stream;
            stream << "(" << state->getId() << ") ";
            for (auto const& elem : mElements) {
                if (elem->isDependency()) {
                    stream << storm::storage::toChar(state->getDependencyState(elem->id())) << "[dep]";
                } else {
                    stream << storm::storage::toChar(state->getElementState(elem->id()));
                    if (elem->isSpareGate()) {
                        stream << "[";
                        size_t useId = state->uses(elem->id());
                        if (useId == elem->id() || state->isActive(useId)) {
                            stream << "actively ";
                        }
                        stream << "using " << useId << "]";
                    }
                }
            }
            return stream.str();
        }

        template<typename ValueType>
        std::string DFT<ValueType>::getStateString(storm::storage::BitVector const& status, DFTStateGenerationInfo const& stateGenerationInfo, size_t id) const {
            std::stringstream stream;
            stream << "(" << id << ") ";
            for (auto const& elem : mElements) {
                if (elem->isDependency()) {
                    stream << storm::storage::toChar(DFTState<ValueType>::getDependencyState(status, stateGenerationInfo, elem->id())) << "[dep]";
                } else {
                    stream << storm::storage::toChar(DFTState<ValueType>::getElementState(status, stateGenerationInfo, elem->id()));
                    if (elem->isSpareGate()) {
                        stream << "[";
                        size_t nrUsedChild = status.getAsInt(stateGenerationInfo.getSpareUsageIndex(elem->id()), stateGenerationInfo.usageInfoBits());
                        size_t useId;
                        if (nrUsedChild == getMaxSpareChildCount()) {
                            useId = elem->id();
                        } else {
                            useId = getChild(elem->id(), nrUsedChild);
                        }
                        if (useId == elem->id() || status[stateGenerationInfo.getSpareActivationIndex(useId)]) {
                            stream << "actively ";
                        }
                        stream << "using " << useId << "]";
                    }
                }
            }
            return stream.str();
        }

        template<typename ValueType>
        size_t DFT<ValueType>::getChild(size_t spareId, size_t nrUsedChild) const {
            STORM_LOG_ASSERT(mElements[spareId]->isSpareGate(), "Element is no spare.");
            return getGate(spareId)->children()[nrUsedChild]->id();
        }

        template<typename ValueType>
        size_t DFT<ValueType>::getNrChild(size_t spareId, size_t childId) const {
            STORM_LOG_ASSERT(mElements[spareId]->isSpareGate(), "Element is no spare.");
            DFTElementVector children = getGate(spareId)->children();
            for (size_t nrChild = 0; nrChild < children.size(); ++nrChild) {
                if (children[nrChild]->id() == childId) {
                    return nrChild;
                }
            }
            STORM_LOG_ASSERT(false, "Child not found.");
            return 0;
        }

        template<typename ValueType>
        std::vector<size_t> DFT<ValueType>::getIndependentSubDftRoots(size_t index) const {
            auto elem = getElement(index);
            auto ISD = elem->independentSubDft(false);
            return ISD;
        }

        template<typename ValueType>
        std::vector<size_t> DFT<ValueType>::immediateFailureCauses(size_t index) const {
            if (isGate(index)) {
                STORM_LOG_ASSERT(false, "Is gate.");
                return {};
            } else {
                return {index};
            }
        }

        template<typename ValueType>
        bool DFT<ValueType>::canHaveNondeterminism() const {
            return !getDependencies().empty();
        }

        template<typename ValueType>
        bool DFT<ValueType>::checkWellFormedness(bool validForAnalysis, std::ostream& stream) const {
            bool wellformed = true;
            // Check independence of spare modules
            // TODO: comparing one element of each spare module sufficient?
            for (auto module1 = mSpareModules.begin(); module1 != mSpareModules.end(); ++module1) {
                size_t firstElement = module1->second.front();
                for (auto module2 = std::next(module1); module2 != mSpareModules.end(); ++module2) {
                    if (std::find(module2->second.begin(), module2->second.end(), firstElement) != module2->second.end()) {
                        if (!wellformed) {
                            stream << std::endl;
                        }
                        stream << "Spare modules of '" << getElement(module1->first)->name() << "' and '" << getElement(module2->first)->name() << "' should not overlap.";
                        wellformed = false;
                    }
                }
            }

            if (validForAnalysis) {
                // Check that each dependency is binary
                for (size_t idDependency : this->getDependencies()) {
                    std::shared_ptr<DFTDependency<ValueType> const> dependency = this->getDependency(idDependency);
                    if (dependency->dependentEvents().size() != 1) {
                        if (!wellformed) {
                            stream << std::endl;
                        }
                        stream << "Dependency '" << dependency->name() << "' is not binary.";
                        wellformed = false;
                    }

                }
            }
            // TODO check VOT gates
            // TODO check only one constant failed event
            return wellformed;
        }

        template<typename ValueType>
        DFTColouring<ValueType> DFT<ValueType>::colourDFT() const {
            return DFTColouring<ValueType>(*this);
        }

        template<typename ValueType>
        std::map<size_t, size_t> DFT<ValueType>::findBijection(size_t index1, size_t index2, DFTColouring<ValueType> const& colouring, bool sparesAsLeaves) const {
            STORM_LOG_TRACE("Considering ids " << index1 << ", " << index2 << " for isomorphism.");
            bool sharedSpareMode = false;
            std::map<size_t, size_t> bijection;

            if (isBasicElement(index1)) {
                if (!isBasicElement(index2)) {
                    return {};
                }
                if (colouring.hasSameColour(index1, index2)) {
                    bijection[index1] = index2;
                    return bijection;
                } else {
                    return {};
                }
            }

            STORM_LOG_ASSERT(isGate(index1), "Element is no gate.");
            STORM_LOG_ASSERT(isGate(index2), "Element is no gate.");
            std::vector<size_t> isubdft1 = getGate(index1)->independentSubDft(false);
            std::vector<size_t> isubdft2 = getGate(index2)->independentSubDft(false);
            if (isubdft1.empty() || isubdft2.empty() || isubdft1.size() != isubdft2.size()) {
                if (isubdft1.empty() && isubdft2.empty() && sparesAsLeaves) {
                    // Check again for shared spares
                    sharedSpareMode = true;
                    isubdft1 = getGate(index1)->independentSubDft(false, true);
                    isubdft2 = getGate(index2)->independentSubDft(false, true);
                    if (isubdft1.empty() || isubdft2.empty() || isubdft1.size() != isubdft2.size()) {
                        return {};
                    }
                } else {
                    return {};
                }
            }
            STORM_LOG_TRACE("Checking subdfts from " << index1 << ", " << index2 << " for isomorphism.");
            auto LHS = colouring.colourSubdft(isubdft1);
            auto RHS = colouring.colourSubdft(isubdft2);
            auto IsoCheck = DFTIsomorphismCheck<ValueType>(LHS, RHS, *this);

            while (IsoCheck.findNextIsomorphism()) {
                bijection = IsoCheck.getIsomorphism();
                if (sharedSpareMode) {
                    bool bijectionSpareCompatible = true;
                    for (size_t elementId : isubdft1) {
                        if (getElement(elementId)->isSpareGate()) {
                            std::shared_ptr<DFTSpare<ValueType>> spareLeft = std::static_pointer_cast<DFTSpare<ValueType>>(mElements[elementId]);
                            std::shared_ptr<DFTSpare<ValueType>> spareRight = std::static_pointer_cast<DFTSpare<ValueType>>(mElements[bijection.at(elementId)]);

                            if (spareLeft->nrChildren() != spareRight->nrChildren()) {
                                bijectionSpareCompatible = false;
                                break;
                            }
                            // Check bijection for spare children
                            for (size_t i = 0; i < spareLeft->nrChildren(); ++i) {
                                size_t childLeftId = spareLeft->children().at(i)->id();
                                size_t childRightId = spareRight->children().at(i)->id();

                                STORM_LOG_ASSERT(bijection.count(childLeftId) == 0, "Child already part of bijection.");
                                if (childLeftId == childRightId) {
                                    // Ignore shared child
                                    continue;
                                }

                                // TODO generalize for more than one parent
                                if (spareLeft->children().at(i)->nrParents() != 1 || spareRight->children().at(i)->nrParents() != 1) {
                                    bijectionSpareCompatible = false;
                                    break;
                                }

                                std::map<size_t, size_t> tmpBijection = findBijection(childLeftId, childRightId, colouring, false);
                                if (!tmpBijection.empty()) {
                                    bijection.insert(tmpBijection.begin(), tmpBijection.end());
                                } else {
                                    bijectionSpareCompatible = false;
                                    break;
                                }
                            }
                            if (!bijectionSpareCompatible) {
                                break;
                            }
                        }
                    }
                    if (bijectionSpareCompatible) {
                        return bijection;
                    }
                } else {
                    return bijection;
                }
            } // end while
            return {};
        }

        template<typename ValueType>
        DFTIndependentSymmetries DFT<ValueType>::findSymmetries(DFTColouring<ValueType> const& colouring) const {
            std::vector<size_t> vec;
            vec.reserve(nrElements());
            storm::utility::iota_n(std::back_inserter(vec), nrElements(), 0);
            BijectionCandidates<ValueType> completeCategories = colouring.colourSubdft(vec);
            std::map<size_t, std::vector<std::vector<size_t>>> res;

            // Find symmetries for gates
            for (auto const& colourClass : completeCategories.gateCandidates) {
                findSymmetriesHelper(colourClass.second, colouring, res);
            }

            // Find symmetries for BEs
            for (auto const& colourClass : completeCategories.beCandidates) {
                findSymmetriesHelper(colourClass.second, colouring, res);
            }

            return DFTIndependentSymmetries(res);
        }

        template<typename ValueType>
        void DFT<ValueType>::findSymmetriesHelper(std::vector<size_t> const& candidates, DFTColouring<ValueType> const& colouring,
                                                  std::map<size_t, std::vector<std::vector<size_t>>>& result) const {
            if (candidates.size() <= 0) {
                return;
            }

            std::set<size_t> foundEqClassFor;
            for (auto it1 = candidates.cbegin(); it1 != candidates.cend(); ++it1) {
                std::vector<std::vector<size_t>> symClass;
                if (foundEqClassFor.count(*it1) > 0) {
                    // This item is already in a class.
                    continue;
                }
                if (!getElement(*it1)->hasOnlyStaticParents()) {
                    continue;
                }

                std::tuple<std::vector<size_t>, std::vector<size_t>, std::vector<size_t>> influencedElem1Ids = getSortedParentAndDependencyIds(*it1);
                auto it2 = it1;
                for (++it2; it2 != candidates.cend(); ++it2) {
                    if (!getElement(*it2)->hasOnlyStaticParents()) {
                        continue;
                    }

                    if (influencedElem1Ids == getSortedParentAndDependencyIds(*it2)) {
                        std::map<size_t, size_t> bijection = findBijection(*it1, *it2, colouring, true);
                        if (!bijection.empty()) {
                            STORM_LOG_TRACE("Subdfts are symmetric");
                            foundEqClassFor.insert(*it2);
                            if (symClass.empty()) {
                                for (auto const& i : bijection) {
                                    symClass.push_back(std::vector<size_t>({i.first}));
                                }
                            }
                            auto symClassIt = symClass.begin();
                            for (auto const& i : bijection) {
                                symClassIt->emplace_back(i.second);
                                ++symClassIt;

                            }
                        }
                    }
                }

                if (!symClass.empty()) {
                    result.emplace(*it1, symClass);
                }
            }
        }

        template<typename ValueType>
        std::vector<size_t> DFT<ValueType>::findModularisationRewrite() const {
            for (auto const& e : mElements) {
                if (e->isGate() && (e->type() == DFTElementType::AND || e->type() == DFTElementType::OR)) {
                    // suitable parent gate! - Lets check the independent submodules of the children
                    auto const& children = std::static_pointer_cast<DFTGate<ValueType>>(e)->children();
                    for (auto const& child : children) {

                        auto ISD = std::static_pointer_cast<DFTGate<ValueType>>(child)->independentSubDft(true);
                        // In the ISD, check for other children:

                        std::vector<size_t> rewrite = {e->id(), child->id()};
                        for (size_t isdElemId : ISD) {
                            if (isdElemId == child->id()) continue;
                            if (std::find_if(children.begin(), children.end(), [&isdElemId](std::shared_ptr<DFTElement<ValueType>> const& e) { return e->id() == isdElemId; }) !=
                                children.end()) {
                                // element in subtree is also child
                                rewrite.push_back(isdElemId);
                            }
                        }
                        if (rewrite.size() > 2 && rewrite.size() < children.size() - 1) {
                            return rewrite;
                        }

                    }
                }
            }
            return {};
        }


        template<typename ValueType>
        std::tuple<std::vector<size_t>, std::vector<size_t>, std::vector<size_t>> DFT<ValueType>::getSortedParentAndDependencyIds(size_t index) const {
            // Parents
            std::vector<size_t> parents = getElement(index)->parentIds();
            std::sort(parents.begin(), parents.end());
            // Ingoing dependencies
            std::vector<size_t> ingoingDeps;
            if (isBasicElement(index)) {
                for (auto const& dep : getBasicElement(index)->ingoingDependencies()) {
                    ingoingDeps.push_back(dep->id());
                }
                std::sort(ingoingDeps.begin(), ingoingDeps.end());
            }
            // Outgoing dependencies
            std::vector<size_t> outgoingDeps;
            for (auto const& dep : getElement(index)->outgoingDependencies()) {
                outgoingDeps.push_back(dep->id());
            }
            std::sort(outgoingDeps.begin(), outgoingDeps.end());
            return std::make_tuple(parents, ingoingDeps, outgoingDeps);
        }

        template<typename ValueType>
        std::set<size_t> DFT<ValueType>::getAllIds() const {
            std::set<size_t> ids;
            for (auto const& elem : mElements) {
                ids.insert(elem->id());
            }
            return ids;
        }

        template<typename ValueType>
        size_t DFT<ValueType>::getIndex(std::string const& name) const {
            auto iter = std::find_if(mElements.begin(), mElements.end(), [&name](DFTElementPointer const& e) { return e->name() == name; });
            STORM_LOG_THROW(iter != mElements.end(), storm::exceptions::InvalidArgumentException, "Event name '" << name << "' not known.");
            return (*iter)->id();
        }

        template<typename ValueType>
        void DFT<ValueType>::setRelevantEvents(std::set<size_t> const& relevantEvents, bool allowDCForRelevantEvents) const {
            mRelevantEvents.clear();
            // Top level element is first element
            mRelevantEvents.push_back(this->getTopLevelIndex());
            for (auto const& elem : mElements) {
                if (relevantEvents.find(elem->id()) != relevantEvents.end() || elem->id() == this->getTopLevelIndex()) {
                    elem->setRelevance(true);
                    elem->setAllowDC(allowDCForRelevantEvents);
                    if (elem->id() != this->getTopLevelIndex()) {
                        // Top level element was already added
                        mRelevantEvents.push_back(elem->id());
                    }
                } else {
                    elem->setRelevance(false);
                    elem->setAllowDC(true);
                }
            }
        }

        template<typename ValueType>
        std::vector<size_t> const& DFT<ValueType>::getRelevantEvents() const {
            return mRelevantEvents;
        }

        template<typename ValueType>
        std::string DFT<ValueType>::getRelevantEventsString() const {
            std::stringstream stream;
            bool first = true;
            for (size_t relevant_id : getRelevantEvents()) {
                if (first) {
                    first = false;
                } else {
                    stream << ", ";
                }
                stream << getElement(relevant_id)->name() << " [" << relevant_id << "]";
            }
            return stream.str();
        }

        template<typename ValueType>
        void DFT<ValueType>::writeStatsToStream(std::ostream& stream) const {
            // Count individual types of elements
            size_t noBE = 0;
            size_t noAnd = 0;
            size_t noOr = 0;
            size_t noVot = 0;
            size_t noPand = 0;
            size_t noPor = 0;
            size_t noSpare = 0;
            size_t noDependency = 0;
            size_t noRestriction = 0;
            for (auto const& elem : mElements) {
                switch (elem->type()) {
                    case DFTElementType::BE_EXP:
                    case DFTElementType::BE_CONST:
                        ++noBE;
                        break;
                    case DFTElementType::AND:
                        ++noAnd;
                        break;
                    case DFTElementType::OR:
                        ++noOr;
                        break;
                    case DFTElementType::VOT:
                        ++noVot;
                        break;
                    case DFTElementType::PAND:
                        ++noPand;
                        break;
                    case DFTElementType::POR:
                        ++noPor;
                        break;
                    case DFTElementType::SPARE:
                        ++noSpare;
                        break;
                    case DFTElementType::PDEP:
                        ++noDependency;
                        break;
                    case DFTElementType::SEQ:
                    case DFTElementType::MUTEX:
                        ++noRestriction;
                        break;
                    default:
                        STORM_LOG_ASSERT(false, "DFT element type " << elem->type() << " not known.");
                        break;
                }
            }
            size_t noStatic = nrStaticElements();
            size_t noDynamic = nrDynamicElements();

            // Check whether numbers are correct
            STORM_LOG_ASSERT(noBE == nrBasicElements(), "No. of BEs does not match.");
            STORM_LOG_ASSERT(noSpare == mNrOfSpares, "No. of SPAREs does not match.");
            STORM_LOG_ASSERT(noDependency == mDependencies.size(), "No. of Dependencies does not match.");
            STORM_LOG_ASSERT(noAnd + noOr + noVot == noStatic, "No. of static gates does not match.");
            STORM_LOG_ASSERT(noPand + noPor + noSpare + noDependency + noRestriction == noDynamic, "No. of dynamic gates does not match.");
            STORM_LOG_ASSERT(noBE + noStatic + noDynamic == nrElements(), "No. of elements does not match.");

            // Print output
            stream << "=============DFT Statistics==============" << std::endl;
            stream << "Number of BEs: " << nrBasicElements() << std::endl;
            stream << "Number of static elements: " << noStatic << std::endl;
            stream << "Number of dynamic elements: " << noDynamic << std::endl;
            stream << "Number of elements: " << nrElements() << std::endl;
            stream << "-----------------------------------------" << std::endl;
            if (noBE > 0) {
                stream << "Number of BEs: " << noBE << std::endl;
            }
            if (noAnd > 0) {
                stream << "Number of AND gates: " << noAnd << std::endl;
            }
            if (noOr > 0) {
                stream << "Number of OR gates: " << noOr << std::endl;
            }
            if (noVot > 0) {
                stream << "Number of VOT gates: " << noVot << std::endl;
            }
            if (noPand > 0) {
                stream << "Number of PAND gates: " << noPand << std::endl;
            }
            if (noPor > 0) {
                stream << "Number of POR gates: " << noPor << std::endl;
            }
            if (noSpare > 0) {
                stream << "Number of SPARE gates: " << noSpare << std::endl;
            }
            if (noDependency > 0) {
                stream << "Number of Dependencies: " << noDependency << std::endl;
            }
            if (noRestriction > 0) {
                stream << "Number of Restrictions: " << noRestriction << std::endl;
            }
            stream << "=========================================" << std::endl;
        }

        // Explicitly instantiate the class.
        template
        class DFT<double>;

#ifdef STORM_HAVE_CARL

        template
        class DFT<RationalFunction>;

#endif

    }
}
