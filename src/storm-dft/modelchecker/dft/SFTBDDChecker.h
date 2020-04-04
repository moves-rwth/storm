#pragma once

#include <memory>
#include <vector>

#include "storm-dft/storage/dft/DFT.h"
#include "storm-dft/transformations/SftToBddTransormator.h"
#include "storm/storage/dd/DdManager.h"

namespace storm {
namespace modelchecker {

/**
 * Main class for the SFTBDDChecker
 *
 */
template <typename ValueType, storm::dd::DdType Type>
class SFTBDDChecker {
   public:
    using Bdd = storm::dd::Bdd<Type>;

    SFTBDDChecker(std::shared_ptr<storm::storage::DFT<ValueType>> dft)
        : dft{std::move(dft)},
          ddManager{std::make_shared<storm::dd::DdManager<Type>>()} {
        // Intentionally left empty.
    }

    Bdd translate(
        std::shared_ptr<storm::storage::DFT<ValueType>> dft) {
        storm::transformations::dft::SftToBddTransformator<Type, ValueType>
            transformer{ddManager};

        return transformer.transform(dft);
    }

   private:
    std::shared_ptr<storm::storage::DFT<ValueType>> dft;
    std::shared_ptr<storm::dd::DdManager<Type>> ddManager;
};

}  // namespace modelchecker
}  // namespace storm
