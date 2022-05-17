#include "storm-config.h"
#include "test/storm_gtest.h"

#include "storm-dft/api/storm-dft.h"

namespace {

TEST(DftParserTest, LoadFromGalileoFile) {
    std::string file = STORM_TEST_RESOURCES_DIR "/dft/and.dft";
    std::shared_ptr<storm::dft::storage::DFT<double>> dft = storm::dft::api::loadDFTGalileoFile<double>(file);
    EXPECT_EQ(3ul, dft->nrElements());
    EXPECT_EQ(2ul, dft->nrBasicElements());
    EXPECT_TRUE(storm::dft::api::isWellFormed(*dft).first);
}

TEST(DftParserTest, LoadFromJsonFile) {
    std::string file = STORM_TEST_RESOURCES_DIR "/dft/and.json";
    std::shared_ptr<storm::dft::storage::DFT<double>> dft = storm::dft::api::loadDFTJsonFile<double>(file);
    EXPECT_EQ(3ul, dft->nrElements());
    EXPECT_EQ(2ul, dft->nrBasicElements());
    EXPECT_TRUE(storm::dft::api::isWellFormed(*dft).first);
}

TEST(DftParserTest, CatchCycles) {
    std::string file = STORM_TEST_RESOURCES_DIR "/dft/cyclic.dft";
    STORM_SILENT_EXPECT_THROW(storm::dft::api::loadDFTGalileoFile<double>(file), storm::exceptions::WrongFormatException);
}

TEST(DftParserTest, LoadSeqChildren) {
    std::string file = STORM_TEST_RESOURCES_DIR "/dft/seqChild.dft";
    std::shared_ptr<storm::dft::storage::DFT<double>> dft = storm::dft::api::loadDFTGalileoFile<double>(file);
    EXPECT_EQ(4ul, dft->nrElements());
    EXPECT_EQ(2ul, dft->nrBasicElements());
    EXPECT_TRUE(storm::dft::api::isWellFormed(*dft).first);
}
}  // namespace
