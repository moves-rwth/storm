#include "storm/transformer/AddUncertainty.h"
#include "storm/adapters/RationalNumberAdapter.h"
#include "storm/exceptions/InvalidArgumentException.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/utility/macros.h"

namespace storm::transformer {

template<typename ValueType>
AddUncertainty<ValueType>::AddUncertainty(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& originalModel) : origModel(originalModel) {}

template<typename ValueType>
std::shared_ptr<storm::models::sparse::Model<Interval>> AddUncertainty<ValueType>::transform(double additiveUncertainty, double minimalTransitionProbability) {
    storage::SparseMatrixBuilder<storm::Interval> newMatrixBuilder =
        storage::SparseMatrixBuilder<storm::Interval>(origModel->getTransitionMatrix().getRowCount(), origModel->getTransitionMatrix().getColumnCount());
    for (uint64_t state = 0; state < origModel->getNumberOfStates(); ++state) {
        uint64_t endOfRowGroup = origModel->getTransitionMatrix().getRowGroupIndices()[state + 1];
        for (uint64_t row = origModel->getTransitionMatrix().getRowGroupIndices()[state]; row < endOfRowGroup; ++row) {
            for (auto const& entry : origModel->getTransitionMatrix().getRow(row)) {
                newMatrixBuilder.addNextValue(row, entry.getColumn(), addUncertainty(entry.getValue(), additiveUncertainty, minimalTransitionProbability));
            }
        }
    }

    switch (origModel->getType()) {
        case storm::models::ModelType::Dtmc:
            return std::make_shared<storm::models::sparse::Dtmc<storm::Interval>>(newMatrixBuilder.build(), origModel->getStateLabeling());
            // TODO add reward models
            // TODO add state valuations, etc.
        default:
            STORM_LOG_ASSERT(false, "Not implemented");
    }
}

template<typename ValueType>
storm::Interval AddUncertainty<ValueType>::addUncertainty(ValueType const& vt, double additiveUncertainty, double minimalValue) {
    if (utility::isOne(vt)) {
        return storm::Interval(1.0, 1.0);
    }
    double center = storm::utility::convertNumber<double>(vt);
    STORM_LOG_THROW(center >= minimalValue, storm::exceptions::InvalidArgumentException, "Transition probability is smaller than minimal value");
    double lowerBound = std::max(center - additiveUncertainty, minimalValue);
    double upperBound = std::min(center + additiveUncertainty, 1.0 - minimalValue);
    return storm::Interval(lowerBound, upperBound);
}

template class AddUncertainty<double>;
}  // namespace storm::transformer