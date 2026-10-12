#include "storm-config.h"
#include "test/storm_gtest.h"

#include <sstream>

#include "storm-parsers/parser/FormulaParser.h"
#include "storm-pomdp/beliefs/policy/ApplyObservationBasedFSCToPomdp.h"
#include "storm-pomdp/beliefs/policy/ObservationBasedFiniteStateController.h"
#include "storm-pomdp/beliefs/policy/PolicyExtractor.h"
#include "storm/adapters/JsonAdapter.h"
#include "storm/environment/Environment.h"
#include "storm/logic/Formulas.h"
#include "storm/modelchecker/prctl/SparseDtmcPrctlModelChecker.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/models/sparse/ChoiceLabeling.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/Pomdp.h"
#include "storm/models/sparse/StandardRewardModel.h"
#include "storm/models/sparse/StateLabeling.h"
#include "storm/storage/Scheduler.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/storage/expressions/ExpressionManager.h"
#include "storm/storage/sparse/ModelComponents.h"
#include "storm/storage/valuations/ValuationDescriptionBuilder.h"
#include "storm/storage/valuations/Valuations.h"
#include "storm/storage/valuations/ValuationsStorage.h"

namespace {

storm::models::sparse::Pomdp<double> buildSingleStatePomdp(bool withRewards = false, bool withChoiceLabels = false) {
    storm::storage::SparseMatrixBuilder<double> matrixBuilder(0, 0, 0, false, true);
    matrixBuilder.newRowGroup(0);
    matrixBuilder.addNextValue(0, 0, 1.0);
    matrixBuilder.addNextValue(1, 0, 1.0);

    storm::models::sparse::StateLabeling stateLabeling(1);
    stateLabeling.addLabel("init");
    stateLabeling.addLabelToState("init", 0);

    storm::storage::sparse::ModelComponents<double> components(matrixBuilder.build(), std::move(stateLabeling));
    components.observabilityClasses = std::vector<uint32_t>{0};
    if (withChoiceLabels) {
        storm::models::sparse::ChoiceLabeling choiceLabeling(2);
        choiceLabeling.addLabel("first");
        choiceLabeling.addLabelToChoice("first", 0);
        choiceLabeling.addLabel("second");
        choiceLabeling.addLabelToChoice("second", 1);
        components.choiceLabeling = std::move(choiceLabeling);
    }
    if (withRewards) {
        components.rewardModels.emplace("rew", storm::models::sparse::StandardRewardModel<double>(std::optional<std::vector<double>>({3.0}),
                                                                                                  std::optional<std::vector<double>>({2.0, 6.0})));
    }
    return storm::models::sparse::Pomdp<double>(std::move(components), true);
}

storm::models::sparse::Pomdp<double> buildSingleStatePomdpWithObservationValuations(bool reverseObservableOrder) {
    storm::storage::SparseMatrixBuilder<double> matrixBuilder(0, 0, 0, false, true);
    matrixBuilder.newRowGroup(0);
    matrixBuilder.addNextValue(0, 0, 1.0);

    storm::models::sparse::StateLabeling stateLabeling(1);
    stateLabeling.addLabel("init");
    stateLabeling.addLabelToState("init", 0);

    auto manager = std::make_shared<storm::expressions::ExpressionManager>();
    auto const enabled = manager->declareBooleanVariable("enabled");
    auto const mode = manager->declareIntegerVariable("mode");
    storm::storage::sparse::ValuationDescriptionBuilder valuationBuilder(manager);
    if (reverseObservableOrder) {
        valuationBuilder.addIntegerVariable(mode, 0, 1);
        valuationBuilder.addBooleanVariable(enabled);
    } else {
        valuationBuilder.addBooleanVariable(enabled);
        valuationBuilder.addIntegerVariable(mode, 0, 1);
    }
    storm::storage::sparse::Valuations observationValuations(valuationBuilder.buildClassDescription(), manager);
    observationValuations.getStorage().resize(1);
    observationValuations.getStorage().writeValue(0, enabled, true);
    observationValuations.getStorage().writeValue(0, mode, int64_t{1});

    storm::storage::sparse::ModelComponents<double> components(matrixBuilder.build(), std::move(stateLabeling));
    components.observabilityClasses = std::vector<uint32_t>{0};
    components.observationValuations = std::move(observationValuations);
    return storm::models::sparse::Pomdp<double>(std::move(components), true);
}

storm::models::sparse::Mdp<double> buildBeliefMdpWithReorderedCutoffChoices() {
    storm::storage::SparseMatrixBuilder<double> matrixBuilder(0, 0, 0, false, true);
    matrixBuilder.newRowGroup(0);
    matrixBuilder.addNextValue(0, 1, 1.0);
    matrixBuilder.newRowGroup(1);
    matrixBuilder.addNextValue(1, 2, 1.0);
    matrixBuilder.addNextValue(2, 2, 1.0);
    matrixBuilder.newRowGroup(3);
    matrixBuilder.addNextValue(3, 2, 1.0);

    storm::models::sparse::StateLabeling stateLabeling(3);
    stateLabeling.addLabel("init");
    stateLabeling.addLabelToState("init", 0);
    stateLabeling.addLabel("truncated");
    stateLabeling.addLabelToState("truncated", 1);

    storm::models::sparse::ChoiceLabeling choiceLabeling(4);
    choiceLabeling.addLabel("__sched_1");
    choiceLabeling.addLabelToChoice("__sched_1", 1);
    choiceLabeling.addLabel("__sched_0");
    choiceLabeling.addLabelToChoice("__sched_0", 2);

    storm::storage::sparse::ModelComponents<double> components(matrixBuilder.build(), std::move(stateLabeling));
    components.choiceLabeling = std::move(choiceLabeling);
    return storm::models::sparse::Mdp<double>(std::move(components));
}

storm::models::sparse::Mdp<double> buildSingleStateBeliefMdpWithUnlabelledSecondChoice() {
    storm::storage::SparseMatrixBuilder<double> matrixBuilder(0, 0, 0, false, true);
    matrixBuilder.newRowGroup(0);
    matrixBuilder.addNextValue(0, 0, 1.0);
    matrixBuilder.addNextValue(1, 0, 1.0);

    storm::models::sparse::StateLabeling stateLabeling(1);
    stateLabeling.addLabel("init");
    stateLabeling.addLabelToState("init", 0);
    stateLabeling.addLabel("truncated");

    storm::models::sparse::ChoiceLabeling choiceLabeling(2);
    choiceLabeling.addLabel("first");
    choiceLabeling.addLabelToChoice("first", 0);

    storm::storage::sparse::ModelComponents<double> components(matrixBuilder.build(), std::move(stateLabeling));
    components.choiceLabeling = std::move(choiceLabeling);
    return storm::models::sparse::Mdp<double>(std::move(components));
}

}  // namespace

TEST(ObservationBasedFiniteStateController, DeterminismTracksReplacedOutputs) {
    storm::pomdp::policy::ObservationBasedFiniteStateController<double> fsc(0);
    storm::storage::Distribution<double, uint64_t> distribution;
    distribution.addProbability(0, 0.5);
    distribution.addProbability(1, 0.5);

    fsc.addRandomisedActionTransition(0, 0, distribution, 0);
    EXPECT_FALSE(fsc.isDeterministic());

    fsc.addDeterministicActionTransition(0, 0, 1, 0);
    EXPECT_TRUE(fsc.isDeterministic());
}

TEST(ObservationBasedFiniteStateController, FormatsPartialSignatureMaps) {
    storm::pomdp::policy::ObservationSignature observationSignature;
    observationSignature.values.emplace("known-observable", "known-value");
    std::optional observationSignatures{std::unordered_map<uint64_t, storm::pomdp::policy::ObservationSignature>{{0, observationSignature}}};
    std::optional actionNames{std::unordered_map<uint64_t, std::unordered_map<uint64_t, std::string>>{{0, {{0, "known-action"}}}}};
    storm::pomdp::policy::ObservationBasedFiniteStateController<double> fsc(0, observationSignatures, actionNames);
    fsc.addDeterministicActionTransition(0, 0, 0, 1);

    storm::storage::Distribution<double, uint64_t> distribution;
    distribution.addProbability(1, 1.0);
    fsc.addRandomisedActionTransition(1, 1, distribution, 0);

    auto const description = fsc.toString();
    EXPECT_NE(description.find("(0)[known-observable=known-value]"), std::string::npos);
    EXPECT_NE(description.find("1 ->"), std::string::npos);
    EXPECT_NE(description.find("(1) :"), std::string::npos);

    std::stringstream jsonOutput;
    fsc.writeJsonToStream(jsonOutput);
    auto const json = storm::json<double>::parse(jsonOutput.str());
    auto const& observable = json.at("nodes").at(0).at("transitions").at(0).at("observation").at("signature").at(0);
    EXPECT_EQ(observable.at("name").get<std::string>(), "known-observable");
    EXPECT_EQ(observable.at("type").get<std::string>(), "string");
    EXPECT_EQ(observable.at("value").get<std::string>(), "known-value");
}

TEST(ObservationBasedFiniteStateController, EscapesSignaturesAndActionNamesInDotOutput) {
    storm::pomdp::policy::ObservationSignature observationSignature;
    observationSignature.values.emplace("observation", "value\"\\\n");
    std::optional observationSignatures{std::unordered_map<uint64_t, storm::pomdp::policy::ObservationSignature>{{0, observationSignature}}};
    std::optional actionNames{std::unordered_map<uint64_t, std::unordered_map<uint64_t, std::string>>{{0, {{0, "action\"\\\n"}}}}};
    storm::pomdp::policy::ObservationBasedFiniteStateController<double> fsc(0, observationSignatures, actionNames);
    fsc.addDeterministicActionTransition(0, 0, 0, 0);

    std::stringstream dotOutput;
    fsc.writeDotToStream(dotOutput);

    auto const dot = dotOutput.str();
    EXPECT_NE(dot.find("observation=value\\\"\\\\\\n"), std::string::npos);
    EXPECT_NE(dot.find("action\\\"\\\\\\n"), std::string::npos);
}

TEST(ApplyObservationBasedFiniteStateController, PreservesRewardsForRandomisedActions) {
    auto pomdp = buildSingleStatePomdp(true);
    storm::pomdp::policy::ObservationBasedFiniteStateController<double> fsc(0);
    storm::storage::Distribution<double, uint64_t> distribution;
    distribution.addProbability(0, 0.25);
    distribution.addProbability(1, 0.75);
    fsc.addRandomisedActionTransition(0, 0, distribution, 0);

    auto inducedDtmc = storm::pomdp::policy::applyObservationBasedFSCToPomdp(pomdp, fsc);

    ASSERT_TRUE(inducedDtmc.hasRewardModel("rew"));
    auto const& rewards = inducedDtmc.getRewardModel("rew").getStateRewardVector();
    ASSERT_EQ(rewards.size(), inducedDtmc.getNumberOfStates());
    EXPECT_DOUBLE_EQ(rewards.at(0), 8.0);
}

TEST(ApplyObservationBasedFiniteStateController, PreservesLabelsOfUnreachableStates) {
    storm::storage::SparseMatrixBuilder<double> matrixBuilder(0, 0, 0, false, true);
    matrixBuilder.newRowGroup(0);
    matrixBuilder.addNextValue(0, 0, 1.0);
    matrixBuilder.addNextValue(1, 1, 1.0);
    matrixBuilder.newRowGroup(2);
    matrixBuilder.addNextValue(2, 1, 1.0);

    storm::models::sparse::StateLabeling stateLabeling(2);
    stateLabeling.addLabel("init");
    stateLabeling.addLabelToState("init", 0);
    stateLabeling.addLabel("goal");
    stateLabeling.addLabelToState("goal", 1);
    stateLabeling.addLabel("safe");
    stateLabeling.addLabelToState("safe", 0);
    stateLabeling.addLabel("empty");

    storm::storage::sparse::ModelComponents<double> components(matrixBuilder.build(), std::move(stateLabeling));
    components.observabilityClasses = std::vector<uint32_t>{0, 1};
    storm::models::sparse::Pomdp<double> pomdp(std::move(components), true);
    storm::pomdp::policy::ObservationBasedFiniteStateController<double> fsc(0);
    fsc.addDeterministicActionTransition(0, 0, 0, 0);

    auto inducedDtmc = storm::pomdp::policy::applyObservationBasedFSCToPomdp(pomdp, fsc);
    ASSERT_EQ(inducedDtmc.getNumberOfStates(), 1);
    auto const& labeling = inducedDtmc.getStateLabeling();
    EXPECT_EQ(labeling.getLabels(), pomdp.getStateLabeling().getLabels());
    ASSERT_TRUE(labeling.containsLabel("goal"));
    EXPECT_TRUE(labeling.getStates("goal").empty());
    EXPECT_TRUE(labeling.getStates("empty").empty());
    EXPECT_TRUE(labeling.getStateHasLabel("safe", 0));
    EXPECT_TRUE(labeling.getStateHasLabel("init", 0));

    storm::Environment env;
    storm::parser::FormulaParser formulaParser;
    auto formula = formulaParser.parseSingleFormulaFromString("P=? [F \"goal\"]");
    storm::modelchecker::SparseDtmcPrctlModelChecker<storm::models::sparse::Dtmc<double>> checker(inducedDtmc);
    auto result = checker.check(env, *formula);
    EXPECT_DOUBLE_EQ(result->asExplicitQuantitativeCheckResult<double>()[0], 0.0);
}

TEST(ApplyObservationBasedFiniteStateController, MatchesObservationValuationsIndependentOfStorageOrder) {
    auto sourcePomdp = buildSingleStatePomdpWithObservationValuations(false);
    auto targetPomdp = buildSingleStatePomdpWithObservationValuations(true);

    auto const sourceObservationSignature = storm::pomdp::policy::getObservationSignature(sourcePomdp, 0);
    EXPECT_EQ(sourceObservationSignature, storm::pomdp::policy::getObservationSignature(targetPomdp, 0));

    std::optional observationSignatures{std::unordered_map<uint64_t, storm::pomdp::policy::ObservationSignature>{{0, sourceObservationSignature}}};
    storm::pomdp::policy::ObservationBasedFiniteStateController<double> fsc(0, observationSignatures, std::nullopt);
    fsc.addDeterministicActionTransition(0, 0, 0, 0);

    auto inducedDtmc = storm::pomdp::policy::applyObservationBasedFSCToPomdp(targetPomdp, fsc);
    EXPECT_EQ(inducedDtmc.getNumberOfStates(), 1);
}

TEST(ApplyObservationBasedFiniteStateController, UnspecifiedOutputUsesLocalActionZeroWithReorderedActionNames) {
    auto pomdp = buildSingleStatePomdp(true, true);
    std::optional actionNames{std::unordered_map<uint64_t, std::unordered_map<uint64_t, std::string>>{{0, {{0, "second"}, {1, "first"}}}}};
    storm::pomdp::policy::ObservationBasedFiniteStateController<double> fsc(7, std::nullopt, actionNames);

    EXPECT_THROW(storm::pomdp::policy::applyObservationBasedFSCToPomdp(pomdp, fsc), storm::exceptions::UnexpectedException);
    auto inducedDtmc = storm::pomdp::policy::applyObservationBasedFSCToPomdp(pomdp, fsc, true);

    EXPECT_EQ(inducedDtmc.getNumberOfStates(), 1);
    EXPECT_DOUBLE_EQ(inducedDtmc.getRewardModel("rew").getStateRewardVector().at(0), 5.0);
}

TEST(ApplyObservationBasedFiniteStateController, UnspecifiedOutputUsesLocalActionZeroWithIncompleteActionNames) {
    auto pomdp = buildSingleStatePomdp(true, true);
    std::optional actionNames{std::unordered_map<uint64_t, std::unordered_map<uint64_t, std::string>>{{0, {{1, "second"}}}}};
    storm::pomdp::policy::ObservationBasedFiniteStateController<double> fsc(7, std::nullopt, actionNames);

    auto inducedDtmc = storm::pomdp::policy::applyObservationBasedFSCToPomdp(pomdp, fsc, true);

    EXPECT_EQ(inducedDtmc.getNumberOfStates(), 1);
    EXPECT_DOUBLE_EQ(inducedDtmc.getRewardModel("rew").getStateRewardVector().at(0), 5.0);
}

TEST(ApplyObservationBasedFiniteStateController, SpecifiedOutputRemapsActionNamesWithDontCareEnabled) {
    auto pomdp = buildSingleStatePomdp(true, true);
    std::optional actionNames{std::unordered_map<uint64_t, std::unordered_map<uint64_t, std::string>>{{0, {{0, "second"}, {1, "first"}}}}};
    storm::pomdp::policy::ObservationBasedFiniteStateController<double> fsc(7, std::nullopt, actionNames);
    fsc.addDeterministicActionTransition(7, 0, 0, 7);

    auto inducedDtmc = storm::pomdp::policy::applyObservationBasedFSCToPomdp(pomdp, fsc, true);

    EXPECT_EQ(inducedDtmc.getNumberOfStates(), 1);
    EXPECT_DOUBLE_EQ(inducedDtmc.getRewardModel("rew").getStateRewardVector().at(0), 9.0);
}

TEST(PolicyExtractor, UsesCutoffSchedulerLabelInsteadOfLocalChoiceIndex) {
    auto pomdp = buildSingleStatePomdp();
    auto beliefMdp = buildBeliefMdpWithReorderedCutoffChoices();

    storm::storage::Scheduler<double> beliefScheduler(3);
    beliefScheduler.setChoice(0, 0);
    beliefScheduler.setChoice(0, 1);
    beliefScheduler.setChoice(0, 2);

    storm::storage::Scheduler<double> schedulerZero(1);
    schedulerZero.setChoice(0, 0);
    storm::storage::Scheduler<double> schedulerOne(1);
    schedulerOne.setChoice(1, 0);
    std::optional<std::vector<storm::storage::Scheduler<double>>> approximationSchedulers(
        std::vector<storm::storage::Scheduler<double>>{schedulerZero, schedulerOne});
    std::unordered_map<uint64_t, uint32_t> beliefStateToObservation{{0, 0}, {1, 0}};

    storm::pomdp::policy::PolicyExtractor<storm::models::sparse::Pomdp<double>, double, double> extractor(pomdp, beliefMdp, beliefStateToObservation,
                                                                                                          beliefScheduler, approximationSchedulers);
    auto fsc = extractor.exportPolicyAsFiniteStateController();

    EXPECT_EQ(fsc.getActionForObservationInNode(1, 0), 1);
}

TEST(PolicyExtractor, AddsEmptyLabelToSelectedUnlabelledChoice) {
    auto pomdp = buildSingleStatePomdp();
    auto beliefMdp = buildSingleStateBeliefMdpWithUnlabelledSecondChoice();
    storm::storage::Scheduler<double> beliefScheduler(1);
    beliefScheduler.setChoice(1, 0);
    std::unordered_map<uint64_t, uint32_t> beliefStateToObservation{{0, 0}};

    storm::pomdp::policy::PolicyExtractor<storm::models::sparse::Pomdp<double>, double, double> extractor(pomdp, beliefMdp, beliefStateToObservation,
                                                                                                          beliefScheduler);
    auto inducedModel = extractor.exportPolicyAsInducedMarkovChain();

    ASSERT_TRUE(inducedModel->hasChoiceLabeling());
    EXPECT_TRUE(inducedModel->getChoiceLabeling().getLabelsOfChoice(0).contains(""));
}
