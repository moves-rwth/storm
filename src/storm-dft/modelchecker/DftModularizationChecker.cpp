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
    : dft{dft}, sylvanBddManager{std::make_shared<storm::dft::storage::SylvanBddManager>()}, modelchecker(true) {
    // Initialize modules
    storm::dft::utility::DftModularizer<ValueType> modularizer;
    auto topModule = modularizer.computeModules(*dft);
    STORM_LOG_DEBUG("Modularization found the following modules:\n" << topModule.toString(*dft));

    // Gather all dynamic modules
    populateDynamicModules(topModule);
}

template<typename ValueType>
void DftModularizationChecker<ValueType>::populateDynamicModules(storm::dft::storage::DftIndependentModule const& module) {
    if (!module.isStatic()) {
        // Found new dynamic module
        dynamicModules.push_back(module);
    } else if (!module.isFullyStatic()) {
        // Module contains dynamic sub-modules -> recursively visit children
        for (auto const& submodule : module.getSubModules()) {
            populateDynamicModules(submodule);
        }
    }
}

template<typename ValueType>
std::vector<ValueType> DftModularizationChecker<ValueType>::check(FormulaVector const& formulas, size_t chunksize) {
    // Gather time points
    storm::dft::adapters::SFTBDDPropertyFormulaAdapter::checkForm(formulas);
    std::set<ValueType> timepointSet;
    for (auto const& formula : formulas) {
        timepointSet.insert(storm::dft::adapters::SFTBDDPropertyFormulaAdapter::getTimebound(formula));
    }
    std::vector<ValueType> timepoints(timepointSet.begin(), timepointSet.end());

    auto newDft = replaceDynamicModules(timepoints);

    storm::dft::adapters::SFTBDDPropertyFormulaAdapter checker{newDft, formulas, {}, sylvanBddManager};
    return checker.check(chunksize);
}

template<typename ValueType>
std::vector<ValueType> DftModularizationChecker<ValueType>::getProbabilitiesAtTimepoints(std::vector<ValueType> const& timepoints, size_t chunksize) {
    auto newDft = replaceDynamicModules(timepoints);
    storm::dft::modelchecker::SFTBDDChecker checker{newDft, sylvanBddManager};
    return checker.getProbabilitiesAtTimepoints(timepoints, chunksize);
}

template<typename ValueType>
std::shared_ptr<storm::dft::storage::DFT<ValueType>> DftModularizationChecker<ValueType>::replaceDynamicModules(std::vector<ValueType> const& timepoints) {
    // Map from module representatives to their sample points
    std::map<size_t, std::map<ValueType, ValueType>> samplePoints;

    // First analyse all dynamic modules
    for (auto const& mod : dynamicModules) {
        STORM_LOG_DEBUG("Analyse dynamic module " << mod.toString(*dft));
        auto result = analyseDynamicModule(mod, timepoints);
        // Remember probabilities for module
        std::map<ValueType, ValueType> activeSamples{};
        for (size_t i{0}; i < timepoints.size(); ++i) {
            auto const probability{boost::get<ValueType>(result[i])};
            auto const timebound{timepoints[i]};
            activeSamples[timebound] = probability;
        }
        samplePoints.insert({mod.getRepresentative(), activeSamples});
    }

    // Second replace all dynamic modules by BEs which have samples corresponding to the previously computed analysis results
    storm::dft::builder::DFTBuilder<ValueType> builder{};
    std::unordered_set<std::string> depInConflict;
    for (auto const id : dft->getAllIds()) {
        auto const element{dft->getElement(id)};
        auto it = samplePoints.find(id);
        if (it != samplePoints.end()) {
            // Replace element by BE
            builder.addBasicElementSamples(element->name(), it->second);
        } else {
            // Keep element
            builder.cloneElement(element);
            // Remember dependency conflict
            if (element->isDependency() && dft->isDependencyInConflict(id)) {
                depInConflict.insert(element->name());
            }
        }
    }
    builder.setTopLevel(dft->getTopLevelElement()->name());

    auto newDft = std::make_shared<storm::dft::storage::DFT<ValueType>>(builder.build());
    // Update dependency conflicts
    for (size_t id : newDft->getDependencies()) {
        // Set dependencies not in conflict
        if (depInConflict.find(newDft->getElement(id)->name()) == depInConflict.end()) {
            newDft->setDependencyNotInConflict(id);
        }
    }

    return getSubDFT(newDft, newDft->getTopLevelElement());
}

template<typename ValueType>
std::shared_ptr<storm::dft::storage::DFT<ValueType>> DftModularizationChecker<ValueType>::getSubDFT(
    std::shared_ptr<storm::dft::storage::DFT<ValueType>> const dft, DFTElementCPointer const element) {
    storm::dft::builder::DFTBuilder<ValueType> builder{};
    std::unordered_set<std::string> depInConflict;
    for (auto const id : dft->getIndependentSubDftRoots(element->id())) {
        auto const tmpElement{dft->getElement(id)};
        builder.cloneElement(tmpElement);
        // Remember dependency conflict
        if (tmpElement->isDependency() && dft->isDependencyInConflict(tmpElement->id())) {
            depInConflict.insert(tmpElement->name());
        }
    }
    builder.setTopLevel(element->name());
    auto subdft = std::make_shared<storm::dft::storage::DFT<ValueType>>(builder.build());
    // Update dependency conflicts
    for (size_t id : subdft->getDependencies()) {
        // Set dependencies not in conflict
        if (depInConflict.find(subdft->getElement(id)->name()) == depInConflict.end()) {
            subdft->setDependencyNotInConflict(id);
        }
    }
    return subdft;
}

template<typename ValueType>
typename storm::dft::modelchecker::DFTModelChecker<ValueType>::dft_results DftModularizationChecker<ValueType>::analyseDynamicModule(
    storm::dft::storage::DftIndependentModule const& module, std::vector<ValueType> const& timepoints) {
    auto element = dft->getElement(module.getRepresentative());
    STORM_LOG_ASSERT(!element->isBasicElement(), "Dynamic module should not be a single BE.");
    STORM_LOG_ASSERT(!module.isStatic() && !module.isFullyStatic(), "Module should be dynamic.");

    auto subDFT{getSubDFT(dft, element)};
    auto wellFormedResult = storm::dft::api::isWellFormed(*subDFT, true);
    STORM_LOG_THROW(wellFormedResult.first, storm::exceptions::InvalidModelException, wellFormedResult.second);

    // Create properties
    std::stringstream propertyStream{};
    for (auto const timebound : timepoints) {
        propertyStream << "Pmin=? [F<=" << timebound << "\"failed\"];";
    }
    auto const props{storm::api::extractFormulasFromProperties(storm::api::parseProperties(propertyStream.str()))};

    return std::move(modelchecker.check(*subDFT, props, false, false, {}));
}

// Explicitly instantiate the class.
template class DftModularizationChecker<double>;

}  // namespace modelchecker
}  // namespace storm::dft
