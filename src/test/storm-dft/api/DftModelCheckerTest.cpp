#include "gtest/gtest.h"
#include "storm-config.h"

#include "storm-dft/api/storm-dft.h"

namespace {

    // Configurations for DFT analysis
    struct DftAnalysisConfig {
        bool useSR;
        bool useMod;
        bool useDC;
    };

    class NoOptimizationsConfig {
    public:
        typedef double ValueType;
        static DftAnalysisConfig createConfig() {
            return DftAnalysisConfig {false, false, false};
        }
    };
    class DontCareConfig {
    public:
        typedef double ValueType;
        static DftAnalysisConfig createConfig() {
            return DftAnalysisConfig {false, false, true};
        }
    };
    class ModularisationConfig {
    public:
        typedef double ValueType;
        static DftAnalysisConfig createConfig() {
            return DftAnalysisConfig {false, true, false};
        }
    };
    class SymmetryReductionConfig {
    public:
        typedef double ValueType;
        static DftAnalysisConfig createConfig() {
            return DftAnalysisConfig {true, false, false};
        }
    };
    class AllOptimizationsConfig {
    public:
        typedef double ValueType;
        static DftAnalysisConfig createConfig() {
            return DftAnalysisConfig {true, true, true};
        }
    };

    // General base class for testing of DFT model checking
    template<typename TestType>
    class DftModelCheckerTest : public ::testing::Test {
    public:
        typedef typename TestType::ValueType ValueType;

        DftModelCheckerTest() : config(TestType::createConfig()) {
        }

        DftAnalysisConfig const& getConfig() const {
            return config;
        }

        double analyzeMTTF(std::string const& file) {
            std::shared_ptr<storm::storage::DFT<double>> dft = storm::api::loadDFTGalileo<double>(file);
            std::string property = "Tmin=? [F \"failed\"]";
            std::vector<std::shared_ptr<storm::logic::Formula const>> properties = storm::api::extractFormulasFromProperties(storm::api::parseProperties(property));
            typename storm::modelchecker::DFTModelChecker<double>::dft_results results = storm::api::analyzeDFT<double>(*dft, properties, config.useSR, config.useMod, config.useDC);
            return boost::get<double>(results[0]);
        }

    private:
        DftAnalysisConfig config;
    };

    typedef ::testing::Types<
            NoOptimizationsConfig,
            DontCareConfig,
            ModularisationConfig,
            SymmetryReductionConfig,
            AllOptimizationsConfig
        > TestingTypes;

    TYPED_TEST_CASE(DftModelCheckerTest, TestingTypes);

    TYPED_TEST(DftModelCheckerTest, AndMTTF) {
        double result = this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/and.dft");
        EXPECT_FLOAT_EQ(result, 3.0);
    }

}
