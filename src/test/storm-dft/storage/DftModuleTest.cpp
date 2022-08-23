#include "storm-config.h"
#include "test/storm_gtest.h"

#include "storm-dft/api/storm-dft.h"
#include "storm-dft/storage/DftModule.h"
#include "storm-dft/utility/DftModularizer.h"

namespace {

TEST(DftModuleTest, Modularization) {
    std::string file = STORM_TEST_RESOURCES_DIR "/dft/all_gates.dft";
    std::shared_ptr<storm::dft::storage::DFT<double>> dft = storm::dft::api::loadDFTGalileoFile<double>(file);

    storm::dft::utility::DftModularizer<double> modularizer;
    auto listOfModules = modularizer.computeModules(*dft);
    EXPECT_EQ(listOfModules.size(), 5ul);
    std::vector<size_t> expectedRepresentatives({8, 17, 28, 36, 37});
    for (size_t i = 0; i < listOfModules.size(); ++i) {
        EXPECT_EQ(listOfModules[i].getRepresentative(), expectedRepresentatives[i]);
    }
}

TEST(DftModuleTest, ModularizationCycle) {
    std::string file = STORM_TEST_RESOURCES_DIR "/dft/fdep_cycle.dft";
    std::shared_ptr<storm::dft::storage::DFT<double>> dft = storm::dft::api::loadDFTGalileoFile<double>(file);

    storm::dft::utility::DftModularizer<double> modularizer;
    auto listOfModules = modularizer.computeModules(*dft);
    EXPECT_EQ(listOfModules.size(), 1ul);
    EXPECT_EQ(listOfModules[0].getRepresentative(), 2ul);
}

}  // namespace
