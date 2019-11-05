#include "gtest/gtest.h"
#include "storm-parsers/parser/PrismParser.h"
#include "storm/builder/ExplicitModelBuilder.h"
#include "storm/models/sparse/Mdp.h"
#include "storm-parsers/parser/NondeterministicModelParser.h"
#include "storm/storage/memorystructure/NondeterministicMemoryStructure.h"
#include "storm/storage/memorystructure/NondeterministicMemoryStructureBuilder.h"
#include "storm/storage/memorystructure/SparseModelNondeterministicTransitionsBasedMemoryProduct.h"

TEST(SparseModelNondeterministicTransitionsBasedMemoryProduct, productStatesMapping) {

    std::string prismModelPath = STORM_TEST_RESOURCES_DIR "/mdp/nondeterministic_transitions_based_memory_product.nm";

    storm::prism::Program program = storm::parser::PrismParser::parse(prismModelPath);
    storm::builder::BuilderOptions options = storm::builder::BuilderOptions(true, true);
    std::shared_ptr<storm::models::sparse::Model<double>> model = storm::builder::ExplicitModelBuilder<double>(program, options).build();
    std::shared_ptr<storm::models::sparse::Mdp<double>> mdp = model->as<storm::models::sparse::Mdp<double>>();

    storm::storage::NondeterministicMemoryStructure completeMemory = storm::storage::NondeterministicMemoryStructureBuilder().buildFullyConnectedMemory(2);
    storm::storage::SparseModelNondeterministicTransitionsBasedMemoryProduct<storm::models::sparse::Mdp<double>> product(*mdp, completeMemory, true);
    model = product.build();

    ASSERT_EQ(22, model->getNumberOfStates());
    // mapping between original states and memory states to product states
    ASSERT_EQ(0, product.getModelState(0)); ASSERT_EQ(0, product.getMemoryState(0));
    ASSERT_EQ(0, product.getModelState(1)); ASSERT_EQ(0, product.getMemoryState(1));
    ASSERT_EQ(0, product.getModelState(2)); ASSERT_EQ(0, product.getMemoryState(2));
    ASSERT_EQ(0, product.getModelState(3)); ASSERT_EQ(0, product.getMemoryState(3));
    ASSERT_EQ(0, product.getModelState(4)); ASSERT_EQ(1, product.getMemoryState(4));
    ASSERT_EQ(0, product.getModelState(5)); ASSERT_EQ(1, product.getMemoryState(5));
    ASSERT_EQ(0, product.getModelState(6)); ASSERT_EQ(1, product.getMemoryState(6));
    ASSERT_EQ(0, product.getModelState(7)); ASSERT_EQ(1, product.getMemoryState(7));
    ASSERT_EQ(1, product.getModelState(8)); ASSERT_EQ(0, product.getMemoryState(8));
    ASSERT_EQ(1, product.getModelState(9)); ASSERT_EQ(0, product.getMemoryState(9));
    ASSERT_EQ(1, product.getModelState(10)); ASSERT_EQ(0, product.getMemoryState(10));
    ASSERT_EQ(1, product.getModelState(11)); ASSERT_EQ(0, product.getMemoryState(11));
    ASSERT_EQ(1, product.getModelState(12)); ASSERT_EQ(0, product.getMemoryState(12));
    ASSERT_EQ(1, product.getModelState(13)); ASSERT_EQ(1, product.getMemoryState(13));
    ASSERT_EQ(1, product.getModelState(14)); ASSERT_EQ(1, product.getMemoryState(14));
    ASSERT_EQ(1, product.getModelState(15)); ASSERT_EQ(1, product.getMemoryState(15));
    ASSERT_EQ(1, product.getModelState(16)); ASSERT_EQ(1, product.getMemoryState(16));
    ASSERT_EQ(1, product.getModelState(17)); ASSERT_EQ(1, product.getMemoryState(17));
    ASSERT_EQ(2, product.getModelState(18)); ASSERT_EQ(0, product.getMemoryState(18));
    ASSERT_EQ(2, product.getModelState(19)); ASSERT_EQ(0, product.getMemoryState(19));
    ASSERT_EQ(2, product.getModelState(20)); ASSERT_EQ(1, product.getMemoryState(20));
    ASSERT_EQ(2, product.getModelState(21)); ASSERT_EQ(1, product.getMemoryState(21));

    for (uint_fast64_t state = 0; state < mdp->getNumberOfStates(); ++state) {
        for (uint_fast64_t memory = 0; memory < completeMemory.getNumberOfStates(); ++memory) {
            ASSERT_TRUE(product.isProductStateReachable(state, memory));
        }
    }

    ASSERT_EQ(0, product.getProductState(0, 0));
    ASSERT_EQ(4, product.getProductState(0, 1));
    ASSERT_EQ(8, product.getProductState(1, 0));
    ASSERT_EQ(13, product.getProductState(1, 1));
    ASSERT_EQ(18, product.getProductState(2, 0));
    ASSERT_EQ(20, product.getProductState(2, 1));

}

TEST(SparseModelNondeterministicTransitionsBasedMemoryProduct, productMappingWithUnreachableStates) {
    std::string tra_file = STORM_TEST_RESOURCES_DIR "/tra/nondeterministic_transitions_based_memory_product.tra";
    std::string lab_file = STORM_TEST_RESOURCES_DIR "/lab/nondeterministic_transitions_based_memory_product.lab";
    std::string rew_file = STORM_TEST_RESOURCES_DIR "/rew/nondeterministic_transitions_based_memory_product.trans.rew";
    storm::models::sparse::Mdp<double> mdp(storm::parser::NondeterministicModelParser<>::parseMdp(tra_file, lab_file, "", rew_file));
    storm::models::sparse::StandardRewardModel<double> rewardModel = mdp.getUniqueRewardModel();
    rewardModel.reduceToStateBasedRewards(mdp.getTransitionMatrix());
    mdp.addRewardModel("weights", storm::models::sparse::StandardRewardModel<double>(boost::none, rewardModel.getStateActionRewardVector()));
    mdp.restrictRewardModels({"weights"});

    storm::storage::NondeterministicMemoryStructure completeMemory = storm::storage::NondeterministicMemoryStructureBuilder().buildFullyConnectedMemory(2);
    storm::storage::SparseModelNondeterministicTransitionsBasedMemoryProduct<storm::models::sparse::Mdp<double>> product(mdp, completeMemory, true);
    std::shared_ptr<storm::models::sparse::Model<double>> model = product.build();

    ASSERT_EQ(22, model->getNumberOfStates());
    // mapping between original states and memory states to product states
    ASSERT_EQ(3, product.getModelState(0)); ASSERT_EQ(0, product.getMemoryState(0));
    ASSERT_EQ(3, product.getModelState(1)); ASSERT_EQ(0, product.getMemoryState(1));
    ASSERT_EQ(3, product.getModelState(2)); ASSERT_EQ(0, product.getMemoryState(2));
    ASSERT_EQ(3, product.getModelState(3)); ASSERT_EQ(0, product.getMemoryState(3));
    ASSERT_EQ(3, product.getModelState(4)); ASSERT_EQ(1, product.getMemoryState(4));
    ASSERT_EQ(3, product.getModelState(5)); ASSERT_EQ(1, product.getMemoryState(5));
    ASSERT_EQ(3, product.getModelState(6)); ASSERT_EQ(1, product.getMemoryState(6));
    ASSERT_EQ(3, product.getModelState(7)); ASSERT_EQ(1, product.getMemoryState(7));
    ASSERT_EQ(4, product.getModelState(8)); ASSERT_EQ(0, product.getMemoryState(8));
    ASSERT_EQ(4, product.getModelState(9)); ASSERT_EQ(0, product.getMemoryState(9));
    ASSERT_EQ(4, product.getModelState(10)); ASSERT_EQ(0, product.getMemoryState(10));
    ASSERT_EQ(4, product.getModelState(11)); ASSERT_EQ(0, product.getMemoryState(11));
    ASSERT_EQ(4, product.getModelState(12)); ASSERT_EQ(0, product.getMemoryState(12));
    ASSERT_EQ(4, product.getModelState(13)); ASSERT_EQ(1, product.getMemoryState(13));
    ASSERT_EQ(4, product.getModelState(14)); ASSERT_EQ(1, product.getMemoryState(14));
    ASSERT_EQ(4, product.getModelState(15)); ASSERT_EQ(1, product.getMemoryState(15));
    ASSERT_EQ(4, product.getModelState(16)); ASSERT_EQ(1, product.getMemoryState(16));
    ASSERT_EQ(4, product.getModelState(17)); ASSERT_EQ(1, product.getMemoryState(17));
    ASSERT_EQ(5, product.getModelState(18)); ASSERT_EQ(0, product.getMemoryState(18));
    ASSERT_EQ(5, product.getModelState(19)); ASSERT_EQ(0, product.getMemoryState(19));
    ASSERT_EQ(5, product.getModelState(20)); ASSERT_EQ(1, product.getMemoryState(20));
    ASSERT_EQ(5, product.getModelState(21)); ASSERT_EQ(1, product.getMemoryState(21));

    ASSERT_FALSE(product.isProductStateReachable(0, 0)); ASSERT_FALSE(product.isProductStateReachable(2, 0));
    ASSERT_FALSE(product.isProductStateReachable(0, 1)); ASSERT_FALSE(product.isProductStateReachable(2, 1));
    ASSERT_FALSE(product.isProductStateReachable(1, 0));
    ASSERT_FALSE(product.isProductStateReachable(1, 1));

    ASSERT_TRUE(product.isProductStateReachable(3, 0)); ASSERT_TRUE(product.isProductStateReachable(5, 0));
    ASSERT_TRUE(product.isProductStateReachable(3, 1)); ASSERT_TRUE(product.isProductStateReachable(5, 1));
    ASSERT_TRUE(product.isProductStateReachable(4, 0));
    ASSERT_TRUE(product.isProductStateReachable(4, 1));

    ASSERT_EQ(0, product.getProductState(3, 0));
    ASSERT_EQ(4, product.getProductState(3, 1));
    ASSERT_EQ(8, product.getProductState(4, 0));
    ASSERT_EQ(13, product.getProductState(4, 1));
    ASSERT_EQ(18, product.getProductState(5, 0));
    ASSERT_EQ(20, product.getProductState(5, 1));
}
