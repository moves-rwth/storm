#include "DftModule.h"

#include "storm-dft/storage/DFT.h"

#include "storm/adapters/RationalFunctionAdapter.h"

namespace storm::dft {
namespace storage {

DftModule::DftModule(size_t representative, std::set<size_t> const& elements) : representative(representative), elements(elements) {
    // Assertion cannot be guaranteed as spare module currently only contain the corresponding BEs and SPAREs
    // STORM_LOG_ASSERT(std::find(this->elements.begin(), this->elements.end(), representative) != this->elements.end(),
    //                 "Representative " + std::to_string(representative) + " must be contained in module.");
}

template<typename ValueType>
std::string DftModule::toString(storm::dft::storage::DFT<ValueType> const& dft) const {
    std::stringstream stream;
    stream << "[" << dft.getElement(representative)->name() << "] = {";
    if (!elements.empty()) {
        auto it = elements.begin();
        STORM_LOG_ASSERT(it != elements.end(), "Element not found.");
        stream << dft.getElement(*it)->name();
        ++it;
        while (it != elements.end()) {
            stream << ", " << dft.getElement(*it)->name();
            ++it;
        }
    }
    stream << "}";
    return stream.str();
}
DftIndependentModule::DftIndependentModule(size_t representative, std::set<size_t> const& elements, std::set<DftIndependentModule> const& submodules,
                                           bool staticElements, bool fullyStatic)
    : DftModule(representative, elements), staticElements(staticElements), fullyStatic(fullyStatic), submodules(submodules) {
    STORM_LOG_ASSERT(std::find(this->elements.begin(), this->elements.end(), representative) != this->elements.end(),
                     "Representative " + std::to_string(representative) + " must be contained in module.");
}

template<typename ValueType>
std::string DftIndependentModule::toString(storm::dft::storage::DFT<ValueType> const& dft, std::string const& indentation) const {
    std::stringstream stream;
    stream << indentation << DftModule::toString(dft);
    std::string subIndentation = indentation + "  ";
    for (DftIndependentModule const& submodule : submodules) {
        stream << "\n" << subIndentation << "Sub-module " << submodule.toString(dft, subIndentation);
    }
    return stream.str();
}

// Explicitly instantiate functions
template std::string DftModule::toString(storm::dft::storage::DFT<double> const&) const;
template std::string DftModule::toString(storm::dft::storage::DFT<storm::RationalFunction> const&) const;

template std::string DftIndependentModule::toString(storm::dft::storage::DFT<double> const&, std::string const&) const;
template std::string DftIndependentModule::toString(storm::dft::storage::DFT<storm::RationalFunction> const&, std::string const&) const;

}  // namespace storage
}  // namespace storm::dft
