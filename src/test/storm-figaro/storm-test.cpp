#include "test/storm_gtest.h"
#include "storm-figaro/settings/FIGAROSettings.h"

int main(int argc, char **argv) {
    storm::settings::initializeFigaroSettings("Storm-figaro (Functional) Testing Suite", "test-figaro");
    storm::test::initialize();
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
