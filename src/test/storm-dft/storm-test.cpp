#include "gtest/gtest.h"
#include "storm-dft/settings/DftSettings.h"

int main(int argc, char **argv) {
  storm::settings::initializeDftSettings("Storm-dft (Functional) Testing Suite", "test-dft");
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
