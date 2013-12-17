#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/solver/GmmxxNondeterministicLinearEquationSolver.h"
#include "src/settings/Settings.h"

TEST(GmmxxNondeterministicLinearEquationSolver, SolveWithStandardOptions) {
    ASSERT_NO_THROW(storm::storage::SparseMatrixBuilder<double> builder);
    storm::storage::SparseMatrixBuilder<double> builder;
    ASSERT_NO_THROW(builder.addNextValue(0, 0, 0.9));

    storm::storage::SparseMatrix<double> A;
    ASSERT_NO_THROW(A = builder.build(2));

    std::vector<uint_fast64_t> nondeterministicChoiceIndices = {0, 2};
    
    std::vector<double> x(1);
    std::vector<double> b = {0.099, 0.5};
    
    ASSERT_NO_THROW(storm::solver::GmmxxNondeterministicLinearEquationSolver<double> solver);
    
    storm::solver::GmmxxNondeterministicLinearEquationSolver<double> solver;
    ASSERT_NO_THROW(solver.solveEquationSystem(true, A, x, b, nondeterministicChoiceIndices));
    ASSERT_LT(std::abs(x[0] - 0.5), storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

    ASSERT_NO_THROW(solver.solveEquationSystem(false, A, x, b, nondeterministicChoiceIndices));
    ASSERT_LT(std::abs(x[0] - 0.989991), storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
}

TEST(GmmxxNondeterministicLinearEquationSolver, MatrixVectorMultiplication) {
    ASSERT_NO_THROW(storm::storage::SparseMatrixBuilder<double> builder);
    storm::storage::SparseMatrixBuilder<double> builder;
    ASSERT_NO_THROW(builder.addNextValue(0, 0, 0.9));
    ASSERT_NO_THROW(builder.addNextValue(0, 1, 0.099));
    ASSERT_NO_THROW(builder.addNextValue(0, 2, 0.001));
    ASSERT_NO_THROW(builder.addNextValue(1, 1, 0.5));
    ASSERT_NO_THROW(builder.addNextValue(1, 2, 0.5));
    ASSERT_NO_THROW(builder.addNextValue(2, 1, 1));
    ASSERT_NO_THROW(builder.addNextValue(3, 2, 1));
    
    storm::storage::SparseMatrix<double> A;
    ASSERT_NO_THROW(A = builder.build());
    
    std::vector<uint_fast64_t> nondeterministicChoiceIndices = {0, 2, 3, 4};
    std::vector<double> x = {0, 1, 0};
    
    ASSERT_NO_THROW(storm::solver::GmmxxNondeterministicLinearEquationSolver<double> solver);
    
    storm::solver::GmmxxNondeterministicLinearEquationSolver<double> solver;
    ASSERT_NO_THROW(solver.performMatrixVectorMultiplication(true, A, x, nondeterministicChoiceIndices, nullptr, 1));
    ASSERT_LT(std::abs(x[0] - 0.099), storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

    x = {0, 1, 0};
    ASSERT_NO_THROW(solver.performMatrixVectorMultiplication(true, A, x, nondeterministicChoiceIndices, nullptr, 2));
    ASSERT_LT(std::abs(x[0] - 0.1881), storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

    x = {0, 1, 0};
    ASSERT_NO_THROW(solver.performMatrixVectorMultiplication(true, A, x, nondeterministicChoiceIndices, nullptr, 20));
    ASSERT_LT(std::abs(x[0] - 0.5), storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

    x = {0, 1, 0};
    ASSERT_NO_THROW(solver.performMatrixVectorMultiplication(false, A, x, nondeterministicChoiceIndices, nullptr, 1));
    ASSERT_LT(std::abs(x[0] - 0.5), storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());

    x = {0, 1, 0};
    ASSERT_NO_THROW(solver.performMatrixVectorMultiplication(false, A, x, nondeterministicChoiceIndices, nullptr, 20));
    ASSERT_LT(std::abs(x[0] - 0.9238082658), storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble());
}