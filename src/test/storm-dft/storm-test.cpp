#include "storm-dft/settings/DftSettings.h"
#include "test/storm_gtest.h"

int main(int argc, char **argv) {
    storm::dft::settings::initializeDftSettings("Storm-dft (Functional) Testing Suite", "test-dft");
    ::testing::InitGoogleTest(&argc, argv);
    storm::test::initialize(&argc, argv);
    return RUN_ALL_TESTS();
}
