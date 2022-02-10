#include "storm-config.h"
#include "test/storm_gtest.h"

#include "storm/models/sparse/StateLabeling.h"

TEST(StateLabelingTest, RemoveLabel) {
    storm::models::sparse::StateLabeling labeling(10);
    EXPECT_EQ(10ul, labeling.getNumberOfItems());
    EXPECT_EQ(0ul, labeling.getNumberOfLabels());

    storm::storage::BitVector statesTest1 = storm::storage::BitVector(10, {1, 4, 6, 7});
    labeling.addLabel("test1", statesTest1);
    EXPECT_TRUE(labeling.containsLabel("test1"));
    EXPECT_FALSE(labeling.containsLabel("abc"));

    storm::storage::BitVector statesTest2 = storm::storage::BitVector(10, {2, 6, 7, 8, 9});
    labeling.addLabel("test2", statesTest2);

    EXPECT_FALSE(labeling.getStateHasLabel("test2", 5));
    labeling.addLabelToState("test2", 5);
    EXPECT_TRUE(labeling.getStateHasLabel("test2", 5));

    EXPECT_TRUE(labeling.getStateHasLabel("test1", 4));
    labeling.removeLabelFromState("test1", 4);
    EXPECT_FALSE(labeling.getStateHasLabel("test1", 4));

    EXPECT_EQ(2ul, labeling.getNumberOfLabels());
    EXPECT_TRUE(labeling.getStateHasLabel("test1", 6));
    labeling.removeLabel("test1");
    EXPECT_FALSE(labeling.containsLabel("test1"));
    EXPECT_EQ(1ul, labeling.getNumberOfLabels());
    EXPECT_TRUE(labeling.getStateHasLabel("test2", 5));
}
