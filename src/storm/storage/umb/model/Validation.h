#pragma once

#include <ostream>

#include "storm/storage/umb/model/UmbModelForward.h"

namespace storm::umb {
/*!
 * Validates the given UMB model and writes potential errors to the given output stream.
 * @return true if the UMB model is valid.
 * @note This only validates the umbModel against the UMB specification. It does not check whether Storm supports all features used in the model.
 * @note The validation checks are incomplete. In particular, this does not apply any checks that would require iterating over dynamic-sized structures.
 */
bool validate(storm::umb::UmbModel const& umbModel, std::ostream& errors);

/*!
 * Validates the given UMB model. If it is invalid, an exception is thrown.
 * @note: This only validates the umbModel against the UMB specification. It does not check whether Storm supports all features used in the model.
 * @note The validation checks are incomplete. In particular, this does not apply any checks that would require iterating over dynamic-sized structures.
 */
void validateOrThrow(storm::umb::UmbModel const& umbModel);

}  // namespace storm::umb
