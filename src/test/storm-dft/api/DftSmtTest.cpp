#include "gtest/gtest.h"
#include "storm-config.h"

#include "storm-dft/api/storm-dft.h"

namespace {
    TEST(DftSmtTest, AndTest) {
        std::shared_ptr<storm::storage::DFT<double>> dft =
                storm::api::loadDFTGalileoFile<double>(STORM_TEST_RESOURCES_DIR "/dft/and.dft");
        EXPECT_TRUE(storm::api::isWellFormed(*dft));
        storm::modelchecker::DFTASFChecker smtChecker(*dft);
        smtChecker.convert();
        smtChecker.toSolver();
        EXPECT_EQ(smtChecker.checkTleNeverFailed(), storm::solver::SmtSolver::CheckResult::Unsat);
    }

    TEST(DftSmtTest, PandTest) {
        std::shared_ptr<storm::storage::DFT<double>> dft =
                storm::api::loadDFTGalileoFile<double>(STORM_TEST_RESOURCES_DIR "/dft/pand.dft");
        EXPECT_TRUE(storm::api::isWellFormed(*dft));
        storm::modelchecker::DFTASFChecker smtChecker(*dft);
        smtChecker.convert();
        smtChecker.toSolver();
        EXPECT_EQ(smtChecker.checkTleNeverFailed(), storm::solver::SmtSolver::CheckResult::Sat);
    }

    TEST(DftSmtTest, SpareTest) {
        std::shared_ptr<storm::storage::DFT<double>> dft =
                storm::api::loadDFTGalileoFile<double>(STORM_TEST_RESOURCES_DIR "/dft/spare_two_modules.dft");
        EXPECT_TRUE(storm::api::isWellFormed(*dft));
        storm::modelchecker::DFTASFChecker smtChecker(*dft);
        smtChecker.convert();
        smtChecker.toSolver();
        EXPECT_EQ(smtChecker.checkTleFailsWithLeq(2), storm::solver::SmtSolver::CheckResult::Unsat);
        EXPECT_EQ(smtChecker.checkTleFailsWithEq(3), storm::solver::SmtSolver::CheckResult::Sat);
    }

    TEST(DftSmtTest, BoundTest) {
        std::shared_ptr<storm::storage::DFT<double>> dft =
                storm::api::loadDFTGalileoFile<double>(STORM_TEST_RESOURCES_DIR "/dft/spare5.dft");
        EXPECT_TRUE(storm::api::isWellFormed(*dft));
        storm::modelchecker::DFTASFChecker smtChecker(*dft);
        smtChecker.convert();
        smtChecker.toSolver();
        EXPECT_EQ(smtChecker.getLeastFailureBound(30), uint64_t(2));
        EXPECT_EQ(smtChecker.getAlwaysFailedBound(30), uint64_t(4));
    }

    TEST(DftSmtTest, FDEPBoundTest) {
        std::shared_ptr<storm::storage::DFT<double>> dft =
                storm::api::loadDFTGalileoFile<double>(STORM_TEST_RESOURCES_DIR "/dft/fdep_bound.dft");
        EXPECT_TRUE(storm::api::isWellFormed(*dft));
        storm::modelchecker::DFTASFChecker smtChecker(*dft);
        smtChecker.convert();
        smtChecker.toSolver();
        EXPECT_EQ(smtChecker.getLeastFailureBound(30), uint64_t(1));
        EXPECT_EQ(smtChecker.getAlwaysFailedBound(30), uint64_t(5));
    }
}