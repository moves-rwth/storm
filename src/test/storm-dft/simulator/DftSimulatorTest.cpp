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

    TEST(DftSimulatorTest, And) {
        double result = simulateDft(STORM_TEST_RESOURCES_DIR "/dft/and.dft", 2, 10000);
        EXPECT_NEAR(result, 0.3995764009, 0.01);
    }

}
