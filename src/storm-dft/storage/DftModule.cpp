#include "DftModule.h"

#include "storm-dft/storage/DFT.h"

#include "storm/adapters/RationalFunctionAdapter.h"

namespace storm::dft {
namespace storage {

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
template std::string DftModule::toString(storm::dft::storage::DFT<double> const&) const;
template std::string DftModule::toString(storm::dft::storage::DFT<storm::RationalFunction> const&) const;

}  // namespace storage
}  // namespace storm::dft
