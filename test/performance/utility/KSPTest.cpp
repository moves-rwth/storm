#include "gtest/gtest.h"
#include "storm-config.h"

#include "src/parser/PrismParser.h"
#include "src/models/sparse/Dtmc.h"
#include "src/builder/ExplicitPrismModelBuilder.h"
#include "src/utility/graph.h"
#include "src/utility/shortestPaths.cpp"

const bool VERBOSE = true;

TEST(KSPTest, crowdsSpeed) {
    if (VERBOSE) std::cout << "Parsing crowds-5-4.pm file and building model ... " << std::endl;
    storm::prism::Program program = storm::parser::PrismParser::parse(STORM_CPP_TESTS_BASE_PATH "/functional/builder/crowds-5-4.pm");
    std::shared_ptr<storm::models::sparse::Model<double>> model = storm::builder::ExplicitPrismModelBuilder<double>().translateProgram(program);

    if (VERBOSE) std::cout << "Initializing ShortestPathsGenerator ..." << std::endl;
    // timekeeping taken from http://en.cppreference.com/w/cpp/chrono#Example
    std::chrono::time_point<std::chrono::system_clock> startTime = std::chrono::system_clock::now();

    auto target = "observe0Greater1";
    storm::utility::ksp::ShortestPathsGenerator<double> spg(model, target);

    storm::storage::BitVector accStates(model->getNumberOfStates(), false);
    double accumProb = 0;

    if (VERBOSE) std::cout << "Accumulating shortest paths ..." << std::endl;
    for (int i = 1; accumProb < 0.15; i++) {
        double pathProb = spg.getDistance(i);
        accumProb += pathProb;

        storm::storage::BitVector statesInPath = spg.getStates(i);
        accStates |= statesInPath;

        if (i % 50000 == 0) {
            if (VERBOSE) std::cout << " --> It/States/AccProb/PathProb: " << i << " / " << accStates.getNumberOfSetBits() << " / " << accumProb << " / " << pathProb << std::endl;
        }
    }

    std::chrono::duration<double> elapsedSeconds = std::chrono::system_clock::now() - startTime;
    if (VERBOSE) std::cout << "Done. Num of states: " << accStates.getNumberOfSetBits() << ". Seconds: " << elapsedSeconds.count() << std::endl;

    EXPECT_LE(elapsedSeconds.count(), 5); // should take less than 5 seconds on a modern PC
}
