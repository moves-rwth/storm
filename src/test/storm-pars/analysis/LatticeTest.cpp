//
// Created by Jip Spel on 20.09.18.
//

// TODO: cleanup includes
#include "gtest/gtest.h"
#include "storm-config.h"
#include "test/storm_gtest.h"
#include "storm-pars/analysis/Lattice.h"
#include "storm/storage/BitVector.h"

TEST(LatticeTest, Simple) {
    auto numberOfStates = 7;
    auto above = storm::storage::BitVector(numberOfStates);
    above.set(0);
    auto below = storm::storage::BitVector(numberOfStates);
    below.set(1);

    auto lattice = storm::analysis::Lattice(above, below, numberOfStates);
    EXPECT_EQ(storm::analysis::Lattice::ABOVE, lattice.compare(0,1));
    EXPECT_EQ(storm::analysis::Lattice::BELOW, lattice.compare(1,0));
    EXPECT_EQ(nullptr, lattice.getNode(2));

    lattice.add(2);
    EXPECT_EQ(storm::analysis::Lattice::ABOVE, lattice.compare(0,2));
    EXPECT_EQ(storm::analysis::Lattice::ABOVE, lattice.compare(2,1));

    lattice.add(3);
    EXPECT_EQ(storm::analysis::Lattice::UNKNOWN, lattice.compare(2,3));
    EXPECT_EQ(storm::analysis::Lattice::UNKNOWN, lattice.compare(3,2));

    lattice.addToNode(4, lattice.getNode(2));
    EXPECT_EQ(storm::analysis::Lattice::SAME, lattice.compare(2,4));
    EXPECT_EQ(storm::analysis::Lattice::ABOVE, lattice.compare(0,4));
    EXPECT_EQ(storm::analysis::Lattice::ABOVE, lattice.compare(4,1));
    EXPECT_EQ(storm::analysis::Lattice::UNKNOWN, lattice.compare(4,3));
    EXPECT_EQ(storm::analysis::Lattice::UNKNOWN, lattice.compare(3,4));

    lattice.addBetween(5, lattice.getNode(0), lattice.getNode(3));

    EXPECT_EQ(storm::analysis::Lattice::BELOW, lattice.compare(5,0));
    EXPECT_EQ(storm::analysis::Lattice::ABOVE, lattice.compare(5,3));
    EXPECT_EQ(storm::analysis::Lattice::ABOVE, lattice.compare(5,1));
    EXPECT_EQ(storm::analysis::Lattice::UNKNOWN, lattice.compare(5,2));
    EXPECT_EQ(storm::analysis::Lattice::UNKNOWN, lattice.compare(5,4));

    lattice.addBetween(6, lattice.getNode(5), lattice.getNode(3));
    EXPECT_EQ(storm::analysis::Lattice::BELOW, lattice.compare(6,0));
    EXPECT_EQ(storm::analysis::Lattice::ABOVE, lattice.compare(6,1));
    EXPECT_EQ(storm::analysis::Lattice::UNKNOWN, lattice.compare(6,2));
    EXPECT_EQ(storm::analysis::Lattice::ABOVE, lattice.compare(6,3));
    EXPECT_EQ(storm::analysis::Lattice::UNKNOWN, lattice.compare(6,4));
    EXPECT_EQ(storm::analysis::Lattice::BELOW, lattice.compare(6,5));

    lattice.addRelationNodes(lattice.getNode(6), lattice.getNode(4));
    EXPECT_EQ(storm::analysis::Lattice::ABOVE, lattice.compare(6,4));

}

TEST(LatticeTest, copy_lattice) {
    auto numberOfStates = 7;
    auto above = storm::storage::BitVector(numberOfStates);
    above.set(0);
    auto below = storm::storage::BitVector(numberOfStates);
    below.set(1);

    auto lattice = storm::analysis::Lattice(above, below, numberOfStates);
    lattice.add(2);
    lattice.add(3);
    lattice.addToNode(4, lattice.getNode(2));
    lattice.addBetween(5, lattice.getNode(0), lattice.getNode(3));
    lattice.addBetween(6, lattice.getNode(5), lattice.getNode(3));



    auto latticeCopy = storm::analysis::Lattice(lattice);
    EXPECT_EQ(storm::analysis::Lattice::ABOVE, latticeCopy.compare(0,1));
    EXPECT_EQ(storm::analysis::Lattice::BELOW, latticeCopy.compare(1,0));

    EXPECT_EQ(storm::analysis::Lattice::ABOVE, latticeCopy.compare(0,2));
    EXPECT_EQ(storm::analysis::Lattice::ABOVE, latticeCopy.compare(2,1));

    EXPECT_EQ(storm::analysis::Lattice::UNKNOWN, latticeCopy.compare(2,3));
    EXPECT_EQ(storm::analysis::Lattice::UNKNOWN, latticeCopy.compare(3,2));

    EXPECT_EQ(storm::analysis::Lattice::SAME, latticeCopy.compare(2,4));
    EXPECT_EQ(storm::analysis::Lattice::ABOVE, latticeCopy.compare(0,4));
    EXPECT_EQ(storm::analysis::Lattice::ABOVE, latticeCopy.compare(4,1));
    EXPECT_EQ(storm::analysis::Lattice::UNKNOWN, latticeCopy.compare(4,3));
    EXPECT_EQ(storm::analysis::Lattice::UNKNOWN, latticeCopy.compare(3,4));

    EXPECT_EQ(storm::analysis::Lattice::BELOW, latticeCopy.compare(5,0));
    EXPECT_EQ(storm::analysis::Lattice::ABOVE, latticeCopy.compare(5,3));
    EXPECT_EQ(storm::analysis::Lattice::ABOVE, latticeCopy.compare(5,1));
    EXPECT_EQ(storm::analysis::Lattice::UNKNOWN, latticeCopy.compare(5,2));
    EXPECT_EQ(storm::analysis::Lattice::UNKNOWN, latticeCopy.compare(5,4));

    lattice.addRelationNodes(lattice.getNode(6), lattice.getNode(4));
    latticeCopy = storm::analysis::Lattice(lattice);
    EXPECT_EQ(storm::analysis::Lattice::BELOW, latticeCopy.compare(6,0));
    EXPECT_EQ(storm::analysis::Lattice::ABOVE, latticeCopy.compare(6,1));
    EXPECT_EQ(storm::analysis::Lattice::ABOVE, latticeCopy.compare(6,2));
    EXPECT_EQ(storm::analysis::Lattice::ABOVE, latticeCopy.compare(6,3));
    EXPECT_EQ(storm::analysis::Lattice::ABOVE, latticeCopy.compare(6,4));
    EXPECT_EQ(storm::analysis::Lattice::BELOW, latticeCopy.compare(6,5));
}
