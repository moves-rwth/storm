#include "storm-config.h"
#include "test/storm_gtest.h"

#include <carl/formula/Constraint.h>
#include <memory>
#include <string>

#include "storm-pars/api/region.h"
#include "storm-pars/modelchecker/instantiation/SparseInstantiationModelChecker.h"
#include "storm-pars/modelchecker/region/SparseDtmcParameterLiftingModelChecker.h"
#include "storm-pars/modelchecker/region/SparseParameterLiftingModelChecker.h"
#include "storm-pars/storage/ParameterRegion.h"
#include "storm-pars/transformer/BinaryDtmcTransformer.h"
#include "storm-pars/transformer/IntervalEndComponentPreserver.h"
#include "storm-pars/transformer/RobustParameterLifter.h"
#include "storm-parsers/api/storm-parsers.h"
#include "storm-parsers/parser/AutoParser.h"
#include "storm-parsers/parser/FormulaParser.h"
#include "storm/adapters/RationalFunctionAdapter.h"
#include "storm/adapters/RationalFunctionForward.h"
#include "storm/adapters/RationalNumberAdapter.h"
#include "storm/adapters/RationalNumberForward.h"
#include "storm/api/bisimulation.h"
#include "storm/api/builder.h"
#include "storm/environment/Environment.h"
#include "storm/environment/solver/MinMaxSolverEnvironment.h"
#include "storm/environment/solver/SolverEnvironment.h"
#include "storm/modelchecker/CheckTask.h"
#include "storm/modelchecker/reachability/SparseDtmcEliminationModelChecker.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Model.h"
#include "storm/solver/IterativeMinMaxLinearEquationSolver.h"
#include "storm/solver/MinMaxLinearEquationSolver.h"
#include "storm/solver/OptimizationDirection.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/storage/bisimulation/BisimulationType.h"
#include "storm/storage/prism/Program.h"
#include "storm/utility/constants.h"
#include "storm/utility/logging.h"
#include "storm/utility/macros.h"
#include "storm/utility/prism.h"
#include "storm/utility/vector.h"

class IntervalEndComponentPreserverTest : public ::testing::Test {
   protected:
    void SetUp() override {
#ifndef STORM_HAVE_Z3
        GTEST_SKIP() << "Z3 not available.";
#endif
    }
};

TEST_F(IntervalEndComponentPreserverTest, Simple) {
    storm::storage::SparseMatrixBuilder<storm::Interval> builder(3, 3);
    //                     0       1       2
    //         ---- group 0/2 ----
    // 0       (       [0, 1]  [0, 1]  0               )       0
    //         ---- group 1/2 ----
    // 1       (       0       0       [1, 1]          )       1
    //         ---- group 2/2 ----
    // 2       (       0       0       0               )       2
    //                 0       1       2
    builder.addNextValue(0, 0, storm::Interval(0, 1));
    builder.addNextValue(0, 1, storm::Interval(0, 1));
    builder.addNextValue(1, 2, storm::Interval(1, 1));
    storm::storage::SparseMatrix<storm::Interval> matrix = builder.build();

    std::vector<storm::Interval> vector = {storm::Interval(0, 0), storm::Interval(1, 1), storm::Interval(0, 0)};

    storm::transformer::IntervalEndComponentPreserver preserver;
    auto newMatrix = preserver.eliminateMECs(matrix, vector);

    // Should be this now
    //                     0       1       2       3
    //         ---- group 0/3 ----
    // 0       (       0       [0, 1]  0       [0, 1]          )       0
    //         ---- group 1/3 ----
    // 1       (       0       0       [1, 1]  0               )       1
    //         ---- group 2/3 ----
    // 2       (       0       0       0       0               )       2
    //         ---- group 3/3 ----
    // 3       (       0       0       0       0               )       3
    //                 0       1       2       3

    ASSERT_EQ(newMatrix->getRowCount(), 4);
    ASSERT_EQ(newMatrix->getColumnCount(), 4);
    ASSERT_EQ(newMatrix->getEntryCount(), 3);

    ASSERT_EQ(newMatrix->getRow(0).getNumberOfEntries(), 2);
    ASSERT_EQ(newMatrix->getRow(0).begin()->getColumn(), 1);
    ASSERT_EQ(newMatrix->getRow(0).begin()->getValue(), storm::Interval(0, 1));
    ASSERT_EQ((newMatrix->getRow(0).begin() + 1)->getColumn(), 3);
    ASSERT_EQ((newMatrix->getRow(0).begin() + 1)->getValue(), storm::Interval(0, 1));

    ASSERT_EQ(newMatrix->getRow(1).getNumberOfEntries(), 1);
    ASSERT_EQ(newMatrix->getRow(1).begin()->getColumn(), 2);
    ASSERT_EQ(newMatrix->getRow(1).begin()->getValue(), storm::Interval(1, 1));

    ASSERT_EQ(newMatrix->getRow(2).getNumberOfEntries(), 0);
    ASSERT_EQ(newMatrix->getRow(3).getNumberOfEntries(), 0);
}
