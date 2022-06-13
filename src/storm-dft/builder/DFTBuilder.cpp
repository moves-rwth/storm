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
bool DFTBuilder<ValueType>::addRestriction(std::string const& name, std::vector<std::string> const& children,
                                           storm::dft::storage::elements::DFTElementType tp) {
    if (children.size() <= 1) {
        STORM_LOG_ERROR("Restrictions require at least two children");
    }
    if (nameInUse(name)) {
        STORM_LOG_ERROR("Element with name '" << name << "' already exists.");
        return false;
    }
    DFTRestrictionPointer restr;
    switch (tp) {
        case storm::dft::storage::elements::DFTElementType::SEQ:
            restr = std::make_shared<storm::dft::storage::elements::DFTSeq<ValueType>>(mNextId++, name);
            break;
        case storm::dft::storage::elements::DFTElementType::MUTEX:
            restr = std::make_shared<storm::dft::storage::elements::DFTMutex<ValueType>>(mNextId++, name);
            break;
        default:
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Gate type not known.");
            break;
    }

    mElements[name] = restr;
    mRestrictionChildNames[restr] = children;
    mRestrictions.push_back(restr);
    return true;
}

template<typename ValueType>
bool DFTBuilder<ValueType>::addStandardGate(std::string const& name, std::vector<std::string> const& children,
                                            storm::dft::storage::elements::DFTElementType tp) {
    STORM_LOG_ASSERT(children.size() > 0, "No child for " << name);
    if (nameInUse(name)) {
        STORM_LOG_ERROR("Element with name '" << name << "' already exists.");
        return false;
    }
    DFTElementPointer element;
    switch (tp) {
        case storm::dft::storage::elements::DFTElementType::AND:
            element = std::make_shared<storm::dft::storage::elements::DFTAnd<ValueType>>(mNextId++, name);
            break;
        case storm::dft::storage::elements::DFTElementType::OR:
            element = std::make_shared<storm::dft::storage::elements::DFTOr<ValueType>>(mNextId++, name);
            break;
        case storm::dft::storage::elements::DFTElementType::PAND:
            element = std::make_shared<storm::dft::storage::elements::DFTPand<ValueType>>(mNextId++, name, pandDefaultInclusive);
            break;
        case storm::dft::storage::elements::DFTElementType::POR:
            element = std::make_shared<storm::dft::storage::elements::DFTPor<ValueType>>(mNextId++, name, porDefaultInclusive);
            break;
        case storm::dft::storage::elements::DFTElementType::SPARE:
            element = std::make_shared<storm::dft::storage::elements::DFTSpare<ValueType>>(mNextId++, name);
            break;
        case storm::dft::storage::elements::DFTElementType::BE:
        case storm::dft::storage::elements::DFTElementType::VOT:
        case storm::dft::storage::elements::DFTElementType::PDEP:
            // Handled separately
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Gate type handled separately.");
        default:
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Gate type not known.");
    }
    mElements[name] = element;
    mChildNames[element] = children;
    return true;
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
            addDepElement(element->name(), children, dependency->probability());
            break;
        }
        case storm::dft::storage::elements::DFTElementType::SEQ:
        case storm::dft::storage::elements::DFTElementType::MUTEX: {
            for (DFTElementPointer const& elem :
                 std::static_pointer_cast<storm::dft::storage::elements::DFTRestriction<ValueType> const>(element)->children()) {
                children.push_back(elem->name());
            }
            addRestriction(element->name(), children, element->type());
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
        case storm::dft::storage::elements::DFTElementType::OR:
        case storm::dft::storage::elements::DFTElementType::PAND:
        case storm::dft::storage::elements::DFTElementType::POR:
        case storm::dft::storage::elements::DFTElementType::SPARE:
            addStandardGate(gate->name(), children, gate->type());
            break;
        case storm::dft::storage::elements::DFTElementType::VOT:
            addVotElement(gate->name(), std::static_pointer_cast<storm::dft::storage::elements::DFTVot<ValueType> const>(gate)->threshold(), children);
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
