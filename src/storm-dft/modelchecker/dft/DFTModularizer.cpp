#include <sstream>

#include "storm-dft/builder/DFTBuilder.h"
#include "storm-dft/modelchecker/dft/DFTModelChecker.h"
#include "storm-dft/modelchecker/dft/DFTModularizer.h"
#include "storm-dft/modelchecker/dft/SFTBDDChecker.h"
#include "storm-dft/modelchecker/dft/SFTBDDPropertyFormulaAdapter.h"
#include "storm-parsers/api/properties.h"
#include "storm/api/properties.h"

namespace storm {
namespace modelchecker {

using ValueType = DFTModularizer::ValueType;
using ElementId = DFTModularizer::ElementId;
using DFTElementCPointer = DFTModularizer::DFTElementCPointer;
using FormulaCPointer = DFTModularizer::FormulaCPointer;
using FormulaVector = DFTModularizer::FormulaVector;

DFTModularizer::DFTModularizer(
    std::shared_ptr<storm::storage::DFT<ValueType>> dft)
    : dft{dft},
      workDFT{dft},
      sylvanBddManager{std::make_shared<storm::storage::SylvanBddManager>()} {
    populateDfsCounters();
    populateElementInfos();
}

std::vector<ValueType> DFTModularizer::check(FormulaVector const &formulas,
                                             size_t const chunksize) {
    workDFT = dft;
    storm::adapters::SFTBDDPropertyFormulaAdapter::checkForm(formulas);
    std::set<ValueType> timepointSet;
    for (auto const &formula : formulas) {
        timepointSet.insert(
            storm::adapters::SFTBDDPropertyFormulaAdapter::getTimebound(
                formula));
    }

    std::vector<ValueType> timepoints(timepointSet.begin(), timepointSet.end());

    auto topLevelElement{workDFT->getTopLevelElement()};
    replaceDynamicModules(topLevelElement, timepoints);

    auto const subDFT{getSubDFT(topLevelElement)};
    storm::adapters::SFTBDDPropertyFormulaAdapter checker{
        subDFT, formulas, {}, sylvanBddManager};
    return checker.check(chunksize);
}

std::vector<ValueType> DFTModularizer::getProbabilitiesAtTimepoints(
    std::vector<ValueType> const &timepoints, size_t const chunksize) {
    workDFT = dft;

    auto topLevelElement{workDFT->getTopLevelElement()};
    replaceDynamicModules(topLevelElement, timepoints);

    auto const subDFT{getSubDFT(topLevelElement)};
    storm::modelchecker::SFTBDDChecker checker{subDFT, sylvanBddManager};
    return checker.getProbabilitiesAtTimepoints(timepoints, chunksize);
}

std::vector<DFTElementCPointer> DFTModularizer::getDecendants(
    DFTElementCPointer const element) {
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
        decendants.insert(decendants.end(), children.begin(), children.end());
    }

    if (element->isBasicElement()) {
        auto const be{
            std::static_pointer_cast<storm::storage::DFTBE<ValueType> const>(
                element)};

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

bool DFTModularizer::isElementStatic(DFTElementCPointer const element) {
    return storm::storage::isStaticGateType(element->type()) ||
           element->isBasicElement();
}

void DFTModularizer::populateDfsCounters() {
    for (auto const &id : dft->getAllIds()) {
        dfsCounters[id] = DfsCounter{};
    }

    // reset date
    lastDate = 0;

    auto const topLevelElement{workDFT->getTopLevelElement()};
    populateDfsCounters(topLevelElement);
}

void DFTModularizer::populateDfsCounters(DFTElementCPointer const element) {
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

void DFTModularizer::populateElementInfos() {
    for (auto const &id : dft->getAllIds()) {
        elementInfos[id] = ElementInfo{};
    }

    auto const topLevelElement{workDFT->getTopLevelElement()};
    populateElementInfos(topLevelElement);

    // free some space
    dfsCounters.clear();
}

void DFTModularizer::populateElementInfos(DFTElementCPointer const element) {
    auto &counter{dfsCounters.at(element->id())};
    auto &elementInfo{elementInfos.at(element->id())};

    if (counter.minFirstVisit == 0) {
        // element was never visited before as min can never be 0

        // minFirstVisit <= secondVisit
        counter.minFirstVisit = counter.secondVisit;
        auto const decendants{getDecendants(element)};
        for (auto const &decendant : decendants) {
            populateElementInfos(decendant);

            auto const &decendantCounter{dfsCounters.at(decendant->id())};
            auto const &decendantElementInfo{elementInfos.at(decendant->id())};

            counter.maxLastVisit =
                std::max({counter.maxLastVisit, decendantCounter.lastVisit,
                          decendantCounter.maxLastVisit});

            counter.minFirstVisit =
                std::min({counter.minFirstVisit, decendantCounter.firstVisit,
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

void DFTModularizer::replaceDynamicModules(
    DFTElementCPointer const element,
    std::vector<ValueType> const &timepoints) {
    if (element->isGate()) {
        auto &elementInfo{elementInfos.at(element->id())};
        if (elementInfo.isModule && !elementInfo.isStatic) {
            analyseDynamic(element, timepoints);
        } else {
            auto const parent{std::static_pointer_cast<
                storm::storage::DFTChildren<ValueType> const>(element)};
            for (auto const &child : parent->children()) {
                replaceDynamicModules(child, timepoints);
            }
        }
    } else if (!element->isBasicElement()) {
        STORM_LOG_ERROR("Wrong Type: " << element->typestring());
        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException,
                        "DFT element not supported in DFTModularizer");
    }
}

std::shared_ptr<storm::storage::DFT<ValueType>> DFTModularizer::getSubDFT(
    DFTElementCPointer const element) {
    storm::builder::DFTBuilder<ValueType> builder{};
    for (auto const id : workDFT->getIndependentSubDftRoots(element->id())) {
        auto const tmpElement{workDFT->getElement(id)};
        builder.copyElement(tmpElement);
    }
    builder.setTopLevel(element->name());
    return std::make_shared<storm::storage::DFT<ValueType>>(builder.build());
}

void DFTModularizer::updateWorkDFT(
    DFTElementCPointer const element,
    std::map<ValueType, ValueType> activeSamples) {
    storm::builder::DFTBuilder<ValueType> builder{};
    for (auto const id : workDFT->getAllIds()) {
        auto const tmpElement{workDFT->getElement(id)};
        if (tmpElement->name() != element->name()) {
            builder.copyElement(tmpElement);
        } else {
            builder.addBasicElementSamples(element->name(), activeSamples);
        }
    }
    builder.setTopLevel(workDFT->getTopLevelElement()->name());

    workDFT = std::make_shared<storm::storage::DFT<ValueType>>(builder.build());
}

void DFTModularizer::analyseDynamic(DFTElementCPointer const element,
                                    std::vector<ValueType> const &timepoints) {
    // Check that analysis is needed
    if (workDFT->getElement(element->id())->isBasicElement()) {
        return;
    }
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

    std::map<ValueType, ValueType> activeSamples{};
    for (size_t i{0}; i < timepoints.size(); ++i) {
        auto const probability{boost::get<ValueType>(result[i])};
        auto const timebound{timepoints[i]};
        activeSamples[timebound] = probability;
    }

    updateWorkDFT(element, activeSamples);
}

}  // namespace modelchecker
}  // namespace storm
