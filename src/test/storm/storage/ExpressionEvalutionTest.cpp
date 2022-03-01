#include "storm/storage/expressions/Expression.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/storage/expressions/ExprtkExpressionEvaluator.h"
#include "storm/storage/expressions/SimpleValuation.h"
#include "test/storm_gtest.h"

TEST(ExpressionEvaluation, NaiveEvaluation) {
    std::shared_ptr<storm::expressions::ExpressionManager> manager(new storm::expressions::ExpressionManager());

    storm::expressions::Variable x;
    storm::expressions::Variable y;
    storm::expressions::Variable z;
    ASSERT_NO_THROW(x = manager->declareBooleanVariable("x"));
    ASSERT_NO_THROW(y = manager->declareIntegerVariable("y"));
    ASSERT_NO_THROW(z = manager->declareRationalVariable("z"));

    storm::expressions::SimpleValuation eval(manager);

    storm::expressions::Expression iteExpression = storm::expressions::ite(x, y + z, manager->integer(3) * z);

    eval.setRationalValue(z, 5.5);
    eval.setBooleanValue(x, true);
    for (int_fast64_t i = 0; i < 1000; ++i) {
        eval.setIntegerValue(y, 3 + i);
        EXPECT_NEAR(8.5 + i, iteExpression.evaluateAsDouble(&eval), 1e-6);
    }

    eval.setBooleanValue(x, false);
    for (int_fast64_t i = 0; i < 1000; ++i) {
        double zValue = i / static_cast<double>(10);
        eval.setRationalValue(z, zValue);
        EXPECT_NEAR(3 * zValue, iteExpression.evaluateAsDouble(&eval), 1e-6);
    }
}

TEST(ExpressionEvaluation, ExprTkEvaluation) {
    std::shared_ptr<storm::expressions::ExpressionManager> manager(new storm::expressions::ExpressionManager());

    storm::expressions::Variable x;
    storm::expressions::Variable y;
    storm::expressions::Variable z;
    ASSERT_NO_THROW(x = manager->declareBooleanVariable("x"));
    ASSERT_NO_THROW(y = manager->declareIntegerVariable("y"));
    ASSERT_NO_THROW(z = manager->declareRationalVariable("z"));

    storm::expressions::Expression iteExpression = storm::expressions::ite(x, y + z, manager->integer(3) * z);
    storm::expressions::ExprtkExpressionEvaluator eval(*manager);

    eval.setRationalValue(z, 5.5);
    eval.setBooleanValue(x, true);
    for (int_fast64_t i = 0; i < 1000; ++i) {
        eval.setIntegerValue(y, 3 + i);
        EXPECT_NEAR(8.5 + i, eval.asRational(iteExpression), 1e-6);
    }

    eval.setBooleanValue(x, false);
    for (int_fast64_t i = 0; i < 1000; ++i) {
        double zValue = i / static_cast<double>(10);
        eval.setRationalValue(z, zValue);
        EXPECT_NEAR(3 * zValue, eval.asRational(iteExpression), 1e-6);
    }
}
