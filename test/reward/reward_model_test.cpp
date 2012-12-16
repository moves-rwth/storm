#include "gtest/gtest.h"

#include "Eigen/Sparse"
#include "src/exceptions/InvalidArgumentException.h"
#include "boost/integer/integer_mask.hpp"
#include <vector>

#include "reward/reward_model.h"

TEST(RewardModelTest, ReadWriteTest) {
	// 50 entries
	mrmc::reward::RewardModel<std::vector, double> rm(50, 0.0);
	
	double values[50];
	for (int i = 0; i < 50; ++i) {
		values[i] = 1.0 + i;
		ASSERT_TRUE(rm.setReward(i, values[i]));

		ASSERT_EQ(rm.getReward(i), values[i]);
	}

}

