#include "DftModularizer.h"

namespace storm::dft {
namespace utility {

template<typename ValueType>
std::vector<storm::dft::storage::DftModule> DftModularizer<ValueType>::computeModules(storm::dft::storage::DFT<ValueType> const &dft) {
    // Initialize data structures
    // dfsCounters/elementInfos must not be cleared because they are either not initialized or were cleared in a previous call of computeModules()
    for (auto const &id : dft.getAllIds()) {
        dfsCounters[id] = DfsCounter{};
        elementInfos[id] = ElementInfo{};
    }
    lastDate = 0;

    // First depth first search of the LTA/DR algorithm.
    populateDfsCounters(dft.getTopLevelElement());

    // Second depth first search of the LTA/DR algorithm.
    populateElementInfos(dft.getTopLevelElement());

    // Create modules
    std::vector<storm::dft::storage::DftModule> modules;
    for (auto const &elementInfo : elementInfos) {
        if (elementInfo.second.isModule) {
            storm::dft::storage::DftModule module(elementInfo.first, dft.getIndependentSubDftRoots(elementInfo.first));
            module.setType(dft);
            STORM_LOG_ASSERT(module.isStaticModule() == elementInfo.second.isStatic,
                             "Computation of module type gave different results than module algorithm.");
            modules.push_back(module);
        }
    }

    // Free some space
    dfsCounters.clear();
    elementInfos.clear();

    return modules;
}

template<typename ValueType>
void DftModularizer<ValueType>::populateDfsCounters(DFTElementCPointer const element) {
    auto &counter{dfsCounters.at(element->id())};

    ++lastDate;
    if (counter.firstVisit == 0) {
        // parent was never visited before
        // as 0 can never be a valid firstVisit
        counter.firstVisit = lastDate;

        // Continue recursively
        for (auto const &descendant : getDescendants(element)) {
            populateDfsCounters(descendant);
        }
        ++lastDate;
        counter.secondVisit = lastDate;
    }
    counter.lastVisit = lastDate;
}

template<typename ValueType>
void DftModularizer<ValueType>::populateElementInfos(DFTElementCPointer const element) {
    auto &counter{dfsCounters.at(element->id())};
    auto &elementInfo{elementInfos.at(element->id())};

    if (counter.minFirstVisit == 0) {
        // element was never visited before as min can never be 0

        // minFirstVisit <= secondVisit
        counter.minFirstVisit = counter.secondVisit;
        for (auto const &descendant : getDescendants(element)) {
            populateElementInfos(descendant);

            auto const &descendantCounter{dfsCounters.at(descendant->id())};
            auto const &descendantElementInfo{elementInfos.at(descendant->id())};

            counter.minFirstVisit = std::min({counter.minFirstVisit, descendantCounter.firstVisit, descendantCounter.minFirstVisit});
            counter.maxLastVisit = std::max({counter.maxLastVisit, descendantCounter.lastVisit, descendantCounter.maxLastVisit});

            // propagate dynamic property
            if (!descendantElementInfo.isStatic && !descendantElementInfo.isModule) {
                elementInfo.isStatic = false;
            }
        }

        if (!element->isStaticElement()) {
            elementInfo.isStatic = false;
        }

        if (!element->isBasicElement() && counter.firstVisit < counter.minFirstVisit && counter.maxLastVisit < counter.secondVisit) {
            elementInfo.isModule = true;
        }
    }
}

template<typename ValueType>
std::vector<typename DftModularizer<ValueType>::DFTElementCPointer> DftModularizer<ValueType>::getDescendants(DFTElementCPointer const element) {
    std::vector<DFTElementCPointer> descendants{};

    if (element->isDependency()) {
        auto const dependency{std::static_pointer_cast<storm::dft::storage::elements::DFTDependency<ValueType> const>(element)};

        auto const triggerElement{std::static_pointer_cast<storm::dft::storage::elements::DFTElement<ValueType> const>(dependency->triggerEvent())};
        descendants.push_back(triggerElement);

        auto const &dependentEvents{dependency->dependentEvents()};
        descendants.insert(descendants.end(), dependentEvents.begin(), dependentEvents.end());
    } else if (element->nrChildren() > 0) {
        auto const parent{std::static_pointer_cast<storm::dft::storage::elements::DFTChildren<ValueType> const>(element)};
        auto const &children = parent->children();
        descendants.insert(descendants.end(), children.begin(), children.end());
    }

    if (element->isBasicElement()) {
        auto const be{std::static_pointer_cast<storm::dft::storage::elements::DFTBE<ValueType> const>(element)};

        auto const &dependencies{be->ingoingDependencies()};
        descendants.insert(descendants.end(), dependencies.begin(), dependencies.end());
    }

    auto const &restrictions{element->restrictions()};
    descendants.insert(descendants.end(), restrictions.begin(), restrictions.end());

    auto const &dependencies{element->outgoingDependencies()};
    descendants.insert(descendants.end(), dependencies.begin(), dependencies.end());

    return descendants;
}

// Explicitly instantiate the class.
template class DftModularizer<double>;
template class DftModularizer<RationalFunction>;

}  // namespace utility
}  // namespace storm::dft
