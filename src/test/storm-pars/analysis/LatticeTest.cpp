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
    auto numberOfStates = 5;
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
}
