#include "test/storm_gtest.h"
#include "storm-config.h"
#include "storm/api/verification.h"
#include "storm-figaro/api/storm-figaro.h"
#include <string>
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include <iostream>




    TEST(figaroAPITest, first) {
         std::shared_ptr<storm::figaro::FigaroProgram> figaromodel =  std::make_shared<storm::figaro::FigaroProgram> (storm::figaro::FigaroProgram());
        std::cout<<"\nhere.....\n\n\n";
        std::string property = "Pmin=? [F<=10 \"failed\"]";
        std::vector<std::shared_ptr<storm::logic::Formula const>> properties = storm::api::extractFormulasFromProperties(storm::api::parseProperties(property));
         std::shared_ptr<storm::models::sparse::Model<double>>
        sparsemodel = storm::figaro::api::checkfigaro<double>(*figaromodel, properties);
        if (sparsemodel->isOfType(storm::models::ModelType::MarkovAutomaton))
        {
            sparsemodel = storm::transformer::NonMarkovianChainTransformer<double>::eliminateNonmarkovianStates(sparsemodel->template as<storm::models::sparse::MarkovAutomaton<double>>(), storm::transformer::EliminationLabelBehavior::MergeLabels);
        }
        double resultValue = 0;
        for (auto property : properties) {
            std::unique_ptr<storm::modelchecker::CheckResult> result(
                    storm::api::verifyWithSparseEngine<double>(sparsemodel,
                                                               storm::api::createTask<double>(property, true)));
            result->filter(storm::modelchecker::ExplicitQualitativeCheckResult(sparsemodel->getInitialStates()));
            resultValue = result->asExplicitQuantitativeCheckResult<double>().getValueMap().begin()->second;
        }
    EXPECT_DOUBLE_EQ(resultValue, 3);
    }
    TEST(figaroAPITest, second) {
    std::shared_ptr<storm::figaro::FigaroProgram> figaromodel =  std::make_shared<storm::figaro::FigaroProgram> (storm::figaro::FigaroProgram());
    std::cout<<"\nhere.....\n\n\n";
    std::string property = "Pmin=? [F<=10 \"failed\"]";
    std::vector<std::shared_ptr<storm::logic::Formula const>> properties = storm::api::extractFormulasFromProperties(storm::api::parseProperties(property));
    std::shared_ptr<storm::models::sparse::Model<double>>
            sparsemodel = storm::figaro::api::checkfigaro<double>(*figaromodel, properties);
    if (sparsemodel->isOfType(storm::models::ModelType::MarkovAutomaton))
    {
        sparsemodel = storm::transformer::NonMarkovianChainTransformer<double>::eliminateNonmarkovianStates(sparsemodel->template as<storm::models::sparse::MarkovAutomaton<double>>(), storm::transformer::EliminationLabelBehavior::MergeLabels);
    }
    double resultValue = 0;
    for (auto property : properties) {
        std::unique_ptr<storm::modelchecker::CheckResult> result(
                storm::api::verifyWithSparseEngine<double>(sparsemodel,
                                                           storm::api::createTask<double>(property, true)));
        result->filter(storm::modelchecker::ExplicitQualitativeCheckResult(sparsemodel->getInitialStates()));
        resultValue = result->asExplicitQuantitativeCheckResult<double>().getValueMap().begin()->second;
    }
    EXPECT_DOUBLE_EQ(resultValue, 3);
}
namespace {
#include "test/storm_gtest.h"
#include "storm-config.h"
#include "storm/api/verification.h"
#include "storm-figaro/api/storm-figaro.h"
#include <string>
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include <iostream>


#include "../resources/examples/testfiles/figaro/01-2trainsElec/No_trim_No_repair/FigaroModel1.h"

    TEST(figaroAPITest, second) {
        std::shared_ptr<storm::figaro::FigaroProgram> figaromodel = std::make_shared<storm::figaro::FigaroProgram>(
                storm::figaro::FigaroProgram());
        std::cout << "\nhere.....\n\n\n";
        std::string property = "Pmin=? [F<=10 \"failed\"]";
        std::vector<std::shared_ptr<storm::logic::Formula const>> properties = storm::api::extractFormulasFromProperties(
                storm::api::parseProperties(property));
        std::shared_ptr<storm::models::sparse::Model<double>>
                sparsemodel = storm::figaro::api::checkfigaro<double>(*figaromodel, properties);
        if (sparsemodel->isOfType(storm::models::ModelType::MarkovAutomaton)) {
            sparsemodel = storm::transformer::NonMarkovianChainTransformer<double>::eliminateNonmarkovianStates(
                    sparsemodel->template as<storm::models::sparse::MarkovAutomaton<double>>(),
                    storm::transformer::EliminationLabelBehavior::MergeLabels);
        }
        double resultValue = 0;
        for (auto property : properties) {
            std::unique_ptr<storm::modelchecker::CheckResult> result(
                    storm::api::verifyWithSparseEngine<double>(sparsemodel,
                                                               storm::api::createTask<double>(property, true)));
            result->filter(storm::modelchecker::ExplicitQualitativeCheckResult(sparsemodel->getInitialStates()));
            resultValue = result->asExplicitQuantitativeCheckResult<double>().getValueMap().begin()->second;
        }
        EXPECT_DOUBLE_EQ(resultValue, 3);
    }

}