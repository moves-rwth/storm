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
}