
#include "storm-config.h"
#include "storm/settings/SettingMemento.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/BuildSettings.h"

#include "storm-dft/modelchecker/dft/SFTBDDChecker.h"
#include "test/storm_gtest.h"

namespace {

    TEST(TestTest, CUDD) {
        storm::modelchecker::SFTBDDChecker<storm::dd::DdType::CUDD> checker{};

        EXPECT_EQ(4, 4);
    }

    TEST(TestTest, Sylvan) {
        storm::modelchecker::SFTBDDChecker<storm::dd::DdType::Sylvan> checker{};

        EXPECT_EQ(4, 4);
    }

}
