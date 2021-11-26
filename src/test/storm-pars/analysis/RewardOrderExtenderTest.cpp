#include <storage/StronglyConnectedComponentDecomposition.h>
#include "storm-config.h"

#include "storm-pars/api/storm-pars.h"
#include "storm-pars/analysis/RewardOrderExtenderDtmc.h"
#include "storm-pars/analysis/Order.h"

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
class RewardOrderExtenderDtmcTester : public storm::analysis::RewardOrderExtenderDtmc<storm::RationalFunction, double> {
   public:
    RewardOrderExtenderDtmcTester(std::shared_ptr<storm::models::sparse::Model<storm::RationalFunction>> model,
                                  std::shared_ptr<storm::logic::Formula const> formula)
        : storm::analysis::RewardOrderExtenderDtmc<storm::RationalFunction, double>(model, formula) {
        // Intentionally left empty}
    }

    FRIEND_TEST(RewardOrderExtenderTest, smt_state_succ_compare);
};

TEST(RewardOrderExtenderTest, smt_state_succ_compare) {
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
    auto extender = RewardOrderExtenderDtmcTester(model, formulas[0]);
    auto order = extender.getInitialOrder();
    ASSERT_EQ(order->compare(5, 7), storm::analysis::Order::NodeComparison::SAME);

    extender.initializeMinMaxValues(region);

    // Note: Due to Storm's parsing, state 6 is s5 in the model and state 5 is s6 in the model.
    // Everything else is the same

    //    // Check if Min and Max values are right
    //    auto minValues = extender.getMinValues(order);
    //    auto maxValues = extender.getMaxValues(order);
    //
    //    ASSERT_EQ(minValues[0], 6.1);
    //    ASSERT_EQ(minValues[1], 5);
    //    ASSERT_EQ(minValues[2], 6);
    //    ASSERT_EQ(minValues[3], 3.5);
    //    ASSERT_EQ(minValues[4], 8.5);
    //    ASSERT_EQ(minValues[5], 0);
    //    ASSERT_EQ(minValues[6], 5);
    //    ASSERT_EQ(minValues[7], 0);
    //
    //    // Cast to float bc they are not "equal" as doubles
    //    ASSERT_EQ((float)maxValues[0], (float)14.9);
    //    ASSERT_EQ(maxValues[1], 13);
    //    ASSERT_EQ(maxValues[2], 14);
    //    ASSERT_EQ(maxValues[3], 7.5);
    //    ASSERT_EQ(maxValues[4], 12.5);
    //    ASSERT_EQ(maxValues[5], 0);
    //    ASSERT_EQ(maxValues[6], 5);
    //    ASSERT_EQ(maxValues[7], 0);
    //
    //    // s5 check
    //    std::vector<uint_fast64_t> succs6 = std::vector<uint_fast64_t>();
    //    succs6.push_back(5);
    //    succs6.push_back(7);
    extender.extendByBackwardReasoning(order, region, 6);

    // 5 & 7 are equal (both reward 0) so state 6 should be above them
    EXPECT_EQ(order->compare(6, 5), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(6, 7), storm::analysis::Order::NodeComparison::ABOVE);

    // s4 check
    //    std::vector<uint_fast64_t> succs4 = std::vector<uint_fast64_t>();
    //    succs4.push_back(6);
    //    succs4.push_back(7);
    extender.extendByBackwardReasoning(order, region, 4);
    EXPECT_EQ(order->compare(4, 5), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(4, 6), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(4, 7), storm::analysis::Order::NodeComparison::ABOVE);

    // s3 check
    //    std::vector<uint_fast64_t> succs3 = std::vector<uint_fast64_t>();
    //    succs3.push_back(5);
    //    succs3.push_back(6);
    extender.extendByBackwardReasoning(order, region, 3);

    EXPECT_EQ(order->compare(3, 5), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(3, 6), storm::analysis::Order::NodeComparison::UNKNOWN);
    EXPECT_EQ(order->compare(3, 7), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(3, 4), storm::analysis::Order::NodeComparison::UNKNOWN);

    // s2 check
    //    std::vector<uint_fast64_t> succs2 = std::vector<uint_fast64_t>();
    //    succs2.push_back(3);
    //    succs2.push_back(4);
    extender.extendByBackwardReasoning(order, region, 2);
    EXPECT_EQ(order->compare(3, 4), storm::analysis::Order::NodeComparison::BELOW);
    EXPECT_EQ(order->compare(2, 3), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(2, 4), storm::analysis::Order::NodeComparison::UNKNOWN);
    EXPECT_EQ(order->compare(2, 5), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(2, 6), storm::analysis::Order::NodeComparison::UNKNOWN);
    EXPECT_EQ(order->compare(2, 7), storm::analysis::Order::NodeComparison::ABOVE);

    // s1 check
    //    std::vector<uint_fast64_t> succs1 = std::vector<uint_fast64_t>();
    //    succs1.push_back(3);
    //    succs1.push_back(4);
    extender.extendByBackwardReasoning(order, region, 1);
    EXPECT_EQ(order->compare(1, 2), storm::analysis::Order::NodeComparison::UNKNOWN);
    EXPECT_EQ(order->compare(1, 3), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(1, 4), storm::analysis::Order::NodeComparison::UNKNOWN);
    EXPECT_EQ(order->compare(1, 5), storm::analysis::Order::NodeComparison::ABOVE);
    EXPECT_EQ(order->compare(1, 6), storm::analysis::Order::NodeComparison::UNKNOWN);
    EXPECT_EQ(order->compare(1, 7), storm::analysis::Order::NodeComparison::ABOVE);

    // s1 and s2 cannot be ordered so s0 cannot be added to the order
    //    std::vector<uint_fast64_t> succs0 = std::vector<uint_fast64_t>();
    //    succs0.push_back(1);
    //    succs0.push_back(2);
    auto res = extender.extendByBackwardReasoning(order, region, 0);

    EXPECT_TRUE(res.first != res.second);
    EXPECT_TRUE(res.first == 1 || res.first == 2);
    EXPECT_TRUE(res.second == 1 || res.second == 2);
}
}