#include "gtest/gtest.h"
#include "storm-pars/settings/ParsSettings.h"

int main(int argc, char **argv) {
  storm::settings::initializeParsSettings("Storm-pars (Functional) Testing Suite", "test-pars");
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
