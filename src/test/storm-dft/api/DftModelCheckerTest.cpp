#include "test/storm_gtest.h"
#include "storm-config.h"

#include "storm-dft/api/storm-dft.h"
#include "storm-dft/transformations/DftTransformator.h"
#include "storm-parsers/api/storm-parsers.h"

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
            return DftAnalysisConfig{false, false, false};
        }
    };

    class DontCareConfig {
    public:
        typedef double ValueType;

        static DftAnalysisConfig createConfig() {
            return DftAnalysisConfig{false, false, true};
        }
    };

    class ModularisationConfig {
    public:
        typedef double ValueType;

        static DftAnalysisConfig createConfig() {
            return DftAnalysisConfig{false, true, false};
        }
    };

    class SymmetryReductionConfig {
    public:
        typedef double ValueType;

        static DftAnalysisConfig createConfig() {
            return DftAnalysisConfig{true, false, false};
        }
    };

    class AllOptimizationsConfig {
    public:
        typedef double ValueType;

        static DftAnalysisConfig createConfig() {
            return DftAnalysisConfig{true, true, true};
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

        double analyze(std::string const& file, std::string const& property) {
            // Load, build and prepare DFT
            storm::transformations::dft::DftTransformator<double> dftTransformator = storm::transformations::dft::DftTransformator<double>();
            std::shared_ptr<storm::storage::DFT<double>> dft = dftTransformator.transformBinaryFDEPs(*(storm::api::loadDFTGalileoFile<double>(file)));
            EXPECT_TRUE(storm::api::isWellFormed(*dft).first);

            // Create property
            std::vector<std::shared_ptr<storm::logic::Formula const>> properties = storm::api::extractFormulasFromProperties(storm::api::parseProperties(property));

            // Create relevant names
            std::vector<std::string> relevantNames;
            if (!config.useDC) {
                relevantNames.push_back("all");
            }
            storm::utility::RelevantEvents relevantEvents = storm::api::computeRelevantEvents<ValueType>(*dft, properties, relevantNames);

            // Perform model checking
            typename storm::modelchecker::DFTModelChecker<double>::dft_results results = storm::api::analyzeDFT<double>(*dft, properties, config.useSR, config.useMod, relevantEvents, false);
            return boost::get<double>(results[0]);
        }

        double analyzeMTTF(std::string const& file) {
            std::string property = "Tmin=? [F \"failed\"]";
            return analyze(file, property);
        }

        double analyzeReliability(std::string const &file, double bound) {
            std::string property = "Pmin=? [F<=" + std::to_string(bound) + " \"failed\"]";
            return analyze(file, property);
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

    TYPED_TEST_SUITE(DftModelCheckerTest, TestingTypes,);

    TYPED_TEST(DftModelCheckerTest, AndMTTF) {
        double result = this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/and.dft");
        EXPECT_FLOAT_EQ(result, 3);
    }

    TYPED_TEST(DftModelCheckerTest, OrMTTF) {
        double result = this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/or.dft");
        EXPECT_FLOAT_EQ(result, 1);
    }

    TYPED_TEST(DftModelCheckerTest, VotingMTTF) {
        double result = this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/voting.dft");
        EXPECT_FLOAT_EQ(result, 5 / 3.0);
        result = this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/voting2.dft");
        EXPECT_FLOAT_EQ(result, 10 / 17.0);
        result = this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/voting3.dft");
        EXPECT_FLOAT_EQ(result, 2685 / 1547.0);
        result = this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/voting4.dft");
        EXPECT_FLOAT_EQ(result, 5 / 6.0);
    }

    TYPED_TEST(DftModelCheckerTest, PandMTTF) {
        double result = this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/pand.dft");
        EXPECT_EQ(result, storm::utility::infinity<double>());
    }

    TYPED_TEST(DftModelCheckerTest, PorMTTF) {
        double result = this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/por.dft");
        EXPECT_EQ(result, storm::utility::infinity<double>());
    }

    TYPED_TEST(DftModelCheckerTest, FdepMTTF) {
        double result = this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/fdep2.dft");
        EXPECT_FLOAT_EQ(result, 2);
        result = this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/fdep3.dft");
        EXPECT_FLOAT_EQ(result, 5 / 2.0);

        if (this->getConfig().useMod) {
            STORM_SILENT_EXPECT_THROW(this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/fdep.dft"), storm::exceptions::NotSupportedException);
            STORM_SILENT_EXPECT_THROW(this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/fdep4.dft"), storm::exceptions::NotSupportedException);
            STORM_SILENT_EXPECT_THROW(this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/fdep5.dft"), storm::exceptions::NotSupportedException);
        } else {
            double result = this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/fdep.dft");
            EXPECT_FLOAT_EQ(result, 2 / 3.0);
            result = this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/fdep4.dft");
            EXPECT_FLOAT_EQ(result, 1);
            result = this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/fdep5.dft");
            EXPECT_FLOAT_EQ(result, 3);
        }
    }

    TYPED_TEST(DftModelCheckerTest, PdepMTTF) {
        double result = this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/pdep.dft");
        EXPECT_FLOAT_EQ(result, 8 / 3.0);
        if (this->getConfig().useMod && !this->getConfig().useDC) {
            STORM_SILENT_EXPECT_THROW(this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/pdep2.dft"), storm::exceptions::NotSupportedException);
        } else {
            result = this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/pdep2.dft");
            EXPECT_FLOAT_EQ(result, 38 / 15.0);
        }
        result = this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/pdep3.dft");
        EXPECT_FLOAT_EQ(result, 67 / 24.0);

        if (this->getConfig().useMod) {
            STORM_SILENT_EXPECT_THROW(this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/pdep4.dft"), storm::exceptions::NotSupportedException);
        } else {
            result = this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/pdep4.dft");
            EXPECT_EQ(result, storm::utility::infinity<double>());
        }
    }

    TYPED_TEST(DftModelCheckerTest, SpareMTTF) {
        double result = this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/spare.dft");
        EXPECT_FLOAT_EQ(result, 46 / 13.0);
        result = this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/spare2.dft");
        EXPECT_FLOAT_EQ(result, 43 / 23.0);
        result = this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/spare3.dft");
        EXPECT_FLOAT_EQ(result, 14 / 11.0);
        result = this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/spare4.dft");
        EXPECT_FLOAT_EQ(result, 18836 / 3887.0);
        result = this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/spare5.dft");
        EXPECT_FLOAT_EQ(result, 8 / 3.0);
        result = this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/spare6.dft");
        EXPECT_FLOAT_EQ(result, 7 / 5.0);
        result = this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/spare7.dft");
        EXPECT_FLOAT_EQ(result, 551 / 150.0);
        result = this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/spare8.dft");
        EXPECT_FLOAT_EQ(result, 249 / 52.0); // DFTCalc has result of 4.33779 due to different semantics of nested spares
        result = this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/spare_dc.dft");
        EXPECT_FLOAT_EQ(result, 78311 / 182700.0);
    }

    TYPED_TEST(DftModelCheckerTest, SeqMTTF) {
        double result = this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/seq.dft");
        EXPECT_FLOAT_EQ(result, 4);
        result = this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/seq2.dft");
        EXPECT_FLOAT_EQ(result, 6);
        result = this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/seq3.dft");
        EXPECT_FLOAT_EQ(result, 6);
        result = this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/seq4.dft");
        EXPECT_FLOAT_EQ(result, 6);
        result = this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/seq5.dft");
        EXPECT_EQ(result, storm::utility::infinity<double>());

        result = this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/mutex.dft");
        EXPECT_FLOAT_EQ(result, 1 / 2.0);
        result = this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/mutex2.dft");
        EXPECT_FLOAT_EQ(result, storm::utility::infinity<double>());
        result = this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/mutex3.dft");
        EXPECT_FLOAT_EQ(result, storm::utility::infinity<double>());
        result = this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/seq6.dft");
        EXPECT_FLOAT_EQ(result, 30000);
    }

    TYPED_TEST(DftModelCheckerTest, Symmetry) {
        double result = this->analyzeMTTF(STORM_TEST_RESOURCES_DIR "/dft/symmetry6.dft");
        EXPECT_FLOAT_EQ(result, 2804183 / 2042040.0);
        result = this->analyzeReliability(STORM_TEST_RESOURCES_DIR "/dft/symmetry6.dft", 1.0);
        EXPECT_FLOAT_EQ(result, 0.3421934224);
    }

    TYPED_TEST(DftModelCheckerTest, HecsReliability) {
        if (!this->getConfig().useDC) {
            // Skip configurations because it takes too long
            GTEST_SKIP();
            return;
        }
        double result = this->analyzeReliability(STORM_TEST_RESOURCES_DIR "/dft/hecs_2_2.dft", 1.0);
        EXPECT_FLOAT_EQ(result, 0.00021997582);
    }
}
