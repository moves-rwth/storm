#pragma once

#include "storm/storage/dd/DdManager.h"

namespace storm {
namespace modelchecker {

/**
 * Main class for the SFTBDDChecker
 *
 */
template <storm::dd::DdType Type>
class SFTBDDChecker {
   public:
    SFTBDDChecker();

   private:
    std::shared_ptr<storm::dd::DdManager<Type>> ddManager;
};

template class SFTBDDChecker<storm::dd::DdType::CUDD>;
template class SFTBDDChecker<storm::dd::DdType::Sylvan>;

}  // namespace modelchecker
}  // namespace storm
