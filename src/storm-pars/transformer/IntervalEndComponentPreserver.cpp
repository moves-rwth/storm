
#include "storm-pars/transformer/IntervalEndComponentPreserver.h"
#include "adapters/RationalFunctionForward.h"
#include "adapters/RationalNumberForward.h"
#include "storage/BitVector.h"
#include "storm-pars/utility/parametric.h"
#include "utility/constants.h"
#include "utility/logging.h"
namespace storm {
namespace transformer {

template<typename ParametricType>
IntervalEndComponentPreserver<ParametricType>::IntervalEndComponentPreserver(storm::storage::SparseMatrix<ParametricType> const& originalMatrix,
                                                                             std::vector<ParametricType> const& originalVector) {
    // Our new matrix has one more state, which is the new sink state. Otherwise, the rows and columns are the same.
    storm::storage::SparseMatrixBuilder<Interval> builder(originalMatrix.getRowCount() + 1, originalMatrix.getColumnCount() + 1, 0, true, false);

    uint64_t sinkState = originalMatrix.getRowCount();
    considered = storage::BitVector(originalMatrix.getRowCount());

    for (uint64_t row = 0; row < originalMatrix.getRowCount(); row++) {
        bool thisHasSelfLoop = false;
        bool thisIsParametric = false;

        for (auto const& entry : originalMatrix.getRow(row)) {
            auto column = entry.getColumn();
            if (column == row) {
                thisHasSelfLoop = true;
            }
            if (!entry.getValue().isConstant()) {
                thisIsParametric = true;
            }
        }

        bool thisIsConsidered = thisHasSelfLoop && thisIsParametric;
        considered.set(row, thisIsConsidered);

        for (auto const& entry : originalMatrix.getRow(row)) {
            auto column = entry.getColumn();

            builder.addNextValue(row, column, Interval());
        }

        // Add a transition to the sink state, which we may want to fill with [0, 1].
        if (thisIsConsidered) {
            builder.addNextValue(row, sinkState, Interval(0, 0));
        }
    }
    // For the new sink state
    builder.addNextValue(sinkState, sinkState, Interval(1, 1));

    for (uint64_t row = 0; row < originalVector.size() + 1; row++) {
        vector.push_back(Interval());
    }

    matrix = builder.build();

    // Now insert the correct iterators for the matrix and vector assignment
    uint64_t startEntryOfRow = 0;
    for (uint64_t row = 0; row < matrix.getRowCount(); row++) {
        for (auto matrixEntryIt = matrix.getRow(row).begin(); matrixEntryIt != matrix.getRow(row).end(); ++matrixEntryIt) {
            matrixAssignment.push_back(matrixEntryIt);
        }
    }

    for (uint64_t row = 0; row < vector.size(); row++) {
        vectorAssignment.push_back(vector.begin() + row);
    }

    std::cout << considered << std::endl;
}

template<typename ParametricType>
void IntervalEndComponentPreserver<ParametricType>::specifyAssignment(storm::storage::SparseMatrix<Interval> const& originalMatrix,
                                                      std::vector<Interval> const& originalVector) {
    auto matrixPlaceholderIterator = matrixAssignment.begin();

    for (uint64_t row = 0; row < originalMatrix.getRowCount(); row++) {
        // Criteria for transformation
        // (1) upper of self-loop is one
        bool upperOfSelfLoopOne = false;
        // (2) lower of all other outgoing transitions are zero
        bool lowerOfAllTransitionsZero = true;

        if (considered.get(row)) {
            for (auto const& entry : originalMatrix.getRow(row)) {
                if (entry.getColumn() == row) {
                    if (utility::isAlmostOne(entry.getValue().upper())) {
                        upperOfSelfLoopOne = true;
                    }
                } else {
                    if (!utility::isAlmostZero(entry.getValue().lower())) {
                        lowerOfAllTransitionsZero = false;
                        break;
                    }
                }
            }
        }

        if (considered.get(row) && upperOfSelfLoopOne && lowerOfAllTransitionsZero) {
            // Write [0, 1] to all transitions
            std::cout << "Doing the interval end component transformation!" << std::endl;
            for (auto const& entry : originalMatrix.getRow(row)) {
                (*matrixPlaceholderIterator)->setValue(Interval(0, 1));
                matrixPlaceholderIterator++;
            }
            // Write [0, 1] to sink state
            (*matrixPlaceholderIterator)->setValue(Interval(0, 1));
            matrixPlaceholderIterator++;
        } else {
            // Write original interval to all transitions
            for (auto const& entry : originalMatrix.getRow(row)) {
                (*matrixPlaceholderIterator)->setValue(entry.getValue());
                matrixPlaceholderIterator++;
            }
            if (considered.get(row)) {
                // Write zero to sink state
                (*matrixPlaceholderIterator)->setValue(Interval(0, 0));
                matrixPlaceholderIterator++;
            }
        }
    }

    for (uint64_t i = 0; i < originalVector.size(); i++) {
        *vectorAssignment[i] = originalVector.at(i);
    }
}

template<typename ParametricType>
storm::storage::SparseMatrix<Interval> const& IntervalEndComponentPreserver<ParametricType>::getMatrix() const {
    return matrix;
}

template<typename ParametricType>
std::vector<Interval> const& IntervalEndComponentPreserver<ParametricType>::getVector() const {
    return vector;
}

template class IntervalEndComponentPreserver<storm::RationalFunction>;

}
}