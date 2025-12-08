#include "storm-config.h"
#include "test/storm_gtest.h"

#include "storm-parsers/parser/PrismParser.h"
#include "storm/adapters/JsonAdapter.h"
#include "storm/simulator/PrismProgramSimulator.h"

TEST(PrismProgramSimulatorTest, KnuthYaoDieDtmc) {
#ifndef STORM_HAVE_Z3
    GTEST_SKIP() << "Z3 not available.";
#endif
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/dtmc/die.pm");
    storm::builder::BuilderOptions options;
    options.setBuildAllRewardModels();
    options.setBuildAllLabels();

    storm::simulator::PrismProgramSimulator<double> sim(program, options);
    sim.setSeed(42);
    EXPECT_FALSE(sim.isContinuousTimeModel());
    EXPECT_EQ("coin_flips", sim.getRewardNames()[0]);

    EXPECT_EQ(0ul, sim.getStateAsJson()["s"]);
    EXPECT_EQ(0ul, sim.getStateAsJson()["d"]);
    auto rew = sim.getCurrentRewards();
    rew = sim.getCurrentRewards();
    EXPECT_EQ(1ul, rew.size());
    EXPECT_EQ(0.0, rew[0]);
    auto labels = sim.getCurrentStateLabelling();
    EXPECT_EQ(0ul, labels.size());
    EXPECT_EQ(1ul, sim.getCurrentNumberOfChoices());
    EXPECT_EQ(0, sim.getCurrentTime());
}

TEST(PrismProgramSimulatorTest, KnuthYaoDieMdp) {
#ifndef STORM_HAVE_Z3
    GTEST_SKIP() << "Z3 not available.";
#endif
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/mdp/die_c1.nm");
    storm::builder::BuilderOptions options;
    options.setBuildAllRewardModels();
    options.setBuildAllLabels();

    storm::simulator::PrismProgramSimulator<double> sim(program, options);
    sim.setSeed(42);
    EXPECT_FALSE(sim.isContinuousTimeModel());
    EXPECT_EQ("coin_flips", sim.getRewardNames()[0]);

    EXPECT_EQ(0ul, sim.getStateAsJson()["s"]);
    EXPECT_EQ(0ul, sim.getStateAsJson()["d"]);
    auto rew = sim.getCurrentRewards();
    rew = sim.getCurrentRewards();
    EXPECT_EQ(1ul, rew.size());
    EXPECT_EQ(0.0, rew[0]);
    auto labels = sim.getCurrentStateLabelling();
    EXPECT_EQ(0ul, labels.size());
    EXPECT_EQ(2ul, sim.getCurrentNumberOfChoices());

    sim.step(0);
    EXPECT_EQ(02ul, sim.getStateAsJson()["s"]);
    EXPECT_EQ(0ul, sim.getStateAsJson()["d"]);
    rew = sim.getCurrentRewards();
    EXPECT_EQ(1ul, rew.size());
    EXPECT_EQ(0.0, rew[0]);
    EXPECT_EQ(0ul, sim.getCurrentStateLabelling().size());
    EXPECT_EQ(1ul, sim.getCurrentNumberOfChoices());

    sim.randomStep();
    EXPECT_EQ(5ul, sim.getStateAsJson()["s"]);
    EXPECT_EQ(0ul, sim.getStateAsJson()["d"]);
    rew = sim.getCurrentRewards();
    EXPECT_EQ(1ul, rew.size());
    EXPECT_EQ(1.0, rew[0]);
    EXPECT_EQ(0ul, sim.getCurrentStateLabelling().size());
    EXPECT_EQ(1ul, sim.getCurrentNumberOfChoices());

    sim.randomStep();
    EXPECT_EQ(7ul, sim.getStateAsJson()["s"]);
    EXPECT_EQ(5ul, sim.getStateAsJson()["d"]);
    rew = sim.getCurrentRewards();
    EXPECT_EQ(1ul, rew.size());
    EXPECT_EQ(1.0, rew[0]);
    labels = sim.getCurrentStateLabelling();
    EXPECT_EQ(2ul, labels.size());
    EXPECT_TRUE(labels.contains("done"));
    EXPECT_TRUE(labels.contains("five"));
    EXPECT_EQ(1ul, sim.getCurrentNumberOfChoices());

    sim.randomStep();
    EXPECT_EQ(7ul, sim.getStateAsJson()["s"]);
    EXPECT_EQ(5ul, sim.getStateAsJson()["d"]);
    rew = sim.getCurrentRewards();
    EXPECT_EQ(1ul, rew.size());
    EXPECT_EQ(0.0, rew[0]);
    labels = sim.getCurrentStateLabelling();
    EXPECT_EQ(2ul, labels.size());
    EXPECT_TRUE(labels.contains("done"));
    EXPECT_TRUE(labels.contains("five"));

    EXPECT_EQ(0, sim.getCurrentTime());
}

TEST(PrismProgramSimulatorTest, SimpleCtmc) {
#ifndef STORM_HAVE_Z3
    GTEST_SKIP() << "Z3 not available.";
#endif
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/ctmc/simple2.sm");
    storm::builder::BuilderOptions options;
    options.setBuildAllRewardModels();
    options.setBuildAllLabels();

    storm::simulator::PrismProgramSimulator<double> sim(program, options);
    sim.setSeed(5);
    EXPECT_TRUE(sim.isContinuousTimeModel());
    EXPECT_EQ(2ul, sim.getRewardNames().size());

    EXPECT_EQ(0ul, sim.getStateAsJson()["s"]);
    EXPECT_EQ(2ul, sim.getCurrentRewards().size());
    EXPECT_EQ(0ul, sim.getCurrentStateLabelling().size());
    EXPECT_EQ(1ul, sim.getCurrentNumberOfChoices());
    EXPECT_EQ(0, sim.getCurrentTime());

    sim.randomStep();
    EXPECT_EQ(2ul, sim.getStateAsJson()["s"]);
    EXPECT_EQ(0ul, sim.getCurrentStateLabelling().size());
    EXPECT_EQ(1ul, sim.getCurrentNumberOfChoices());
    EXPECT_NEAR(0.007095, sim.getCurrentTime(), 1e-6);

    sim.randomStep();
    EXPECT_EQ(3ul, sim.getStateAsJson()["s"]);
    EXPECT_EQ(0ul, sim.getCurrentStateLabelling().size());
    EXPECT_EQ(1ul, sim.getCurrentNumberOfChoices());
    EXPECT_NEAR(0.007095 + 0.452143, sim.getCurrentTime(), 1e-6);

    sim.randomStep();
    EXPECT_EQ(4ul, sim.getStateAsJson()["s"]);
    EXPECT_EQ(0ul, sim.getCurrentStateLabelling().size());
    EXPECT_EQ(1ul, sim.getCurrentNumberOfChoices());
    EXPECT_NEAR(0.007095 + 0.452143 + 0.094114, sim.getCurrentTime(), 1e-6);
}

TEST(PrismProgramSimulatorTest, SimpleMA) {
#ifndef STORM_HAVE_Z3
    GTEST_SKIP() << "Z3 not available.";
#endif
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_TEST_RESOURCES_DIR "/ma/simple.ma");
    storm::builder::BuilderOptions options;
    options.setBuildAllRewardModels();
    options.setBuildAllLabels();

    storm::simulator::PrismProgramSimulator<double> sim(program, options);
    sim.setSeed(5);
    EXPECT_TRUE(sim.isContinuousTimeModel());
    EXPECT_EQ(0ul, sim.getRewardNames().size());

    // 1st run
    EXPECT_EQ(0ul, sim.getStateAsJson()["s"]);
    EXPECT_EQ(0ul, sim.getCurrentRewards().size());
    auto labels = sim.getCurrentStateLabelling();
    EXPECT_EQ(0ul, labels.size());
    EXPECT_EQ(2ul, sim.getCurrentNumberOfChoices());
    EXPECT_EQ(0, sim.getCurrentTime());

    sim.step(1);
    EXPECT_EQ(0ul, sim.getStateAsJson()["s"]);
    labels = sim.getCurrentStateLabelling();
    EXPECT_EQ(0ul, labels.size());
    EXPECT_EQ(2ul, sim.getCurrentNumberOfChoices());
    EXPECT_EQ(0, sim.getCurrentTime());

    sim.step(1);
    EXPECT_EQ(2ul, sim.getStateAsJson()["s"]);
    EXPECT_EQ(0ul, sim.getCurrentStateLabelling().size());
    EXPECT_EQ(1ul, sim.getCurrentNumberOfChoices());
    EXPECT_EQ(0, sim.getCurrentTime());

    sim.randomStep();
    EXPECT_EQ(4ul, sim.getStateAsJson()["s"]);
    EXPECT_EQ(0ul, sim.getCurrentStateLabelling().size());
    EXPECT_EQ(1ul, sim.getCurrentNumberOfChoices());
    EXPECT_NEAR(0.037679, sim.getCurrentTime(), 1e-6);

    // 2nd run
    sim.resetToInitial();
    EXPECT_EQ(0ul, sim.getStateAsJson()["s"]);
    EXPECT_EQ(0, sim.getCurrentTime());
    sim.step(0);
    EXPECT_EQ(1ul, sim.getStateAsJson()["s"]);
    EXPECT_EQ(0, sim.getCurrentTime());
    sim.randomStep();
    EXPECT_EQ(0ul, sim.getStateAsJson()["s"]);
    EXPECT_NEAR(0.388465, sim.getCurrentTime(), 1e-6);

    sim.step(0);
    EXPECT_EQ(1ul, sim.getStateAsJson()["s"]);
    EXPECT_NEAR(0.388465, sim.getCurrentTime(), 1e-6);
    sim.randomStep();
    EXPECT_EQ(0ul, sim.getStateAsJson()["s"]);
    EXPECT_NEAR(0.388465 + 0.050540, sim.getCurrentTime(), 1e-6);

    sim.step(0);
    EXPECT_EQ(1ul, sim.getStateAsJson()["s"]);
    EXPECT_NEAR(0.388465 + 0.050540, sim.getCurrentTime(), 1e-6);
    sim.randomStep();
    EXPECT_EQ(3ul, sim.getStateAsJson()["s"]);
    EXPECT_NEAR(0.388465 + 0.050540 + 0.066677, sim.getCurrentTime(), 1e-6);
}
