#include "test/storm_gtest.h"
#include "storm-config.h"

#include "storm-dft/api/storm-dft.h"
#include "storm-dft/transformations/DftTransformator.h"
#include "storm-dft/generator/DftNextStateGenerator.h"
#include "storm-dft/simulator/DFTTraceSimulator.h"
#include "storm-dft/storage/dft/SymmetricUnits.h"


namespace {

    // Helper function
    double simulateDft(std::string const& file, double timebound, size_t noRuns) {
        // Load, build and prepare DFT
        storm::transformations::dft::DftTransformator<double> dftTransformator = storm::transformations::dft::DftTransformator<double>();
        std::shared_ptr<storm::storage::DFT<double>> dft = dftTransformator.transformBinaryFDEPs(*(storm::api::loadDFTGalileoFile<double>(file)));
        EXPECT_TRUE(storm::api::isWellFormed(*dft).first);

        // Set relevant events
        storm::utility::RelevantEvents relevantEvents = storm::api::computeRelevantEvents<double>(*dft, {}, {}, false);
        dft->setRelevantEvents(relevantEvents);

        // Find symmetries
        std::map<size_t, std::vector<std::vector<size_t>>> emptySymmetry;
        storm::storage::DFTIndependentSymmetries symmetries(emptySymmetry);
        storm::storage::DFTStateGenerationInfo stateGenerationInfo(dft->buildStateGenerationInfo(symmetries));
        
        // Init random number generator
        boost::mt19937 gen(5u);
        storm::dft::simulator::DFTTraceSimulator<double> simulator(*dft, stateGenerationInfo, gen);
        
        size_t count = 0;;
        bool res;
        for (size_t i=0; i<noRuns; ++i) {
            res = simulator.simulateCompleteTrace(timebound);
            if (res) {
                ++count;
            }
        }
        return (double) count / noRuns;
    }

    TEST(DftSimulatorTest, AndUnreliability) {
        double result = simulateDft(STORM_TEST_RESOURCES_DIR "/dft/and.dft", 2, 10000);
        EXPECT_NEAR(result, 0.3995764009, 0.01);
    }

    TEST(DftSimulatorTest, OrUnreliability) {
        double result = simulateDft(STORM_TEST_RESOURCES_DIR "/dft/or.dft", 1, 10000);
        EXPECT_NEAR(result, 0.6321205588, 0.01);
    }

    TEST(DftSimulatorTest, VotingUnreliability) {
        double result = simulateDft(STORM_TEST_RESOURCES_DIR "/dft/voting.dft", 1, 10000);
        EXPECT_NEAR(result, 0.4511883639, 0.01);
        result = simulateDft(STORM_TEST_RESOURCES_DIR "/dft/voting2.dft", 1, 10000);
        EXPECT_NEAR(result, 0.8173164759, 0.01);
        result = simulateDft(STORM_TEST_RESOURCES_DIR "/dft/voting3.dft", 1, 10000);
        EXPECT_NEAR(result, 0.3496529873, 0.01);
        result = simulateDft(STORM_TEST_RESOURCES_DIR "/dft/voting4.dft", 1, 10000);
        EXPECT_NEAR(result, 0.693568287, 0.01);
    }

    TEST(DftSimulatorTest, PandUnreliability) {
        double result = simulateDft(STORM_TEST_RESOURCES_DIR "/dft/pand.dft", 1, 10000);
        EXPECT_NEAR(result, 0.03087312562, 0.01);
    }

    TEST(DftSimulatorTest, PorUnreliability) {
        double result = simulateDft(STORM_TEST_RESOURCES_DIR "/dft/por.dft", 1, 10000);
        EXPECT_NEAR(result, 0.2753355179, 0.01);
    }

    TEST(DftSimulatorTest, FdepUnreliability) {
        double result = simulateDft(STORM_TEST_RESOURCES_DIR "/dft/fdep.dft", 1, 10000);
        EXPECT_NEAR(result, 0.7768698399, 0.01);
        result = simulateDft(STORM_TEST_RESOURCES_DIR "/dft/fdep2.dft", 1, 10000);
        EXPECT_NEAR(result, 0.3934693403, 0.01);
        result = simulateDft(STORM_TEST_RESOURCES_DIR "/dft/fdep3.dft", 1, 10000);
        EXPECT_NEAR(result, 0.329679954, 0.01);
        result = simulateDft(STORM_TEST_RESOURCES_DIR "/dft/fdep4.dft", 1, 10000);
        EXPECT_NEAR(result, 0.601280086, 0.01);
        result = simulateDft(STORM_TEST_RESOURCES_DIR "/dft/fdep5.dft", 1, 10000);
        EXPECT_NEAR(result, 0.1548181217, 0.01);
        result = simulateDft(STORM_TEST_RESOURCES_DIR "/dft/fdep6.dft", 1, 10000);
        EXPECT_NEAR(result, 0.9985116987, 0.01);
    }

    TEST(DftSimulatorTest, PdepUnreliability) {
        double result = simulateDft(STORM_TEST_RESOURCES_DIR "/dft/pdep.dft", 1, 10000);
        EXPECT_NEAR(result, 0.2017690905, 0.01);
        result = simulateDft(STORM_TEST_RESOURCES_DIR "/dft/pdep2.dft", 1, 10000);
        EXPECT_NEAR(result, 0.2401091405, 0.01);
        result = simulateDft(STORM_TEST_RESOURCES_DIR "/dft/pdep3.dft", 1, 10000);
        EXPECT_NEAR(result, 0.2259856274, 0.01);
        // Examle pdep4 contains non-determinism which is not handled in simulation
        //result = simulateDft(STORM_TEST_RESOURCES_DIR "/dft/pdep4.dft", 1, 10000);
        //EXPECT_NEAR(result, 0.008122157897, 0.01);
    }

    TEST(DftSimulatorTest, SpareUnreliability) {
        double result = simulateDft(STORM_TEST_RESOURCES_DIR "/dft/spare.dft", 1, 10000);
        EXPECT_NEAR(result, 0.1118530638, 0.01);
        result = simulateDft(STORM_TEST_RESOURCES_DIR "/dft/spare2.dft", 1, 10000);
        EXPECT_NEAR(result, 0.2905027469, 0.01);
        result = simulateDft(STORM_TEST_RESOURCES_DIR "/dft/spare3.dft", 1, 10000);
        EXPECT_NEAR(result, 0.4660673246, 0.01);
        result = simulateDft(STORM_TEST_RESOURCES_DIR "/dft/spare4.dft", 1, 10000);
        EXPECT_NEAR(result, 0.01273070783, 0.01);
        result = simulateDft(STORM_TEST_RESOURCES_DIR "/dft/spare5.dft", 1, 10000);
        EXPECT_NEAR(result, 0.2017690905, 0.01);
        result = simulateDft(STORM_TEST_RESOURCES_DIR "/dft/spare6.dft", 1, 10000);
        EXPECT_NEAR(result, 0.4693712702, 0.01);
        result = simulateDft(STORM_TEST_RESOURCES_DIR "/dft/spare7.dft", 1, 10000);
        EXPECT_NEAR(result, 0.06108774525, 0.01);
        result = simulateDft(STORM_TEST_RESOURCES_DIR "/dft/spare8.dft", 1, 10000);
        EXPECT_NEAR(result, 0.02686144489, 0.01);
    }

    TEST(DftSimulatorTest, SymmetryUnreliability) {
        double result = simulateDft(STORM_TEST_RESOURCES_DIR "/dft/symmetry6.dft", 1, 10000);
        EXPECT_NEAR(result, 0.3421934224, 0.01);
    }

    TEST(DftSimulatorTest, HecsUnreliability) {
        double result = simulateDft(STORM_TEST_RESOURCES_DIR "/dft/hecs_2_2.dft", 1, 10000);
        EXPECT_NEAR(result, 0.00021997582, 0.001);
    }

}
