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
std::size_t DFTBuilder<ValueType>::mUniqueOffset = 0;

template<typename ValueType>
storm::dft::storage::DFT<ValueType> DFTBuilder<ValueType>::build() {
    // Build parent/child connections between elements
    for (auto& elem : mChildNames) {
        DFTGatePointer gate = std::static_pointer_cast<storm::dft::storage::elements::DFTGate<ValueType>>(elem.first);
        for (auto const& child : elem.second) {
            auto itFind = mElements.find(child);
            if (itFind != mElements.end()) {
                // Child found
                DFTElementPointer childElement = itFind->second;
                if (childElement->isRestriction()) {
                    STORM_LOG_WARN("Restriction '" << child << "' is not used as input for gate '" << gate->name()
                                                   << "', because restrictions have no output.");
                } else if (childElement->isDependency()) {
                    STORM_LOG_WARN("Dependency '" << child << "' is not used as input for gate '" << gate->name() << "', because dependencies have no output.");
                } else {
                    gate->pushBackChild(childElement);
                    childElement->addParent(gate);
                }
            } else {
                // Child not found -> find first dependent event to assure that child is dependency
                // TODO: Not sure whether this is the intended behaviour?
                auto itFind = mElements.find(child + "_1");
                STORM_LOG_THROW(itFind != mElements.end(), storm::exceptions::WrongFormatException,
                                "Child '" << child << "' for gate '" << gate->name() << "' not found.");
                STORM_LOG_THROW(itFind->second->isDependency(), storm::exceptions::WrongFormatException, "Child '" << child << "'is no dependency.");
                STORM_LOG_TRACE("Ignore functional dependency " << child << " in gate " << gate->name());
            }
        }
    }

    // Build connections for restrictions
    for (auto& elem : mRestrictionChildNames) {
        for (auto const& childName : elem.second) {
            auto itFind = mElements.find(childName);
            STORM_LOG_THROW(itFind != mElements.end(), storm::exceptions::WrongFormatException,
                            "Child '" << childName << "' for gate '" << elem.first->name() << "' not found.");
            DFTElementPointer childElement = itFind->second;
            STORM_LOG_THROW(childElement->isGate() || childElement->isBasicElement(), storm::exceptions::WrongFormatException,
                            "Child '" << childElement->name() << "' of restriction '" << elem.first->name() << "' must be gate or BE.");
            elem.first->pushBackChild(childElement);
            childElement->addRestriction(elem.first);
        }
    }

    // Build connections for dependencies
    for (auto& elem : mDependencyChildNames) {
        bool first = true;
        std::vector<std::shared_ptr<storm::dft::storage::elements::DFTBE<ValueType>>> dependencies;
        for (auto const& childName : elem.second) {
            auto itFind = mElements.find(childName);
            STORM_LOG_THROW(itFind != mElements.end(), storm::exceptions::WrongFormatException,
                            "Child '" << childName << "' for gate '" << elem.first->name() << "' not found.");
            DFTElementPointer childElement = itFind->second;
            if (!first) {
                STORM_LOG_THROW(childElement->isBasicElement(), storm::exceptions::WrongFormatException,
                                "Child '" << childName << "' of dependency '" << elem.first->name() << "' must be BE.");
                dependencies.push_back(std::static_pointer_cast<storm::dft::storage::elements::DFTBE<ValueType>>(childElement));
            } else {
                first = false;
                STORM_LOG_THROW(childElement->isGate() || childElement->isBasicElement(), storm::exceptions::WrongFormatException,
                                "Child '" << childName << "' of dependency '" << elem.first->name() << "' must be gate or BE.");
                elem.first->setTriggerElement(std::static_pointer_cast<storm::dft::storage::elements::DFTGate<ValueType>>(childElement));
                childElement->addOutgoingDependency(elem.first);
            }
        }
        for (auto& be : dependencies) {
            elem.first->addDependentEvent(be);
            be->addIngoingDependency(elem.first);
        }
    }

    // Sort elements topologically
    DFTElementVector elems = topoSort();
    // compute rank
    for (auto& elem : mElements) {
        computeRank(elem.second);
    }
    // Set ids
    size_t id = 0;
    for (DFTElementPointer e : elems) {
        e->setId(id++);
    }

    STORM_LOG_THROW(!mTopLevelIdentifier.empty(), storm::exceptions::WrongFormatException, "No top level element defined.");
    storm::dft::storage::DFT<ValueType> dft(elems, mElements[mTopLevelIdentifier]);

    // Set layout info
    for (auto& elem : mElements) {
        if (mLayoutInfo.count(elem.first) > 0) {
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

    mElements[element->name()] = element;
    return true;
}

template<typename ValueType>
bool DFTBuilder<ValueType>::addBasicElementConst(std::string const& name, bool failed) {
    return addElement(std::make_shared<storm::dft::storage::elements::BEConst<ValueType>>(mNextId++, name, failed));
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
    return addElement(std::make_shared<storm::dft::storage::elements::BEExponential<ValueType>>(mNextId++, name, rate, dormancyFactor, transient));
}

template<typename ValueType>
bool DFTBuilder<ValueType>::addBasicElementSamples(std::string const& name, std::map<ValueType, ValueType> const& activeSamples) {
    // Check if it can fail
    auto be = std::make_shared<storm::dft::storage::elements::BESamples<ValueType>>(mNextId++, name, activeSamples);
    if (!be->canFail()) {
        --mNextId;  // Created BE cannot be used
        // Add constant failed BE
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
    return addGate(std::make_shared<storm::dft::storage::elements::DFTAnd<ValueType>>(mNextId++, name), children);
}

template<typename ValueType>
bool DFTBuilder<ValueType>::addOrGate(std::string const& name, std::vector<std::string> const& children) {
    return addGate(std::make_shared<storm::dft::storage::elements::DFTOr<ValueType>>(mNextId++, name), children);
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
    return addGate(std::make_shared<storm::dft::storage::elements::DFTVot<ValueType>>(mNextId++, name, threshold), children);
}

template<typename ValueType>
bool DFTBuilder<ValueType>::addPandGate(std::string const& name, std::vector<std::string> const& children, bool inclusive) {
    return addGate(std::make_shared<storm::dft::storage::elements::DFTPand<ValueType>>(mNextId++, name, inclusive), children);
}

template<typename ValueType>
bool DFTBuilder<ValueType>::addPorGate(std::string const& name, std::vector<std::string> const& children, bool inclusive) {
    return addGate(std::make_shared<storm::dft::storage::elements::DFTPor<ValueType>>(mNextId++, name, inclusive), children);
}

template<typename ValueType>
bool DFTBuilder<ValueType>::addSpareGate(std::string const& name, std::vector<std::string> const& children) {
    return addGate(std::make_shared<storm::dft::storage::elements::DFTSpare<ValueType>>(mNextId++, name), children);
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
    return addDependency(std::make_shared<storm::dft::storage::elements::DFTDependency<ValueType>>(mNextId++, name, probability), children);
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
    return addRestriction(std::make_shared<storm::dft::storage::elements::DFTSeq<ValueType>>(mNextId++, name), children);
}

template<typename ValueType>
bool DFTBuilder<ValueType>::addMutex(std::string const& name, std::vector<std::string> const& children) {
    return addRestriction(std::make_shared<storm::dft::storage::elements::DFTMutex<ValueType>>(mNextId++, name), children);
}

template<typename ValueType>
unsigned DFTBuilder<ValueType>::computeRank(DFTElementPointer const& elem) {
    if (elem->rank() == static_cast<decltype(elem->rank())>(-1)) {
        if (elem->nrChildren() == 0 || elem->isDependency() || elem->isRestriction()) {
            elem->setRank(0);
        } else {
            DFTGatePointer gate = std::static_pointer_cast<storm::dft::storage::elements::DFTGate<ValueType>>(elem);
            unsigned maxrnk = 0;
            unsigned newrnk = 0;

            for (DFTElementPointer const& child : gate->children()) {
                newrnk = computeRank(child);
                if (newrnk > maxrnk) {
                    maxrnk = newrnk;
                }
            }
            elem->setRank(maxrnk + 1);
        }
    }

    return elem->rank();
}

template<typename ValueType>
void DFTBuilder<ValueType>::topoVisit(DFTElementPointer const& n,
                                      std::map<DFTElementPointer, topoSortColour, storm::dft::storage::OrderElementsById<ValueType>>& visited,
                                      DFTElementVector& L) {
    STORM_LOG_THROW(visited[n] != topoSortColour::GREY, storm::exceptions::WrongFormatException, "DFT is cyclic");
    if (visited[n] == topoSortColour::WHITE) {
        if (n->isGate()) {
            visited[n] = topoSortColour::GREY;
            for (DFTElementPointer const& c : std::static_pointer_cast<storm::dft::storage::elements::DFTGate<ValueType>>(n)->children()) {
                topoVisit(c, visited, L);
            }
        }
        // TODO restrictions and dependencies have no parents, so this can be done more efficiently.
        else if (n->isRestriction()) {
            visited[n] = topoSortColour::GREY;
            for (DFTElementPointer const& c : std::static_pointer_cast<storm::dft::storage::elements::DFTRestriction<ValueType>>(n)->children()) {
                topoVisit(c, visited, L);
            }
        } else if (n->isDependency()) {
            visited[n] = topoSortColour::GREY;
            for (DFTBEPointer const& c : std::static_pointer_cast<storm::dft::storage::elements::DFTDependency<ValueType>>(n)->dependentEvents()) {
                topoVisit(c, visited, L);
            }
            topoVisit(std::static_pointer_cast<storm::dft::storage::elements::DFTDependency<ValueType>>(n)->triggerEvent(), visited, L);
        }
        visited[n] = topoSortColour::BLACK;
        L.push_back(n);
    }
}

template<typename ValueType>
std::vector<std::shared_ptr<storm::dft::storage::elements::DFTElement<ValueType>>> DFTBuilder<ValueType>::topoSort() {
    std::map<DFTElementPointer, topoSortColour, storm::dft::storage::OrderElementsById<ValueType>> visited;
    for (auto const& e : mElements) {
        visited.insert(std::make_pair(e.second, topoSortColour::WHITE));
    }

    DFTElementVector L;
    for (auto const& e : visited) {
        topoVisit(e.first, visited, L);
    }
    // std::reverse(L.begin(), L.end());
    return L;
}

template<typename ValueType>
std::string DFTBuilder<ValueType>::getUniqueName(std::string name) {
    return name + "_" + std::to_string(++mUniqueOffset);
}

template<typename ValueType>
void DFTBuilder<ValueType>::copyElement(DFTElementCPointer element) {
    std::vector<std::string> children;
    switch (element->type()) {
        case storm::dft::storage::elements::DFTElementType::BE:
            copyBE(std::static_pointer_cast<storm::dft::storage::elements::DFTBE<ValueType> const>(element));
            break;
        case storm::dft::storage::elements::DFTElementType::AND:
        case storm::dft::storage::elements::DFTElementType::OR:
        case storm::dft::storage::elements::DFTElementType::PAND:
        case storm::dft::storage::elements::DFTElementType::POR:
        case storm::dft::storage::elements::DFTElementType::SPARE:
        case storm::dft::storage::elements::DFTElementType::VOT: {
            for (DFTElementPointer const& elem : std::static_pointer_cast<storm::dft::storage::elements::DFTGate<ValueType> const>(element)->children()) {
                children.push_back(elem->name());
            }
            copyGate(std::static_pointer_cast<storm::dft::storage::elements::DFTGate<ValueType> const>(element), children);
            break;
        }
        case storm::dft::storage::elements::DFTElementType::PDEP: {
            auto dependency = std::static_pointer_cast<storm::dft::storage::elements::DFTDependency<ValueType> const>(element);
            children.push_back(dependency->triggerEvent()->name());
            for (auto const& depEv : dependency->dependentEvents()) {
                children.push_back(depEv->name());
            }
            addPdep(element->name(), children, dependency->probability());
            break;
        }
        case storm::dft::storage::elements::DFTElementType::SEQ:
        case storm::dft::storage::elements::DFTElementType::MUTEX: {
            for (DFTElementPointer const& elem :
                 std::static_pointer_cast<storm::dft::storage::elements::DFTRestriction<ValueType> const>(element)->children()) {
                children.push_back(elem->name());
            }
            if (element->type() == storm::dft::storage::elements::DFTElementType::SEQ) {
                addSequenceEnforcer(element->name(), children);
            } else {
                addMutex(element->name(), children);
            }
            break;
        }
        default:
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "DFT type '" << element->type() << "' not known.");
            break;
    }
}

template<typename ValueType>
void DFTBuilder<ValueType>::copyBE(DFTBECPointer be) {
    switch (be->beType()) {
        case storm::dft::storage::elements::BEType::CONSTANT: {
            auto beConst = std::static_pointer_cast<storm::dft::storage::elements::BEConst<ValueType> const>(be);
            addBasicElementConst(beConst->name(), beConst->failed());
            break;
        }
        case storm::dft::storage::elements::BEType::EXPONENTIAL: {
            auto beExp = std::static_pointer_cast<storm::dft::storage::elements::BEExponential<ValueType> const>(be);
            addBasicElementExponential(beExp->name(), beExp->activeFailureRate(), beExp->dormancyFactor(), beExp->isTransient());
            break;
        }
        case storm::dft::storage::elements::BEType::SAMPLES: {
            auto beSamples = std::static_pointer_cast<storm::dft::storage::elements::BESamples<ValueType> const>(be);
            addBasicElementSamples(beSamples->name(), beSamples->activeSamples());
            break;
        }
        default:
            STORM_LOG_ASSERT(false, "BE type not known.");
            break;
    }
}

template<typename ValueType>
void DFTBuilder<ValueType>::copyGate(DFTGateCPointer gate, std::vector<std::string> const& children) {
    switch (gate->type()) {
        case storm::dft::storage::elements::DFTElementType::AND:
            addAndGate(gate->name(), children);
            break;
        case storm::dft::storage::elements::DFTElementType::OR:
            addOrGate(gate->name(), children);
            break;
        case storm::dft::storage::elements::DFTElementType::PAND:
            addPandGate(gate->name(), children);
            break;
        case storm::dft::storage::elements::DFTElementType::POR:
            addPorGate(gate->name(), children);
            break;
        case storm::dft::storage::elements::DFTElementType::SPARE:
            addSpareGate(gate->name(), children);
            break;
        case storm::dft::storage::elements::DFTElementType::VOT:
            addVotingGate(gate->name(), std::static_pointer_cast<storm::dft::storage::elements::DFTVot<ValueType> const>(gate)->threshold(), children);
            break;
        default:
            STORM_LOG_ASSERT(false, "Dft type not known.");
            break;
    }
}

// Explicitly instantiate the class.
template class DFTBuilder<double>;
template class DFTBuilder<RationalFunction>;

}  // namespace builder
}  // namespace storm::dft
