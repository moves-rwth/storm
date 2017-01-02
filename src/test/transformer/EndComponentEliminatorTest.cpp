#include "gtest/gtest.h"

#include "storm/transformer/EndComponentEliminator.h"


TEST(NeutralECRemover, SimpleModelTest) {
    
    
    storm::storage::SparseMatrixBuilder<double> builder(12, 5, 19, true, true, 5);
    ASSERT_NO_THROW(builder.newRowGroup(0));
    ASSERT_NO_THROW(builder.addNextValue(0, 0, 1.0));
    ASSERT_NO_THROW(builder.addNextValue(1, 1, 0.3));
    ASSERT_NO_THROW(builder.addNextValue(1, 2, 0.1));
    ASSERT_NO_THROW(builder.addNextValue(1, 3, 0.4));
    ASSERT_NO_THROW(builder.addNextValue(1, 4, 0.2));
    ASSERT_NO_THROW(builder.newRowGroup(2));
    ASSERT_NO_THROW(builder.addNextValue(2, 1, 0.7));
    ASSERT_NO_THROW(builder.addNextValue(2, 3, 0.3));
    ASSERT_NO_THROW(builder.addNextValue(3, 1, 0.1));
    ASSERT_NO_THROW(builder.addNextValue(3, 4, 0.9));
    ASSERT_NO_THROW(builder.addNextValue(4, 1, 0.2));
    ASSERT_NO_THROW(builder.addNextValue(4, 4, 0.8));
    ASSERT_NO_THROW(builder.newRowGroup(5));
    ASSERT_NO_THROW(builder.addNextValue(5, 2, 1.0));
    ASSERT_NO_THROW(builder.newRowGroup(6));
    ASSERT_NO_THROW(builder.addNextValue(6, 1, 1.0));
    ASSERT_NO_THROW(builder.addNextValue(7, 2, 1.0));
    ASSERT_NO_THROW(builder.addNextValue(8, 3, 1.0));
    ASSERT_NO_THROW(builder.newRowGroup(9));
    ASSERT_NO_THROW(builder.addNextValue(9, 4, 1.0));
    ASSERT_NO_THROW(builder.addNextValue(10, 1, 0.4));
    ASSERT_NO_THROW(builder.addNextValue(10, 4, 0.6));
    ASSERT_NO_THROW(builder.addNextValue(11, 3, 1));
    storm::storage::SparseMatrix<double> matrix;
    ASSERT_NO_THROW(matrix = builder.build());
    
    storm::storage::BitVector subsystem(5, true);
    subsystem.set(2, false);
    
    storm::storage::BitVector possibleEcRows(12, true);
    possibleEcRows.set(3, false);
    possibleEcRows.set(6, false);
    possibleEcRows.set(7, false);
    possibleEcRows.set(8, false);
    possibleEcRows.set(11, false);
    
    storm::storage::BitVector allowEmptyRows(5, true);
    allowEmptyRows.set(1, false);
    allowEmptyRows.set(4, false);
    
    
    auto res = storm::transformer::EndComponentEliminator<double>::transform(matrix, subsystem, possibleEcRows, allowEmptyRows);
    
    // Expected data
    storm::storage::SparseMatrixBuilder<double> expectedBuilder(8, 3, 8, true, true, 3);
    ASSERT_NO_THROW(expectedBuilder.newRowGroup(0));
    ASSERT_NO_THROW(expectedBuilder.addNextValue(0, 2, 1.0));
    ASSERT_NO_THROW(expectedBuilder.addNextValue(2, 0, 1.0));
    ASSERT_NO_THROW(expectedBuilder.newRowGroup(3));
    ASSERT_NO_THROW(expectedBuilder.addNextValue(3, 0, 0.4));
    ASSERT_NO_THROW(expectedBuilder.addNextValue(3, 2, 0.5));
    ASSERT_NO_THROW(expectedBuilder.newRowGroup(5));
    ASSERT_NO_THROW(expectedBuilder.addNextValue(5, 0, 1.0));
    ASSERT_NO_THROW(expectedBuilder.addNextValue(6, 0, 0.3));
    ASSERT_NO_THROW(expectedBuilder.addNextValue(6, 2, 0.7));
    ASSERT_NO_THROW(expectedBuilder.addNextValue(7, 2, 1.0));
    storm::storage::SparseMatrix<double> expectedMatrix;
    ASSERT_NO_THROW(expectedMatrix = expectedBuilder.build());
    
    std::vector<uint_fast64_t> expectedNewToOldRowMapping = {6,7,8,1,0,11,2,3};
    
    std::vector<uint_fast64_t> expectedOldToNewStateMapping = {1,2,std::numeric_limits<uint_fast64_t>::max(), 0, 2};
    
    // Note that there are other possible solutions that yield equivalent matrices / vectors.
    // In particular, the ordering within the row groups depends on the MEC decomposition implementation.
    // However, this is not checked here...
    EXPECT_EQ(expectedMatrix, res.matrix);
    EXPECT_EQ(expectedNewToOldRowMapping, res.newToOldRowMapping);
    EXPECT_EQ(expectedOldToNewStateMapping, res.oldToNewStateMapping);
    //std::cout << "Original matrix:" << std::endl << matrix << std::endl << std::endl << "Computation Result: " << std::endl << res.matrix << std::endl<< std::endl << "expected Matrix" << std::endl<< expectedMatrix << std::endl;
    
}
