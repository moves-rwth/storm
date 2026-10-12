#include "storm-pars/scp/AffinePMdpDecomposition.h"

#include <algorithm>
#include <limits>

#include "storm-pars/utility/parametric.h"
#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/exceptions/NotSupportedException.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/utility/constants.h"
#include "storm/utility/macros.h"

namespace storm::pars::scp {

UnpackedValue AffinePMdpDecomposition::decomposeAffine(storm::RationalFunction const& value, std::string const& errorContext) {
    STORM_LOG_THROW(storm::utility::parametric::isLinear(value), storm::exceptions::NotSupportedException,
                    "SCP requires an affine pMDP (every transition probability, and every state-action reward for reward properties, must have total "
                    "degree <= 1 in the parameters), but "
                        << errorContext << " is '" << value << "', which is not affine.");

    std::set<storm::RationalFunctionVariable> occurring;
    storm::utility::parametric::gatherOccurringVariables(value, occurring);

    UnpackedValue unpacked;
    storm::utility::parametric::Valuation<storm::RationalFunction> zeroValuation;
    for (auto const& var : occurring) {
        zeroValuation[var] = storm::utility::zero<storm::RationalFunctionCoefficient>();
    }
    unpacked.constant = storm::utility::parametric::evaluate<double>(value, zeroValuation);

    unpacked.parameterCoefficients.reserve(occurring.size());
    for (auto const& var : occurring) {
        storm::RationalFunction derivative = value.derivative(var);
        STORM_LOG_ASSERT(derivative.isConstant(), "Derivative of an affine function w.r.t. one of its own parameters must be constant.");
        unpacked.parameterCoefficients.emplace_back(var, storm::utility::convertNumber<double>(derivative.constantPart()));
    }

    parameters.insert(occurring.begin(), occurring.end());
    return unpacked;
}

AffinePMdpDecomposition::AffinePMdpDecomposition(storm::models::sparse::Model<storm::RationalFunction> const& model,
                                                 storm::models::sparse::StandardRewardModel<storm::RationalFunction> const* rewardModel) {
    auto const& matrix = model.getTransitionMatrix();
    auto const& rowGroupIndices = matrix.getRowGroupIndices();

    rows.reserve(matrix.getRowCount());
    for (uint64_t row = 0; row < matrix.getRowCount(); ++row) {
        std::vector<UnpackedTransition> unpackedRow;
        unpackedRow.reserve(matrix.getRow(row).getNumberOfEntries());

        // Only used to produce a precise error message; cheap relative to the rest of this loop.
        uint64_t state = std::upper_bound(rowGroupIndices.begin(), rowGroupIndices.end(), row) - rowGroupIndices.begin() - 1;

        for (auto const& entry : matrix.getRow(row)) {
            UnpackedValue decomposed = decomposeAffine(entry.getValue(), "the transition from state " + std::to_string(state) + " (row " + std::to_string(row) +
                                                                             ") to state " + std::to_string(entry.getColumn()));
            UnpackedTransition unpacked;
            unpacked.successor = entry.getColumn();
            unpacked.constant = decomposed.constant;
            unpacked.parameterCoefficients = std::move(decomposed.parameterCoefficients);
            unpackedRow.push_back(std::move(unpacked));
        }
        rows.push_back(std::move(unpackedRow));
    }

    if (rewardModel != nullptr) {
        std::vector<storm::RationalFunction> rewardVector = rewardModel->getTotalRewardVector(matrix);
        rewardRows.reserve(rewardVector.size());
        for (uint64_t row = 0; row < rewardVector.size(); ++row) {
            uint64_t state = std::upper_bound(rowGroupIndices.begin(), rowGroupIndices.end(), row) - rowGroupIndices.begin() - 1;
            rewardRows.push_back(decomposeAffine(rewardVector[row], "the reward of state " + std::to_string(state) + " (row " + std::to_string(row) + ")"));
        }
    }
}

std::vector<UnpackedTransition> const& AffinePMdpDecomposition::getRow(uint64_t row) const {
    return rows.at(row);
}

uint64_t AffinePMdpDecomposition::getNumberOfRows() const {
    return rows.size();
}

UnpackedValue const& AffinePMdpDecomposition::getReward(uint64_t row) const {
    return rewardRows.at(row);
}

std::set<storm::RationalFunctionVariable> const& AffinePMdpDecomposition::getParameters() const {
    return parameters;
}

std::optional<bool> AffinePMdpDecomposition::isGraphPreserving(storm::storage::ParameterRegion<storm::RationalFunction> const& region,
                                                               uint64_t maxVertices) const {
    uint64_t const n = parameters.size();
    // The shift below is UB for n >= 64; guard well before that, and before it could exceed maxVertices anyway.
    if (n >= 63 || (uint64_t{1} << n) > maxVertices) {
        return std::nullopt;
    }

    auto const vertices = region.getVerticesOfRegion(parameters);
    constexpr double epsilon = 1e-9;

    for (auto const& row : rows) {
        for (auto const& transition : row) {
            double minValue = std::numeric_limits<double>::infinity();
            double maxValue = -std::numeric_limits<double>::infinity();
            for (auto const& vertex : vertices) {
                double value = transition.constant;
                for (auto const& [var, coeff] : transition.parameterCoefficients) {
                    value += coeff * storm::utility::convertNumber<double>(vertex.at(var));
                }
                minValue = std::min(minValue, value);
                maxValue = std::max(maxValue, value);
            }
            bool const isConstantThroughoutRegion = (maxValue - minValue) <= epsilon;
            bool const touchesBoundary = minValue <= epsilon || maxValue >= 1.0 - epsilon;
            if (!isConstantThroughoutRegion && touchesBoundary) {
                return false;
            }
        }
    }
    return true;
}

}  // namespace storm::pars::scp
