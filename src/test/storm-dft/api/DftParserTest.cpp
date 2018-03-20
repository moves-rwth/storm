#include "gtest/gtest.h"
#include "storm-config.h"

#include "storm-dft/api/storm-dft.h"

namespace {

    TEST(DftParserTest, LoadFromGalileo) {
        std::string file = STORM_TEST_RESOURCES_DIR "/dft/and.dft";
        std::shared_ptr<storm::storage::DFT<double>> dft = storm::api::loadDFTGalileo<double>(file);
    }

}
