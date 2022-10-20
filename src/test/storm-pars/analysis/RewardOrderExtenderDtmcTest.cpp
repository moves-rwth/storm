#include <storage/StronglyConnectedComponentDecomposition.h>
#include "storm-config.h"

#include "storm-pars/analysis/Order.h"
#include "storm-pars/analysis/RewardOrderExtender.h"
#include "storm-pars/api/storm-pars.h"

#include "storm-parsers/api/storm-parsers.h"
#include "storm-parsers/parser/AutoParser.h"
#include "storm-parsers/parser/PrismParser.h"

#include "storm/api/builder.h"
#include "storm/api/storm.h"
#include "storm/logic/Formulas.h"
#include "storm/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "storm/models/sparse/StandardRewardModel.h"

#include "test/storm_gtest.h"
namespace {
class RewardOrderExtenderTester : public storm::analysis::RewardOrderExtender<storm::RationalFunction, double> {
   public:
    RewardOrderExtenderTester(std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> model,
                              std::shared_ptr<storm::logic::Formula const> formula)
        : storm::analysis::RewardOrderExtender<storm::RationalFunction, double>(model, formula) {
        // Intentionally left empty
    }

    FRIEND_TEST(RewardOrderExtenderTest, RewardTest1);
    FRIEND_TEST(RewardOrderExtenderTest, RewardTest2);
    FRIEND_TEST(RewardOrderExtenderTest, RewardTest3);
    FRIEND_TEST(RewardOrderExtenderTest, RewardTest4);
};

TEST(RewardOrderExtenderTest, RewardTest1) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/rewardTest01.pm";
    std::string formulaAsString = "Rmax=? [F s=6 | s=7]";

    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, "");
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas =
        storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model =
        storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();

    ASSERT_EQ(model->getNumberOfStates(), 8);
    ASSERT_EQ(model->getNumberOfTransitions(), 14);

    auto vars = storm::models::sparse::getProbabilityParameters(*model);
    auto region = storm::api::parseRegion<storm::RationalFunction>("0.1 <= p <= 0.9", vars);

    // Extender
    auto extender = RewardOrderExtenderTester(model, formulas[0]);
    auto order = extender.getInitialOrder(region);

    // Note: Due to Storm's parsing, state 6 is s5 in the model and state 5 is s6 in the model.
    // Everything else is the same
    ASSERT_EQ(order->compare(5, 7), storm::analysis::Order::NodeComparison::SAME);

    extender.initializeMinMaxValues(region, order);

    // s5 check
    extender.extendByBackwardReasoning(order, region, 6);

    // 5 & 7 are equal (both reward 0) so state 6 should be above them
    EXPECT_EQ(order->compare(6, 5), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(6, 7), storm::analysis::Order::NodeComparison::ABOVE);

    // s4 check
    extender.extendByBackwardReasoning(order, region, 4);
    EXPECT_EQ(order->compare(4, 5), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(4, 6), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(4, 7), storm::analysis::Order::NodeComparison::ABOVE);

    // s3 check
    extender.extendByBackwardReasoning(order, region, 3);
    EXPECT_EQ(order->compare(3, 5), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(3, 6), storm::analysis::Order::NodeComparison::UNKNOWN);
    EXPECT_EQ(order->compare(3, 7), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(3, 4), storm::analysis::Order::NodeComparison::UNKNOWN);

    // s2 check
    extender.extendByBackwardReasoning(order, region, 2);
    EXPECT_EQ(order->compare(3, 4), storm::analysis::Order::NodeComparison::BELOW);
    EXPECT_EQ(order->compare(2, 3), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(2, 4), storm::analysis::Order::NodeComparison::UNKNOWN);
    EXPECT_EQ(order->compare(2, 5), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(2, 6), storm::analysis::Order::NodeComparison::UNKNOWN);
    EXPECT_EQ(order->compare(2, 7), storm::analysis::Order::NodeComparison::ABOVE);

    // s1 check
    extender.extendByBackwardReasoning(order, region, 1);
    EXPECT_EQ(order->compare(1, 2), storm::analysis::Order::NodeComparison::UNKNOWN);
    EXPECT_EQ(order->compare(1, 3), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(1, 4), storm::analysis::Order::NodeComparison::UNKNOWN);
    EXPECT_EQ(order->compare(1, 5), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(1, 6), storm::analysis::Order::NodeComparison::UNKNOWN);
    EXPECT_EQ(order->compare(1, 7), storm::analysis::Order::NodeComparison::ABOVE);

    // s1 and s2 cannot be ordered so s0 cannot be added to the order
    auto res = extender.extendByBackwardReasoning(order, region, 0);

    EXPECT_TRUE(res.first != res.second);
    EXPECT_TRUE(res.first == 1 || res.first == 2);
    EXPECT_TRUE(res.second == 1 || res.second == 2);
}

TEST(RewardOrderExtenderTest, RewardTest2) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/rewardTest02.pm";
    std::string formulaAsString = "Rmax=? [F s=6 | s=7]";

    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, "");
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas =
        storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model =
        storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();

    ASSERT_EQ(model->getNumberOfStates(), 8);
    ASSERT_EQ(model->getNumberOfTransitions(), 14);

    auto vars = storm::models::sparse::getProbabilityParameters(*model);
    auto region = storm::api::parseRegion<storm::RationalFunction>("0.1 <= p <= 0.9", vars);

    // Extender
    auto extender = RewardOrderExtenderTester(model, formulas[0]);
    auto order = extender.getInitialOrder(region);
    ASSERT_EQ(order->compare(5, 7), storm::analysis::Order::NodeComparison::SAME);

    extender.initializeMinMaxValues(region, order);

    // Note: Due to Storm's parsing, state 6 is s5 in the model and state 5 is s6 in the model.
    // Everything else is the same

    // s5 check
    extender.extendByBackwardReasoning(order, region, 6);

    // 5 & 7 are equal (both reward 0) so state 6 should be above them
    EXPECT_EQ(order->compare(6, 5), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(6, 7), storm::analysis::Order::NodeComparison::ABOVE);

    // s4 check
    extender.extendByBackwardReasoning(order, region, 4);
    EXPECT_EQ(order->compare(4, 5), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(4, 6), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(4, 7), storm::analysis::Order::NodeComparison::ABOVE);

    // s3 check
    extender.extendByBackwardReasoning(order, region, 3);
    EXPECT_EQ(order->compare(3, 5), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(3, 6), storm::analysis::Order::NodeComparison::UNKNOWN);
    EXPECT_EQ(order->compare(3, 7), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(3, 4), storm::analysis::Order::NodeComparison::UNKNOWN);

    // s2 check
    extender.extendByBackwardReasoning(order, region, 2);
    EXPECT_EQ(order->compare(3, 4), storm::analysis::Order::NodeComparison::BELOW);
    EXPECT_EQ(order->compare(2, 3), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(2, 4), storm::analysis::Order::NodeComparison::UNKNOWN);
    EXPECT_EQ(order->compare(2, 5), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(2, 6), storm::analysis::Order::NodeComparison::UNKNOWN);
    EXPECT_EQ(order->compare(2, 7), storm::analysis::Order::NodeComparison::ABOVE);

    // s1 check
    extender.extendByBackwardReasoning(order, region, 1);
    EXPECT_EQ(order->compare(1, 2), storm::analysis::Order::NodeComparison::BELOW);
    EXPECT_EQ(order->compare(1, 3), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(1, 4), storm::analysis::Order::NodeComparison::UNKNOWN);
    EXPECT_EQ(order->compare(1, 5), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(1, 6), storm::analysis::Order::NodeComparison::UNKNOWN);
    EXPECT_EQ(order->compare(1, 7), storm::analysis::Order::NodeComparison::ABOVE);
}

TEST(RewardOrderExtenderTest, RewardTest3) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/rewardTest03.pm";
    std::string formulaAsString = "Rmax=? [F s=6]";

    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, "");
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas =
        storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model =
        storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();

    ASSERT_EQ(model->getNumberOfStates(), 7);
    ASSERT_EQ(model->getNumberOfTransitions(), 10);

    auto vars = storm::models::sparse::getProbabilityParameters(*model);
    auto region = storm::api::parseRegion<storm::RationalFunction>("0.1 <= p <= 0.9", vars);

    // Extender
    auto extender = RewardOrderExtenderTester(model, formulas[0]);
    auto order = extender.getInitialOrder(region);
    extender.initializeMinMaxValues(region, order);
    extender.extendOrder(order, region);

    EXPECT_EQ(order->compare(0, 1), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(0, 2), storm::analysis::Order::NodeComparison::BELOW);
    EXPECT_EQ(order->compare(0, 3), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(0, 4), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(0, 5), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(0, 6), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(1, 2), storm::analysis::Order::NodeComparison::BELOW);
    EXPECT_EQ(order->compare(1, 3), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(1, 4), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(1, 5), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(1, 6), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(2, 3), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(2, 4), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(2, 5), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(2, 6), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(3, 4), storm::analysis::Order::NodeComparison::BELOW);
    EXPECT_EQ(order->compare(3, 5), storm::analysis::Order::NodeComparison::UNKNOWN);
    EXPECT_EQ(order->compare(3, 6), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(4, 5), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(4, 6), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(5, 6), storm::analysis::Order::NodeComparison::ABOVE);
}

TEST(RewardOrderExtenderTest, RewardTest4) {
    std::string programFile = STORM_TEST_RESOURCES_DIR "/pdtmc/rewardTest04.pm";
    std::string formulaAsString = "Rmax=? [F s=6]";

    storm::prism::Program program = storm::api::parseProgram(programFile);
    program = storm::utility::prism::preprocess(program, "");
    std::vector<std::shared_ptr<const storm::logic::Formula>> formulas =
        storm::api::extractFormulasFromProperties(storm::api::parsePropertiesForPrismProgram(formulaAsString, program));
    std::shared_ptr<storm::models::sparse::Dtmc<storm::RationalFunction>> model =
        storm::api::buildSparseModel<storm::RationalFunction>(program, formulas)->as<storm::models::sparse::Dtmc<storm::RationalFunction>>();

    ASSERT_EQ(model->getNumberOfStates(), 7);
    ASSERT_EQ(model->getNumberOfTransitions(), 11);

    auto vars = storm::models::sparse::getProbabilityParameters(*model);
    auto region = storm::api::parseRegion<storm::RationalFunction>("0.1 <= p <= 0.9", vars);

    // Extender
    auto extender = RewardOrderExtenderTester(model, formulas[0]);
    auto order = extender.getInitialOrder(region);
    extender.initializeMinMaxValues(region, order);
    extender.extendOrder(order, region);

    EXPECT_EQ(order->compare(0, 1), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(0, 2), storm::analysis::Order::NodeComparison::BELOW);
    EXPECT_EQ(order->compare(0, 3), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(0, 4), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(0, 5), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(0, 6), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(1, 2), storm::analysis::Order::NodeComparison::BELOW);
    EXPECT_EQ(order->compare(1, 3), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(1, 4), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(1, 5), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(1, 6), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(2, 3), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(2, 4), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(2, 5), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(2, 6), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(3, 4), storm::analysis::Order::NodeComparison::BELOW);
    EXPECT_EQ(order->compare(3, 5), storm::analysis::Order::NodeComparison::SAME);
    EXPECT_EQ(order->compare(3, 6), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(4, 5), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(4, 6), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(5, 6), storm::analysis::Order::NodeComparison::ABOVE);
}
}  // namespace