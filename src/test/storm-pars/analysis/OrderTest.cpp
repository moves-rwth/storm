#include "test/storm_gtest.h"
#include "storm-config.h"
#include "test/storm_gtest.h"
#include "storm-pars/analysis/Order.h"
#include "storm/storage/BitVector.h"

TEST(OrderTest, Simple) {
    auto numberOfStates = 7;
    auto above = storm::storage::BitVector(numberOfStates);
    above.set(0);
    auto below = storm::storage::BitVector(numberOfStates);
    below.set(1);
    auto initialMiddle = storm::storage::BitVector(numberOfStates);
    std::vector<uint_fast64_t> statesSorted;

    auto order = storm::analysis::Order(&above, &below, &initialMiddle, numberOfStates, &statesSorted);
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order.compare(0,1));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, order.compare(1,0));
    EXPECT_EQ(nullptr, order.getNode(2));

    order.add(2);
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order.compare(0,2));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, order.compare(2,0));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order.compare(2,1));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, order.compare(1,2));

    order.add(3);
    EXPECT_EQ(storm::analysis::Order::NodeComparison::UNKNOWN, order.compare(2,3));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::UNKNOWN, order.compare(3,2));

    order.addToNode(4, order.getNode(2));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::SAME, order.compare(2,4));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::SAME, order.compare(4,2));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order.compare(0,4));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, order.compare(4,0));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order.compare(4,1));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, order.compare(1,4));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::UNKNOWN, order.compare(4,3));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::UNKNOWN, order.compare(3,4));

    order.addBetween(5, order.getNode(0), order.getNode(3));

    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, order.compare(5,0));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order.compare(0,5));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order.compare(5,3));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, order.compare(3,5));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order.compare(5,1));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, order.compare(1,5));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::UNKNOWN, order.compare(5,2));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::UNKNOWN, order.compare(2,5));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::UNKNOWN, order.compare(5,4));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::UNKNOWN, order.compare(4,5));

    order.addBetween(6, order.getNode(5), order.getNode(3));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, order.compare(6,0));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order.compare(0,6));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order.compare(6,1));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, order.compare(1,6));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::UNKNOWN, order.compare(6,2));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::UNKNOWN, order.compare(2,6));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order.compare(6,3));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, order.compare(3,6));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::UNKNOWN, order.compare(6,4));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::UNKNOWN, order.compare(6,4));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, order.compare(6,5));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order.compare(5,6));

    order.addRelationNodes(order.getNode(6), order.getNode(4));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order.compare(6,4));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, order.compare(4,6));

}

TEST(OrderTest, copy_order) {
    auto numberOfStates = 7;
    auto above = storm::storage::BitVector(numberOfStates);
    above.set(0);
    auto below = storm::storage::BitVector(numberOfStates);
    below.set(1);
    auto initialMiddle = storm::storage::BitVector(numberOfStates);
    std::vector<uint_fast64_t> statesSorted;

    auto order = storm::analysis::Order(&above, &below, &initialMiddle, numberOfStates, &statesSorted);
    order.add(2);
    order.add(3);
    order.addToNode(4, order.getNode(2));
    order.addBetween(5, order.getNode(0), order.getNode(3));
    order.addBetween(6, order.getNode(5), order.getNode(3));



    auto orderCopy = storm::analysis::Order(order);
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, orderCopy.compare(0,1));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, orderCopy.compare(1,0));

    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, orderCopy.compare(0,2));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, orderCopy.compare(2,0));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, orderCopy.compare(2,1));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, orderCopy.compare(1,2));

    EXPECT_EQ(storm::analysis::Order::NodeComparison::UNKNOWN, orderCopy.compare(2,3));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::UNKNOWN, orderCopy.compare(3,2));

    EXPECT_EQ(storm::analysis::Order::NodeComparison::SAME, orderCopy.compare(2,4));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::SAME, orderCopy.compare(4,2));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, orderCopy.compare(0,4));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, orderCopy.compare(4,0));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, orderCopy.compare(4,1));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, orderCopy.compare(1,4));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::UNKNOWN, orderCopy.compare(4,3));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::UNKNOWN, orderCopy.compare(3,4));

    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, orderCopy.compare(5,0));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, orderCopy.compare(0,5));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, orderCopy.compare(5,3));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, orderCopy.compare(3,5));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, orderCopy.compare(5,1));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, orderCopy.compare(1,5));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::UNKNOWN, orderCopy.compare(5,2));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::UNKNOWN, orderCopy.compare(5,2));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::UNKNOWN, orderCopy.compare(5,4));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::UNKNOWN, orderCopy.compare(5,4));

    order.addRelationNodes(order.getNode(6), order.getNode(4));
    orderCopy = storm::analysis::Order(order);
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, orderCopy.compare(6,0));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, orderCopy.compare(0,6));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, orderCopy.compare(6,1));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, orderCopy.compare(1,6));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, orderCopy.compare(6,2));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, orderCopy.compare(2,6));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, orderCopy.compare(6,3));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, orderCopy.compare(3,6));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, orderCopy.compare(6,4));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, orderCopy.compare(4,6));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, orderCopy.compare(6,5));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, orderCopy.compare(5,6));
}

TEST(OrderTest, merge_nodes) {
    auto numberOfStates = 7;
    auto above = storm::storage::BitVector(numberOfStates);
    above.set(0);
    auto below = storm::storage::BitVector(numberOfStates);
    below.set(1);
    auto initialMiddle = storm::storage::BitVector(numberOfStates);
    std::vector<uint_fast64_t> statesSorted;

    auto order = storm::analysis::Order(&above, &below, &initialMiddle, numberOfStates, &statesSorted);
    order.add(2);
    order.add(3);
    order.addToNode(4, order.getNode(2));
    order.addBetween(5, order.getNode(0), order.getNode(3));
    order.addBetween(6, order.getNode(5), order.getNode(3));

    order.mergeNodes(order.getNode(4), order.getNode(5));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::SAME, order.compare(2,4));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::SAME, order.compare(2,5));

    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order.compare(0,5));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order.compare(0,2));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::ABOVE, order.compare(0,4));

    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, order.compare(6,2));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, order.compare(6,4));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, order.compare(6,5));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, order.compare(3,2));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, order.compare(3,4));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, order.compare(3,5));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, order.compare(1,2));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, order.compare(1,4));
    EXPECT_EQ(storm::analysis::Order::NodeComparison::BELOW, order.compare(1,5));
}
