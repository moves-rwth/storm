#include "DFT.h"

#include <map>

#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/WrongFormatException.h"
#include "storm/utility/iota_n.h"
#include "storm/utility/vector.h"

#include "storm-dft/builder/DFTBuilder.h"
#include "storm-dft/storage/DFTIsomorphism.h"
#include "storm-dft/utility/RelevantEvents.h"

namespace storm::dft {
namespace storage {

template<typename ValueType>
DFT<ValueType>::DFT(DFTElementVector const& elements, DFTElementPointer const& tle)
    : mElements(elements), mNrOfBEs(0), mNrOfSpares(0), mNrRepresentatives(0), mTopLevelIndex(tle->id()), mMaxSpareChildCount(0) {
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
            mMaxSpareChildCount =
                std::max(mMaxSpareChildCount, std::static_pointer_cast<storm::dft::storage::elements::DFTSpare<ValueType>>(elem)->children().size());
            for (auto const& spareReprs : std::static_pointer_cast<storm::dft::storage::elements::DFTSpare<ValueType>>(elem)->children()) {
                STORM_LOG_THROW(spareReprs->isGate() || spareReprs->isBasicElement(), storm::exceptions::WrongFormatException,
                                "Child '" << spareReprs->name() << "' of spare '" << elem->name() << "' must be gate or BE.");
                std::set<size_t> module = {spareReprs->id()};
                spareReprs->extendSpareModule(module);
                std::set<size_t> sparesAndBes;
                for (size_t modelem : module) {
                    if (mElements[modelem]->isSpareGate() || mElements[modelem]->isBasicElement()) {
                        sparesAndBes.insert(modelem);
                        mRepresentants.insert(std::make_pair(modelem, spareReprs->id()));
                    }
                }
                mModules.insert(std::make_pair(spareReprs->id(), storm::dft::storage::DftModule(spareReprs->id(), sparesAndBes)));
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
    for (auto const& module : mModules) {
        for (auto const& index : module.second.getElements()) {
            topModuleSet.erase(index);
        }
    }
    // Extend top module and insert those elements which are part of the top module and a spare module
    mElements[mTopLevelIndex]->extendSpareModule(topModuleSet);
    storm::dft::storage::DftModule topModule(mTopLevelIndex, topModuleSet);

    // Clear all spare modules where at least one element is also in the top module.
    // These spare modules will be activated from the beginning.
    if (!topModule.getElements().empty()) {
        size_t topModuleId = *topModule.getElements().begin();
        for (auto& module : mModules) {
            auto& spareModule = module.second;
            auto const& spareModuleElements = spareModule.getElements();
            if (std::find(spareModuleElements.begin(), spareModuleElements.end(), topModuleId) != spareModuleElements.end()) {
                STORM_LOG_WARN("Elements of spare module '"
                               << getElement(spareModule.getRepresentative())->name()
                               << "' also contained in top module. All elements of this spare module will be activated from the beginning on.");
                spareModule.clear();
            }
        }
    }
    mModules.insert(std::make_pair(mTopLevelIndex, topModule));

    // Reserve space for failed spares
    ++mMaxSpareChildCount;
    mStateVectorSize = DFTStateGenerationInfo::getStateVectorSize(nrElements(), mNrOfSpares, mNrRepresentatives, mMaxSpareChildCount);

    // Set relevant events: empty list corresponds to only setting the top-level event as relevant
    setRelevantEvents({}, false);
}

template<typename ValueType>
void DFT<ValueType>::setDynamicBehaviorInfo() {
    std::vector<bool> dynamicBehaviorVector(mElements.size(), false);

    std::queue<DFTElementPointer> elementQueue;

    // deal with all dynamic elements
    for (auto const& element : mElements) {
        switch (element->type()) {
            case storm::dft::storage::elements::DFTElementType::PAND:
            case storm::dft::storage::elements::DFTElementType::POR:
            case storm::dft::storage::elements::DFTElementType::MUTEX: {
                auto gate = std::static_pointer_cast<storm::dft::storage::elements::DFTChildren<ValueType>>(element);
                dynamicBehaviorVector[gate->id()] = true;
                for (auto const& child : gate->children()) {
                    // only enqueue static children
                    if (!dynamicBehaviorVector.at(child->id())) {
                        elementQueue.push(child);
                    }
                }
                break;
            }
                // TODO different cases
            case storm::dft::storage::elements::DFTElementType::SPARE: {
                auto spare = std::static_pointer_cast<storm::dft::storage::elements::DFTSpare<ValueType>>(element);

                // Iterate over all children (representatives of spare modules)
                for (auto const& child : spare->children()) {
                    // Case 1: Shared Module
                    // If child only has one parent, it is this SPARE -> nothing to check
                    if (child->nrParents() > 1) {
                        // TODO make more efficient by directly setting ALL spares which share a module to be dynamic
                        for (auto const& parent : child->parents()) {
                            if (parent->isSpareGate() and parent->id() != spare->id()) {
                                dynamicBehaviorVector[spare->id()] = true;
                                break;  // inner loop
                            }
                        }
                    }
                    // Case 2: Triggering outside events
                    // If the SPARE was already detected to have dynamic behavior, do not proceed
                    if (!dynamicBehaviorVector[spare->id()]) {
                        for (auto const& memberID : module(child->id()).getElements()) {
                            // Iterate over all members of the module child represents
                            auto member = getElement(memberID);
                            for (auto const& dep : member->outgoingDependencies()) {
                                // If the member has outgoing dependencies, check if those trigger something outside the module
                                for (auto const& depEvent : dep->dependentEvents()) {
                                    // If a dependent event is not found in the module, SPARE is dynamic
                                    auto const& childModuleElements = module(child->id()).getElements();
                                    if (std::find(childModuleElements.begin(), childModuleElements.end(), depEvent->id()) == childModuleElements.end()) {
                                        dynamicBehaviorVector[spare->id()] = true;
                                        break;  // depEvent-loop
                                    }
                                }
                                if (dynamicBehaviorVector[spare->id()]) {
                                    break;
                                }  // dependency-loop
                            }
                            if (dynamicBehaviorVector[spare->id()]) {
                                break;
                            }  // module-loop
                        }
                    }
                    if (dynamicBehaviorVector[spare->id()]) {
                        break;
                    }  // child-loop
                }
                // if during the computation, dynamic behavior was detected, add children to queue
                if (dynamicBehaviorVector[spare->id()]) {
                    for (auto const& child : spare->children()) {
                        // only enqueue static children
                        if (!dynamicBehaviorVector.at(child->id())) {
                            elementQueue.push(child);
                        }
                    }
                }
                break;
            }
            case storm::dft::storage::elements::DFTElementType::SEQ: {
                auto seq = std::static_pointer_cast<storm::dft::storage::elements::DFTSeq<ValueType>>(element);
                // A SEQ only has dynamic behavior if not all children are BEs
                if (!seq->allChildrenBEs()) {
                    dynamicBehaviorVector[seq->id()] = true;
                    for (auto const& child : seq->children()) {
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
            case storm::dft::storage::elements::DFTElementType::AND:
            case storm::dft::storage::elements::DFTElementType::OR:
            case storm::dft::storage::elements::DFTElementType::VOT: {
                // check all parents and if one has dynamic behavior, propagate it
                dynamicBehaviorVector[currentElement->id()] = true;
                auto gate = std::static_pointer_cast<storm::dft::storage::elements::DFTGate<ValueType>>(currentElement);
                for (auto const& child : gate->children()) {
                    // only enqueue static children
                    if (!dynamicBehaviorVector.at(child->id())) {
                        elementQueue.push(child);
                    }
                }
                break;
            }
                // BEs
            case storm::dft::storage::elements::DFTElementType::BE: {
                auto be = std::static_pointer_cast<storm::dft::storage::elements::DFTBE<ValueType>>(currentElement);
                dynamicBehaviorVector[be->id()] = true;
                // add all ingoing dependencies to queue
                for (auto const& dep : be->ingoingDependencies()) {
                    if (!dynamicBehaviorVector.at(dep->id())) {
                        elementQueue.push(dep);
                    }
                }
                break;
            }
            case storm::dft::storage::elements::DFTElementType::PDEP: {
                auto dep = std::static_pointer_cast<storm::dft::storage::elements::DFTDependency<ValueType>>(currentElement);
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
DFTStateGenerationInfo DFT<ValueType>::buildStateGenerationInfo(storm::dft::storage::DFTIndependentSymmetries const& symmetries) const {
    DFTStateGenerationInfo generationInfo(nrElements(), mNrOfSpares, mNrRepresentatives, mMaxSpareChildCount);

    // Generate pre- and post-set info for restrictions, and mutexes
    for (auto const& elem : mElements) {
        if (!elem->isDependency() && !elem->isRestriction()) {
            // Ids of elements which are the direct predecessor in the list of children of a restriction
            std::vector<size_t> seqRestrictionPres;
            // Ids of elements which are the direct successor in the list of children of a restriction
            std::vector<size_t> seqRestrictionPosts;
            // Ids of elements which are under the same mutex
            std::vector<size_t> mutexRestrictionElements;

            for (auto const& restr : elem->restrictions()) {
                if (restr->isSeqEnforcer()) {
                    auto it = restr->children().cbegin();
                    for (; it != restr->children().cend(); ++it) {
                        if ((*it)->id() == elem->id()) {
                            break;
                        }
                    }
                    STORM_LOG_ASSERT(it != restr->children().cend(), "Child " << elem->id() << " not found in restriction " << *restr);
                    // Add child following element in SEQ
                    ++it;
                    if (it != restr->children().cend()) {
                        seqRestrictionPosts.push_back((*it)->id());
                    }
                    // Add child before element in SEQ
                    --it;
                    if (it != restr->children().cbegin()) {
                        --it;
                        seqRestrictionPres.push_back((*it)->id());
                    }
                } else {
                    STORM_LOG_ASSERT(restr->isMutex(), "Restriction " << *restr << " is neither SEQ nor MUTEX.");
                    bool found = false;
                    for (auto it = restr->children().cbegin(); it != restr->children().cend(); ++it) {
                        if ((*it)->id() != elem->id()) {
                            mutexRestrictionElements.push_back((*it)->id());
                        } else {
                            found = true;
                        }
                    }
                    STORM_LOG_ASSERT(found, "Child " << elem->id() << " is not included in restriction " << *restr);
                }
            }

            generationInfo.setRestrictionPreElements(elem->id(), seqRestrictionPres);
            generationInfo.setRestrictionPostElements(elem->id(), seqRestrictionPosts);
            generationInfo.setMutexElements(elem->id(), mutexRestrictionElements);
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
                    STORM_LOG_ASSERT(!visited[symmetricElement], "Symmetric element " << symmetricElement << " already considered before.");
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
        std::shared_ptr<storm::dft::storage::elements::DFTDependency<ValueType> const> dependency = getDependency(idDependency);
        visitQueue.push(dependency->id());
        visitQueue.push(dependency->triggerEvent()->id());
        STORM_LOG_THROW(dependency->dependentEvents().size() == 1, storm::exceptions::NotSupportedException,
                        "Direct state generation does not support n-ary dependencies. Consider rewriting them by setting the binary dependency flag.");
        visitQueue.push(dependency->dependentEvents()[0]->id());
    }
    stateIndex = performStateGenerationInfoDFS(generationInfo, visitQueue, visited, stateIndex);
    STORM_LOG_ASSERT(visitQueue.empty(), "VisitQueue not empty");

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
            for (auto const& child : std::static_pointer_cast<storm::dft::storage::elements::DFTGate<ValueType>>(mElements[id])->children()) {
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
        storm::dft::builder::DFTBuilder<ValueType> builder;

        for (size_t id : subdft.second) {
            builder.cloneElement(mElements[id]);
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

    storm::dft::builder::DFTBuilder<ValueType> builder;

    // Accumulate elements which must be rewritten
    std::set<size_t> rewriteSet;
    for (std::vector<size_t> rewrites : rewriteIds) {
        rewriteSet.insert(rewrites.front());
    }
    // Copy all other elements which do not change
    for (auto elem : mElements) {
        if (rewriteSet.count(elem->id()) == 0) {
            builder.cloneElement(elem);
        }
    }

    size_t uniqueIndex = 0;  // Counter to ensure unique names
    // Add rewritten elements
    for (std::vector<size_t> rewrites : rewriteIds) {
        STORM_LOG_ASSERT(rewrites.size() > 1, "No rewritten elements.");
        STORM_LOG_ASSERT(mElements[rewrites[1]]->hasParents(), "Rewritten elements has no parents.");
        STORM_LOG_ASSERT(mElements[rewrites[1]]->parents().front()->isGate(), "Rewritten element has no parent gate.");
        DFTGatePointer originalParent = std::static_pointer_cast<storm::dft::storage::elements::DFTGate<ValueType>>(mElements[rewrites[0]]);
        STORM_LOG_ASSERT(std::find_if(mElements[rewrites[1]]->parents().begin(), mElements[rewrites[1]]->parents().end(),
                                      [&originalParent](std::shared_ptr<storm::dft::storage::elements::DFTElement<ValueType>> const& p) {
                                          return p->id() == originalParent->id();
                                      }) != mElements[rewrites[1]]->parents().end(),
                         "Rewritten element has not the same parent");
        std::string newParentName = originalParent->name() + "_" + std::to_string(++uniqueIndex);

        // Accumulate children names
        std::vector<std::string> childrenNames;
        for (size_t i = 1; i < rewrites.size(); ++i) {
            STORM_LOG_ASSERT(std::find_if(mElements[rewrites[i]]->parents().begin(), mElements[rewrites[i]]->parents().end(),
                                          [&originalParent](std::shared_ptr<storm::dft::storage::elements::DFTElement<ValueType>> const& p) {
                                              return p->id() == originalParent->id();
                                          }) != mElements[rewrites[i]]->parents().end(),
                             "Children have not the same father for rewrite " << mElements[rewrites[i]]->name());

            childrenNames.push_back(mElements[rewrites[i]]->name());
        }

        // Add element in-between parent and children
        switch (originalParent->type()) {
            case storm::dft::storage::elements::DFTElementType::AND:
                builder.addAndGate(newParentName, childrenNames);
                break;
            case storm::dft::storage::elements::DFTElementType::OR:
                builder.addOrGate(newParentName, childrenNames);
                break;
            default:
                // Other elements are not supported
                STORM_LOG_ASSERT(false, "Dft type " << originalParent->type() << " can not be rewritten.");
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
        builder.cloneElementWithNewChildren(originalParent, childrenNames);
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
            case storm::dft::storage::elements::DFTElementType::AND:
            case storm::dft::storage::elements::DFTElementType::OR:
            case storm::dft::storage::elements::DFTElementType::VOT:
            case storm::dft::storage::elements::DFTElementType::BE:
                break;
            case storm::dft::storage::elements::DFTElementType::PAND:
            case storm::dft::storage::elements::DFTElementType::SPARE:
            case storm::dft::storage::elements::DFTElementType::POR:
            case storm::dft::storage::elements::DFTElementType::SEQ:
            case storm::dft::storage::elements::DFTElementType::MUTEX:
            case storm::dft::storage::elements::DFTElementType::PDEP:
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
            case storm::dft::storage::elements::DFTElementType::AND:
            case storm::dft::storage::elements::DFTElementType::OR:
            case storm::dft::storage::elements::DFTElementType::VOT:
                ++noStatic;
                break;
            case storm::dft::storage::elements::DFTElementType::BE:
            case storm::dft::storage::elements::DFTElementType::PAND:
            case storm::dft::storage::elements::DFTElementType::SPARE:
            case storm::dft::storage::elements::DFTElementType::POR:
            case storm::dft::storage::elements::DFTElementType::SEQ:
            case storm::dft::storage::elements::DFTElementType::MUTEX:
            case storm::dft::storage::elements::DFTElementType::PDEP:
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
        stream << "[" << elem->id() << "]" << *elem << '\n';
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
std::string DFT<ValueType>::getModulesString() const {
    std::stringstream stream;
    for (auto const& module : mModules) {
        stream << module.second.toString(*this) << "\n";
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
            stream << "\t** " << storm::dft::storage::toChar(state->getDependencyState(elem->id())) << "[dep]";
        } else {
            stream << "\t** " << storm::dft::storage::toChar(state->getElementState(elem->id()));
            if (elem->isSpareGate()) {
                size_t useId = state->uses(elem->id());
                if (useId == elem->id() || state->isActive(useId)) {
                    stream << "actively ";
                }
                stream << "using " << useId;
            }
        }
        stream << '\n';
    }
    return stream.str();
}

template<typename ValueType>
std::string DFT<ValueType>::getStateString(DFTStatePointer const& state) const {
    std::stringstream stream;
    stream << "(" << state->getId() << ") ";
    for (auto const& elem : mElements) {
        if (elem->isDependency()) {
            stream << storm::dft::storage::toChar(state->getDependencyState(elem->id())) << "[dep]";
        } else {
            stream << storm::dft::storage::toChar(state->getElementState(elem->id()));
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
            stream << storm::dft::storage::toChar(DFTState<ValueType>::getDependencyState(status, stateGenerationInfo, elem->id())) << "[dep]";
        } else {
            stream << storm::dft::storage::toChar(DFTState<ValueType>::getElementState(status, stateGenerationInfo, elem->id()));
            if (elem->isSpareGate()) {
                stream << "[";
                size_t useId = this->uses(status, stateGenerationInfo, elem->id());
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
DFTColouring<ValueType> DFT<ValueType>::colourDFT() const {
    return DFTColouring<ValueType>(*this);
}

template<typename ValueType>
std::map<size_t, size_t> DFT<ValueType>::findBijection(size_t index1, size_t index2, DFTColouring<ValueType> const& colouring, bool sparesAsLeaves) const {
    STORM_LOG_TRACE("Considering ids " << index1 << ", " << index2 << " for isomorphism.");
    bool sharedSpareMode = false;
    std::map<size_t, size_t> bijection;

    if (getElement(index1)->isRelevant() || getElement(index2)->isRelevant()) {
        // Relevant events need to be uniquely identified and cannot be symmetric.
        return {};
    }

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
                    std::shared_ptr<storm::dft::storage::elements::DFTSpare<ValueType>> spareLeft =
                        std::static_pointer_cast<storm::dft::storage::elements::DFTSpare<ValueType>>(mElements[elementId]);
                    std::shared_ptr<storm::dft::storage::elements::DFTSpare<ValueType>> spareRight =
                        std::static_pointer_cast<storm::dft::storage::elements::DFTSpare<ValueType>>(mElements[bijection.at(elementId)]);

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
    }  // end while
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
        if (e->isGate() &&
            (e->type() == storm::dft::storage::elements::DFTElementType::AND || e->type() == storm::dft::storage::elements::DFTElementType::OR)) {
            // suitable parent gate! - Lets check the independent submodules of the children
            auto const& children = std::static_pointer_cast<storm::dft::storage::elements::DFTGate<ValueType>>(e)->children();
            for (auto const& child : children) {
                auto ISD = std::static_pointer_cast<storm::dft::storage::elements::DFTGate<ValueType>>(child)->independentSubDft(true);
                // In the ISD, check for other children:

                std::vector<size_t> rewrite = {e->id(), child->id()};
                for (size_t isdElemId : ISD) {
                    if (isdElemId == child->id())
                        continue;
                    if (std::find_if(children.begin(), children.end(),
                                     [&isdElemId](std::shared_ptr<storm::dft::storage::elements::DFTElement<ValueType>> const& e) {
                                         return e->id() == isdElemId;
                                     }) != children.end()) {
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
bool DFT<ValueType>::existsName(std::string const& name) const {
    return std::find_if(mElements.begin(), mElements.end(), [&name](DFTElementPointer const& e) { return e->name() == name; }) != mElements.end();
}

template<typename ValueType>
size_t DFT<ValueType>::getIndex(std::string const& name) const {
    auto iter = std::find_if(mElements.begin(), mElements.end(), [&name](DFTElementPointer const& e) { return e->name() == name; });
    STORM_LOG_THROW(iter != mElements.end(), storm::exceptions::InvalidArgumentException, "Event name '" << name << "' not known.");
    return (*iter)->id();
}

template<typename ValueType>
void DFT<ValueType>::setRelevantEvents(storm::dft::utility::RelevantEvents const& relevantEvents, bool const allowDCForRelevant) const {
    mRelevantEvents.clear();
    STORM_LOG_THROW(relevantEvents.checkRelevantNames(*this), storm::exceptions::InvalidArgumentException, "One of the relevant elements does not exist.");
    // Top level element is first element
    mRelevantEvents.push_back(this->getTopLevelIndex());
    for (auto& elem : mElements) {
        if (relevantEvents.isRelevant(elem->name()) || elem->id() == this->getTopLevelIndex()) {
            elem->setRelevance(true);
            elem->setAllowDC(allowDCForRelevant);
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
            case storm::dft::storage::elements::DFTElementType::BE:
                ++noBE;
                break;
            case storm::dft::storage::elements::DFTElementType::AND:
                ++noAnd;
                break;
            case storm::dft::storage::elements::DFTElementType::OR:
                ++noOr;
                break;
            case storm::dft::storage::elements::DFTElementType::VOT:
                ++noVot;
                break;
            case storm::dft::storage::elements::DFTElementType::PAND:
                ++noPand;
                break;
            case storm::dft::storage::elements::DFTElementType::POR:
                ++noPor;
                break;
            case storm::dft::storage::elements::DFTElementType::SPARE:
                ++noSpare;
                break;
            case storm::dft::storage::elements::DFTElementType::PDEP:
                ++noDependency;
                break;
            case storm::dft::storage::elements::DFTElementType::SEQ:
            case storm::dft::storage::elements::DFTElementType::MUTEX:
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
    stream << "=============DFT Statistics==============\n";
    stream << "Number of BEs: " << nrBasicElements() << '\n';
    stream << "Number of static elements: " << noStatic << '\n';
    stream << "Number of dynamic elements: " << noDynamic << '\n';
    stream << "Number of elements: " << nrElements() << '\n';
    stream << "-----------------------------------------\n";
    if (noBE > 0) {
        stream << "Number of BEs: " << noBE << '\n';
    }
    if (noAnd > 0) {
        stream << "Number of AND gates: " << noAnd << '\n';
    }
    if (noOr > 0) {
        stream << "Number of OR gates: " << noOr << '\n';
    }
    if (noVot > 0) {
        stream << "Number of VOT gates: " << noVot << '\n';
    }
    if (noPand > 0) {
        stream << "Number of PAND gates: " << noPand << '\n';
    }
    if (noPor > 0) {
        stream << "Number of POR gates: " << noPor << '\n';
    }
    if (noSpare > 0) {
        stream << "Number of SPARE gates: " << noSpare << '\n';
    }
    if (noDependency > 0) {
        stream << "Number of Dependencies: " << noDependency << '\n';
    }
    if (noRestriction > 0) {
        stream << "Number of Restrictions: " << noRestriction << '\n';
    }
    stream << "=========================================\n";
}

std::set<storm::RationalFunctionVariable> getParameters(DFT<storm::RationalFunction> const& dft) {
    std::set<storm::RationalFunctionVariable> result;
    for (size_t i = 0; i < dft.nrElements(); ++i) {
        std::shared_ptr<storm::dft::storage::elements::DFTElement<storm::RationalFunction> const> element = dft.getElement(i);
        switch (element->type()) {
            case storm::dft::storage::elements::DFTElementType::BE: {
                auto be = std::static_pointer_cast<storm::dft::storage::elements::DFTBE<storm::RationalFunction> const>(element);
                if (be->beType() == storm::dft::storage::elements::BEType::EXPONENTIAL) {
                    auto beExp = std::static_pointer_cast<storm::dft::storage::elements::BEExponential<storm::RationalFunction> const>(element);
                    beExp->activeFailureRate().gatherVariables(result);
                    beExp->dormancyFactor().gatherVariables(result);
                }
                break;
            }
            case storm::dft::storage::elements::DFTElementType::PDEP: {
                auto dep = std::static_pointer_cast<storm::dft::storage::elements::DFTDependency<storm::RationalFunction> const>(element);
                dep->probability().gatherVariables(result);
                break;
            }
            case storm::dft::storage::elements::DFTElementType::AND:
            case storm::dft::storage::elements::DFTElementType::OR:
            case storm::dft::storage::elements::DFTElementType::VOT:
            case storm::dft::storage::elements::DFTElementType::PAND:
            case storm::dft::storage::elements::DFTElementType::POR:
            case storm::dft::storage::elements::DFTElementType::SPARE:
            case storm::dft::storage::elements::DFTElementType::SEQ:
            case storm::dft::storage::elements::DFTElementType::MUTEX:
                // Do nothing
                break;
            default:
                STORM_LOG_THROW(false, storm::exceptions::NotImplementedException, "DFT type '" << element->type() << "' not known.");
        }
    }
    return result;
}

// Explicitly instantiate the class.
template class DFT<double>;
template class DFT<RationalFunction>;

}  // namespace storage
}  // namespace storm::dft
