#include "gmock/gmock.h"
#include "storm-config.h"
#include "test/storm_gtest.h"

#include "storm-dft/api/storm-dft.h"

namespace {

TEST(DftValidatorTest, Cyclic) {
    std::string file = STORM_TEST_RESOURCES_DIR "/dft/cyclic.dft";
    STORM_SILENT_EXPECT_THROW(storm::dft::api::loadDFTGalileoFile<double>(file), storm::exceptions::WrongFormatException);
}

TEST(DftValidatorTest, NonBinaryDependency) {
    std::string file = STORM_TEST_RESOURCES_DIR "/dft/fdep.dft";
    std::shared_ptr<storm::dft::storage::DFT<double>> dft = storm::dft::api::loadDFTGalileoFile<double>(file);
    EXPECT_TRUE(storm::dft::api::isWellFormed(*dft, false).first);
    auto result = storm::dft::api::isWellFormed(*dft, true);
    EXPECT_FALSE(result.first);
    EXPECT_THAT(result.second, ::testing::MatchesRegex("DFT has dependency with more than one dependent event."));
}

TEST(DftValidatorTest, MultipleConstantFailed) {
    std::string file = STORM_TEST_RESOURCES_DIR "/dft/const_be_test.dft";
    std::shared_ptr<storm::dft::storage::DFT<double>> dft = storm::dft::api::loadDFTGalileoFile<double>(file);
    EXPECT_TRUE(storm::dft::api::isWellFormed(*dft, false).first);
    auto result = storm::dft::api::isWellFormed(*dft, true);
    EXPECT_FALSE(result.first);
    EXPECT_THAT(result.second, ::testing::MatchesRegex("DFT has more than one constant failed BE."));
}

TEST(DftValidatorTest, OverlappingSpareModules) {
    std::string file = STORM_TEST_RESOURCES_DIR "/dft/spare_overlapping.dft";
    std::shared_ptr<storm::dft::storage::DFT<double>> dft = storm::dft::api::loadDFTGalileoFile<double>(file);
    EXPECT_TRUE(storm::dft::api::isWellFormed(*dft, false).first);
    auto result = storm::dft::api::isWellFormed(*dft, true);
    EXPECT_FALSE(result.first);
    EXPECT_THAT(result.second, ::testing::MatchesRegex("Spare modules .* should not overlap."));
}

TEST(DftValidatorTest, SharedPrimaryModule) {
    std::string file = STORM_TEST_RESOURCES_DIR "/dft/spare_shared_primary.dft";
    std::shared_ptr<storm::dft::storage::DFT<double>> dft = storm::dft::api::loadDFTGalileoFile<double>(file);
    EXPECT_TRUE(storm::dft::api::isWellFormed(*dft, false).first);
    auto result = storm::dft::api::isWellFormed(*dft, true);
    EXPECT_FALSE(result.first);
    EXPECT_THAT(result.second, ::testing::HasSubstr("shared primary module"));
}

TEST(DftValidatorTest, SpareConstantFailed) {
    std::string file = STORM_TEST_RESOURCES_DIR "/dft/spare_const_failed.dft";
    std::shared_ptr<storm::dft::storage::DFT<double>> dft = storm::dft::api::loadDFTGalileoFile<double>(file);
    EXPECT_TRUE(storm::dft::api::isWellFormed(*dft, false).first);
    auto result = storm::dft::api::isWellFormed(*dft, true);
    EXPECT_FALSE(result.first);
    EXPECT_THAT(result.second, ::testing::MatchesRegex("Spare module of .* contains a constant failed BE .*"));
}

TEST(DftValidatorTest, NonExponential) {
    std::string file = STORM_TEST_RESOURCES_DIR "/dft/all_be_distributions.dft";
    std::shared_ptr<storm::dft::storage::DFT<double>> dft = storm::dft::api::loadDFTGalileoFile<double>(file);
    EXPECT_TRUE(storm::dft::api::isWellFormed(*dft, false).first);
    auto result = storm::dft::api::isWellFormed(*dft, true);
    EXPECT_FALSE(result.first);
    EXPECT_THAT(result.second, ::testing::HasSubstr("DFT has BE distributions which are neither exponential nor constant failed/failsafe."));
}

}  // namespace
