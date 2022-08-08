#pragma once
#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/StandardRewardModel.h"

namespace storm {
namespace transformer {
std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> makeRewardsConstant(storm::models::sparse::Dtmc<storm::RationalFunction> const& pMC);
}
}  // namespace storm