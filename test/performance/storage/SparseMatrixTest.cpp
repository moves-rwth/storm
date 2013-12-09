#include "gtest/gtest.h"
#include "src/storage/SparseMatrix.h"

TEST(SparseMatrix, Iteration) {
    storm::storage::SparseMatrix<double> matrix;
    for (uint_fast64_t row = 0; row < 10000; ++row) {
        for (uint_fast64_t column = 0; column < row; ++column) {
            ASSERT_NO_THROW(matrix.addNextValue(row, column, row+column));
        }
    }
    ASSERT_NO_THROW(matrix.finalize());
    
    for (uint_fast64_t row = 0; row < matrix.getRowCount(); ++row) {
        for (auto const& entry : matrix.getRow(row)) {
            // The following can never be true, but prevents the compiler from optimizing away the loop.
            if (entry.column() > matrix.getColumnCount()) {
                ASSERT_TRUE(false);
            }
        }
    }
}

TEST(SparseMatrix, Multiplication) {
    storm::storage::SparseMatrix<double> matrix;
    for (uint_fast64_t row = 0; row < 2000; ++row) {
        for (uint_fast64_t column = 0; column < row; ++column) {
            ASSERT_NO_THROW(matrix.addNextValue(row, column, row+column));
        }
    }
    ASSERT_NO_THROW(matrix.finalize());
    
    std::vector<double> x(matrix.getColumnCount(), 1.0);
    std::vector<double> result(matrix.getRowCount());
    
    for (uint_fast64_t i = 0; i < 5000; ++i) {
        matrix.multiplyWithVector(x, result);
        
        // The following can never be true, but prevents the compiler from optimizing away the loop.
        if (result.size() > matrix.getRowCount()) {
            ASSERT_TRUE(false);
        }
    }
}