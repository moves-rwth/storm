#include "storm-dft/settings/DftSettings.h"
#include "test/storm_gtest.h"

int main(int argc, char **argv) {
    storm::settings::initializeDftSettings("Storm-dft (Functional) Testing Suite", "test-dft");
    storm::test::initialize();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
