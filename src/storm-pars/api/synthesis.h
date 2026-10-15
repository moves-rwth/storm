#pragma once

#include <memory>
#include <optional>

#include "storm-pars/modelchecker/region/RegionCheckEngine.h"
#include "storm-pars/storage/ParameterRegion.h"
#include "storm-pars/utility/FeasibilityResult.h"
#include "storm-pars/utility/FeasibilitySynthesisTask.h"
#include "storm/environment/Environment.h"
#include "storm/models/sparse/Model.h"

namespace storm::pars {

enum class FeasibilityStatus { Feasible, Infeasible, Unknown };

template<typename ConstantType>
struct FeasibilitySynthesisResult {
    FeasibilityStatus status = FeasibilityStatus::Unknown;
    /// Present exactly when status is Feasible. The witness is checked at ConstantType precision.
    std::optional<FeasibilityResult<ConstantType>> witness;
};

}  // namespace storm::pars

namespace storm::api {

/*!
 * Checks whether every well-defined parameter valuation in the region satisfies the bounded probability or reward formula.
 * Models must be sparse pDTMCs or pMDPs with exactly one initial state.
 *
 * Provide either region or regionBound. A regionBound b constructs the closed interval [b, 1-b] for every model
 * parameter, including reward parameters; b must be in [0, 1/2]. No model-specific bounds are inferred.
 * Both inputs together are rejected. Both may be omitted only for models without parameters.
 * The region must cover all model parameters and meet the selected engine's preconditions.
 * Checking arithmetic is selected by the engine.
 * Unsupported input throws.
 */
bool verify(storm::Environment const& env, std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> const& model,
            std::shared_ptr<storm::logic::Formula const> const& formula,
            std::optional<storm::storage::ParameterRegion<storm::RationalFunction>> const& region = std::nullopt,
            std::optional<storm::RationalFunctionCoefficient> const& regionBound = std::nullopt,
            storm::modelchecker::RegionCheckEngine engine = storm::modelchecker::RegionCheckEngine::ParameterLifting);

/*!
 * Searches for a well-defined parameter valuation in the region satisfying the bounded probability or reward formula.
 * Region arguments and validation follow verify(). MDP scheduler quantification follows the formula.
 *
 * Returns Feasible with a witness, Infeasible when the absence of a satisfying valuation is established,
 * or Unknown when no decision is obtained. Unsupported input throws.
 * Witness values are checked and returned as doubles.
 */
storm::pars::FeasibilitySynthesisResult<double> checkFeasibility(
    storm::Environment const& env, std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> const& model,
    std::shared_ptr<storm::logic::Formula const> const& formula,
    std::optional<storm::storage::ParameterRegion<storm::RationalFunction>> const& region = std::nullopt,
    std::optional<storm::RationalFunctionCoefficient> const& regionBound = std::nullopt,
    storm::pars::FeasibilityMethod method = storm::pars::FeasibilityMethod::PLA,
    storm::modelchecker::RegionCheckEngine regionEngine = storm::modelchecker::RegionCheckEngine::ParameterLifting);

}  // namespace storm::api
