#pragma once

#include "storm/utility/macros.h"

#include "storm-dft/storage/DFT.h"
#include "storm/adapters/JsonAdapter.h"

namespace storm::dft {
namespace storage {

/**
 * Exports a DFT into the JSON format for visualizing it.
 */
template<typename ValueType>
class DftJsonExporter {
    typedef typename storm::json<double> Json;

    using DFTElementPointer = std::shared_ptr<storm::dft::storage::elements::DFTElement<ValueType>>;
    using DFTElementCPointer = std::shared_ptr<storm::dft::storage::elements::DFTElement<ValueType> const>;
    using DFTGatePointer = std::shared_ptr<storm::dft::storage::elements::DFTGate<ValueType>>;

   public:
    static void toFile(storm::dft::storage::DFT<ValueType> const& dft, std::string const& filepath);

    static void toStream(storm::dft::storage::DFT<ValueType> const& dft, std::ostream& os);

   private:
    static Json translate(storm::dft::storage::DFT<ValueType> const& dft);

    static Json translateNode(DFTElementCPointer const& element);
};

}  // namespace storage
}  // namespace storm::dft
