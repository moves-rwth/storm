#include "storm-config.h"
#include "test/storm_gtest.h"

#include "storm/utility/random.h"

namespace {

TEST(SamplingTest, SampleExponential) {
    storm::utility::RandomProbabilityGenerator<double> dist(5u);

    // Ensure that pseudo random numbers are the same on all machines
    double reference[] = {0.011352194735198223, 0.35595966617002761,  0.0904286229501646,  0.77693019063043178, 0.018822806624073801,
                          0.10108027454097809,  0.087433897364154994, 0.13335480534208866, 0.93816592681955202, 0.33034562021306652};
    for (int i = 0; i < 10; ++i) {
        EXPECT_NEAR(dist.randomExponential(5), reference[i], 1e-10);
    }
}

}  // namespace
