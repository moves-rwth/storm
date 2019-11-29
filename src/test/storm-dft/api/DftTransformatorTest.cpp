#include "test/storm_gtest.h"
#include "storm-config.h"

#include "storm-dft/api/storm-dft.h"
#include "storm-dft/transformations/DftTransformator.h"

namespace {

    TEST(DftTransformatorTest, UniqueConstantFailedTest) {
        std::string file = STORM_TEST_RESOURCES_DIR "/dft/const_be_test.dft";
        std::shared_ptr<storm::storage::DFT<double>> originalDft = storm::api::loadDFTGalileoFile<double>(file);
        auto dftTransformator = storm::transformations::dft::DftTransformator<double>();
        std::shared_ptr<storm::storage::DFT<double>> transformedDft = dftTransformator.transformUniqueFailedBe(
                *originalDft);

        auto bes = transformedDft->getBasicElements();
        uint64_t constBeFailedCount = 0;
        uint64_t constBeFailsafeCount = 0;
        for (auto &be : bes) {
            if (be->type() == storm::storage::DFTElementType::BE_CONST) {
                if (be->canFail()) {
                    ++constBeFailedCount;
                } else {
                    ++constBeFailsafeCount;
                }
            }
        }

        EXPECT_EQ(1ul, constBeFailedCount);
        EXPECT_EQ(3ul, constBeFailsafeCount);
    }

    TEST(DftTransformatorTest, BinaryFDEPTest) {
        std::string file = STORM_TEST_RESOURCES_DIR "/dft/fdep5.dft";
        std::shared_ptr<storm::storage::DFT<double>> originalDft = storm::api::loadDFTGalileoFile<double>(file);
        auto dftTransformator = storm::transformations::dft::DftTransformator<double>();
        std::shared_ptr<storm::storage::DFT<double>> transformedDft = dftTransformator.transformBinaryFDEPs(
                *originalDft);

        uint64_t dependencyCount = transformedDft->getDependencies().size();

        EXPECT_EQ(2ul, dependencyCount);
    }

    TEST(DftTransformatorTest, PDEPTransformTest) {
        std::string file = STORM_TEST_RESOURCES_DIR "/dft/pdep4.dft";
        std::shared_ptr<storm::storage::DFT<double>> originalDft = storm::api::loadDFTGalileoFile<double>(file);
        auto dftTransformator = storm::transformations::dft::DftTransformator<double>();
        std::shared_ptr<storm::storage::DFT<double>> transformedDft = dftTransformator.transformBinaryFDEPs(
                *originalDft);

        uint64_t fdepCount = 0;
        uint64_t pdepCount = 0;

        for (auto depIndex : transformedDft->getDependencies()) {
            auto dep = transformedDft->getDependency(depIndex);
            if (dep->probability() == 1) {
                ++fdepCount;
            } else {
                ++pdepCount;
            }
        }

        EXPECT_EQ(1ul, pdepCount);
        EXPECT_EQ(2ul, fdepCount);
        EXPECT_EQ(4ul, transformedDft->nrBasicElements());
    }

}