#include "gtest/gtest.h"
#include "storm/settings/SettingsManager.h"

int main(int argc, char **argv) {
  storm::settings::initializeAll("Storm-pars (Functional) Testing Suite", "test-pars");
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
