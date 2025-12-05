#include "storm-config.h"
#include "test/storm_gtest.h"

#include "storm-parsers/api/storm-parsers.h"
#include "storm/api/builder.h"
#include "storm/simulator/SparseModelSimulator.h"
#include "storm/simulator/TraceSimulator.h"

// Helper functions
template<typename ValueType>
std::shared_ptr<storm::simulator::SparseModelSimulator<ValueType>> createModelSimulator(std::string const& file) {
#ifndef STORM_HAVE_Z3
    GTEST_SKIP() << "Z3 not available.";
#endif
    storm::prism::Program program = storm::api::parseProgram(file, true);
    storm::builder::BuilderOptions options;
    options.setBuildAllLabels();
    auto model = storm::api::buildSparseModel<ValueType>(program, options);

    auto sparseModelSimulator = std::make_shared<storm::simulator::SparseModelSimulator<ValueType>>(model);
    sparseModelSimulator->setSeed(5);
    return sparseModelSimulator;
}

template<typename ValueType>
ValueType parseNumber(std::string const& input) {
    return storm::utility::convertNumber<ValueType>(input);
}

TEST(TraceSimulatorTest, KnuthYaoDieDtmc) {
    auto modelSimulator = createModelSimulator<double>(STORM_TEST_RESOURCES_DIR "/dtmc/die.pm");
    storm::simulator::TraceSimulator<double> simulator(modelSimulator);

    double result = simulator.simulateStepBoundedReachability("one", 100, 10000);
    EXPECT_NEAR(parseNumber<double>("1/6"), result, 0.01);

    result = simulator.simulateStepBoundedReachability("init", 100, 10000);
    EXPECT_NEAR(parseNumber<double>("1"), result, 0.01);

    result = simulator.simulateStepBoundedReachability("two", 100, 10000);
    EXPECT_NEAR(parseNumber<double>("1/6"), result, 0.01);
}

TEST(TraceSimulatorTest, KnuthYaoDiceMdp) {
    auto modelSimulator = createModelSimulator<double>(STORM_TEST_RESOURCES_DIR "/mdp/two_dice.nm");
    storm::simulator::TraceSimulator<double> simulator(modelSimulator);

    double result = simulator.simulateStepBoundedReachability("two", 100, 10000);
    EXPECT_NEAR(parseNumber<double>("1/32"), result, 0.01);

    result = simulator.simulateStepBoundedReachability("three", 100, 10000);
    EXPECT_NEAR(parseNumber<double>("2/32"), result, 0.01);

    result = simulator.simulateStepBoundedReachability("four", 100, 20000);
    EXPECT_NEAR(parseNumber<double>("3/32"), result, 0.01);
}

TEST(TraceSimulatorTest, EmbeddedCtmc) {
    auto modelSimulator = createModelSimulator<double>(STORM_TEST_RESOURCES_DIR "/ctmc/embedded2.sm");
    storm::simulator::TraceSimulator<double> simulator(modelSimulator);

    double result = simulator.simulateTimeBoundedReachability("network_full", 10000, 10000);
    EXPECT_NEAR(0.0019216435246119591, result, 0.01);

    result = simulator.simulateTimeBoundedReachability("fail_io", 10000, 10000);
    EXPECT_NEAR(0.001911229643, result, 0.01);
}

TEST(TraceSimulatorTest, TandemCtmc) {
    auto modelSimulator = createModelSimulator<double>(STORM_TEST_RESOURCES_DIR "/ctmc/tandem5.sm");
    storm::simulator::TraceSimulator<double> simulator(modelSimulator);

    double result = simulator.simulateTimeBoundedReachability("network_full", 10, 10000);
    EXPECT_NEAR(0.015446370562428037, result, 0.01);

    result = simulator.simulateTimeBoundedReachability("first_queue_full", 10, 10000);
    EXPECT_NEAR(0.999999837225515, result, 0.01);
}

TEST(TraceSimulatorTest, ServerMA) {
    auto modelSimulator = createModelSimulator<double>(STORM_TEST_RESOURCES_DIR "/ma/server.ma");
    storm::simulator::TraceSimulator<double> simulator(modelSimulator);

    double result = simulator.simulateTimeBoundedReachability("error", 1, 10000);
    EXPECT_TRUE(storm::utility::isBetween(0.382120908, result, 0.455504));
}
