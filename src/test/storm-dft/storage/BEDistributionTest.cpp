#include "test/storm_gtest.h"

#include "storm-dft/api/storm-dft.h"
#include "storm-dft/modelchecker/SFTBDDChecker.h"
#include "storm-parsers/api/storm-parsers.h"

namespace {

TEST(BEDistributionTest, ConstantFail) {
    auto dft = storm::dft::api::loadDFTGalileoFile<double>(STORM_TEST_RESOURCES_DIR "/dft/be_fail.dft");
    double timebound = 0.8;

    // Perform BDD-based analysis on FT
    auto checker = std::make_shared<storm::dft::modelchecker::SFTBDDChecker>(dft);
    double resultBDD = checker->getProbabilityAtTimebound(timebound);
    EXPECT_NEAR(resultBDD, 0.3296799540, 1e-10);

    // Perform Markovian analysis on FT
    auto dft2 = storm::dft::api::prepareForMarkovAnalysis<double>(*dft);
    EXPECT_TRUE(storm::dft::api::isWellFormed(*dft2).first);
    std::string property = "Pmax=? [F<=" + std::to_string(timebound) + " \"failed\"]";
    std::vector<std::shared_ptr<storm::logic::Formula const>> properties = storm::api::extractFormulasFromProperties(storm::api::parseProperties(property));
    double resultMC = boost::get<double>(storm::dft::api::analyzeDFT<double>(*dft2, properties)[0]);
    EXPECT_NEAR(resultBDD, resultMC, 1e-10);
}

TEST(BEDistributionTest, ConstantNonFail) {
    auto dft = storm::dft::api::loadDFTGalileoFile<double>(STORM_TEST_RESOURCES_DIR "/dft/be_constant.dft");
    double timebound = 0.8;

    // Perform BDD-based analysis on FT
    auto checker = std::make_shared<storm::dft::modelchecker::SFTBDDChecker>(dft);
    double resultBDD = checker->getProbabilityAtTimebound(timebound);
    EXPECT_NEAR(resultBDD, 0.9592377960, 1e-10);

    // Perform Markovian analysis on FT
    auto dft2 = storm::dft::api::prepareForMarkovAnalysis<double>(*dft);
    EXPECT_TRUE(storm::dft::api::isWellFormed(*dft2).first);
    std::string property = "P=? [F<=" + std::to_string(timebound) + " \"failed\"]";
    std::vector<std::shared_ptr<storm::logic::Formula const>> properties = storm::api::extractFormulasFromProperties(storm::api::parseProperties(property));
    double resultMC = boost::get<double>(storm::dft::api::analyzeDFT<double>(*dft2, properties)[0]);
    EXPECT_NEAR(resultBDD, resultMC, 1e-10);
}

TEST(BEDistributionTest, ConstantNonFail2) {
    auto dft = storm::dft::api::loadDFTGalileoFile<double>(STORM_TEST_RESOURCES_DIR "/dft/be_nonfail2.dft");
    double timebound = 0.8;

    // Perform BDD-based analysis on FT
    auto checker = std::make_shared<storm::dft::modelchecker::SFTBDDChecker>(dft);
    double resultBDD = checker->getProbabilityAtTimebound(timebound);
    EXPECT_EQ(resultBDD, 0);

    // Perform Markovian analysis on FT
    auto dft2 = storm::dft::api::prepareForMarkovAnalysis<double>(*dft);
    EXPECT_TRUE(storm::dft::api::isWellFormed(*dft2).first);
    std::string property = "P=? [F<=" + std::to_string(timebound) + " \"failed\"]";
    std::vector<std::shared_ptr<storm::logic::Formula const>> properties = storm::api::extractFormulasFromProperties(storm::api::parseProperties(property));
    double resultMC = boost::get<double>(storm::dft::api::analyzeDFT<double>(*dft2, properties)[0]);
    EXPECT_EQ(resultBDD, resultMC);
}

TEST(BEDistributionTest, Probability) {
    auto dft = storm::dft::api::loadDFTGalileoFile<double>(STORM_TEST_RESOURCES_DIR "/dft/be_probabilistic.dft");
    double timebound = 0.8;

    // Perform BDD-based analysis on FT
    auto checker = std::make_shared<storm::dft::modelchecker::SFTBDDChecker>(dft);
    double resultBDD = checker->getProbabilityAtTimebound(timebound);
    EXPECT_NEAR(resultBDD, 0.1095403852, 1e-10);

    // Perform Markovian analysis on FT
    auto dft2 = storm::dft::api::prepareForMarkovAnalysis<double>(*dft);
    EXPECT_TRUE(storm::dft::api::isWellFormed(*dft2).first);
    std::string property = "Pmax=? [F<=" + std::to_string(timebound) + " \"failed\"]";
    std::vector<std::shared_ptr<storm::logic::Formula const>> properties = storm::api::extractFormulasFromProperties(storm::api::parseProperties(property));
    double resultMC = boost::get<double>(storm::dft::api::analyzeDFT<double>(*dft2, properties)[0]);
    EXPECT_NEAR(resultBDD, resultMC, 1e-10);
}

TEST(BEDistributionTest, Exponential) {
    auto dft = storm::dft::api::loadDFTGalileoFile<double>(STORM_TEST_RESOURCES_DIR "/dft/and.dft");
    double timebound = 0.8;

    // Perform BDD-based analysis on FT
    auto checker = std::make_shared<storm::dft::modelchecker::SFTBDDChecker>(dft);
    double resultBDD = checker->getProbabilityAtTimebound(timebound);
    EXPECT_NEAR(resultBDD, 0.108688872, 1e-10);

    // Perform Markovian analysis on FT
    auto dft2 = storm::dft::api::prepareForMarkovAnalysis<double>(*dft);
    EXPECT_TRUE(storm::dft::api::isWellFormed(*dft2).first);
    std::string property = "P=? [F<=" + std::to_string(timebound) + " \"failed\"]";
    std::vector<std::shared_ptr<storm::logic::Formula const>> properties = storm::api::extractFormulasFromProperties(storm::api::parseProperties(property));
    double resultMC = boost::get<double>(storm::dft::api::analyzeDFT<double>(*dft2, properties)[0]);
    EXPECT_NEAR(resultBDD, resultMC, 1e-10);
}

TEST(BEDistributionTest, Erlang) {
    auto dft = storm::dft::api::loadDFTGalileoFile<double>(STORM_TEST_RESOURCES_DIR "/dft/be_erlang.dft");
    double timebound = 0.8;

    // Perform BDD-based analysis on FT
    auto checker = std::make_shared<storm::dft::modelchecker::SFTBDDChecker>(dft);
    double resultBDD = checker->getProbabilityAtTimebound(timebound);
    EXPECT_NEAR(resultBDD, 0.4949009834, 1e-10);

    // Perform Markovian analysis on FT
    auto dft2 = storm::dft::api::prepareForMarkovAnalysis<double>(*dft);
    EXPECT_TRUE(storm::dft::api::isWellFormed(*dft2).first);
    std::string property = "P=? [F<=" + std::to_string(timebound) + " \"failed\"]";
    std::vector<std::shared_ptr<storm::logic::Formula const>> properties = storm::api::extractFormulasFromProperties(storm::api::parseProperties(property));
    double resultMC = boost::get<double>(storm::dft::api::analyzeDFT<double>(*dft2, properties)[0]);
    EXPECT_NEAR(resultBDD, resultMC, 1e-10);
}

TEST(BEDistributionTest, Weibull) {
    auto dft = storm::dft::api::loadDFTGalileoFile<double>(STORM_TEST_RESOURCES_DIR "/dft/be_weibull.dft");
    double timebound = 2;

    // Perform BDD-based analysis on FT
    auto checker = std::make_shared<storm::dft::modelchecker::SFTBDDChecker>(dft);
    double resultBDD = checker->getProbabilityAtTimebound(timebound);
    EXPECT_NEAR(resultBDD, 0.0382982486, 1e-10);
}

TEST(BEDistributionTest, LogNormal) {
    auto dft = storm::dft::api::loadDFTGalileoFile<double>(STORM_TEST_RESOURCES_DIR "/dft/be_lognormal.dft");
    double timebound = 0.8;

    // Perform BDD-based analysis on FT
    auto checker = std::make_shared<storm::dft::modelchecker::SFTBDDChecker>(dft);
    double resultBDD = checker->getProbabilityAtTimebound(timebound);
    EXPECT_NEAR(resultBDD, 0.2336675428, 1e-10);
}

}  // namespace
