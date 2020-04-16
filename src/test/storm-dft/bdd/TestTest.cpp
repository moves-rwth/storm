#include "storm-config.h"
#include "storm-dft/api/storm-dft.h"
#include "storm-dft/modelchecker/dft/SFTBDDChecker.h"
#include "storm/settings/SettingMemento.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/BuildSettings.h"
#include "test/storm_gtest.h"

namespace {

std::string const AndBdd =
    R"|({"nodes":[{"classes":"be_const","data":{"distribution":"const","failed":false,"id":"0","name":"x1","type":"be_const"},"group":"nodes"},{"classes":"be_const","data":{"distribution":"const","failed":false,"id":"1","name":"x2","type":"be_const"},"group":"nodes"},{"classes":"and","data":{"children":["0","1"],"id":"2","name":"F","type":"and"},"group":"nodes"},{"classes":"be_const","data":{"distribution":"const","failed":true,"id":"3","name":"constantBeTrigger","type":"be_const"},"group":"nodes"},{"classes":"pdep","data":{"children":["3","0"],"id":"4","name":"x1_pdep","probability":"0.5","type":"pdep"},"group":"nodes"},{"classes":"pdep","data":{"children":["3","1"],"id":"5","name":"x2_pdep","probability":"0.5","type":"pdep"},"group":"nodes"}],"toplevel":"2"})|";

std::string const OrBdd =
    R"|({"nodes":[{"classes":"be_const","data":{"distribution":"const","failed":false,"id":"0","name":"x1","type":"be_const"},"group":"nodes"},{"classes":"be_const","data":{"distribution":"const","failed":false,"id":"1","name":"x2","type":"be_const"},"group":"nodes"},{"classes":"or","data":{"children":["0","1"],"id":"2","name":"F","type":"or"},"group":"nodes"},{"classes":"be_const","data":{"distribution":"const","failed":true,"id":"3","name":"constantBeTrigger","type":"be_const"},"group":"nodes"},{"classes":"pdep","data":{"children":["3","0"],"id":"4","name":"x1_pdep","probability":"0.5","type":"pdep"},"group":"nodes"},{"classes":"pdep","data":{"children":["3","1"],"id":"5","name":"x2_pdep","probability":"0.5","type":"pdep"},"group":"nodes"}],"toplevel":"2"})|";

std::string const AndOrBdd =
    R"|({"nodes":[{"classes":"be_const","data":{"distribution":"const","failed":false,"id":"0","name":"x1","type":"be_const"},"group":"nodes"},{"classes":"be_const","data":{"distribution":"const","failed":false,"id":"1","name":"x2","type":"be_const"},"group":"nodes"},{"classes":"and","data":{"children":["0","1"],"id":"2","name":"F1","type":"and"},"group":"nodes"},{"classes":"be_const","data":{"distribution":"const","failed":false,"id":"3","name":"x3","type":"be_const"},"group":"nodes"},{"classes":"be_const","data":{"distribution":"const","failed":false,"id":"4","name":"x4","type":"be_const"},"group":"nodes"},{"classes":"and","data":{"children":["1","3","4"],"id":"5","name":"F2","type":"and"},"group":"nodes"},{"classes":"or","data":{"children":["2","5"],"id":"6","name":"F","type":"or"},"group":"nodes"},{"classes":"be_const","data":{"distribution":"const","failed":true,"id":"7","name":"constantBeTrigger","type":"be_const"},"group":"nodes"},{"classes":"pdep","data":{"children":["7","0"],"id":"8","name":"x1_pdep","probability":"0.5","type":"pdep"},"group":"nodes"},{"classes":"pdep","data":{"children":["7","1"],"id":"9","name":"x2_pdep","probability":"0.5","type":"pdep"},"group":"nodes"},{"classes":"pdep","data":{"children":["7","3"],"id":"10","name":"x3_pdep","probability":"0.5","type":"pdep"},"group":"nodes"},{"classes":"pdep","data":{"children":["7","4"],"id":"11","name":"x4_pdep","probability":"0.5","type":"pdep"},"group":"nodes"}],"toplevel":"6"})|";

std::string const VotBdd =
    R"|({"nodes":[{"classes":"be_const","data":{"distribution":"const","failed":false,"id":"0","name":"x1","type":"be_const"},"group":"nodes"},{"classes":"be_const","data":{"distribution":"const","failed":false,"id":"1","name":"x2","type":"be_const"},"group":"nodes"},{"classes":"be_const","data":{"distribution":"const","failed":false,"id":"2","name":"x3","type":"be_const"},"group":"nodes"},{"classes":"be_const","data":{"distribution":"const","failed":false,"id":"3","name":"x4","type":"be_const"},"group":"nodes"},{"classes":"vot","data":{"children":["0","1","2","3"],"id":"4","name":"F","type":"vot","voting":2},"group":"nodes"},{"classes":"be_const","data":{"distribution":"const","failed":true,"id":"5","name":"constantBeTrigger","type":"be_const"},"group":"nodes"},{"classes":"pdep","data":{"children":["5","0"],"id":"6","name":"x1_pdep","probability":"0.5","type":"pdep"},"group":"nodes"},{"classes":"pdep","data":{"children":["5","1"],"id":"7","name":"x2_pdep","probability":"0.5","type":"pdep"},"group":"nodes"},{"classes":"pdep","data":{"children":["5","2"],"id":"8","name":"x3_pdep","probability":"0.5","type":"pdep"},"group":"nodes"},{"classes":"pdep","data":{"children":["5","3"],"id":"9","name":"x4_pdep","probability":"0.5","type":"pdep"},"group":"nodes"}],"toplevel":"4"})|";

TEST(TestTest, And) {
    auto dft = storm::api::loadDFTJsonString<double>(AndBdd);
    storm::modelchecker::SFTBDDChecker<double> checker{dft};

    checker.exportBddToDot("/tmp/test/and.dot");
}

TEST(TestTest, Or) {
    auto dft = storm::api::loadDFTJsonString<double>(OrBdd);
    storm::modelchecker::SFTBDDChecker<double> checker{dft};
    auto result = checker.getBdd();

    checker.exportBddToDot("/tmp/test/or.dot");
}

TEST(TestTest, AndOr) {
    auto dft = storm::api::loadDFTJsonString<double>(AndOrBdd);
    storm::modelchecker::SFTBDDChecker<double> checker{dft};
    auto result = checker.getBdd();

    checker.exportBddToDot("/tmp/test/andOr.dot");
}

TEST(TestTest, Vot) {
    auto dft = storm::api::loadDFTJsonString<double>(VotBdd);
    storm::modelchecker::SFTBDDChecker<double> checker{dft};
    auto result = checker.getBdd();

    checker.exportBddToDot("/tmp/test/vot.dot");
}

}  // namespace
