#pragma once

#include <iostream>
#include <memory>

#include "storm/models/sparse/Model.h"

namespace storm {
namespace exporter {

struct DirectEncodingOptions {
    bool allowPlaceholders = true;
};
/*!
 * Exports a sparse model into the explicit DRN format.
 *
 * @param os           Stream to export to
 * @param sparseModel  Model to export
 * @param parameters   List of parameters
 */
template<typename ValueType>
void explicitExportSparseModel(std::ostream& os, std::shared_ptr<storm::models::sparse::Model<ValueType>> sparseModel,
                               std::vector<std::string> const& parameters, DirectEncodingOptions const& options = DirectEncodingOptions());

/*!
 * Accumulate parameters in the model.
 *
 * @param sparseModel Model.
 * @return List of parameters in the model.
 */
template<typename ValueType>
std::vector<std::string> getParameters(std::shared_ptr<storm::models::sparse::Model<ValueType>> sparseModel);

/*!
 * Generate placeholders for rational functions in the model.
 *
 * @param sparseModel Model.
 * @param exitRates Exit rates.
 * @return Mapping of the form:rational function -> placeholder name.
 */
template<typename ValueType>
std::unordered_map<ValueType, std::string> generatePlaceholders(std::shared_ptr<storm::models::sparse::Model<ValueType>> sparseModel,
                                                                std::vector<ValueType> exitRates);

/*!
 * Write value to stream while using the placeholders.
 * @param os Output stream.
 * @param value Value.
 * @param placeholders Placeholders.
 */
template<typename ValueType>
void writeValue(std::ostream& os, ValueType value, std::unordered_map<ValueType, std::string> const& placeholders);
}  // namespace exporter
}  // namespace storm
