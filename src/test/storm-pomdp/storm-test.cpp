#include "test/storm_gtest.h"
#include "storm/settings/SettingsManager.h"

int main(int argc, char **argv) {
  storm::settings::initializeAll("Storm-pomdp (Functional) Testing Suite", "test-pomdp");
  storm::test::initialize();
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
