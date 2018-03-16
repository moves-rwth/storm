#include "gtest/gtest.h"
#include "storm-config.h"

#include "storm-dft/api/storm-dft.h"

namespace {

    // Base holding information about test example
    struct DftExample {
        std::string file;
        double expectedValue;
    };
    struct DftAnalysisConfig {
        DftExample example;
        bool useSR;
        bool useMod;
        bool useDC;
    };

    // Base test for regression test
    class DftAnalysisTestCase : public ::testing::TestWithParam<std::tuple<DftExample, bool, bool, bool>>
    {
    protected:
        DftAnalysisConfig analysisConfig {
                std::get<0>(GetParam()),
                std::get<1>(GetParam()),
                std::get<2>(GetParam()),
                std::get<3>(GetParam())
        };
    };

    TEST_P(DftAnalysisTestCase, AnalyzeMTTF) {
        std::stringstream stream;
        stream << STORM_TEST_RESOURCES_DIR << "/dft/" << analysisConfig.example.file << ".dft";
        std::shared_ptr<storm::storage::DFT<double>> dft = storm::api::loadDFTGalileo<double>(stream.str());

        std::string property = "Tmin=? [F \"failed\"]";
        std::vector<std::shared_ptr<storm::logic::Formula const>> properties = storm::api::extractFormulasFromProperties(storm::api::parseProperties(property));

        typename storm::modelchecker::DFTModelChecker<double>::dft_results results = storm::api::analyzeDFT<double>(*dft, properties, analysisConfig.useSR, analysisConfig.useMod, analysisConfig.useDC);
        double result = boost::get<double>(results[0]);
        EXPECT_FLOAT_EQ(result, analysisConfig.example.expectedValue);
    }

    TEST(DftApiTest, LoadFromGalileo) {
        std::string file = STORM_TEST_RESOURCES_DIR "/dft/and.dft";
        std::shared_ptr<storm::storage::DFT<double>> dft = storm::api::loadDFTGalileo<double>(file);
    }

    INSTANTIATE_TEST_CASE_P(RegularPolygon, DftAnalysisTestCase, ::testing::Combine(
            testing::Values(
                DftExample {"and", 3.0}
            ),
            ::testing::Bool(), // useSR
            ::testing::Bool(), // useMod
            ::testing::Bool() // useDC
        )
    );


    TEST(DftApiTest, AnalyzeMTTF) {
    }
}
