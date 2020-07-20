#include "storm-config.h"
#include "storm-dft/api/storm-dft.h"
#include "storm-dft/modelchecker/dft/SFTBDDChecker.h"
#include "storm-dft/modelchecker/dft/SFTBDDPropertyFormulaAdapter.h"
#include "storm-dft/transformations/SftToBddTransformator.h"
#include "storm-dft/utility/MTTFHelper.h"
#include "storm-parsers/api/properties.h"
#include "storm/api/properties.h"
#include "storm/settings/SettingMemento.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/BuildSettings.h"
#include "test/storm_gtest.h"

namespace {

std::string const AndBdd =
    R"|(
{"nodes":[{"classes":"be_exp","data":{"distribution":"exp","dorm":"1","id":"0","name":"x1","rate":"0.693147","type":"be_exp"},"group":"nodes"},{"classes":"be_exp","data":{"distribution":"exp","dorm":"1","id":"1","name":"x2","rate":"0.693147","type":"be_exp"},"group":"nodes"},{"classes":"and","data":{"children":["0","1"],"id":"2","name":"F","type":"and"},"group":"nodes"}],"toplevel":"2"}
)|";

std::string const OrBdd =
    R"|(
{"nodes":[{"classes":"be_exp","data":{"distribution":"exp","dorm":"1","id":"0","name":"x1","rate":"0.693147","type":"be_exp"},"group":"nodes"},{"classes":"be_exp","data":{"distribution":"exp","dorm":"1","id":"1","name":"x2","rate":"0.693147","type":"be_exp"},"group":"nodes"},{"classes":"or","data":{"children":["0","1"],"id":"2","name":"F","type":"or"},"group":"nodes"}],"toplevel":"2"}
)|";

std::string const AndOrBdd =
    R"|(
{"nodes":[{"classes":"be_exp","data":{"distribution":"exp","dorm":"1","id":"0","name":"x1","rate":"0.693147","type":"be_exp"},"group":"nodes"},{"classes":"be_exp","data":{"distribution":"exp","dorm":"1","id":"1","name":"x2","rate":"0.693147","type":"be_exp"},"group":"nodes"},{"classes":"or","data":{"children":["0","1"],"id":"2","name":"F1","type":"or"},"group":"nodes"},{"classes":"be_exp","data":{"distribution":"exp","dorm":"1","id":"3","name":"x3","rate":"0.693147","type":"be_exp"},"group":"nodes"},{"classes":"be_exp","data":{"distribution":"exp","dorm":"1","id":"4","name":"x4","rate":"0.693147","type":"be_exp"},"group":"nodes"},{"classes":"or","data":{"children":["3","4"],"id":"5","name":"F2","type":"or"},"group":"nodes"},{"classes":"and","data":{"children":["2","5"],"id":"6","name":"F","type":"and"},"group":"nodes"}],"toplevel":"6"}
)|";

std::string const VotBdd =
    R"|(
{"nodes":[{"classes":"be","data":{"distribution":"exp","dorm":"1","id":"0","name":"x1","rate":"0.693147","type":"be"},"group":"nodes"},{"classes":"be","data":{"distribution":"exp","dorm":"1","id":"1","name":"x2","rate":"0.693147","type":"be"},"group":"nodes"},{"classes":"be","data":{"distribution":"exp","dorm":"1","id":"2","name":"x3","rate":"0.693147","type":"be"},"group":"nodes"},{"classes":"be","data":{"distribution":"exp","dorm":"1","id":"3","name":"x4","rate":"0.693147","type":"be"},"group":"nodes"},{"classes":"vot","data":{"children":["0","1","2","3"],"id":"4","name":"F","type":"vot","voting":2},"group":"nodes"}],"toplevel":"4"}
)|";

TEST(TestBdd, AndOrFormulaFail) {
    auto dft = storm::api::loadDFTJsonString<double>(AndOrBdd);
    auto manager = std::make_shared<storm::storage::SylvanBddManager>();
    storm::adapters::SFTBDDPropertyFormulaAdapter checker{dft};

    auto const props{storm::api::extractFormulasFromProperties(
        storm::api::parseProperties("P=? [F < 1 !\"F2_failed\"];"))};

    STORM_LOG_ERROR(
        "NotSupportedException: "
        "Can only use negation with... is intended");
    EXPECT_THROW(checker.check(props),
                 storm::exceptions::NotSupportedException);
}

TEST(TestBdd, AndOrFormula) {
    auto dft = storm::api::loadDFTJsonString<double>(AndOrBdd);
    auto manager = std::make_shared<storm::storage::SylvanBddManager>();
    storm::adapters::SFTBDDPropertyFormulaAdapter checker{dft};

    auto const props{
        storm::api::extractFormulasFromProperties(storm::api::parseProperties(
            "P=? [F <= 1 \"failed\"];"
            "P=? [F <= 1 \"F_failed\"];"
            "P=? [F <= 1 \"F1_failed\" & \"F2_failed\"];"
            "P=? [F  = 1 !\"failed\"];"
            "P=? [F  = 1 !\"F1_failed\"];"
            "P=? [F  = 1 !\"F2_failed\"];"
            "P=? [F <= 1 \"F1_failed\"];"
            "P=? [F <= 1 \"F2_failed\"];"))};

    auto const resultProbs{checker.check(props)};
    auto const result{checker.formulasToBdd(props)};

    EXPECT_EQ(result.size(), 8);

    EXPECT_EQ(resultProbs[0], resultProbs[1]);
    EXPECT_EQ(resultProbs[1], resultProbs[2]);
    EXPECT_NEAR(resultProbs[0], 0.5625, 1e-6);

    EXPECT_NEAR(resultProbs[3], 1 - resultProbs[0], 1e-6);

    EXPECT_EQ(resultProbs[6], resultProbs[7]);
    EXPECT_NEAR(resultProbs[6], 0.75, 1e-6);

    EXPECT_EQ(resultProbs[4], resultProbs[5]);
    EXPECT_NEAR(resultProbs[4], 1 - resultProbs[6], 1e-6);

    EXPECT_EQ(result[0], result[1]);
    EXPECT_EQ(result[1], result[2]);
    EXPECT_EQ(result[2], result[0]);

    EXPECT_EQ(result[0].GetBDD(), result[1].GetBDD());
    EXPECT_EQ(result[1].GetBDD(), result[2].GetBDD());
    EXPECT_EQ(result[2].GetBDD(), result[0].GetBDD());

    EXPECT_EQ(result[0].GetBDD(), (!result[3]).GetBDD());
    EXPECT_NE(result[0].GetBDD(), result[3].GetBDD());

    EXPECT_EQ(result[6].GetBDD(), (!result[4]).GetBDD());
    EXPECT_NE(result[6].GetBDD(), result[4].GetBDD());

    EXPECT_EQ(result[7].GetBDD(), (!result[5]).GetBDD());
    EXPECT_NE(result[7].GetBDD(), result[5].GetBDD());

    EXPECT_NE(result[3].GetBDD(), result[4].GetBDD());

    manager->exportBddToDot(result[0], "/tmp/test/andOrFormula_top.dot");
    manager->exportBddToDot(result[1], "/tmp/test/andOrFormula_F.dot");
    manager->exportBddToDot(result[2], "/tmp/test/andOrFormula_FormulaAnd.dot");
    manager->exportBddToDot(result[3], "/tmp/test/andOrFormula_NotF.dot");
    manager->exportBddToDot(result[4], "/tmp/test/andOrFormula_NotF1.dot");
    manager->exportBddToDot(result[5], "/tmp/test/andOrFormula_NotF2.dot");
    manager->exportBddToDot(result[6], "/tmp/test/andOrFormula_F1.dot");
    manager->exportBddToDot(result[7], "/tmp/test/andOrFormula_F2.dot");
}

TEST(TestBdd, AndMTTF) {
    auto dft = storm::api::loadDFTJsonString<double>(AndBdd);
    EXPECT_NEAR(storm::dft::utility::MTTFHelperProceeding(dft), 2.164042561, 1e-6);
    EXPECT_NEAR(storm::dft::utility::MTTFHelperVariableChange(dft), 2.164042561, 1e-6);
}

TEST(TestBdd, VotMTTF) {
    auto dft = storm::api::loadDFTJsonString<double>(VotBdd);
    EXPECT_NEAR(storm::dft::utility::MTTFHelperProceeding(dft), 0.8415721072, 1e-6);
    EXPECT_NEAR(storm::dft::utility::MTTFHelperVariableChange(dft), 0.8415721072, 1e-6);
}

TEST(TestBdd, And) {
    auto dft = storm::api::loadDFTJsonString<double>(AndBdd);
    storm::modelchecker::SFTBDDChecker checker{dft};

    EXPECT_NEAR(checker.getProbabilityAtTimebound(1), 0.25, 1e-6);
    checker.exportBddToDot("/tmp/test/and.dot");
}

TEST(TestBdd, Or) {
    auto dft = storm::api::loadDFTJsonString<double>(OrBdd);
    storm::modelchecker::SFTBDDChecker checker{dft};
    auto result = checker.getTopLevelGateBdd();

    EXPECT_NEAR(checker.getProbabilityAtTimebound(1), 0.75, 1e-6);
    checker.exportBddToDot("/tmp/test/or.dot");
}

TEST(TestBdd, AndOr) {
    auto dft = storm::api::loadDFTJsonString<double>(AndOrBdd);
    storm::modelchecker::SFTBDDChecker checker{dft};
    auto result = checker.getTopLevelGateBdd();

    EXPECT_NEAR(checker.getProbabilityAtTimebound(1), 0.5625, 1e-6);
    checker.exportBddToDot("/tmp/test/andOr.dot");
}

TEST(TestBdd, Vot) {
    auto dft = storm::api::loadDFTJsonString<double>(VotBdd);
    storm::modelchecker::SFTBDDChecker checker{dft};
    auto result = checker.getTopLevelGateBdd();

    EXPECT_NEAR(checker.getProbabilityAtTimebound(1), 0.6875, 1e-6);
    checker.exportBddToDot("/tmp/test/vot.dot");
}

TEST(TestBdd, AndOrRelevantEvents) {
    auto dft = storm::api::loadDFTJsonString<double>(AndOrBdd);
    auto manager = std::make_shared<storm::storage::SylvanBddManager>();
    storm::transformations::dft::SftToBddTransformator<double> transformer{
        dft, manager};

    std::set<std::string> relevantEventNames = {"F", "F1", "F2", "x1"};

    auto const result = transformer.transform(relevantEventNames);

    EXPECT_EQ(result.size(), 4);

    for (auto const &i : result) {
        manager->exportBddToDot(i.second, "/tmp/test/" + i.first + ".dot");
    }
}

TEST(TestBdd, AndOrRelevantEventsChecked) {
    auto dft = storm::api::loadDFTJsonString<double>(AndOrBdd);
    storm::modelchecker::SFTBDDChecker checker{dft};
    auto relevantEvents = checker.getRelevantEventBdds({"F", "F1", "F2", "x1"});

    EXPECT_NEAR(checker.getProbabilityAtTimebound(relevantEvents["F"], 1),
                0.5625, 1e-6);

    EXPECT_NEAR(checker.getProbabilityAtTimebound(relevantEvents["F1"], 1),
                0.75, 1e-6);
    EXPECT_NEAR(checker.getProbabilityAtTimebound(relevantEvents["F2"], 1),
                0.75, 1e-6);

    EXPECT_NEAR(checker.getProbabilityAtTimebound(relevantEvents["x1"], 1), 0.5,
                1e-6);
}

}  // namespace
