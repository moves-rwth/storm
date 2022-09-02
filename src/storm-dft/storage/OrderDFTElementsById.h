#pragma once

#include <memory>

namespace storm::dft {
namespace storage {

// Forward declarations
namespace elements {

template<typename ValueType>
class DFTGate;
template<typename ValueType>
class DFTElement;

}  // namespace elements

template<typename ValueType>
struct OrderElementsById {
    bool operator()(std::shared_ptr<storm::dft::storage::elements::DFTGate<ValueType>> const& a,
                    std::shared_ptr<storm::dft::storage::elements::DFTGate<ValueType>> const& b) const;

    bool operator()(std::shared_ptr<storm::dft::storage::elements::DFTElement<ValueType>> const& a,
                    std::shared_ptr<storm::dft::storage::elements::DFTElement<ValueType>> const& b) const;
};

template<typename ValueType>
struct OrderElementsByRank {
    bool operator()(std::shared_ptr<storm::dft::storage::elements::DFTGate<ValueType>> const& a,
                    std::shared_ptr<storm::dft::storage::elements::DFTGate<ValueType>> const& b) const;
};

}  // namespace storage
}  // namespace storm::dft
