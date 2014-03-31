#include "gtest/gtest.h"

#include <map>

#include "src/storage/expressions/SimpleValuation.h"

TEST(Expression, SimpleValuationTest) {
    ASSERT_NO_THROW(storm::expressions::SimpleValuation evaluation(1, 1, 1));
}