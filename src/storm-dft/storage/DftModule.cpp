#include "DftModule.h"

#include "storm-dft/storage/DFT.h"

#include "storm/adapters/RationalFunctionAdapter.h"

namespace storm::dft {
namespace storage {

DftModule::DftModule(size_t representative, std::vector<size_t> const& elements)
    : representative(representative), elements(elements), staticModule(std::nullopt) {
    // Assertion cannot be guaranteed as spare module currently only contain the corresponding BEs and SPAREs
    // STORM_LOG_ASSERT(std::find(this->elements.begin(), this->elements.end(), representative) != this->elements.end(),
    //                 "Representative " + std::to_string(representative) + " must be contained in module.");
}

template<typename ValueType>
void DftModule::setType(storm::dft::storage::DFT<ValueType> const& dft) {
    staticModule = true;
    for (auto id : elements) {
        if (!dft.getElement(id)->isStaticElement()) {
            staticModule = false;
            return;
        }
    }
}

bool DftModule::isStaticModule() const {
    STORM_LOG_ASSERT(staticModule.has_value(), "Type of module was not initialized.");
    return staticModule.value();
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

// Explicitly instantiate functions
template void DftModule::setType(storm::dft::storage::DFT<double> const&);
template std::string DftModule::toString(storm::dft::storage::DFT<double> const&) const;

template void DftModule::setType(storm::dft::storage::DFT<storm::RationalFunction> const&);
template std::string DftModule::toString(storm::dft::storage::DFT<storm::RationalFunction> const&) const;

}  // namespace storage
}  // namespace storm::dft
