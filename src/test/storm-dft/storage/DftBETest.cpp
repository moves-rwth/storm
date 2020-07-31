#include "test/storm_gtest.h"
#include "storm-config.h"

#include "storm-dft/storage/dft/DFTElements.h"

namespace {

    TEST(DftBETest, FailureConstant) {
        storm::storage::BEConst<double> be(0, "Test", true);
        EXPECT_TRUE(be.failed());
        EXPECT_TRUE(be.canFail());
    
        EXPECT_EQ(1, be.getUnreliability(0));
        EXPECT_EQ(1, be.getUnreliability(10));
    }

    TEST(DftBETest, FailureExponential) {
        storm::storage::BEExponential<double> be(0, "Test", 3, 0.5);
            
        EXPECT_TRUE(be.canFail());
        EXPECT_EQ(1.5, be.passiveFailureRate());
    
        EXPECT_EQ(0, be.getUnreliability(0));
        EXPECT_FLOAT_EQ(0.7768698399, be.getUnreliability(0.5));
        EXPECT_FLOAT_EQ(0.9502129316, be.getUnreliability(1));
        EXPECT_FLOAT_EQ(0.9975212478, be.getUnreliability(2));
    }

    TEST(DftBETest, FailureSamples) {
        // Weibull distribution with shape 5 and scale 1
        std::map<double, double> samples = {
            {0.0, 0.0},
            {0.25, 0.0009760858180243304},
            {0.5, 0.03076676552365587},
            {0.75, 0.21124907114638225},
            {1.0, 0.6321205588285577},
            {1.25, 0.9527242505937095},
            {1.5, 0.9994964109502631},
            {1.75, 0.999999925546118},
            {2.0, 0.9999999999999873}
        };
        storm::storage::BESamples<double> be(0, "Weibull", samples);

        EXPECT_TRUE(be.canFail());

        EXPECT_EQ(0, be.getUnreliability(0));
        for (double i = 0; i <= 2.0; i += 0.25) {
            EXPECT_FLOAT_EQ(1-exp(-pow(i, 5)), be.getUnreliability(i));
        }
    }

}
