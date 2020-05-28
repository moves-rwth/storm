#pragma once

#include <memory>
#include <sstream>
#include <vector>

#include "DFTModelChecker.h"
#include "storm-dft/builder/DFTBuilder.h"
#include "storm-dft/modelchecker/dft/DFTModelChecker.h"
#include "storm-dft/modelchecker/dft/SFTBDDChecker.h"
#include "storm-dft/modelchecker/dft/SFTBDDPropertyFormulaAdapter.h"
#include "storm-dft/storage/dft/DFT.h"
#include "storm-parsers/api/properties.h"
#include "storm/api/properties.h"
#include "storm/logic/Formula.h"

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
    using FormulaCPointer = std::shared_ptr<storm::logic::Formula const>;
    using FormulaVector = std::vector<FormulaCPointer>;

    /**
     * Calculates Modules
     */
    DFTModularizer(std::shared_ptr<storm::storage::DFT<ValueType>> dft)
        : dft{std::move(dft)},
          sylvanBddManager{
              std::make_shared<storm::storage::SylvanBddManager>()} {
        populateDfsCounters();
        populateElementInfos();
    }

    /**
     * Calculate the properties specified by the formulas
     * \param formuals
     * The Properties to check for.
     *
     * \note Does not work with events in dynamic modules.
     */
    std::vector<double> check(FormulaVector const &formulas,
                              size_t const chunksize = 0) {
        storm::adapters::SFTBDDPropertyFormulaAdapter::checkForm(formulas);
        std::set<ValueType> timepointSet;
        for (auto const &formula : formulas) {
            timepointSet.insert(
                storm::adapters::SFTBDDPropertyFormulaAdapter::getTimebound(
                    formula));
        }

        std::vector<ValueType> timepoints(timepointSet.begin(),
                                          timepointSet.end());

        auto topLevelElement{std::static_pointer_cast<
            storm::storage::DFTElement<ValueType> const>(
            workDFT->getTopLevelGate())};
        replaceDynamicModules(topLevelElement, timepoints);

        auto const subDFT{getSubDFT(topLevelElement)};
        storm::adapters::SFTBDDPropertyFormulaAdapter checker{sylvanBddManager,
                                                              subDFT};
        return checker.check(formulas, chunksize);
    }

    /**
     * \return
     * The Probabilities that the top level gate fails at the given timepoints.
     */
    std::vector<double> getProbabilitiesAtTimepoints(
        std::vector<double> const &timepoints, size_t const chunksize = 0) {
        workDFT = dft;

        auto topLevelElement{std::static_pointer_cast<
            storm::storage::DFTElement<ValueType> const>(
            workDFT->getTopLevelGate())};
        replaceDynamicModules(topLevelElement, timepoints);

        auto const subDFT{getSubDFT(topLevelElement)};
        storm::modelchecker::SFTBDDChecker checker{sylvanBddManager,
                                                              subDFT};
        return checker.getProbabilitiesAtTimepoints(timepoints, chunksize);
    }

    /**
     * \return
     * The Probability that the top level gate fails at the given timebound.
     */
    double getProbabilityAtTimebound(double const timebound) {
        auto const result{getProbabilitiesAtTimepoints({timebound})};
        return result.at(0);
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
            auto const decendants{getDecendants(element)};
            for (auto const &decendant : decendants) {
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
                               std::vector<double> const &timepoints) {
        if (element->isGate()) {
            auto &elementInfo{elementInfos.at(element->id())};
            auto const parent{std::static_pointer_cast<
                storm::storage::DFTChildren<ValueType> const>(element)};

            if (elementInfo.isModule && !elementInfo.isStatic) {
                analyseDynamic(element, timepoints);
            } else {
                for (auto const child : parent->children()) {
                    replaceDynamicModules(child, timepoints);
                }
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
     * Update the workdDFT.
     * Replace the given element with a sample BE
     */
    void updateWorkDFT(DFTElementCPointer const element,
                       std::map<double, double> activeSamples) {
        storm::builder::DFTBuilder<ValueType> builder{};
        for (auto const id : workDFT->getAllIds()) {
            auto const tmpElement{workDFT->getElement(id)};
            if (tmpElement->name() != element->name()) {
                builder.copyElement(tmpElement);
            } else {
                builder.addBasicElementSamples(element->name(), activeSamples);
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
    void analyseDynamic(DFTElementCPointer const element,
                        std::vector<double> const &timepoints) {
        auto subDFT{getSubDFT(element)};
        subDFT->checkWellFormedness(true, std::cout);

        storm::modelchecker::DFTModelChecker<ValueType> checker{true};
        std::stringstream propertyStream{};

        for (auto const timebound : timepoints) {
            propertyStream << "Pmin=? [F<=" << timebound << "\"failed\"];";
        }

        auto const props{storm::api::extractFormulasFromProperties(
            storm::api::parseProperties(propertyStream.str()))};
        auto const result{checker.check(*subDFT, props, false, false, {})};

        checker.printResults(result);

        std::map<double, double> activeSamples{};
        for (size_t i{0}; i < timepoints.size(); ++i) {
            auto const probability{boost::get<double>(result[i])};
            auto const timebound{timepoints[i]};
            activeSamples[timebound] = probability;
        }

        updateWorkDFT(element, activeSamples);
    }

    // don't reinitialise Sylvan BDD
    // temporary
    std::shared_ptr<storm::storage::SylvanBddManager> sylvanBddManager;
};

}  // namespace modelchecker
}  // namespace storm
