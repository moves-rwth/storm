#include "gtest/gtest.h"
#include "storm-config.h"

#include "storm-dft/api/storm-dft.h"

namespace {

    TEST(DftParserTest, LoadFromGalileoFile) {
        std::string file = STORM_TEST_RESOURCES_DIR "/dft/and.dft";
        std::shared_ptr<storm::storage::DFT<double>> dft = storm::api::loadDFTGalileoFile<double>(file);
        EXPECT_EQ(3ul, dft->nrElements());
        EXPECT_EQ(2ul, dft->nrBasicElements());
        EXPECT_TRUE(storm::api::isWellFormed(*dft));
    }

    TEST(DftParserTest, LoadFromJsonFile) {
        std::string file = STORM_TEST_RESOURCES_DIR "/dft/and.json";
        std::shared_ptr<storm::storage::DFT<double>> dft = storm::api::loadDFTJsonFile<double>(file);
        EXPECT_EQ(3ul, dft->nrElements());
        EXPECT_EQ(2ul, dft->nrBasicElements());
        EXPECT_TRUE(storm::api::isWellFormed(*dft));
    }
}
