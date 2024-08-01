#include "ImportanceFunction.h"

namespace storm::dft {
namespace simulator {

template<typename ValueType>
ImportanceFunction<ValueType>::ImportanceFunction(storm::dft::storage::DFT<ValueType> const& dft) : dft(dft) {}

template<typename ValueType>
BECountImportanceFunction<ValueType>::BECountImportanceFunction(storm::dft::storage::DFT<ValueType> const& dft) : ImportanceFunction<ValueType>(dft) {}

template<typename ValueType>
double BECountImportanceFunction<ValueType>::getImportance(typename ImportanceFunction<ValueType>::DFTStatePointer state) const {
    size_t count = 0;
    for (auto be : this->dft.getBasicElements()) {
        if (state->hasFailed(be->id())) {
            ++count;
        }
    }
    return count;
}

template<typename ValueType>
std::pair<double, double> BECountImportanceFunction<ValueType>::getImportanceRange() const {
    return std::make_pair(0, this->dft.nrBasicElements());
}

template class BECountImportanceFunction<double>;
template class BECountImportanceFunction<storm::RationalFunction>;

}  // namespace simulator
}  // namespace storm::dft
