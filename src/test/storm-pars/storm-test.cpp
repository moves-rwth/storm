#include "storm-pars/settings/ParsSettings.h"
#include "test/storm_gtest.h"

int main(int argc, char **argv) {
    storm::settings::initializeParsSettings("Storm-pars (Functional) Testing Suite", "test-pars");
    ::testing::InitGoogleTest(&argc, argv);
    storm::test::initialize(&argc, argv);
    return RUN_ALL_TESTS();
}
