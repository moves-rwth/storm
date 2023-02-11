#include "storm/exceptions/InvalidStateException.h"
#include "storm/exceptions/OutOfRangeException.h"
#include "storm/storage/SparseMatrix.h"
#include "test/storm_gtest.h"

#include "storm-config.h"

#ifdef STORM_HAVE_CUDAFORSTORM

#include "cudaForStorm.h"

TEST(CudaPlugin, SpMV_4x4) {
    storm::storage::SparseMatrixBuilder<double> matrixBuilder(4, 4, 10);
    ASSERT_NO_THROW(matrixBuilder.addNextValue(0, 1, 1.0));
    ASSERT_NO_THROW(matrixBuilder.addNextValue(0, 3, -1.0));
    ASSERT_NO_THROW(matrixBuilder.addNextValue(1, 0, 8.0));
    ASSERT_NO_THROW(matrixBuilder.addNextValue(1, 1, 7.0));
    ASSERT_NO_THROW(matrixBuilder.addNextValue(1, 2, -5.0));
    ASSERT_NO_THROW(matrixBuilder.addNextValue(1, 3, 2.0));
    ASSERT_NO_THROW(matrixBuilder.addNextValue(2, 0, 2.0));
    ASSERT_NO_THROW(matrixBuilder.addNextValue(2, 1, 2.0));
    ASSERT_NO_THROW(matrixBuilder.addNextValue(2, 2, 4.0));
    ASSERT_NO_THROW(matrixBuilder.addNextValue(2, 3, 4.0));

    storm::storage::SparseMatrix<double> matrix;
    ASSERT_NO_THROW(matrix = matrixBuilder.build());

    ASSERT_EQ(4, matrix.getRowCount());
    ASSERT_EQ(4, matrix.getColumnCount());
    ASSERT_EQ(10, matrix.getEntryCount());

    std::vector<double> x({0, 4, 1, 1});
    std::vector<double> b({0, 0, 0, 0});

    ASSERT_NO_THROW(
        basicValueIteration_spmv_uint64_double(matrix.getColumnCount(), matrix.__internal_getRowIndications(), matrix.__internal_getColumnsAndValues(), x, b));

    ASSERT_EQ(b.at(0), 3);
    ASSERT_EQ(b.at(1), 25);
    ASSERT_EQ(b.at(2), 16);
    ASSERT_EQ(b.at(3), 0);
}

TEST(CudaPlugin, SpMV_VerySmall) {
    storm::storage::SparseMatrixBuilder<double> matrixBuilder(2, 2, 2);
    ASSERT_NO_THROW(matrixBuilder.addNextValue(0, 0, 1.0));
    ASSERT_NO_THROW(matrixBuilder.addNextValue(1, 1, 2.0));

    storm::storage::SparseMatrix<double> matrix;
    ASSERT_NO_THROW(matrix = matrixBuilder.build());

    ASSERT_EQ(2, matrix.getRowCount());
    ASSERT_EQ(2, matrix.getColumnCount());
    ASSERT_EQ(2, matrix.getEntryCount());

    std::vector<double> x({4.0, 8.0});
    std::vector<double> b({0.0, 0.0});

    ASSERT_NO_THROW(
        basicValueIteration_spmv_uint64_double(matrix.getColumnCount(), matrix.__internal_getRowIndications(), matrix.__internal_getColumnsAndValues(), x, b));

    ASSERT_EQ(b.at(0), 4.0);
    ASSERT_EQ(b.at(1), 16.0);
}

TEST(CudaPlugin, AddVectorsInplace) {
    std::vector<double> vectorA_1 = {0.0, 42.0, 21.4, 3.1415, 1.0, 7.3490390, 94093053905390.21, -0.000000000023};
    std::vector<double> vectorA_2 = {0.0, 42.0, 21.4, 3.1415, 1.0, 7.3490390, 94093053905390.21, -0.000000000023};
    std::vector<double> vectorA_3 = {0.0, 42.0, 21.4, 3.1415, 1.0, 7.3490390, 94093053905390.21, -0.000000000023};
    std::vector<double> vectorB = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    std::vector<double> vectorC = {-5000.0, -5000.0, -5000.0, -5000.0, -5000.0, -5000.0, -5000.0, -5000.0};

    ASSERT_EQ(vectorA_1.size(), 8);
    ASSERT_EQ(vectorA_2.size(), 8);
    ASSERT_EQ(vectorA_3.size(), 8);
    ASSERT_EQ(vectorB.size(), 8);
    ASSERT_EQ(vectorC.size(), 8);

    ASSERT_NO_THROW(basicValueIteration_addVectorsInplace_double(vectorA_1, vectorB));
    ASSERT_NO_THROW(basicValueIteration_addVectorsInplace_double(vectorA_2, vectorC));

    ASSERT_EQ(vectorA_1.size(), 8);
    ASSERT_EQ(vectorA_2.size(), 8);
    ASSERT_EQ(vectorA_3.size(), 8);
    ASSERT_EQ(vectorB.size(), 8);
    ASSERT_EQ(vectorC.size(), 8);

    for (size_t i = 0; i < vectorA_3.size(); ++i) {
        double cpu_result_b = vectorA_3.at(i) + vectorB.at(i);
        double cpu_result_c = vectorA_3.at(i) + vectorC.at(i);

        ASSERT_EQ(cpu_result_b, vectorA_1.at(i));
        ASSERT_EQ(cpu_result_c, vectorA_2.at(i));
    }
}

TEST(CudaPlugin, ReduceGroupedVector) {
    std::vector<double> groupedVector = {
        0.0,       -1000.0, 0.000004,       // Group 0
        5.0,                                // Group 1
        0.0,       1.0,     2.0,      3.0,  // Group 2
        -1000.0,   -3.14,   -0.0002,        // Group 3 (neg only)
        25.25,     25.25,   25.25,          // Group 4
        0.0,       0.0,     1.0,            // Group 5
        -0.000001, 0.000001                 // Group 6
    };
    std::vector<uint_fast64_t> grouping = {0, 3, 4, 8, 11, 14, 17, 19};

    std::vector<double> result_minimize = {-1000.0,  // Group 0
                                           5.0,     0.0, -1000.0, 25.25, 0.0, -0.000001};
    std::vector<double> result_maximize = {0.000004, 5.0, 3.0, -0.0002, 25.25, 1.0, 0.000001};

    std::vector<double> result_cuda_minimize = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    std::vector<double> result_cuda_maximize = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

    ASSERT_NO_THROW(basicValueIteration_reduceGroupedVector_uint64_double_minimize(groupedVector, grouping, result_cuda_minimize));
    ASSERT_NO_THROW(basicValueIteration_reduceGroupedVector_uint64_double_maximize(groupedVector, grouping, result_cuda_maximize));

    for (size_t i = 0; i < result_minimize.size(); ++i) {
        ASSERT_EQ(result_minimize.at(i), result_cuda_minimize.at(i));
        ASSERT_EQ(result_maximize.at(i), result_cuda_maximize.at(i));
    }
}

TEST(CudaPlugin, equalModuloPrecision) {
    std::vector<double> x = {123.45, 67.8, 901.23, 456789.012, 3.456789, -4567890.12};
    std::vector<double> y1 = {0.45, 0.8, 0.23, 0.012, 0.456789, -0.12};
    std::vector<double> y2 = {0.45, 0.8, 0.23, 456789.012, 0.456789, -4567890.12};
    std::vector<double> x2;
    std::vector<double> x3;
    std::vector<double> y3;
    std::vector<double> y4;
    x2.reserve(1000);
    x3.reserve(1000);
    y3.reserve(1000);
    y4.reserve(1000);
    for (size_t i = 0; i < 1000; ++i) {
        x2.push_back(static_cast<double>(i));
        y3.push_back(1.0);
        x3.push_back(-(1000.0 - static_cast<double>(i)));
        y4.push_back(1.0);
    }

    double maxElement1 = 0.0;
    double maxElement2 = 0.0;
    double maxElement3 = 0.0;
    double maxElement4 = 0.0;
    ASSERT_NO_THROW(basicValueIteration_equalModuloPrecision_double_NonRelative(x, y1, maxElement1));
    ASSERT_NO_THROW(basicValueIteration_equalModuloPrecision_double_NonRelative(x, y2, maxElement2));

    ASSERT_NO_THROW(basicValueIteration_equalModuloPrecision_double_Relative(x2, y3, maxElement3));
    ASSERT_NO_THROW(basicValueIteration_equalModuloPrecision_double_Relative(x3, y4, maxElement4));

    ASSERT_DOUBLE_EQ(4567890.0, maxElement1);
    ASSERT_DOUBLE_EQ(901.0, maxElement2);

    ASSERT_DOUBLE_EQ(998.0, maxElement3);
    ASSERT_DOUBLE_EQ(1001.0, maxElement4);
}

#endif
