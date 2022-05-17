#include "OrderDFTElementsById.h"
#include "storm-dft/storage/DFTElements.h"

namespace storm {
namespace storage {

template<typename ValueType>
bool OrderElementsById<ValueType>::operator()(std::shared_ptr<storm::dft::storage::elements::DFTGate<ValueType>> const& a,
                                              std::shared_ptr<storm::dft::storage::elements::DFTGate<ValueType>> const& b) const {
    return a->id() < b->id();
}

template<typename ValueType>
bool OrderElementsById<ValueType>::operator()(const std::shared_ptr<storm::dft::storage::elements::DFTElement<ValueType>>& a,
                                              const std::shared_ptr<storm::dft::storage::elements::DFTElement<ValueType>>& b) const {
    return a->id() < b->id();
}

template<typename ValueType>
bool OrderElementsByRank<ValueType>::operator()(const std::shared_ptr<storm::dft::storage::elements::DFTGate<ValueType>>& a,
                                                const std::shared_ptr<storm::dft::storage::elements::DFTGate<ValueType>>& b) const {
    return a->rank() > b->rank();
}

// Explicitly instantiate the class.
template struct OrderElementsById<double>;
template struct OrderElementsByRank<double>;

#ifdef STORM_HAVE_CARL
template struct OrderElementsById<RationalFunction>;
template struct OrderElementsByRank<RationalFunction>;
#endif
}  // namespace storage
}  // namespace storm
