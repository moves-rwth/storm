#include "test/storm_gtest.h"
#include "storm/settings/SettingsManager.h"


int main(int argc, char **argv) {
    storm::settings::initializeAll("Storm-permissive (Functional) Testing Suite", "test-permissive");
    storm::test::initialize();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
