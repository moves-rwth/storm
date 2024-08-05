
#include "storm-pars/transformer/IntervalEndComponentPreserver.h"
#include "adapters/RationalFunctionForward.h"
#include "adapters/RationalNumberForward.h"
#include "storage/BitVector.h"
#include "storage/RobustMaximalEndComponentDecomposition.h"
#include "storage/StronglyConnectedComponentDecomposition.h"
#include "storm-pars/utility/parametric.h"
#include "utility/constants.h"
#include "utility/logging.h"
#include "utility/macros.h"
namespace storm {
namespace transformer {

template<typename ParametricType>
IntervalEndComponentPreserver<ParametricType>::IntervalEndComponentPreserver(storm::storage::SparseMatrix<ParametricType> const& originalMatrix,
                                                                             std::vector<ParametricType> const& originalVector) {
    
    storage::StronglyConnectedComponentDecomposition<ParametricType> decomposition(originalMatrix);
    auto const& indexMap = decomposition.computeStateToSccIndexMap(originalMatrix.getRowCount());

    for (auto const& group : decomposition) {
        if (!group.isTrivial()) {
            std::cout << "Group: ";
            for (auto const& state : group) {
                std::cout << state << " ";
            }
            std::cout << std::endl;
        }
    }

    // Our new matrix has one more state, which is the new sink state. Otherwise, the rows and columns are the same.
    storm::storage::SparseMatrixBuilder<Interval> builder(originalMatrix.getRowCount() + 1, originalMatrix.getColumnCount() + 1, 0, true, false);

    uint64_t sinkState = originalMatrix.getRowCount();
    considered = storage::BitVector(originalMatrix.getRowCount());

    for (uint64_t row = 0; row < originalMatrix.getRowCount(); row++) {
        // non-trivial sccs may be in the end components
        bool thisIsConsidered = !decomposition.getBlock(indexMap.at(row)).isTrivial();
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
}

template<typename ParametricType>
void IntervalEndComponentPreserver<ParametricType>::specifyAssignment(storm::storage::SparseMatrix<Interval> const& originalMatrix,
                                                      std::vector<Interval> const& originalVector) {
    storage::RobustMaximalEndComponentDecomposition<Interval> decomposition(originalMatrix, originalMatrix.transpose());

    auto const& indexMap = decomposition.computeStateToSccIndexMap(originalMatrix.getRowCount());
    auto matrixPlaceholderIterator = matrixAssignment.begin();

    for (auto const& group : decomposition) {
        if (!group.isTrivial()) {
            std::cout << "Non-trivial MEC: ";
            for (auto const& state : group) {
                std::cout << state << " ";
            }
            std::cout << std::endl;
        }
    }

    for (uint64_t row = 0; row < originalMatrix.getRowCount(); row++) {
        const uint64_t blockIndex = indexMap.at(row);
        if (blockIndex < decomposition.size() && !decomposition.getBlock(blockIndex).isTrivial()) {
            STORM_LOG_ASSERT(considered.get(row), "Non-considered row " << row << " in non-trivial end component");
            // Write [0, 1] to all transitions
            for (auto const& entry : originalMatrix.getRow(row)) {
                if (entry.getColumn() == row) {
                    // Self-loop
                    (*matrixPlaceholderIterator)->setValue(Interval(0, 0));
                } else {
                    (*matrixPlaceholderIterator)->setValue(Interval(0, 1));
                }
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