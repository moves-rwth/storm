#include "storm-config.h"
#include "test/storm_gtest.h"

#include "storm-parsers/api/storm-parsers.h"
#include "storm/api/builder.h"
#include "storm/simulator/SparseModelSimulator.h"
#include "storm/simulator/TraceSimulator.h"

template<typename CValueType>
class SparseModelSimulator {
   public:
    typedef CValueType ValueType;

    static std::shared_ptr<storm::simulator::TraceSimulator<ValueType>> createSimulator(std::string const& file, uint64_t seed) {
        storm::prism::Program program = storm::api::parseProgram(file, true);
        storm::builder::BuilderOptions options;
        options.setBuildAllLabels();
        auto model = storm::api::buildSparseModel<ValueType>(program, options);

        auto sparseModelSimulator = std::make_shared<storm::simulator::SparseModelSimulator<ValueType>>(model);
        sparseModelSimulator->setSeed(seed);
        return std::make_shared<storm::simulator::TraceSimulator<ValueType>>(sparseModelSimulator);
    }
};

template<typename TestType>
class TraceSimulatorTest : public ::testing::Test {
   public:
    typedef typename TestType::ValueType ValueType;

    TraceSimulatorTest() {}

    void SetUp() override {
#ifndef STORM_HAVE_Z3
        GTEST_SKIP() << "Z3 not available.";
#endif
    }

    std::shared_ptr<storm::simulator::TraceSimulator<ValueType>> createSimulator(std::string const& file) {
        return TestType::createSimulator(file, 5);
    }

    ValueType parseNumber(std::string const& input) {
        return storm::utility::convertNumber<ValueType>(input);
    }
};

typedef ::testing::Types<SparseModelSimulator<double>> TestingTypes;

TYPED_TEST_SUITE(TraceSimulatorTest, TestingTypes, );

TYPED_TEST(TraceSimulatorTest, KnuthYaoDieDtmc) {
    auto simulator = this->createSimulator(STORM_TEST_RESOURCES_DIR "/dtmc/die.pm");

    double result = simulator->simulateStepBoundedReachability("one", 100, 10000);
    EXPECT_NEAR(this->parseNumber("1/6"), result, 0.01);

    result = simulator->simulateStepBoundedReachability("two", 100, 10000);
    EXPECT_NEAR(this->parseNumber("1/6"), result, 0.01);
}

TYPED_TEST(TraceSimulatorTest, KnuthYaoDiceMdp) {
    auto simulator = this->createSimulator(STORM_TEST_RESOURCES_DIR "/mdp/two_dice.nm");

    double result = simulator->simulateStepBoundedReachability("two", 100, 10000);
    EXPECT_NEAR(this->parseNumber("1/32"), result, 0.01);

    result = simulator->simulateStepBoundedReachability("three", 100, 10000);
    EXPECT_NEAR(this->parseNumber("2/32"), result, 0.01);

    result = simulator->simulateStepBoundedReachability("four", 100, 2 * 10000);
    EXPECT_NEAR(this->parseNumber("3/32"), result, 0.01);
}

TYPED_TEST(TraceSimulatorTest, EmbeddedCtmc) {
    auto simulator = this->createSimulator(STORM_TEST_RESOURCES_DIR "/ctmc/embedded2.sm");

    double result = simulator->simulateTimeBoundedReachability("network_full", 10000, 10000);
    EXPECT_NEAR(0.0019216435246119591, result, 0.01);

    result = simulator->simulateTimeBoundedReachability("fail_io", 10000, 10000);
    EXPECT_NEAR(0.001911229643, result, 0.01);
}

TYPED_TEST(TraceSimulatorTest, TandemCtmc) {
    auto simulator = this->createSimulator(STORM_TEST_RESOURCES_DIR "/ctmc/tandem5.sm");

    double result = simulator->simulateTimeBoundedReachability("network_full", 10, 10000);
    EXPECT_NEAR(0.015446370562428037, result, 0.01);

    result = simulator->simulateTimeBoundedReachability("first_queue_full", 10, 10000);
    EXPECT_NEAR(0.999999837225515, result, 0.01);
}

TYPED_TEST(TraceSimulatorTest, ServerMA) {
    auto simulator = this->createSimulator(STORM_TEST_RESOURCES_DIR "/ma/server.ma");

    double result = simulator->simulateTimeBoundedReachability("error", 1, 10000);
    EXPECT_TRUE(storm::utility::isBetween(0.382120908, result, 0.455504));
}
