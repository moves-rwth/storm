#include "gtest/gtest.h"
#include "src/storage/BitVector.h"
#include "src/exceptions/InvalidArgumentException.h"

TEST(BitVectorTest, GetSetTest) {
	storm::storage::BitVector *bv = NULL;
	ASSERT_NO_THROW(bv = new storm::storage::BitVector(32));

	for (int i = 0; i < 32; ++i) {
		bv->set(i, i % 2 == 0);
	}

	for (int i = 0; i < 32; ++i) {
		ASSERT_EQ(bv->get(i), i % 2 == 0);
	}
	delete bv;
}

TEST(BitVectorTest, InitialZeroTest) {
	storm::storage::BitVector bvA(32);

	for (int i = 0; i < 32; ++i) {
		ASSERT_FALSE(bvA.get(i));
	}
}

TEST(BitVectorTest, ResizeTest) {
	storm::storage::BitVector bvA(32);
	
	for (int i = 0; i < 32; ++i) {
		bvA.set(i, true);
	}

	bvA.resize(70);

	for (int i = 0; i < 32; ++i) {
		ASSERT_TRUE(bvA.get(i));
	}

	bool result;
	for (int i = 32; i < 70; ++i) {
		result = true;
		ASSERT_NO_THROW(result = bvA.get(i));
		ASSERT_FALSE(result);
	}
}

TEST(BitVectorTest, OperatorNotTest) {
	storm::storage::BitVector bvA(32);
	storm::storage::BitVector bvB(32);

	for (int i = 0; i < 32; ++i) {
		bvA.set(i, i % 2 == 0);
		bvB.set(i, i % 2 == 1);
	}

	storm::storage::BitVector bvN = ~bvB;

	for (int i = 0; i < 32; ++i) {
		ASSERT_EQ(bvA.get(i), bvN.get(i));
	}
}

TEST(BitVectorTest, OperatorAndTest) {
	storm::storage::BitVector bvA(32);
	storm::storage::BitVector bvB(32);

	for (int i = 0; i < 32; ++i) {
		bvA.set(i, i % 2 == 0);
		bvB.set(i, i % 2 == 1);
	}

	storm::storage::BitVector bvN = bvA & bvB;

	for (int i = 0; i < 32; ++i) {
		ASSERT_FALSE(bvN.get(i));
	}
}

TEST(BitVectorTest, OperatorOrTest) {
	storm::storage::BitVector bvA(32);
	storm::storage::BitVector bvB(32);

	for (int i = 0; i < 32; ++i) {
		bvA.set(i, i % 2 == 0);
		bvB.set(i, i % 2 == 1);
	}

	storm::storage::BitVector bvN = bvA | bvB;

	for (int i = 0; i < 32; ++i) {
		ASSERT_TRUE(bvN.get(i));
	}
}

TEST(BitVectorTest, OperatorXorTest) {
	storm::storage::BitVector bvA(32);
	storm::storage::BitVector bvB(32);

	for (int i = 0; i < 32; ++i) {
		bvA.set(i, true);
		bvB.set(i, i % 2 == 1);
	}

	storm::storage::BitVector bvN = bvA ^ bvB;
	storm::storage::BitVector bvO = ~bvB;
	storm::storage::BitVector bvP = bvA ^ bvA;

	for (int i = 0; i < 32; ++i) {
		ASSERT_EQ(bvN.get(i), bvO.get(i));
		// A XOR A = 0
		ASSERT_FALSE(bvP.get(i));
	}
}
