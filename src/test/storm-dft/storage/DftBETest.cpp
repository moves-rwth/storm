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

}
