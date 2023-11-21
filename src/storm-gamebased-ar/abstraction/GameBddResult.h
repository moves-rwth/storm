#pragma once

#include "storm/storage/dd/Bdd.h"

namespace storm {
namespace abstraction {

template<storm::dd::DdType DdType>
struct GameBddResult {
    GameBddResult();
    GameBddResult(storm::dd::Bdd<DdType> const& gameBdd, uint_fast64_t numberOfPlayer2Variables);

    storm::dd::Bdd<DdType> bdd;
    uint_fast64_t numberOfPlayer2Variables;
};

}  // namespace abstraction
}  // namespace storm
