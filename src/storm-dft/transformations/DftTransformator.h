#pragma once

#include "storm-dft/builder/DFTBuilder.h"
#include "storm-dft/storage/DFT.h"
#include "storm/utility/macros.h"

namespace storm::dft {
namespace transformations {

/*!
 * Transformator for DFT -> DFT.
 */
template<typename ValueType>
class DftTransformator {
   public:
    /*!
     * Constructor.
     *
     * @param dft DFT
     */
    DftTransformator();

    std::shared_ptr<storm::storage::DFT<ValueType>> transformUniqueFailedBe(storm::storage::DFT<ValueType> const &dft);

    std::shared_ptr<storm::storage::DFT<ValueType>> transformBinaryFDEPs(storm::storage::DFT<ValueType> const &dft);

   private:
    std::vector<std::string> getChildrenVector(std::shared_ptr<storm::dft::storage::elements::DFTElement<ValueType> const> element);
};

}  // namespace transformations
}  // namespace storm::dft
