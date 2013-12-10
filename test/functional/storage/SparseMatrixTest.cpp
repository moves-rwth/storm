#include "gtest/gtest.h"
#include "src/storage/SparseMatrix.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/exceptions/OutOfRangeException.h"

TEST(SparseMatrix, SimpleCreation) {
    ASSERT_NO_THROW(storm::storage::SparseMatrix<double> matrix);
    ASSERT_NO_THROW(storm::storage::SparseMatrix<double> matrix(3));
    ASSERT_NO_THROW(storm::storage::SparseMatrix<double> matrix(3, 4));
    ASSERT_NO_THROW(storm::storage::SparseMatrix<double> matrix(3, 4, 5));
}

TEST(SparseMatrix, CreationWithMovingContents) {
    ASSERT_NO_THROW(storm::storage::SparseMatrix<double> matrix(4, {0, 2, 5, 5}, {1, 2, 0, 1, 3}, {1.0, 1.2, 0.5, 0.7, 0.2}));
    storm::storage::SparseMatrix<double> matrix(4, {0, 2, 5, 5}, {1, 2, 0, 1, 3}, {1.0, 1.2, 0.5, 0.7, 0.2});
    ASSERT_EQ(3, matrix.getRowCount());
    ASSERT_EQ(4, matrix.getColumnCount());
    ASSERT_EQ(5, matrix.getEntryCount());
}

TEST(SparseMatrix, CreationWithDimensions) {
    storm::storage::SparseMatrix<double> matrix(3, 4, 5);
    ASSERT_NO_THROW(matrix.addNextValue(0, 1, 1.0));
    ASSERT_NO_THROW(matrix.addNextValue(0, 2, 1.2));
    ASSERT_NO_THROW(matrix.addNextValue(1, 0, 0.5));
    ASSERT_NO_THROW(matrix.addNextValue(1, 1, 0.7));
    ASSERT_NO_THROW(matrix.addNextValue(1, 3, 0.2));
    ASSERT_NO_THROW(matrix.finalize());
    
    ASSERT_EQ(3, matrix.getRowCount());
    ASSERT_EQ(4, matrix.getColumnCount());
    ASSERT_EQ(5, matrix.getEntryCount());
}

TEST(SparseMatrix, CreationWithoutNumberOfEntries) {
    storm::storage::SparseMatrix<double> matrix(3, 4);
    ASSERT_NO_THROW(matrix.addNextValue(0, 1, 1.0));
    ASSERT_NO_THROW(matrix.addNextValue(0, 2, 1.2));
    ASSERT_NO_THROW(matrix.addNextValue(1, 0, 0.5));
    ASSERT_NO_THROW(matrix.addNextValue(1, 1, 0.7));
    ASSERT_NO_THROW(matrix.addNextValue(1, 3, 0.2));
    ASSERT_NO_THROW(matrix.finalize());
    
    ASSERT_EQ(3, matrix.getRowCount());
    ASSERT_EQ(4, matrix.getColumnCount());
    ASSERT_EQ(5, matrix.getEntryCount());
}

TEST(SparseMatrix, CreationWithNumberOfRows) {
    storm::storage::SparseMatrix<double> matrix(3);
    ASSERT_NO_THROW(matrix.addNextValue(0, 1, 1.0));
    ASSERT_NO_THROW(matrix.addNextValue(0, 2, 1.2));
    ASSERT_NO_THROW(matrix.addNextValue(1, 0, 0.5));
    ASSERT_NO_THROW(matrix.addNextValue(1, 1, 0.7));
    ASSERT_NO_THROW(matrix.addNextValue(1, 3, 0.2));
    ASSERT_NO_THROW(matrix.finalize());
    
    ASSERT_EQ(3, matrix.getRowCount());
    ASSERT_EQ(4, matrix.getColumnCount());
    ASSERT_EQ(5, matrix.getEntryCount());
}

TEST(SparseMatrix, CreationWithoutDimensions) {
    storm::storage::SparseMatrix<double> matrix;
    ASSERT_NO_THROW(matrix.addNextValue(0, 1, 1.0));
    ASSERT_NO_THROW(matrix.addNextValue(0, 2, 1.2));
    ASSERT_NO_THROW(matrix.addNextValue(1, 0, 0.5));
    ASSERT_NO_THROW(matrix.addNextValue(1, 1, 0.7));
    ASSERT_NO_THROW(matrix.addNextValue(1, 3, 0.2));
    ASSERT_NO_THROW(matrix.finalize());
    
    ASSERT_EQ(2, matrix.getRowCount());
    ASSERT_EQ(4, matrix.getColumnCount());
    ASSERT_EQ(5, matrix.getEntryCount());
}

TEST(SparseMatrix, CopyConstruct) {
    storm::storage::SparseMatrix<double> matrix;
    ASSERT_NO_THROW(matrix.addNextValue(0, 1, 1.0));
    ASSERT_NO_THROW(matrix.addNextValue(0, 2, 1.2));
    ASSERT_NO_THROW(matrix.addNextValue(1, 0, 0.5));
    ASSERT_NO_THROW(matrix.addNextValue(1, 1, 0.7));
    ASSERT_NO_THROW(matrix.addNextValue(1, 3, 0.2));
    ASSERT_NO_THROW(matrix.finalize());
    
    ASSERT_NO_THROW(storm::storage::SparseMatrix<double> copy(matrix));
    storm::storage::SparseMatrix<double> copy(matrix);
    ASSERT_TRUE(matrix == copy);
}

TEST(SparseMatrix, CopyAssign) {
    storm::storage::SparseMatrix<double> matrix;
    ASSERT_NO_THROW(matrix.addNextValue(0, 1, 1.0));
    ASSERT_NO_THROW(matrix.addNextValue(0, 2, 1.2));
    ASSERT_NO_THROW(matrix.addNextValue(1, 0, 0.5));
    ASSERT_NO_THROW(matrix.addNextValue(1, 1, 0.7));
    ASSERT_NO_THROW(matrix.addNextValue(1, 3, 0.2));
    ASSERT_NO_THROW(matrix.finalize());
    
    ASSERT_NO_THROW(storm::storage::SparseMatrix<double> copy = matrix);
    storm::storage::SparseMatrix<double> copy = matrix;
    ASSERT_TRUE(matrix == copy);
}

TEST(SparseMatrix, AddNextValue) {
    storm::storage::SparseMatrix<double> matrix(3, 4, 5);
    ASSERT_NO_THROW(matrix.addNextValue(0, 1, 1.0));
    ASSERT_NO_THROW(matrix.addNextValue(0, 2, 1.2));
    ASSERT_THROW(matrix.addNextValue(0, 4, 0.5), storm::exceptions::OutOfRangeException);
    ASSERT_THROW(matrix.addNextValue(3, 1, 0.5), storm::exceptions::OutOfRangeException);
    
    storm::storage::SparseMatrix<double> matrix2(3, 4);
    ASSERT_NO_THROW(matrix2.addNextValue(0, 1, 1.0));
    ASSERT_NO_THROW(matrix2.addNextValue(0, 2, 1.2));
    ASSERT_THROW(matrix2.addNextValue(0, 4, 0.5), storm::exceptions::OutOfRangeException);
    ASSERT_THROW(matrix.addNextValue(3, 1, 0.5), storm::exceptions::OutOfRangeException);

    storm::storage::SparseMatrix<double> matrix3(3);
    ASSERT_NO_THROW(matrix3.addNextValue(0, 1, 1.0));
    ASSERT_NO_THROW(matrix3.addNextValue(1, 2, 1.2));
    ASSERT_NO_THROW(matrix3.addNextValue(2, 4, 0.5));
    ASSERT_THROW(matrix3.addNextValue(3, 1, 0.2), storm::exceptions::OutOfRangeException);
    
    storm::storage::SparseMatrix<double> matrix4;
    ASSERT_NO_THROW(matrix4.addNextValue(0, 1, 1.0));
    ASSERT_NO_THROW(matrix4.addNextValue(1, 2, 1.2));
    ASSERT_NO_THROW(matrix4.addNextValue(2, 4, 0.5));
    ASSERT_NO_THROW(matrix4.addNextValue(3, 1, 0.2));
}

TEST(SparseMatrix, Finalize) {
    storm::storage::SparseMatrix<double> matrix(3, 4, 5);
    ASSERT_NO_THROW(matrix.addNextValue(0, 1, 1.0));
    ASSERT_NO_THROW(matrix.addNextValue(0, 2, 1.2));
    ASSERT_NO_THROW(matrix.addNextValue(1, 0, 0.5));
    ASSERT_NO_THROW(matrix.addNextValue(1, 1, 0.7));
    ASSERT_NO_THROW(matrix.addNextValue(1, 3, 0.2));
    ASSERT_FALSE(matrix.isInitialized());
    ASSERT_NO_THROW(matrix.finalize());
    ASSERT_TRUE(matrix.isInitialized());
    
    storm::storage::SparseMatrix<double> matrix2(3, 4, 5);
    ASSERT_NO_THROW(matrix2.addNextValue(0, 1, 1.0));
    ASSERT_NO_THROW(matrix2.addNextValue(0, 2, 1.2));
    ASSERT_NO_THROW(matrix2.addNextValue(1, 0, 0.5));
    ASSERT_NO_THROW(matrix2.addNextValue(1, 1, 0.7));
    ASSERT_THROW(matrix2.finalize(), storm::exceptions::InvalidStateException);
    
    storm::storage::SparseMatrix<double> matrix3;
    ASSERT_NO_THROW(matrix3.addNextValue(0, 1, 1.0));
    ASSERT_NO_THROW(matrix3.addNextValue(0, 2, 1.2));
    ASSERT_NO_THROW(matrix3.addNextValue(1, 0, 0.5));
    ASSERT_NO_THROW(matrix3.addNextValue(1, 1, 0.7));
    ASSERT_NO_THROW(matrix3.addNextValue(1, 3, 0.2));
    ASSERT_NO_THROW(matrix3.finalize());
    ASSERT_EQ(2, matrix3.getRowCount());
    ASSERT_EQ(4, matrix3.getColumnCount());
    ASSERT_EQ(5, matrix3.getEntryCount());
    
    storm::storage::SparseMatrix<double> matrix4;
    ASSERT_NO_THROW(matrix4.addNextValue(0, 1, 1.0));
    ASSERT_NO_THROW(matrix4.addNextValue(0, 2, 1.2));
    ASSERT_NO_THROW(matrix4.addNextValue(1, 0, 0.5));
    ASSERT_NO_THROW(matrix4.addNextValue(1, 1, 0.7));
    ASSERT_NO_THROW(matrix4.addNextValue(1, 3, 0.2));
    ASSERT_NO_THROW(matrix4.finalize(4));
    ASSERT_EQ(4, matrix4.getRowCount());
    ASSERT_EQ(4, matrix4.getColumnCount());
    ASSERT_EQ(5, matrix4.getEntryCount());
    
    storm::storage::SparseMatrix<double> matrix5;
    ASSERT_NO_THROW(matrix5.addNextValue(0, 1, 1.0));
    ASSERT_NO_THROW(matrix5.addNextValue(0, 2, 1.2));
    ASSERT_NO_THROW(matrix5.addNextValue(1, 0, 0.5));
    ASSERT_NO_THROW(matrix5.addNextValue(1, 1, 0.7));
    ASSERT_NO_THROW(matrix5.addNextValue(1, 3, 0.2));
    ASSERT_NO_THROW(matrix5.finalize(0, 6));
    ASSERT_EQ(2, matrix5.getRowCount());
    ASSERT_EQ(6, matrix5.getColumnCount());
    ASSERT_EQ(5, matrix5.getEntryCount());
}

TEST(SparseMatrix, MakeAbsorbing) {
    storm::storage::SparseMatrix<double> matrix(3, 4, 5);
    ASSERT_NO_THROW(matrix.addNextValue(0, 1, 1.0));
    ASSERT_NO_THROW(matrix.addNextValue(0, 2, 1.2));
    ASSERT_NO_THROW(matrix.addNextValue(1, 0, 0.5));
    ASSERT_NO_THROW(matrix.addNextValue(1, 1, 0.7));
    ASSERT_NO_THROW(matrix.addNextValue(1, 3, 0.2));
    ASSERT_NO_THROW(matrix.finalize());
    
    storm::storage::BitVector absorbingRows(3);
    absorbingRows.set(1);
    
    ASSERT_NO_THROW(matrix.makeRowsAbsorbing(absorbingRows));
    
    storm::storage::SparseMatrix<double> matrix2(3, 4, 3);
    ASSERT_NO_THROW(matrix2.addNextValue(0, 1, 1.0));
    ASSERT_NO_THROW(matrix2.addNextValue(0, 2, 1.2));
    ASSERT_NO_THROW(matrix2.addNextValue(1, 1, 1));
    ASSERT_NO_THROW(matrix2.finalize());
    
    ASSERT_TRUE(matrix == matrix2);
}

TEST(SparseMatrix, MakeRowGroupAbsorbing) {
    storm::storage::SparseMatrix<double> matrix(5, 4, 9);
    ASSERT_NO_THROW(matrix.addNextValue(0, 1, 1.0));
    ASSERT_NO_THROW(matrix.addNextValue(0, 2, 1.2));
    ASSERT_NO_THROW(matrix.addNextValue(1, 0, 0.5));
    ASSERT_NO_THROW(matrix.addNextValue(1, 1, 0.7));
    ASSERT_NO_THROW(matrix.addNextValue(2, 0, 0.5));
    ASSERT_NO_THROW(matrix.addNextValue(3, 2, 1.1));
    ASSERT_NO_THROW(matrix.addNextValue(4, 0, 0.1));
    ASSERT_NO_THROW(matrix.addNextValue(4, 1, 0.2));
    ASSERT_NO_THROW(matrix.addNextValue(4, 3, 0.3));
    ASSERT_NO_THROW(matrix.finalize());
    
    std::vector<uint_fast64_t> rowGroupIndices = {0, 2, 4, 5};
    
    storm::storage::BitVector absorbingRowGroups(3);
    absorbingRowGroups.set(1);
    
    ASSERT_NO_THROW(matrix.makeRowsAbsorbing(absorbingRowGroups, rowGroupIndices));

    storm::storage::SparseMatrix<double> matrix2;
    ASSERT_NO_THROW(matrix2.addNextValue(0, 1, 1.0));
    ASSERT_NO_THROW(matrix2.addNextValue(0, 2, 1.2));
    ASSERT_NO_THROW(matrix2.addNextValue(1, 0, 0.5));
    ASSERT_NO_THROW(matrix2.addNextValue(1, 1, 0.7));
    ASSERT_NO_THROW(matrix2.addNextValue(2, 1, 1));
    ASSERT_NO_THROW(matrix2.addNextValue(3, 1, 1));
    ASSERT_NO_THROW(matrix2.addNextValue(4, 0, 0.1));
    ASSERT_NO_THROW(matrix2.addNextValue(4, 1, 0.2));
    ASSERT_NO_THROW(matrix2.addNextValue(4, 3, 0.3));
    ASSERT_NO_THROW(matrix2.finalize());
    
    ASSERT_TRUE(matrix == matrix2);
}

TEST(SparseMatrix, ConstrainedRowSumVector) {
    storm::storage::SparseMatrix<double> matrix(5, 4, 9);
    ASSERT_NO_THROW(matrix.addNextValue(0, 1, 1.0));
    ASSERT_NO_THROW(matrix.addNextValue(0, 2, 1.2));
    ASSERT_NO_THROW(matrix.addNextValue(1, 0, 0.5));
    ASSERT_NO_THROW(matrix.addNextValue(1, 1, 0.7));
    ASSERT_NO_THROW(matrix.addNextValue(2, 0, 0.5));
    ASSERT_NO_THROW(matrix.addNextValue(3, 2, 1.1));
    ASSERT_NO_THROW(matrix.addNextValue(4, 0, 0.1));
    ASSERT_NO_THROW(matrix.addNextValue(4, 1, 0.2));
    ASSERT_NO_THROW(matrix.addNextValue(4, 3, 0.3));
    ASSERT_NO_THROW(matrix.finalize());
    
    storm::storage::BitVector columnConstraint(4);
    columnConstraint.set(1);
    columnConstraint.set(3);
    
    ASSERT_NO_THROW(std::vector<double> constrainedRowSum = matrix.getConstrainedRowSumVector(storm::storage::BitVector(5, true), columnConstraint));
    std::vector<double> constrainedRowSum = matrix.getConstrainedRowSumVector(storm::storage::BitVector(5, true), columnConstraint);
    ASSERT_TRUE(constrainedRowSum == std::vector<double>({1.0, 0.7, 0, 0, 0.5}));
    
    storm::storage::SparseMatrix<double> matrix2(5, 4, 9);
    ASSERT_NO_THROW(matrix2.addNextValue(0, 1, 1.0));
    ASSERT_NO_THROW(matrix2.addNextValue(0, 2, 1.2));
    ASSERT_NO_THROW(matrix2.addNextValue(1, 0, 0.5));
    ASSERT_NO_THROW(matrix2.addNextValue(1, 1, 0.7));
    ASSERT_NO_THROW(matrix2.addNextValue(2, 0, 0.5));
    ASSERT_NO_THROW(matrix2.addNextValue(3, 2, 1.1));
    ASSERT_NO_THROW(matrix2.addNextValue(3, 3, 1.2));
    ASSERT_NO_THROW(matrix2.addNextValue(4, 0, 0.1));
    ASSERT_NO_THROW(matrix2.addNextValue(4, 3, 0.3));
    ASSERT_NO_THROW(matrix2.finalize());
    
    std::vector<uint_fast64_t> rowGroupIndices = {0, 2, 4, 5};
    
    storm::storage::BitVector rowGroupConstraint(3);
    rowGroupConstraint.set(1);
    
    storm::storage::BitVector columnConstraint2(4);
    columnConstraint2.set(2);
    columnConstraint2.set(3);
    
    ASSERT_NO_THROW(std::vector<double> constrainedRowSum2 = matrix2.getConstrainedRowSumVector(rowGroupConstraint, rowGroupIndices, columnConstraint2));
    std::vector<double> constrainedRowSum2 = matrix2.getConstrainedRowSumVector(rowGroupConstraint, rowGroupIndices, columnConstraint2);
    ASSERT_TRUE(constrainedRowSum2 == std::vector<double>({0, 2.3}));
}

TEST(SparseMatrix, Submatrix) {
    storm::storage::SparseMatrix<double> matrix(5, 4, 9);
    ASSERT_NO_THROW(matrix.addNextValue(0, 1, 1.0));
    ASSERT_NO_THROW(matrix.addNextValue(0, 2, 1.2));
    ASSERT_NO_THROW(matrix.addNextValue(1, 0, 0.5));
    ASSERT_NO_THROW(matrix.addNextValue(1, 1, 0.7));
    ASSERT_NO_THROW(matrix.addNextValue(2, 0, 0.5));
    ASSERT_NO_THROW(matrix.addNextValue(3, 2, 1.1));
    ASSERT_NO_THROW(matrix.addNextValue(4, 0, 0.1));
    ASSERT_NO_THROW(matrix.addNextValue(4, 1, 0.2));
    ASSERT_NO_THROW(matrix.addNextValue(4, 3, 0.3));
    ASSERT_NO_THROW(matrix.finalize());
    
    std::vector<uint_fast64_t> rowGroupIndices = {0, 1, 2, 4, 5};
    
    storm::storage::BitVector rowGroupConstraint(4);
    rowGroupConstraint.set(2);
    rowGroupConstraint.set(3);
    
    storm::storage::BitVector columnConstraint(4);
    columnConstraint.set(0);
    columnConstraint.set(3);
    
    ASSERT_NO_THROW(storm::storage::SparseMatrix<double> matrix2 = matrix.getSubmatrix(rowGroupConstraint, columnConstraint, rowGroupIndices, false));
    storm::storage::SparseMatrix<double> matrix2 = matrix.getSubmatrix(rowGroupConstraint, columnConstraint, rowGroupIndices, false);
    
    storm::storage::SparseMatrix<double> matrix3(3, 2, 3);
    ASSERT_NO_THROW(matrix3.addNextValue(0, 0, 0.5));
    ASSERT_NO_THROW(matrix3.addNextValue(2, 0, 0.1));
    ASSERT_NO_THROW(matrix3.addNextValue(2, 1, 0.3));
    ASSERT_NO_THROW(matrix3.finalize());
    
    ASSERT_TRUE(matrix2 == matrix3);
    
    std::vector<uint_fast64_t> rowGroupToIndexMapping = {0, 0, 1, 0};

    ASSERT_NO_THROW(storm::storage::SparseMatrix<double> matrix4 = matrix.getSubmatrix(rowGroupToIndexMapping, rowGroupIndices));
    storm::storage::SparseMatrix<double> matrix4 = matrix.getSubmatrix(rowGroupToIndexMapping, rowGroupIndices);
    
    storm::storage::SparseMatrix<double> matrix5(4, 4, 8);
    ASSERT_NO_THROW(matrix5.addNextValue(0, 1, 1.0));
    ASSERT_NO_THROW(matrix5.addNextValue(0, 2, 1.2));
    ASSERT_NO_THROW(matrix5.addNextValue(1, 0, 0.5));
    ASSERT_NO_THROW(matrix5.addNextValue(1, 1, 0.7));
    ASSERT_NO_THROW(matrix5.addNextValue(2, 2, 1.1));
    ASSERT_NO_THROW(matrix5.addNextValue(3, 0, 0.1));
    ASSERT_NO_THROW(matrix5.addNextValue(3, 1, 0.2));
    ASSERT_NO_THROW(matrix5.addNextValue(3, 3, 0.3));
    ASSERT_NO_THROW(matrix5.finalize());
    
    ASSERT_TRUE(matrix4 == matrix5);
}

TEST(SparseMatrix, Transpose) {
    storm::storage::SparseMatrix<double> matrix(5, 4, 9);
    ASSERT_NO_THROW(matrix.addNextValue(0, 1, 1.0));
    ASSERT_NO_THROW(matrix.addNextValue(0, 2, 1.2));
    ASSERT_NO_THROW(matrix.addNextValue(1, 0, 0.5));
    ASSERT_NO_THROW(matrix.addNextValue(1, 1, 0.7));
    ASSERT_NO_THROW(matrix.addNextValue(2, 0, 0.5));
    ASSERT_NO_THROW(matrix.addNextValue(3, 2, 1.1));
    ASSERT_NO_THROW(matrix.addNextValue(4, 0, 0.1));
    ASSERT_NO_THROW(matrix.addNextValue(4, 1, 0.2));
    ASSERT_NO_THROW(matrix.addNextValue(4, 3, 0.3));
    ASSERT_NO_THROW(matrix.finalize());
    
    ASSERT_NO_THROW(storm::storage::SparseMatrix<double> transposeResult = matrix.transpose());
    storm::storage::SparseMatrix<double> transposeResult = matrix.transpose();
    
    storm::storage::SparseMatrix<double> matrix2(4, 5, 9);
    ASSERT_NO_THROW(matrix2.addNextValue(0, 1, 0.5));
    ASSERT_NO_THROW(matrix2.addNextValue(0, 2, 0.5));
    ASSERT_NO_THROW(matrix2.addNextValue(0, 4, 0.1));
    ASSERT_NO_THROW(matrix2.addNextValue(1, 0, 1.0));
    ASSERT_NO_THROW(matrix2.addNextValue(1, 1, 0.7));
    ASSERT_NO_THROW(matrix2.addNextValue(1, 4, 0.2));
    ASSERT_NO_THROW(matrix2.addNextValue(2, 0, 1.2));
    ASSERT_NO_THROW(matrix2.addNextValue(2, 3, 1.1));
    ASSERT_NO_THROW(matrix2.addNextValue(3, 4, 0.3));
    ASSERT_NO_THROW(matrix2.finalize());

    ASSERT_TRUE(transposeResult == matrix2);
}

TEST(SparseMatrix, EquationSystem) {
    storm::storage::SparseMatrix<double> matrix(4, 4, 7);
    ASSERT_NO_THROW(matrix.addNextValue(0, 0, 1.1));
    ASSERT_NO_THROW(matrix.addNextValue(0, 1, 1.2));
    ASSERT_NO_THROW(matrix.addNextValue(1, 1, 0.5));
    ASSERT_NO_THROW(matrix.addNextValue(1, 3, 0.7));
    ASSERT_NO_THROW(matrix.addNextValue(2, 0, 0.5));
    ASSERT_NO_THROW(matrix.addNextValue(2, 2, 0.99));
    ASSERT_NO_THROW(matrix.addNextValue(3, 3, 0.11));
    ASSERT_NO_THROW(matrix.finalize());
    
    ASSERT_NO_THROW(matrix.convertToEquationSystem());
    
    storm::storage::SparseMatrix<double> matrix2(4, 4, 7);
    ASSERT_NO_THROW(matrix2.addNextValue(0, 0, 1 - 1.1));
    ASSERT_NO_THROW(matrix2.addNextValue(0, 1, -1.2));
    ASSERT_NO_THROW(matrix2.addNextValue(1, 1, 1 - 0.5));
    ASSERT_NO_THROW(matrix2.addNextValue(1, 3, -0.7));
    ASSERT_NO_THROW(matrix2.addNextValue(2, 0, -0.5));
    ASSERT_NO_THROW(matrix2.addNextValue(2, 2, 1 - 0.99));
    ASSERT_NO_THROW(matrix2.addNextValue(3, 3, 1 - 0.11));
    ASSERT_NO_THROW(matrix2.finalize());
    
    ASSERT_TRUE(matrix == matrix2);
}

TEST(SparseMatrix, JacobiDecomposition) {
    storm::storage::SparseMatrix<double> matrix(4, 4, 7);
    ASSERT_NO_THROW(matrix.addNextValue(0, 0, 1.1));
    ASSERT_NO_THROW(matrix.addNextValue(0, 1, 1.2));
    ASSERT_NO_THROW(matrix.addNextValue(1, 1, 0.5));
    ASSERT_NO_THROW(matrix.addNextValue(1, 3, 0.7));
    ASSERT_NO_THROW(matrix.addNextValue(2, 0, 0.5));
    ASSERT_NO_THROW(matrix.addNextValue(2, 2, 0.99));
    ASSERT_NO_THROW(matrix.addNextValue(3, 3, 0.11));
    ASSERT_NO_THROW(matrix.finalize());
    
    ASSERT_NO_THROW(matrix.getJacobiDecomposition());
    std::pair<storm::storage::SparseMatrix<double>, storm::storage::SparseMatrix<double>> jacobiDecomposition = matrix.getJacobiDecomposition();
    
    storm::storage::SparseMatrix<double> LU(4, 4, 3);
    ASSERT_NO_THROW(LU.addNextValue(0, 1, 1.2));
    ASSERT_NO_THROW(LU.addNextValue(1, 3, 0.7));
    ASSERT_NO_THROW(LU.addNextValue(2, 0, 0.5));
    ASSERT_NO_THROW(LU.finalize());
    
    storm::storage::SparseMatrix<double> Dinv(4, 4, 4);
    ASSERT_NO_THROW(Dinv.addNextValue(0, 0, 1 / 1.1));
    ASSERT_NO_THROW(Dinv.addNextValue(1, 1, 1 / 0.5));
    ASSERT_NO_THROW(Dinv.addNextValue(2, 2, 1 / 0.99));
    ASSERT_NO_THROW(Dinv.addNextValue(3, 3, 1 / 0.11));
    ASSERT_NO_THROW(Dinv.finalize());
    
    ASSERT_TRUE(LU == jacobiDecomposition.first);
    ASSERT_TRUE(Dinv == jacobiDecomposition.second);
}

TEST(SparseMatrix, PointwiseMultiplicationVector) {
    storm::storage::SparseMatrix<double> matrix(5, 4, 9);
    ASSERT_NO_THROW(matrix.addNextValue(0, 1, 1.0));
    ASSERT_NO_THROW(matrix.addNextValue(0, 2, 1.2));
    ASSERT_NO_THROW(matrix.addNextValue(1, 0, 0.5));
    ASSERT_NO_THROW(matrix.addNextValue(1, 1, 0.7));
    ASSERT_NO_THROW(matrix.addNextValue(2, 0, 0.5));
    ASSERT_NO_THROW(matrix.addNextValue(3, 2, 1.1));
    ASSERT_NO_THROW(matrix.addNextValue(4, 0, 0.1));
    ASSERT_NO_THROW(matrix.addNextValue(4, 1, 0.2));
    ASSERT_NO_THROW(matrix.addNextValue(4, 3, 0.3));
    ASSERT_NO_THROW(matrix.finalize());
    
    storm::storage::SparseMatrix<double> matrix2(5, 4, 9);
    ASSERT_NO_THROW(matrix2.addNextValue(0, 1, 1.0));
    ASSERT_NO_THROW(matrix2.addNextValue(0, 2, 1.2));
    ASSERT_NO_THROW(matrix2.addNextValue(1, 0, 0.5));
    ASSERT_NO_THROW(matrix2.addNextValue(1, 1, 0.7));
    ASSERT_NO_THROW(matrix2.addNextValue(2, 0, 0.5));
    ASSERT_NO_THROW(matrix2.addNextValue(3, 2, 1.1));
    ASSERT_NO_THROW(matrix2.addNextValue(4, 0, 0.1));
    ASSERT_NO_THROW(matrix2.addNextValue(4, 1, 0.2));
    ASSERT_NO_THROW(matrix2.addNextValue(4, 3, 0.3));
    ASSERT_NO_THROW(matrix2.finalize());
    
    ASSERT_NO_THROW(std::vector<double> pointwiseProductRowSums = matrix.getPointwiseProductRowSumVector(matrix2));
    std::vector<double> pointwiseProductRowSums = matrix.getPointwiseProductRowSumVector(matrix2);
    
    std::vector<double> correctResult = {1.0*1.0+1.2*1.2, 0.5*0.5+0.7*0.7, 0.5*0.5, 1.1*1.1, 0.1*0.1+0.2*0.2+0.3*0.3};
    ASSERT_TRUE(pointwiseProductRowSums == correctResult);
}

TEST(SparseMatrix, MatrixVectorMultiply) {
    storm::storage::SparseMatrix<double> matrix(5, 4, 9);
    ASSERT_NO_THROW(matrix.addNextValue(0, 1, 1.0));
    ASSERT_NO_THROW(matrix.addNextValue(0, 2, 1.2));
    ASSERT_NO_THROW(matrix.addNextValue(1, 0, 0.5));
    ASSERT_NO_THROW(matrix.addNextValue(1, 1, 0.7));
    ASSERT_NO_THROW(matrix.addNextValue(2, 0, 0.5));
    ASSERT_NO_THROW(matrix.addNextValue(3, 2, 1.1));
    ASSERT_NO_THROW(matrix.addNextValue(4, 0, 0.1));
    ASSERT_NO_THROW(matrix.addNextValue(4, 1, 0.2));
    ASSERT_NO_THROW(matrix.addNextValue(4, 3, 0.3));
    ASSERT_NO_THROW(matrix.finalize());
    
    std::vector<double> x = {1, 0.3, 1.4, 7.1};
    std::vector<double> result(matrix.getRowCount());
    
    ASSERT_NO_THROW(matrix.multiplyWithVector(x, result));
    
    std::vector<double> correctResult = {1.0*0.3+1.2*1.4, 0.5*1+0.7*0.3, 0.5*1, 1.1*1.4, 0.1*1+0.2*0.3+0.3*7.1};
    ASSERT_TRUE(result == correctResult);
}

TEST(SparseMatrix, Iteration) {
    storm::storage::SparseMatrix<double> matrix(5, 4, 9);
    ASSERT_NO_THROW(matrix.addNextValue(0, 1, 1.0));
    ASSERT_NO_THROW(matrix.addNextValue(0, 2, 1.2));
    ASSERT_NO_THROW(matrix.addNextValue(1, 0, 0.5));
    ASSERT_NO_THROW(matrix.addNextValue(1, 1, 0.7));
    ASSERT_NO_THROW(matrix.addNextValue(2, 0, 0.5));
    ASSERT_NO_THROW(matrix.addNextValue(3, 2, 1.1));
    ASSERT_NO_THROW(matrix.addNextValue(4, 0, 0.1));
    ASSERT_NO_THROW(matrix.addNextValue(4, 1, 0.2));
    ASSERT_NO_THROW(matrix.addNextValue(4, 3, 0.3));
    ASSERT_NO_THROW(matrix.finalize());
    
    for (auto const& entry : matrix.getRow(4)) {
        if (entry.first == 0) {
            ASSERT_EQ(0.1, entry.second);
        } else if (entry.first == 1) {
            ASSERT_EQ(0.2, entry.second);
        } else if (entry.first == 3) {
            ASSERT_EQ(0.3, entry.second);
        } else {
            ASSERT_TRUE(false);
        }
    }
    
    for (storm::storage::SparseMatrix<double>::iterator it = matrix.begin(4), ite = matrix.end(4); it != ite; ++it) {
        if (it->first == 0) {
            ASSERT_EQ(0.1, it->second);
        } else if (it->first == 1) {
            ASSERT_EQ(0.2, it->second);
        } else if (it->first == 3) {
            ASSERT_EQ(0.3, it->second);
        } else {
            ASSERT_TRUE(false);
        }
    }
}

TEST(SparseMatrix, RowSum) {
    storm::storage::SparseMatrix<double> matrix(5, 4, 8);
    ASSERT_NO_THROW(matrix.addNextValue(0, 1, 1.0));
    ASSERT_NO_THROW(matrix.addNextValue(0, 2, 1.2));
    ASSERT_NO_THROW(matrix.addNextValue(1, 0, 0.5));
    ASSERT_NO_THROW(matrix.addNextValue(1, 1, 0.7));
    ASSERT_NO_THROW(matrix.addNextValue(3, 2, 1.1));
    ASSERT_NO_THROW(matrix.addNextValue(4, 0, 0.1));
    ASSERT_NO_THROW(matrix.addNextValue(4, 1, 0.2));
    ASSERT_NO_THROW(matrix.addNextValue(4, 3, 0.3));
    ASSERT_NO_THROW(matrix.finalize());
    
    ASSERT_EQ(0, matrix.getRowSum(2));
    ASSERT_EQ(0.1+0.2+0.3, matrix.getRowSum(4));
}

TEST(SparseMatrix, IsSubmatrix) {
    storm::storage::SparseMatrix<double> matrix(5, 4, 8);
    ASSERT_NO_THROW(matrix.addNextValue(0, 1, 1.0));
    ASSERT_NO_THROW(matrix.addNextValue(0, 2, 1.2));
    ASSERT_NO_THROW(matrix.addNextValue(1, 0, 0.5));
    ASSERT_NO_THROW(matrix.addNextValue(1, 1, 0.7));
    ASSERT_NO_THROW(matrix.addNextValue(3, 2, 1.1));
    ASSERT_NO_THROW(matrix.addNextValue(4, 0, 0.1));
    ASSERT_NO_THROW(matrix.addNextValue(4, 1, 0.2));
    ASSERT_NO_THROW(matrix.addNextValue(4, 3, 0.3));
    ASSERT_NO_THROW(matrix.finalize());
    
    storm::storage::SparseMatrix<double> matrix2(5, 4, 5);
    ASSERT_NO_THROW(matrix2.addNextValue(0, 1, 1.0));
    ASSERT_NO_THROW(matrix2.addNextValue(1, 0, 0.5));
    ASSERT_NO_THROW(matrix2.addNextValue(1, 1, 0.7));
    ASSERT_NO_THROW(matrix2.addNextValue(4, 0, 0.1));
    ASSERT_NO_THROW(matrix2.addNextValue(4, 1, 0.2));
    ASSERT_NO_THROW(matrix2.finalize());
    
    ASSERT_TRUE(matrix2.isSubmatrixOf(matrix));
    
    storm::storage::SparseMatrix<double> matrix3(5, 4, 5);
    ASSERT_NO_THROW(matrix3.addNextValue(0, 3, 1.0));
    ASSERT_NO_THROW(matrix3.addNextValue(1, 0, 0.5));
    ASSERT_NO_THROW(matrix3.addNextValue(1, 1, 0.7));
    ASSERT_NO_THROW(matrix3.addNextValue(4, 0, 0.1));
    ASSERT_NO_THROW(matrix3.addNextValue(4, 1, 0.2));
    ASSERT_NO_THROW(matrix3.finalize());
    
    ASSERT_FALSE(matrix3.isSubmatrixOf(matrix));
    ASSERT_FALSE(matrix3.isSubmatrixOf(matrix2));
}