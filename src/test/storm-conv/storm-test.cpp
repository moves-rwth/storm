#include "test/storm_gtest.h"
#include "storm-conv/settings/ConvSettings.h"

int main(int argc, char **argv) {
  storm::settings::initializeConvSettings("Storm-conv Testing Suite", "test-conv");
  storm::test::initialize();
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
