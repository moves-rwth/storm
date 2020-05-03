#pragma once

#include <memory>
#include <sstream>
#include <vector>

#include "DFTModelChecker.h"
#include "storm-dft/builder/DFTBuilder.h"
#include "storm-dft/modelchecker/dft/DFTModelChecker.h"
#include "storm-dft/modelchecker/dft/SFTBDDChecker.h"
#include "storm-dft/storage/dft/DFT.h"
#include "storm-parsers/api/properties.h"
#include "storm/api/properties.h"

namespace storm {
namespace modelchecker {

/**
 * Main class for BDD accelerated DFT checking
 *
 */
template <typename ValueType>
class DFTModularizer {
   public:
    using ElementId = size_t;
    using DFTElementCPointer =
        std::shared_ptr<storm::storage::DFTElement<ValueType> const>;

    /**
     * Calculates Modules
     */
    DFTModularizer(std::shared_ptr<storm::storage::DFT<ValueType>> dft)
        : dft{std::move(dft)} {
        populateDfsCounters();
        populateElementInfos();
    }

    /**
     * \return
     * The Probability that the top level gate fails.
     */
    double getProbabilityAtTimebound(double const timebound) {
        workDFT = dft;

        auto topLevelElement{std::static_pointer_cast<
            storm::storage::DFTElement<ValueType> const>(
            workDFT->getTopLevelGate())};
        replaceDynamicModules(topLevelElement, timebound);

        topLevelElement = std::static_pointer_cast<
            storm::storage::DFTElement<ValueType> const>(
            workDFT->getTopLevelGate());

        auto const probability{analyseStatic(topLevelElement, timebound)};
        return probability;
    }

   private:
    std::shared_ptr<storm::storage::DFT<ValueType>> dft;

    /**
     * \return All connected DFTElements of the given element
     */
    static std::vector<DFTElementCPointer> getDecendants(
        DFTElementCPointer const element) noexcept {
        std::vector<DFTElementCPointer> decendants{};

        if (element->isDependency()) {
            auto const dependency{std::static_pointer_cast<
                storm::storage::DFTDependency<ValueType> const>(element)};

            auto const triggerElement{std::static_pointer_cast<
                storm::storage::DFTElement<ValueType> const>(
                dependency->triggerEvent())};
            decendants.push_back(triggerElement);

            auto const &dependentEvents{dependency->dependentEvents()};
            decendants.insert(decendants.end(), dependentEvents.begin(),
                              dependentEvents.end());
        } else if (element->nrChildren() > 0) {
            auto const parent{std::static_pointer_cast<
                storm::storage::DFTChildren<ValueType> const>(element)};
            auto const &children = parent->children();
            decendants.insert(decendants.end(), children.begin(),
                              children.end());
        }

        if (element->isBasicElement()) {
            auto const be{std::static_pointer_cast<
                storm::storage::DFTBE<ValueType> const>(element)};

            auto const &dependencies{be->ingoingDependencies()};
            decendants.insert(decendants.end(), dependencies.begin(),
                              dependencies.end());
        }

        auto const &restrictions{element->restrictions()};
        decendants.insert(decendants.end(), restrictions.begin(),
                          restrictions.end());

        auto const &dependencies{element->outgoingDependencies()};
        decendants.insert(decendants.end(), dependencies.begin(),
                          dependencies.end());

        return decendants;
    }

    /**
     * \return whether the given element is static i.e. not dynamic
     */
    static bool isElementStatic(DFTElementCPointer const element) {
        if (element->isGate()) {
            auto const gate{std::static_pointer_cast<
                storm::storage::DFTGate<ValueType> const>(element)};
            return !gate->isDynamicGate();
        } else if (element->isRestriction() || element->isDependency()) {
            return false;
        }
        return true;
    }

    struct DfsCounter {
        uint64_t firstVisit{0};
        uint64_t secondVisit{0};
        uint64_t lastVisit{0};

        uint64_t minFirstVisit{0};
        uint64_t maxLastVisit{0};
    };

    struct ElementInfo {
        bool isModule{false};
        bool isStatic{true};
    };

    std::map<ElementId, DfsCounter> dfsCounters{};
    std::map<ElementId, ElementInfo> elementInfos{};

    uint64_t lastDate{};

    /**
     * Populates firstVisit, secondVisit and lastVisit.
     *
     * \note
     * This corresponds to the first depth first search
     * of the LTA/DR algorithm found in doi:10.1109/24.537011.
     */
    void populateDfsCounters() {
        for (auto const &id : dft->getAllIds()) {
            dfsCounters[id] = DfsCounter{};
        }

        // reset date
        lastDate = 0;

        auto const topLevelElement{std::static_pointer_cast<
            storm::storage::DFTElement<ValueType> const>(
            dft->getTopLevelGate())};
        populateDfsCounters(topLevelElement);
    }

    /**
     * Internal Rekursive Implementation of populateDfsCounters.
     *
     * \note
     * Should not be called manually.
     */
    void populateDfsCounters(DFTElementCPointer const element) {
        auto &counter{dfsCounters.at(element->id())};

        ++lastDate;
        if (counter.firstVisit == 0) {
            // parent was never visited before
            // as 0 can never be a valid firstVisit
            counter.firstVisit = lastDate;

            for (auto const &decendant : getDecendants(element)) {
                populateDfsCounters(decendant);
            }
            ++lastDate;
            counter.secondVisit = lastDate;
        }
        counter.lastVisit = lastDate;
    }

    /**
     * Populates elementInfos.
     * Frees dfsCounters.
     *
     * \note
     * This corresponds to the second depth first search
     * of the LTA/DR algorithm found in doi:10.1109/24.537011.
     */
    void populateElementInfos() {
        for (auto const &id : dft->getAllIds()) {
            elementInfos[id] = ElementInfo{};
        }

        auto const topLevelElement{std::static_pointer_cast<
            storm::storage::DFTElement<ValueType> const>(
            dft->getTopLevelGate())};
        populateElementInfos(topLevelElement);

        // free some space
        dfsCounters.clear();
    }

    /**
     * Internal Rekursive Implementation of populateElementInfos.
     *
     * \note
     * Should not be called manually.
     */
    void populateElementInfos(DFTElementCPointer const element) {
        auto &counter{dfsCounters.at(element->id())};
        auto &elementInfo{elementInfos.at(element->id())};

        if (counter.minFirstVisit == 0) {
            // element was never visited before as min can never be 0

            // minFistVisit <= secondVisit
            counter.minFirstVisit = counter.secondVisit;
            for (auto const &decendant : getDecendants(element)) {
                populateElementInfos(decendant);

                auto const &decendantCounter{dfsCounters.at(decendant->id())};
                auto const &decendantElementInfo{
                    elementInfos.at(decendant->id())};

                counter.maxLastVisit =
                    std::max({counter.maxLastVisit, decendantCounter.lastVisit,
                              decendantCounter.maxLastVisit});

                counter.minFirstVisit = std::min(
                    {counter.minFirstVisit, decendantCounter.firstVisit,
                     decendantCounter.minFirstVisit});

                // propagate dynamic property
                if (!decendantElementInfo.isStatic &&
                    !decendantElementInfo.isModule) {
                    elementInfo.isStatic = false;
                }
            }

            if (!isElementStatic(element)) {
                elementInfo.isStatic = false;
            }

            if (!element->isBasicElement() &&
                counter.firstVisit < counter.minFirstVisit &&
                counter.maxLastVisit < counter.secondVisit) {
                elementInfo.isModule = true;
            }
        }
    }

    std::shared_ptr<storm::storage::DFT<ValueType>> workDFT{};

    /**
     * Calculate dynamic Modules and replace them with BE's in workDFT
     */
    void replaceDynamicModules(DFTElementCPointer const element,
                               double const timebound) {
        if (element->isGate()) {
            auto &elementInfo{elementInfos.at(element->id())};
            auto const parent{std::static_pointer_cast<
                storm::storage::DFTChildren<ValueType> const>(element)};

            if (elementInfo.isModule && !elementInfo.isStatic) {
                analyseDynamic(element, timebound);
            }

            for (auto const child : parent->children()) {
                replaceDynamicModules(child, timebound);
            }
        } else if (!element->isBasicElement()) {
            STORM_LOG_ERROR("Wrong Type: " << element->typestring());
            STORM_LOG_THROW(false, storm::exceptions::NotSupportedException,
                            "DFT element not supported in DFTModularizer");
        }
    }

    /**
     * \return DFT with the given element as the root
     */
    std::shared_ptr<storm::storage::DFT<ValueType>> getSubDFT(
        DFTElementCPointer const element) {
        storm::builder::DFTBuilder<ValueType> builder{};
        for (auto const id :
             workDFT->getIndependentSubDftRoots(element->id())) {
            auto const tmpElement{workDFT->getElement(id)};
            builder.copyElement(tmpElement);
        }
        builder.setTopLevel(element->name());
        return std::make_shared<storm::storage::DFT<ValueType>>(
            builder.build());
    }

    /**
     * \return
     * A lambda such that a basic element with it
     * would fauil with the given probability at the given timebound
     */
    constexpr static ValueType inverseExp(const ValueType probability,
                                          const ValueType timebound) {
        return -(std::log(1 - probability) / timebound);
    }

    /**
     * Update the workdDFT.
     * Replaces the given element with a BE
     * that fails with the given probability at the given timebound.
     */
    void updateWorkDFT(DFTElementCPointer const element,
                       ValueType const probability, double const timebound) {
        auto const lambda{inverseExp(probability, timebound)};
        storm::builder::DFTBuilder<ValueType> builder{};
        for (auto const id : workDFT->getAllIds()) {
            auto const tmpElement{workDFT->getElement(id)};
            if (tmpElement->name() != element->name()) {
                builder.copyElement(tmpElement);
            } else {
                builder.addBasicElementExponential(element->name(), lambda, 1);
            }
        }
        builder.setTopLevel(workDFT->getTopLevelGate()->name());

        workDFT =
            std::make_shared<storm::storage::DFT<ValueType>>(builder.build());
    }

    /**
     * Analyse the static Module with the given element as the root.
     *
     * \note
     * Updates the workDFT with the calculated probability
     */
    double analyseStatic(DFTElementCPointer const element,
                         double const timebound) {
        auto subDFT{getSubDFT(element)};
        storm::modelchecker::SFTBDDChecker<ValueType> checker{subDFT};
        auto const probability{checker.getProbabilityAtTimebound(timebound)};

        updateWorkDFT(element, probability, timebound);

        return probability;
    }

    /**
     * Analyse the static Module with the given element as the root.
     *
     * \note
     * Updates the workDFT with the calculated probability
     */
    double analyseDynamic(DFTElementCPointer const element,
                          double const timebound) {
        auto subDFT{getSubDFT(element)};
        subDFT->checkWellFormedness(true, std::cout);

        storm::modelchecker::DFTModelChecker<ValueType> checker{true};
        std::stringstream propertyStream{};
        propertyStream << "Pmin=? [F<=" << timebound << "\"failed\"]";
        auto const props{storm::api::extractFormulasFromProperties(
            storm::api::parseProperties(propertyStream.str()))};
        auto const result{
            checker.check(*subDFT, props, false, false, subDFT->getAllIds())};
        checker.printResults(result);

        auto const probability{boost::get<double>(result[0])};

        updateWorkDFT(element, probability, timebound);
        return probability;
    }

    // don't reinitialise Sylvan BDD
    // temporary
    storm::storage::SylvanBddManager manager{};
};

}  // namespace modelchecker
}  // namespace storm
