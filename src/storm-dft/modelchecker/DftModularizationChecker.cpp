#include "DftModularizationChecker.h"

#include <sstream>

#include "storm-dft/adapters/SFTBDDPropertyFormulaAdapter.h"
#include "storm-dft/api/storm-dft.h"
#include "storm-dft/builder/DFTBuilder.h"
#include "storm-dft/modelchecker/DFTModelChecker.h"
#include "storm-dft/modelchecker/SFTBDDChecker.h"
#include "storm-dft/utility/DftModularizer.h"

#include "storm-parsers/api/properties.h"
#include "storm/api/properties.h"
#include "storm/exceptions/InvalidModelException.h"

namespace storm::dft {
namespace modelchecker {

template<typename ValueType>
DftModularizationChecker<ValueType>::DftModularizationChecker(std::shared_ptr<storm::dft::storage::DFT<ValueType>> dft)
    : dft{dft}, workDFT{dft}, sylvanBddManager{std::make_shared<storm::dft::storage::SylvanBddManager>()} {
    // Initialize modules
    storm::dft::utility::DftModularizer<ValueType> modularizer;
    auto listOfModules = modularizer.computeModules(*dft);
    for (auto const &mod : listOfModules) {
        modules.insert({mod.getRepresentative(), mod});
    }

    STORM_LOG_DEBUG("Modularization found the following modules:");
    for (auto const &mod : modules) {
        STORM_LOG_DEBUG(mod.second.toString(*dft));
    }
}

template<typename ValueType>
std::vector<ValueType> DftModularizationChecker<ValueType>::check(FormulaVector const &formulas, size_t chunksize) {
    workDFT = dft;
    storm::dft::adapters::SFTBDDPropertyFormulaAdapter::checkForm(formulas);
    std::set<ValueType> timepointSet;
    for (auto const &formula : formulas) {
        timepointSet.insert(storm::dft::adapters::SFTBDDPropertyFormulaAdapter::getTimebound(formula));
    }

    std::vector<ValueType> timepoints(timepointSet.begin(), timepointSet.end());

    auto topLevelElement{workDFT->getTopLevelElement()};
    replaceDynamicModules(topLevelElement, timepoints);

    auto const subDFT{getSubDFT(topLevelElement)};
    storm::dft::adapters::SFTBDDPropertyFormulaAdapter checker{subDFT, formulas, {}, sylvanBddManager};
    return checker.check(chunksize);
}

template<typename ValueType>
std::vector<ValueType> DftModularizationChecker<ValueType>::getProbabilitiesAtTimepoints(std::vector<ValueType> const &timepoints, size_t chunksize) {
    workDFT = dft;

    replaceDynamicModules(workDFT->getTopLevelElement(), timepoints);

    // Get toplevel element again, because it might have changed in the workDFT
    auto const subDFT{getSubDFT(workDFT->getTopLevelElement())};
    storm::dft::modelchecker::SFTBDDChecker checker{subDFT, sylvanBddManager};
    return checker.getProbabilitiesAtTimepoints(timepoints, chunksize);
}

template<typename ValueType>
void DftModularizationChecker<ValueType>::replaceDynamicModules(DFTElementCPointer const element, std::vector<ValueType> const &timepoints) {
    if (element->isGate()) {
        auto it = modules.find(element->id());
        if (it != modules.end() && !it->second.isStaticModule()) {
            STORM_LOG_DEBUG("Analyse dynamic module with root " << element->name());
            analyseDynamic(element, timepoints);
        } else {
            auto const parent{std::static_pointer_cast<storm::dft::storage::elements::DFTChildren<ValueType> const>(element)};
            for (auto const &child : parent->children()) {
                replaceDynamicModules(child, timepoints);
            }
        }
    } else if (!element->isBasicElement()) {
        STORM_LOG_THROW(false, storm::exceptions::NotSupportedException,
                        "DFT element " << element->name() << " of type " << element->typestring() << " not supported in DFTModularizationChecker.");
    }
}

template<typename ValueType>
std::shared_ptr<storm::dft::storage::DFT<ValueType>> DftModularizationChecker<ValueType>::getSubDFT(DFTElementCPointer const element) {
    storm::dft::builder::DFTBuilder<ValueType> builder{};
    std::unordered_set<std::string> depInConflict;
    for (auto const id : workDFT->getIndependentSubDftRoots(element->id())) {
        auto const tmpElement{workDFT->getElement(id)};
        builder.cloneElement(tmpElement);
        // Remember dependency conflict
        if (tmpElement->isDependency() && workDFT->isDependencyInConflict(tmpElement->id())) {
            depInConflict.insert(tmpElement->name());
        }
    }
    builder.setTopLevel(element->name());
    auto dft = std::make_shared<storm::dft::storage::DFT<ValueType>>(builder.build());
    // Update dependency conflicts
    for (size_t id : dft->getDependencies()) {
        // Set dependencies not in conflict
        if (depInConflict.find(dft->getElement(id)->name()) == depInConflict.end()) {
            dft->setDependencyNotInConflict(id);
        }
    }
    return dft;
}

template<typename ValueType>
void DftModularizationChecker<ValueType>::updateWorkDFT(DFTElementCPointer const element, std::map<ValueType, ValueType> activeSamples) {
    storm::dft::builder::DFTBuilder<ValueType> builder{};
    std::unordered_set<std::string> depInConflict;
    for (auto const id : workDFT->getAllIds()) {
        auto const tmpElement{workDFT->getElement(id)};
        if (tmpElement->name() != element->name()) {
            builder.cloneElement(tmpElement);
            // Remember dependency conflict
            if (tmpElement->isDependency() && workDFT->isDependencyInConflict(id)) {
                depInConflict.insert(tmpElement->name());
            }
        } else {
            builder.addBasicElementSamples(element->name(), activeSamples);
        }
    }
    builder.setTopLevel(workDFT->getTopLevelElement()->name());

    workDFT = std::make_shared<storm::dft::storage::DFT<ValueType>>(builder.build());
    // Update dependency conflicts
    for (size_t id : workDFT->getDependencies()) {
        // Set dependencies not in conflict
        if (depInConflict.find(workDFT->getElement(id)->name()) == depInConflict.end()) {
            workDFT->setDependencyNotInConflict(id);
        }
    }
}

template<typename ValueType>
void DftModularizationChecker<ValueType>::analyseDynamic(DFTElementCPointer const element, std::vector<ValueType> const &timepoints) {
    // Check that analysis is needed
    if (workDFT->getElement(element->id())->isBasicElement()) {
        return;
    }
    auto subDFT{getSubDFT(element)};
    auto wellFormedResult = storm::dft::api::isWellFormed(*subDFT, true);
    STORM_LOG_THROW(wellFormedResult.first, storm::exceptions::InvalidModelException, wellFormedResult.second);

    storm::dft::modelchecker::DFTModelChecker<ValueType> checker{true};
    std::stringstream propertyStream{};

    for (auto const timebound : timepoints) {
        propertyStream << "Pmin=? [F<=" << timebound << "\"failed\"];";
    }

    auto const props{storm::api::extractFormulasFromProperties(storm::api::parseProperties(propertyStream.str()))};
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

// Explicitly instantiate the class.
template class DftModularizationChecker<double>;

}  // namespace modelchecker
}  // namespace storm::dft
