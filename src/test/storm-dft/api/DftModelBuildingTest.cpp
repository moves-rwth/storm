#include "test/storm_gtest.h"
#include "storm-config.h"

#include "storm-dft/api/storm-dft.h"
#include "storm-dft/builder/ExplicitDFTModelBuilder.h"
#include "storm-parsers/api/storm-parsers.h"

namespace {

    TEST(DftModelBuildingTest, RelevantEvents) {
        // Initialize
        std::string file = STORM_TEST_RESOURCES_DIR "/dft/dont_care.dft";
        std::shared_ptr<storm::storage::DFT<double>> dft = storm::api::loadDFTGalileoFile<double>(file);
        EXPECT_TRUE(storm::api::isWellFormed(*dft).first);
        std::string property = "Tmin=? [F \"failed\"]";
        std::vector<std::shared_ptr<storm::logic::Formula const>> properties = storm::api::extractFormulasFromProperties(storm::api::parseProperties(property));
        std::map<size_t, std::vector<std::vector<size_t>>> emptySymmetry;
        storm::storage::DFTIndependentSymmetries symmetries(emptySymmetry);

        // Set relevant events (none)
        storm::utility::RelevantEvents relevantEvents({}, false);
        dft->setRelevantEvents(relevantEvents);
        // Build model
        storm::builder::ExplicitDFTModelBuilder<double> builder(*dft, symmetries);
        builder.buildModel(0, 0.0);
        std::shared_ptr<storm::models::sparse::Model<double>> model = builder.getModel();
        EXPECT_EQ(8ul, model->getNumberOfStates());
        EXPECT_EQ(13ul, model->getNumberOfTransitions());

        // Set relevant events (all)
        relevantEvents = storm::utility::RelevantEvents({"all"}, false);
        dft->setRelevantEvents(relevantEvents);
        // Build model
        storm::builder::ExplicitDFTModelBuilder<double> builder2(*dft, symmetries);
        builder2.buildModel(0, 0.0);
        model = builder2.getModel();
        EXPECT_EQ(512ul, model->getNumberOfStates());
        EXPECT_EQ(2305ul, model->getNumberOfTransitions());

        // Set relevant events (H)
        relevantEvents = storm::utility::RelevantEvents({"H"}, false);
        dft->setRelevantEvents(relevantEvents);
        // Build model
        storm::builder::ExplicitDFTModelBuilder<double> builder3(*dft, symmetries);
        builder3.buildModel(0, 0.0);
        model = builder3.getModel();
        EXPECT_EQ(12ul, model->getNumberOfStates());
        EXPECT_EQ(25ul, model->getNumberOfTransitions());


        // Set relevant events (H, I)
        relevantEvents = storm::utility::RelevantEvents({"H", "I"}, false);
        dft->setRelevantEvents(relevantEvents);
        // Build model
        storm::builder::ExplicitDFTModelBuilder<double> builder4(*dft, symmetries);
        builder4.buildModel(0, 0.0);
        model = builder4.getModel();
        EXPECT_EQ(16ul, model->getNumberOfStates());
        EXPECT_EQ(33ul, model->getNumberOfTransitions());

        // Set relevant events (none)
        relevantEvents = storm::utility::RelevantEvents({}, true);
        dft->setRelevantEvents(relevantEvents);
        // Build model
        storm::builder::ExplicitDFTModelBuilder<double> builder5(*dft, symmetries);
        builder5.buildModel(0, 0.0);
        model = builder5.getModel();
        EXPECT_EQ(8ul, model->getNumberOfStates());
        EXPECT_EQ(13ul, model->getNumberOfTransitions());

        // Set relevant events (all)
        relevantEvents = storm::utility::RelevantEvents({"all"}, true);
        dft->setRelevantEvents(relevantEvents);
        // Build model
        storm::builder::ExplicitDFTModelBuilder<double> builder6(*dft, symmetries);
        builder6.buildModel(0, 0.0);
        model = builder6.getModel();
        EXPECT_EQ(8ul, model->getNumberOfStates());
        EXPECT_EQ(13ul, model->getNumberOfTransitions());


        // Set relevant events (H, I)
        relevantEvents = storm::utility::RelevantEvents({"H", "I"}, true);
        dft->setRelevantEvents(relevantEvents);
        // Build model
        storm::builder::ExplicitDFTModelBuilder<double> builder7(*dft, symmetries);
        builder7.buildModel(0, 0.0);
        model = builder7.getModel();
        EXPECT_EQ(8ul, model->getNumberOfStates());
        EXPECT_EQ(13ul, model->getNumberOfTransitions());
    }

}
