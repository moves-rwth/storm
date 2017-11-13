#include "gtest/gtest.h"
#include "storm-config.h"

#include "storm/solver/EigenLinearEquationSolver.h"
#include "storm/settings/SettingsManager.h"

#include "storm/utility/constants.h"
#include "storm/settings/modules/EigenEquationSolverSettings.h"

TEST(EigenLinearEquationSolver, SolveWithStandardOptions) {
    ASSERT_NO_THROW(storm::storage::SparseMatrixBuilder<double> builder);
    storm::storage::SparseMatrixBuilder<double> builder;
    ASSERT_NO_THROW(builder.addNextValue(0, 0, 2));
    ASSERT_NO_THROW(builder.addNextValue(0, 1, 4));
    ASSERT_NO_THROW(builder.addNextValue(0, 2, -2));
    ASSERT_NO_THROW(builder.addNextValue(1, 0, 4));
    ASSERT_NO_THROW(builder.addNextValue(1, 1, -1));
    ASSERT_NO_THROW(builder.addNextValue(1, 2, 5));
    ASSERT_NO_THROW(builder.addNextValue(2, 0, -1));
    ASSERT_NO_THROW(builder.addNextValue(2, 1, -1));
    ASSERT_NO_THROW(builder.addNextValue(2, 2, 3));
    
    storm::storage::SparseMatrix<double> A;
    ASSERT_NO_THROW(A = builder.build());
    
    std::vector<double> x(3);
    std::vector<double> b = {16, -4, -7};
    
    ASSERT_NO_THROW(storm::solver::EigenLinearEquationSolver<double> solver(A));
    
    storm::solver::EigenLinearEquationSolver<double> solver(A);
    ASSERT_NO_THROW(solver.solveEquations(env, x, b));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::getModule<storm::settings::modules::EigenEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[1] - 3), storm::settings::getModule<storm::settings::modules::EigenEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[2] - (-1)), storm::settings::getModule<storm::settings::modules::EigenEquationSolverSettings>().getPrecision());
}

TEST(EigenLinearEquationSolver, SparseLU) {
    ASSERT_NO_THROW(storm::storage::SparseMatrixBuilder<double> builder);
    storm::storage::SparseMatrixBuilder<double> builder;
    ASSERT_NO_THROW(builder.addNextValue(0, 0, 2));
    ASSERT_NO_THROW(builder.addNextValue(0, 1, 4));
    ASSERT_NO_THROW(builder.addNextValue(0, 2, -2));
    ASSERT_NO_THROW(builder.addNextValue(1, 0, 4));
    ASSERT_NO_THROW(builder.addNextValue(1, 1, -1));
    ASSERT_NO_THROW(builder.addNextValue(1, 2, 5));
    ASSERT_NO_THROW(builder.addNextValue(2, 0, -1));
    ASSERT_NO_THROW(builder.addNextValue(2, 1, -1));
    ASSERT_NO_THROW(builder.addNextValue(2, 2, 3));
    
    storm::storage::SparseMatrix<double> A;
    ASSERT_NO_THROW(A = builder.build());
    
    std::vector<double> x(3);
    std::vector<double> b = {16, -4, -7};
    
    storm::solver::EigenLinearEquationSolver<double> solver(A);
    solver.getSettings().setSolutionMethod(storm::solver::EigenLinearEquationSolverSettings<double>::SolutionMethod::SparseLU);
    solver.getSettings().setMaximalNumberOfIterations(10000);
    solver.getSettings().setPreconditioner(storm::solver::EigenLinearEquationSolverSettings<double>::Preconditioner::None);
    ASSERT_NO_THROW(solver.solveEquations(env, x, b));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::getModule<storm::settings::modules::EigenEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[1] - 3), storm::settings::getModule<storm::settings::modules::EigenEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[2] - (-1)), storm::settings::getModule<storm::settings::modules::EigenEquationSolverSettings>().getPrecision());
}

#ifdef STORM_HAVE_CARL
TEST(EigenLinearEquationSolver, SparseLU_RationalNumber) {
    ASSERT_NO_THROW(storm::storage::SparseMatrixBuilder<storm::RationalNumber> builder);
    storm::storage::SparseMatrixBuilder<storm::RationalNumber> builder;
    ASSERT_NO_THROW(builder.addNextValue(0, 0, 2));
    ASSERT_NO_THROW(builder.addNextValue(0, 1, 4));
    ASSERT_NO_THROW(builder.addNextValue(0, 2, -2));
    ASSERT_NO_THROW(builder.addNextValue(1, 0, 4));
    ASSERT_NO_THROW(builder.addNextValue(1, 1, -1));
    ASSERT_NO_THROW(builder.addNextValue(1, 2, 5));
    ASSERT_NO_THROW(builder.addNextValue(2, 0, -1));
    ASSERT_NO_THROW(builder.addNextValue(2, 1, -1));
    ASSERT_NO_THROW(builder.addNextValue(2, 2, 3));
    
    storm::storage::SparseMatrix<storm::RationalNumber> A;
    ASSERT_NO_THROW(A = builder.build());
    
    std::vector<storm::RationalNumber> x(3);
    std::vector<storm::RationalNumber> b = {16, -4, -7};
    
    storm::solver::EigenLinearEquationSolver<storm::RationalNumber> solver(A);
    ASSERT_NO_THROW(solver.solveEquations(env, x, b));
    ASSERT_TRUE(storm::utility::isOne(x[0]));
    ASSERT_TRUE(x[1] == 3);
    ASSERT_TRUE(x[2] == -1);
}

TEST(EigenLinearEquationSolver, SparseLU_RationalFunction) {
    ASSERT_NO_THROW(storm::storage::SparseMatrixBuilder<storm::RationalFunction> builder);
    storm::storage::SparseMatrixBuilder<storm::RationalFunction> builder;
    ASSERT_NO_THROW(builder.addNextValue(0, 0, storm::RationalFunction(2)));
    ASSERT_NO_THROW(builder.addNextValue(0, 1, storm::RationalFunction(4)));
    ASSERT_NO_THROW(builder.addNextValue(0, 2, storm::RationalFunction(-2)));
    ASSERT_NO_THROW(builder.addNextValue(1, 0, storm::RationalFunction(4)));
    ASSERT_NO_THROW(builder.addNextValue(1, 1, storm::RationalFunction(-1)));
    ASSERT_NO_THROW(builder.addNextValue(1, 2, storm::RationalFunction(5)));
    ASSERT_NO_THROW(builder.addNextValue(2, 0, storm::RationalFunction(-1)));
    ASSERT_NO_THROW(builder.addNextValue(2, 1, storm::RationalFunction(-1)));
    ASSERT_NO_THROW(builder.addNextValue(2, 2, storm::RationalFunction(3)));
    
    storm::storage::SparseMatrix<storm::RationalFunction> A;
    ASSERT_NO_THROW(A = builder.build());
    
    std::vector<storm::RationalFunction> x(3);
    std::vector<storm::RationalFunction> b = {storm::RationalFunction(16), storm::RationalFunction(-4), storm::RationalFunction(-7)};
    
    storm::solver::EigenLinearEquationSolver<storm::RationalFunction> solver(A);
    ASSERT_NO_THROW(solver.solveEquations(env, x, b));
    ASSERT_TRUE(storm::utility::isOne(x[0]));
    ASSERT_TRUE(x[1] == storm::RationalFunction(3));
    ASSERT_TRUE(x[2] == storm::RationalFunction(-1));
}
#endif

TEST(EigenLinearEquationSolver, DGMRES) {
    ASSERT_NO_THROW(storm::storage::SparseMatrixBuilder<double> builder);
    storm::storage::SparseMatrixBuilder<double> builder;
    ASSERT_NO_THROW(builder.addNextValue(0, 0, 2));
    ASSERT_NO_THROW(builder.addNextValue(0, 1, 4));
    ASSERT_NO_THROW(builder.addNextValue(0, 2, -2));
    ASSERT_NO_THROW(builder.addNextValue(1, 0, 4));
    ASSERT_NO_THROW(builder.addNextValue(1, 1, -1));
    ASSERT_NO_THROW(builder.addNextValue(1, 2, 5));
    ASSERT_NO_THROW(builder.addNextValue(2, 0, -1));
    ASSERT_NO_THROW(builder.addNextValue(2, 1, -1));
    ASSERT_NO_THROW(builder.addNextValue(2, 2, 3));
    
    storm::storage::SparseMatrix<double> A;
    ASSERT_NO_THROW(A = builder.build());
    
    std::vector<double> x(3);
    std::vector<double> b = {16, -4, -7};
    
    storm::solver::EigenLinearEquationSolver<double> solver(A);
    solver.getSettings().setSolutionMethod(storm::solver::EigenLinearEquationSolverSettings<double>::SolutionMethod::DGMRES);
    solver.getSettings().setPrecision(1e-6);
    solver.getSettings().setMaximalNumberOfIterations(10000);
    solver.getSettings().setPreconditioner(storm::solver::EigenLinearEquationSolverSettings<double>::Preconditioner::None);
    solver.getSettings().setNumberOfIterationsUntilRestart(50);
    
    ASSERT_NO_THROW(solver.solveEquations(env, x, b));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::getModule<storm::settings::modules::EigenEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[1] - 3), storm::settings::getModule<storm::settings::modules::EigenEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[2] - (-1)), storm::settings::getModule<storm::settings::modules::EigenEquationSolverSettings>().getPrecision());
}

TEST(EigenLinearEquationSolver, DGMRES_Ilu) {
    ASSERT_NO_THROW(storm::storage::SparseMatrixBuilder<double> builder);
    storm::storage::SparseMatrixBuilder<double> builder;
    ASSERT_NO_THROW(builder.addNextValue(0, 0, 2));
    ASSERT_NO_THROW(builder.addNextValue(0, 1, 4));
    ASSERT_NO_THROW(builder.addNextValue(0, 2, -2));
    ASSERT_NO_THROW(builder.addNextValue(1, 0, 4));
    ASSERT_NO_THROW(builder.addNextValue(1, 1, -1));
    ASSERT_NO_THROW(builder.addNextValue(1, 2, 5));
    ASSERT_NO_THROW(builder.addNextValue(2, 0, -1));
    ASSERT_NO_THROW(builder.addNextValue(2, 1, -1));
    ASSERT_NO_THROW(builder.addNextValue(2, 2, 3));
    
    storm::storage::SparseMatrix<double> A;
    ASSERT_NO_THROW(A = builder.build());
    
    std::vector<double> x(3);
    std::vector<double> b = {16, -4, -7};
    
    storm::solver::EigenLinearEquationSolver<double> solver(A);
    solver.getSettings().setSolutionMethod(storm::solver::EigenLinearEquationSolverSettings<double>::SolutionMethod::DGMRES);
    solver.getSettings().setPrecision(1e-6);
    solver.getSettings().setMaximalNumberOfIterations(10000);
    solver.getSettings().setPreconditioner(storm::solver::EigenLinearEquationSolverSettings<double>::Preconditioner::Ilu);
    ASSERT_NO_THROW(solver.solveEquations(env, x, b));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::getModule<storm::settings::modules::EigenEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[1] - 3), storm::settings::getModule<storm::settings::modules::EigenEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[2] - (-1)), storm::settings::getModule<storm::settings::modules::EigenEquationSolverSettings>().getPrecision());
}

TEST(EigenLinearEquationSolver, DGMRES_Diagonal) {
    ASSERT_NO_THROW(storm::storage::SparseMatrixBuilder<double> builder);
    storm::storage::SparseMatrixBuilder<double> builder;
    ASSERT_NO_THROW(builder.addNextValue(0, 0, 2));
    ASSERT_NO_THROW(builder.addNextValue(0, 1, 4));
    ASSERT_NO_THROW(builder.addNextValue(0, 2, -2));
    ASSERT_NO_THROW(builder.addNextValue(1, 0, 4));
    ASSERT_NO_THROW(builder.addNextValue(1, 1, -1));
    ASSERT_NO_THROW(builder.addNextValue(1, 2, 5));
    ASSERT_NO_THROW(builder.addNextValue(2, 0, -1));
    ASSERT_NO_THROW(builder.addNextValue(2, 1, -1));
    ASSERT_NO_THROW(builder.addNextValue(2, 2, 3));
    
    storm::storage::SparseMatrix<double> A;
    ASSERT_NO_THROW(A = builder.build());
    
    std::vector<double> x(3);
    std::vector<double> b = {16, -4, -7};
    
    storm::solver::EigenLinearEquationSolver<double> solver(A);
    solver.getSettings().setSolutionMethod(storm::solver::EigenLinearEquationSolverSettings<double>::SolutionMethod::DGMRES);
    solver.getSettings().setPrecision(1e-6);
    solver.getSettings().setMaximalNumberOfIterations(10000);
    solver.getSettings().setPreconditioner(storm::solver::EigenLinearEquationSolverSettings<double>::Preconditioner::Diagonal);
    ASSERT_NO_THROW(solver.solveEquations(env, x, b));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::getModule<storm::settings::modules::EigenEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[1] - 3), storm::settings::getModule<storm::settings::modules::EigenEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[2] - (-1)), storm::settings::getModule<storm::settings::modules::EigenEquationSolverSettings>().getPrecision());
}

TEST(EigenLinearEquationSolver, GMRES) {
    ASSERT_NO_THROW(storm::storage::SparseMatrixBuilder<double> builder);
    storm::storage::SparseMatrixBuilder<double> builder;
    ASSERT_NO_THROW(builder.addNextValue(0, 0, 2));
    ASSERT_NO_THROW(builder.addNextValue(0, 1, 4));
    ASSERT_NO_THROW(builder.addNextValue(0, 2, -2));
    ASSERT_NO_THROW(builder.addNextValue(1, 0, 4));
    ASSERT_NO_THROW(builder.addNextValue(1, 1, -1));
    ASSERT_NO_THROW(builder.addNextValue(1, 2, 5));
    ASSERT_NO_THROW(builder.addNextValue(2, 0, -1));
    ASSERT_NO_THROW(builder.addNextValue(2, 1, -1));
    ASSERT_NO_THROW(builder.addNextValue(2, 2, 3));
    
    storm::storage::SparseMatrix<double> A;
    ASSERT_NO_THROW(A = builder.build());
    
    std::vector<double> x(3);
    std::vector<double> b = {16, -4, -7};
    
    storm::solver::EigenLinearEquationSolver<double> solver(A);
    solver.getSettings().setSolutionMethod(storm::solver::EigenLinearEquationSolverSettings<double>::SolutionMethod::GMRES);
    solver.getSettings().setPrecision(1e-6);
    solver.getSettings().setMaximalNumberOfIterations(10000);
    solver.getSettings().setPreconditioner(storm::solver::EigenLinearEquationSolverSettings<double>::Preconditioner::None);
    solver.getSettings().setNumberOfIterationsUntilRestart(50);
    
    ASSERT_NO_THROW(solver.solveEquations(env, x, b));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::getModule<storm::settings::modules::EigenEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[1] - 3), storm::settings::getModule<storm::settings::modules::EigenEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[2] - (-1)), storm::settings::getModule<storm::settings::modules::EigenEquationSolverSettings>().getPrecision());
}

TEST(EigenLinearEquationSolver, GMRES_Ilu) {
    ASSERT_NO_THROW(storm::storage::SparseMatrixBuilder<double> builder);
    storm::storage::SparseMatrixBuilder<double> builder;
    ASSERT_NO_THROW(builder.addNextValue(0, 0, 2));
    ASSERT_NO_THROW(builder.addNextValue(0, 1, 4));
    ASSERT_NO_THROW(builder.addNextValue(0, 2, -2));
    ASSERT_NO_THROW(builder.addNextValue(1, 0, 4));
    ASSERT_NO_THROW(builder.addNextValue(1, 1, -1));
    ASSERT_NO_THROW(builder.addNextValue(1, 2, 5));
    ASSERT_NO_THROW(builder.addNextValue(2, 0, -1));
    ASSERT_NO_THROW(builder.addNextValue(2, 1, -1));
    ASSERT_NO_THROW(builder.addNextValue(2, 2, 3));
    
    storm::storage::SparseMatrix<double> A;
    ASSERT_NO_THROW(A = builder.build());
    
    std::vector<double> x(3);
    std::vector<double> b = {16, -4, -7};
    
    storm::solver::EigenLinearEquationSolver<double> solver(A);
    solver.getSettings().setSolutionMethod(storm::solver::EigenLinearEquationSolverSettings<double>::SolutionMethod::GMRES);
    solver.getSettings().setPrecision(1e-6);
    solver.getSettings().setMaximalNumberOfIterations(10000);
    solver.getSettings().setPreconditioner(storm::solver::EigenLinearEquationSolverSettings<double>::Preconditioner::Ilu);
    ASSERT_NO_THROW(solver.solveEquations(env, x, b));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::getModule<storm::settings::modules::EigenEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[1] - 3), storm::settings::getModule<storm::settings::modules::EigenEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[2] - (-1)), storm::settings::getModule<storm::settings::modules::EigenEquationSolverSettings>().getPrecision());
}

TEST(EigenLinearEquationSolver, GMRES_Diagonal) {
    ASSERT_NO_THROW(storm::storage::SparseMatrixBuilder<double> builder);
    storm::storage::SparseMatrixBuilder<double> builder;
    ASSERT_NO_THROW(builder.addNextValue(0, 0, 2));
    ASSERT_NO_THROW(builder.addNextValue(0, 1, 4));
    ASSERT_NO_THROW(builder.addNextValue(0, 2, -2));
    ASSERT_NO_THROW(builder.addNextValue(1, 0, 4));
    ASSERT_NO_THROW(builder.addNextValue(1, 1, -1));
    ASSERT_NO_THROW(builder.addNextValue(1, 2, 5));
    ASSERT_NO_THROW(builder.addNextValue(2, 0, -1));
    ASSERT_NO_THROW(builder.addNextValue(2, 1, -1));
    ASSERT_NO_THROW(builder.addNextValue(2, 2, 3));
    
    storm::storage::SparseMatrix<double> A;
    ASSERT_NO_THROW(A = builder.build());
    
    std::vector<double> x(3);
    std::vector<double> b = {16, -4, -7};
    
    storm::solver::EigenLinearEquationSolver<double> solver(A);
    solver.getSettings().setSolutionMethod(storm::solver::EigenLinearEquationSolverSettings<double>::SolutionMethod::GMRES);
    solver.getSettings().setPrecision(1e-6);
    solver.getSettings().setMaximalNumberOfIterations(10000);
    solver.getSettings().setPreconditioner(storm::solver::EigenLinearEquationSolverSettings<double>::Preconditioner::Diagonal);
    ASSERT_NO_THROW(solver.solveEquations(env, x, b));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::getModule<storm::settings::modules::EigenEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[1] - 3), storm::settings::getModule<storm::settings::modules::EigenEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[2] - (-1)), storm::settings::getModule<storm::settings::modules::EigenEquationSolverSettings>().getPrecision());
}

TEST(EigenLinearEquationSolver, BiCGSTAB) {
    ASSERT_NO_THROW(storm::storage::SparseMatrixBuilder<double> builder);
    storm::storage::SparseMatrixBuilder<double> builder;
    ASSERT_NO_THROW(builder.addNextValue(0, 0, 2));
    ASSERT_NO_THROW(builder.addNextValue(0, 1, 4));
    ASSERT_NO_THROW(builder.addNextValue(0, 2, -2));
    ASSERT_NO_THROW(builder.addNextValue(1, 0, 4));
    ASSERT_NO_THROW(builder.addNextValue(1, 1, -1));
    ASSERT_NO_THROW(builder.addNextValue(1, 2, 5));
    ASSERT_NO_THROW(builder.addNextValue(2, 0, -1));
    ASSERT_NO_THROW(builder.addNextValue(2, 1, -1));
    ASSERT_NO_THROW(builder.addNextValue(2, 2, 3));
    
    storm::storage::SparseMatrix<double> A;
    ASSERT_NO_THROW(A = builder.build());
    
    std::vector<double> x(3);
    std::vector<double> b = {16, -4, -7};
    
    storm::solver::EigenLinearEquationSolver<double> solver(A);
    solver.getSettings().setSolutionMethod(storm::solver::EigenLinearEquationSolverSettings<double>::SolutionMethod::BiCGSTAB);
    solver.getSettings().setPrecision(1e-6);
    solver.getSettings().setMaximalNumberOfIterations(10000);
    solver.getSettings().setPreconditioner(storm::solver::EigenLinearEquationSolverSettings<double>::Preconditioner::None);

    ASSERT_NO_THROW(solver.solveEquations(env, x, b));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::getModule<storm::settings::modules::EigenEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[1] - 3), storm::settings::getModule<storm::settings::modules::EigenEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[2] - (-1)), storm::settings::getModule<storm::settings::modules::EigenEquationSolverSettings>().getPrecision());
}

TEST(EigenLinearEquationSolver, BiCGSTAB_Ilu) {
    ASSERT_NO_THROW(storm::storage::SparseMatrixBuilder<double> builder);
    storm::storage::SparseMatrixBuilder<double> builder;
    ASSERT_NO_THROW(builder.addNextValue(0, 0, 2));
    ASSERT_NO_THROW(builder.addNextValue(0, 1, 4));
    ASSERT_NO_THROW(builder.addNextValue(0, 2, -2));
    ASSERT_NO_THROW(builder.addNextValue(1, 0, 4));
    ASSERT_NO_THROW(builder.addNextValue(1, 1, -1));
    ASSERT_NO_THROW(builder.addNextValue(1, 2, 5));
    ASSERT_NO_THROW(builder.addNextValue(2, 0, -1));
    ASSERT_NO_THROW(builder.addNextValue(2, 1, -1));
    ASSERT_NO_THROW(builder.addNextValue(2, 2, 3));
    
    storm::storage::SparseMatrix<double> A;
    ASSERT_NO_THROW(A = builder.build());
    
    std::vector<double> x(3);
    std::vector<double> b = {16, -4, -7};
    
    storm::solver::EigenLinearEquationSolver<double> solver(A);
    solver.getSettings().setSolutionMethod(storm::solver::EigenLinearEquationSolverSettings<double>::SolutionMethod::BiCGSTAB);
    solver.getSettings().setPrecision(1e-6);
    solver.getSettings().setMaximalNumberOfIterations(10000);
    solver.getSettings().setPreconditioner(storm::solver::EigenLinearEquationSolverSettings<double>::Preconditioner::Ilu);
    ASSERT_NO_THROW(solver.solveEquations(env, x, b));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::getModule<storm::settings::modules::EigenEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[1] - 3), storm::settings::getModule<storm::settings::modules::EigenEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[2] - (-1)), storm::settings::getModule<storm::settings::modules::EigenEquationSolverSettings>().getPrecision());
}

TEST(EigenLinearEquationSolver, BiCGSTAB_Diagonal) {
    ASSERT_NO_THROW(storm::storage::SparseMatrixBuilder<double> builder);
    storm::storage::SparseMatrixBuilder<double> builder;
    ASSERT_NO_THROW(builder.addNextValue(0, 0, 2));
    ASSERT_NO_THROW(builder.addNextValue(0, 1, 4));
    ASSERT_NO_THROW(builder.addNextValue(0, 2, -2));
    ASSERT_NO_THROW(builder.addNextValue(1, 0, 4));
    ASSERT_NO_THROW(builder.addNextValue(1, 1, -1));
    ASSERT_NO_THROW(builder.addNextValue(1, 2, 5));
    ASSERT_NO_THROW(builder.addNextValue(2, 0, -1));
    ASSERT_NO_THROW(builder.addNextValue(2, 1, -1));
    ASSERT_NO_THROW(builder.addNextValue(2, 2, 3));
    
    storm::storage::SparseMatrix<double> A;
    ASSERT_NO_THROW(A = builder.build());
    
    std::vector<double> x(3);
    std::vector<double> b = {16, -4, -7};
    
    storm::solver::EigenLinearEquationSolver<double> solver(A);
    solver.getSettings().setSolutionMethod(storm::solver::EigenLinearEquationSolverSettings<double>::SolutionMethod::BiCGSTAB);
    solver.getSettings().setPrecision(1e-6);
    solver.getSettings().setMaximalNumberOfIterations(10000);
    solver.getSettings().setPreconditioner(storm::solver::EigenLinearEquationSolverSettings<double>::Preconditioner::Diagonal);
    ASSERT_NO_THROW(solver.solveEquations(env, x, b));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::getModule<storm::settings::modules::EigenEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[1] - 3), storm::settings::getModule<storm::settings::modules::EigenEquationSolverSettings>().getPrecision());
    ASSERT_LT(std::abs(x[2] - (-1)), storm::settings::getModule<storm::settings::modules::EigenEquationSolverSettings>().getPrecision());
}

TEST(EigenLinearEquationSolver, MatrixVectorMultiplication) {
    ASSERT_NO_THROW(storm::storage::SparseMatrixBuilder<double> builder);
    storm::storage::SparseMatrixBuilder<double> builder;
    ASSERT_NO_THROW(builder.addNextValue(0, 1, 0.5));
    ASSERT_NO_THROW(builder.addNextValue(0, 4, 0.5));
    ASSERT_NO_THROW(builder.addNextValue(1, 2, 0.5));
    ASSERT_NO_THROW(builder.addNextValue(1, 4, 0.5));
    ASSERT_NO_THROW(builder.addNextValue(2, 3, 0.5));
    ASSERT_NO_THROW(builder.addNextValue(2, 4, 0.5));
    ASSERT_NO_THROW(builder.addNextValue(3, 4, 1));
    ASSERT_NO_THROW(builder.addNextValue(4, 4, 1));
    
    storm::storage::SparseMatrix<double> A;
    ASSERT_NO_THROW(A = builder.build());
    
    std::vector<double> x(5);
    x[4] = 1;
    
    storm::solver::EigenLinearEquationSolver<double> solver(A);
    ASSERT_NO_THROW(solver.repeatedMultiply(x, nullptr, 4));
    ASSERT_LT(std::abs(x[0] - 1), storm::settings::getModule<storm::settings::modules::EigenEquationSolverSettings>().getPrecision());
}
