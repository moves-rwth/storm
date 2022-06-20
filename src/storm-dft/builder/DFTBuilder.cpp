#include "DFTBuilder.h"

#include <algorithm>

#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/exceptions/WrongFormatException.h"
#include "storm/utility/macros.h"

#include "storm-dft/storage/DFT.h"
#include "storm-dft/storage/OrderDFTElementsById.h"

namespace storm::dft {
namespace builder {

template<typename ValueType>
storm::dft::storage::DFT<ValueType> DFTBuilder<ValueType>::build() {
    STORM_LOG_THROW(!mTopLevelName.empty(), storm::exceptions::WrongFormatException, "No top level element defined.");

    // Build parent/child connections for gates
    for (auto& gateChildrenPair : mChildNames) {
        STORM_LOG_ASSERT(gateChildrenPair.first->isGate(), "Element " << *gateChildrenPair.first << " with children is not a gate.");
        DFTGatePointer gate = std::static_pointer_cast<storm::dft::storage::elements::DFTGate<ValueType>>(gateChildrenPair.first);
        for (std::string const& childName : gateChildrenPair.second) {
            auto itFind = mElements.find(childName);
            STORM_LOG_THROW(itFind != mElements.end(), storm::exceptions::WrongFormatException,
                            "Child '" << childName << "' for gate '" << gate->name() << "' not found.");
            DFTElementPointer childElement = itFind->second;
            if (childElement->isRestriction()) {
                STORM_LOG_WARN("Restriction '" << childName << "' is ignored as input for gate '" << gate->name() << "', because restrictions have no output.");
            } else if (childElement->isDependency()) {
                STORM_LOG_WARN("Dependency '" << childName << "' is ignored as input for gate '" << gate->name() << "', because dependencies have no output.");
            } else {
                gate->addChild(childElement);
                childElement->addParent(gate);
            }
        }
    }

    // Build connections for restrictions
    for (auto& restrictionChildrenPair : mRestrictionChildNames) {
        DFTRestrictionPointer restriction = restrictionChildrenPair.first;
        for (std::string const& childName : restrictionChildrenPair.second) {
            auto itFind = mElements.find(childName);
            STORM_LOG_THROW(itFind != mElements.end(), storm::exceptions::WrongFormatException,
                            "Child '" << childName << "' for restriction '" << restriction->name() << "' not found.");
            DFTElementPointer childElement = itFind->second;
            STORM_LOG_THROW(childElement->isGate() || childElement->isBasicElement(), storm::exceptions::WrongFormatException,
                            "Child '" << childElement->name() << "' of restriction '" << restriction->name() << "' must be gate or BE.");
            restriction->addChild(childElement);
            childElement->addRestriction(restriction);
        }
    }

    // Build connections for dependencies
    for (auto& dependencyChildrenPair : mDependencyChildNames) {
        DFTDependencyPointer dependency = dependencyChildrenPair.first;
        bool triggerElement = true;
        for (std::string const& childName : dependencyChildrenPair.second) {
            auto itFind = mElements.find(childName);
            STORM_LOG_THROW(itFind != mElements.end(), storm::exceptions::WrongFormatException,
                            "Child '" << childName << "' for dependency '" << dependency->name() << "' not found.");
            DFTElementPointer childElement = itFind->second;
            if (triggerElement) {
                triggerElement = false;
                STORM_LOG_THROW(childElement->isGate() || childElement->isBasicElement(), storm::exceptions::WrongFormatException,
                                "Trigger element '" << childName << "' of dependency '" << dependency->name() << "' must be gate or BE.");
                dependency->setTriggerElement(std::static_pointer_cast<storm::dft::storage::elements::DFTGate<ValueType>>(childElement));
                childElement->addOutgoingDependency(dependency);
            } else {
                STORM_LOG_THROW(childElement->isBasicElement(), storm::exceptions::WrongFormatException,
                                "Dependent element '" << childName << "' of dependency '" << dependency->name() << "' must be BE.");
                DFTBEPointer dependentBE = std::static_pointer_cast<storm::dft::storage::elements::DFTBE<ValueType>>(childElement);
                dependency->addDependentEvent(dependentBE);
                dependentBE->addIngoingDependency(dependency);
            }
        }
    }

    // Sort elements topologically
    DFTElementVector elements = sortTopological();
    // Set ids according to order
    size_t id = 0;
    for (DFTElementPointer e : elements) {
        e->setId(id++);
    }
    // Compute rank
    computeRank(mElements[mTopLevelName]);  // Start with top level element
    for (auto& elem : mElements) {
        computeRank(elem.second);
    }

    // Create DFT
    storm::dft::storage::DFT<ValueType> dft(elements, mElements[mTopLevelName]);

    // Set layout info
    for (auto& elem : mElements) {
        if (mLayoutInfo.count(elem.first) > 0) {
            // Use given layout info
            dft.setElementLayoutInfo(elem.second->id(), mLayoutInfo.at(elem.first));
        } else {
            // Set default layout
            dft.setElementLayoutInfo(elem.second->id(), storm::dft::storage::DFTLayoutInfo());
        }
    }

    return dft;
}

template<typename ValueType>
bool DFTBuilder<ValueType>::addElement(DFTElementPointer element) {
    if (nameInUse(element->name())) {
        STORM_LOG_ERROR("Element with name '" << element->name() << "' already exists.");
        return false;
    }
    STORM_LOG_ASSERT(mNextId == mElements.size(), "Current id " << mNextId << " and number of elements " << mElements.size() << " do not match.");
    element->setId(mNextId++);
    mElements[element->name()] = element;
    return true;
}

template<typename ValueType>
bool DFTBuilder<ValueType>::addBasicElementConst(std::string const& name, bool failed) {
    return addElement(std::make_shared<storm::dft::storage::elements::BEConst<ValueType>>(0, name, failed));
}

template<typename ValueType>
bool DFTBuilder<ValueType>::addBasicElementProbability(std::string const& name, ValueType probability, ValueType dormancyFactor) {
    // Handle special cases
    if (storm::utility::isZero<ValueType>(probability)) {
        return addBasicElementConst(name, false);
    } else if (storm::utility::isOne<ValueType>(probability)) {
        return addBasicElementConst(name, true);
    }
    // TODO: check 0 <= dormancyFactor <= 1
    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Constant probability distribution is not supported for basic element '" << name << "'.");
    return false;
}

template<typename ValueType>
bool DFTBuilder<ValueType>::addBasicElementExponential(std::string const& name, ValueType rate, ValueType dormancyFactor, bool transient) {
    // Handle special cases
    if (storm::utility::isZero<ValueType>(rate)) {
        return addBasicElementConst(name, false);
    }

    // TODO: check 0 <= dormancyFactor <= 1
    return addElement(std::make_shared<storm::dft::storage::elements::BEExponential<ValueType>>(0, name, rate, dormancyFactor, transient));
}

template<typename ValueType>
bool DFTBuilder<ValueType>::addBasicElementSamples(std::string const& name, std::map<ValueType, ValueType> const& activeSamples) {
    // Check if it can fail
    auto be = std::make_shared<storm::dft::storage::elements::BESamples<ValueType>>(0, name, activeSamples);
    if (!be->canFail()) {
        // Add constant failed BE instead
        return addBasicElementConst(name, false);
    }

    return addElement(be);
}

template<typename ValueType>
bool DFTBuilder<ValueType>::addGate(DFTGatePointer gate, std::vector<std::string> const& children) {
    if (children.size() == 0) {
        STORM_LOG_ERROR("No children given for gate " << gate << ".");
        return false;
    }
    mChildNames[gate] = children;
    return addElement(gate);
}

template<typename ValueType>
bool DFTBuilder<ValueType>::addAndGate(std::string const& name, std::vector<std::string> const& children) {
    return addGate(std::make_shared<storm::dft::storage::elements::DFTAnd<ValueType>>(0, name), children);
}

template<typename ValueType>
bool DFTBuilder<ValueType>::addOrGate(std::string const& name, std::vector<std::string> const& children) {
    return addGate(std::make_shared<storm::dft::storage::elements::DFTOr<ValueType>>(0, name), children);
}

template<typename ValueType>
bool DFTBuilder<ValueType>::addVotingGate(std::string const& name, unsigned threshold, std::vector<std::string> const& children) {
    // Handle special cases
    if (children.size() == threshold) {
        return addAndGate(name, children);
    }
    if (threshold == 1) {
        return addOrGate(name, children);
    }

    if (threshold > children.size()) {
        STORM_LOG_ERROR("Voting gate " << name << " has threshold " << threshold << " higher than the number of children " << children.size() << ".");
        return false;
    }
    return addGate(std::make_shared<storm::dft::storage::elements::DFTVot<ValueType>>(0, name, threshold), children);
}

template<typename ValueType>
bool DFTBuilder<ValueType>::addPandGate(std::string const& name, std::vector<std::string> const& children, bool inclusive) {
    return addGate(std::make_shared<storm::dft::storage::elements::DFTPand<ValueType>>(0, name, inclusive), children);
}

template<typename ValueType>
bool DFTBuilder<ValueType>::addPorGate(std::string const& name, std::vector<std::string> const& children, bool inclusive) {
    return addGate(std::make_shared<storm::dft::storage::elements::DFTPor<ValueType>>(0, name, inclusive), children);
}

template<typename ValueType>
bool DFTBuilder<ValueType>::addSpareGate(std::string const& name, std::vector<std::string> const& children) {
    return addGate(std::make_shared<storm::dft::storage::elements::DFTSpare<ValueType>>(0, name), children);
}

template<typename ValueType>
bool DFTBuilder<ValueType>::addDependency(DFTDependencyPointer dependency, std::vector<std::string> const& children) {
    if (children.size() <= 1) {
        STORM_LOG_ERROR("Dependency " << dependency->name() << " requires at least two children.");
        return false;
    }
    if (storm::utility::isZero(dependency->probability())) {
        STORM_LOG_WARN("Dependency " << dependency->name() << " with probability 0 is superfluous.");
        // Element is superfluous
        return true;
    }
    // TODO: collect constraints for SMT solving

    mDependencyChildNames[dependency] = children;
    return addElement(dependency);
}

template<typename ValueType>
bool DFTBuilder<ValueType>::addPdep(std::string const& name, std::vector<std::string> const& children, ValueType probability) {
    return addDependency(std::make_shared<storm::dft::storage::elements::DFTDependency<ValueType>>(0, name, probability), children);
}

template<typename ValueType>
bool DFTBuilder<ValueType>::addRestriction(DFTRestrictionPointer restriction, std::vector<std::string> const& children) {
    if (children.size() <= 1) {
        STORM_LOG_ERROR("Restrictions require at least two children");
        return false;
    }

    mRestrictionChildNames[restriction] = children;
    return addElement(restriction);
}

template<typename ValueType>
bool DFTBuilder<ValueType>::addSequenceEnforcer(std::string const& name, std::vector<std::string> const& children) {
    return addRestriction(std::make_shared<storm::dft::storage::elements::DFTSeq<ValueType>>(0, name), children);
}

template<typename ValueType>
bool DFTBuilder<ValueType>::addMutex(std::string const& name, std::vector<std::string> const& children) {
    return addRestriction(std::make_shared<storm::dft::storage::elements::DFTMutex<ValueType>>(0, name), children);
}

template<typename ValueType>
bool DFTBuilder<ValueType>::setTopLevel(std::string const& tle) {
    if (!mTopLevelName.empty()) {
        STORM_LOG_ERROR("Top level element was already set");
        return false;
    }
    if (!nameInUse(tle)) {
        STORM_LOG_ERROR("Element with name '" << tle << "' not known.");
        return false;
    }
    mTopLevelName = tle;
    return true;
}

template<typename ValueType>
void DFTBuilder<ValueType>::addLayoutInfo(std::string const& name, double x, double y) {
    if (!nameInUse(name)) {
        STORM_LOG_ERROR("Element with name '" << name << "' not found.");
    }
    mLayoutInfo[name] = storm::dft::storage::DFTLayoutInfo(x, y);
}

template<typename ValueType>
void DFTBuilder<ValueType>::cloneElement(DFTElementCPointer element) {
    switch (element->type()) {
        case storm::dft::storage::elements::DFTElementType::BE:
            addElement(element->clone());
            break;
        case storm::dft::storage::elements::DFTElementType::AND:
        case storm::dft::storage::elements::DFTElementType::OR:
        case storm::dft::storage::elements::DFTElementType::VOT:
        case storm::dft::storage::elements::DFTElementType::PAND:
        case storm::dft::storage::elements::DFTElementType::POR:
        case storm::dft::storage::elements::DFTElementType::SPARE:
        case storm::dft::storage::elements::DFTElementType::SEQ:
        case storm::dft::storage::elements::DFTElementType::MUTEX: {
            auto elemWithChildren = std::static_pointer_cast<storm::dft::storage::elements::DFTChildren<ValueType> const>(element);
            std::vector<std::string> children{};
            for (DFTElementPointer const& elem : elemWithChildren->children()) {
                children.push_back(elem->name());
            }
            cloneElementWithNewChildren(elemWithChildren, children);
            break;
        }
        case storm::dft::storage::elements::DFTElementType::PDEP: {
            auto dependency = std::static_pointer_cast<storm::dft::storage::elements::DFTDependency<ValueType> const>(element);
            std::vector<std::string> children = {dependency->triggerEvent()->name()};
            for (auto const& depEv : dependency->dependentEvents()) {
                children.push_back(depEv->name());
            }
            addDependency(std::static_pointer_cast<storm::dft::storage::elements::DFTDependency<ValueType>>(dependency->clone()), children);
            break;
        }
        default:
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "DFT element type '" << element->type() << "' not known.");
            break;
    }
}

template<typename ValueType>
void DFTBuilder<ValueType>::cloneElementWithNewChildren(DFTChildrenCPointer elemWithChildren, std::vector<std::string> const& children) {
    switch (elemWithChildren->type()) {
        case storm::dft::storage::elements::DFTElementType::AND:
        case storm::dft::storage::elements::DFTElementType::OR:
        case storm::dft::storage::elements::DFTElementType::VOT:
        case storm::dft::storage::elements::DFTElementType::PAND:
        case storm::dft::storage::elements::DFTElementType::POR:
        case storm::dft::storage::elements::DFTElementType::SPARE:
            addGate(std::static_pointer_cast<storm::dft::storage::elements::DFTGate<ValueType>>(elemWithChildren->clone()), children);
            break;
        case storm::dft::storage::elements::DFTElementType::SEQ:
        case storm::dft::storage::elements::DFTElementType::MUTEX:
            addRestriction(std::static_pointer_cast<storm::dft::storage::elements::DFTRestriction<ValueType>>(elemWithChildren->clone()), children);
            break;
        default:
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "DFT element type '" << elemWithChildren->type() << "' not known.");
            break;
    }
}

template<typename ValueType>
void DFTBuilder<ValueType>::topologicalVisit(DFTElementPointer const& element,
                                             std::map<DFTElementPointer, topoSortColour, storm::dft::storage::OrderElementsById<ValueType>>& visited,
                                             DFTElementVector& visitedElements) {
    STORM_LOG_THROW(visited[element] != topoSortColour::GREY, storm::exceptions::WrongFormatException, "DFT is cyclic.");
    if (visited[element] == topoSortColour::WHITE) {
        // Mark as currently visiting
        visited[element] = topoSortColour::GREY;
        // Element was not visited before
        if (element->isGate()) {
            for (DFTElementPointer const& child : std::static_pointer_cast<storm::dft::storage::elements::DFTGate<ValueType>>(element)->children()) {
                // Recursively visit all children
                topologicalVisit(child, visited, visitedElements);
            }
        }
        // TODO: restrictions and dependencies have no parents, so this can be done more efficiently.
        else if (element->isRestriction()) {
            for (DFTElementPointer const& child : std::static_pointer_cast<storm::dft::storage::elements::DFTRestriction<ValueType>>(element)->children()) {
                // Recursively visit all children
                topologicalVisit(child, visited, visitedElements);
            }
        } else if (element->isDependency()) {
            for (DFTBEPointer const& child : std::static_pointer_cast<storm::dft::storage::elements::DFTDependency<ValueType>>(element)->dependentEvents()) {
                // Recursively visit all dependent children
                topologicalVisit(child, visited, visitedElements);
            }
            // Visit trigger element
            topologicalVisit(std::static_pointer_cast<storm::dft::storage::elements::DFTDependency<ValueType>>(element)->triggerEvent(), visited,
                             visitedElements);
        } else {
            STORM_LOG_ASSERT(element->isBasicElement(), "Unknown element type " << element->type() << " for " << *element);
        }
        // Mark as completely visited
        visited[element] = topoSortColour::BLACK;
        // Children have all been visited before -> add element to list
        visitedElements.push_back(element);
    }
}

template<typename ValueType>
std::vector<std::shared_ptr<storm::dft::storage::elements::DFTElement<ValueType>>> DFTBuilder<ValueType>::sortTopological() {
    // Prepare map
    std::map<DFTElementPointer, topoSortColour, storm::dft::storage::OrderElementsById<ValueType>> visited;
    for (auto const& e : mElements) {
        visited.insert(std::make_pair(e.second, topoSortColour::WHITE));
    }

    DFTElementVector visitedElements;  // Return argument
    // Start from top level element
    topologicalVisit(mElements[mTopLevelName], visited, visitedElements);
    for (auto const& e : visited) {
        // Visit all elements to account for restrictions/dependencies/etc. not directly reachable from top level element
        topologicalVisit(e.first, visited, visitedElements);
    }
    return visitedElements;
}

template<typename ValueType>
size_t DFTBuilder<ValueType>::computeRank(DFTElementPointer const& elem) {
    if (elem->rank() == static_cast<decltype(elem->rank())>(-1)) {
        // Compute rank
        if (elem->isBasicElement() || elem->isDependency() || elem->isRestriction()) {
            // Rank is 0 for BEs/dependencies/restrictions
            elem->setRank(0);
        } else {
            // Rank is maximal rank of children + 1
            DFTGatePointer gate = std::static_pointer_cast<storm::dft::storage::elements::DFTGate<ValueType>>(elem);
            size_t maxRank = 0;
            for (DFTElementPointer const& child : gate->children()) {
                maxRank = std::max(maxRank, computeRank(child));
            }
            elem->setRank(maxRank + 1);
        }
    }

    return elem->rank();
}

// Explicitly instantiate the class.
template class DFTBuilder<double>;
template class DFTBuilder<RationalFunction>;

}  // namespace builder
}  // namespace storm::dft
