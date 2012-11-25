#include "gtest/gtest.h"
#include "src/storage/BitVector.h"
#include "src/exceptions/invalid_argument.h"

TEST(BitVectorTest, GetSetTest) {
	mrmc::vector::BitVector *bv = NULL;
	ASSERT_NO_THROW(bv = new mrmc::vector::BitVector(32));

	for (int i = 0; i < 32; ++i) {
		bv->set(i, i % 2 == 0);
	}

	for (int i = 0; i < 32; ++i) {
		ASSERT_EQ(bv->get(i), i % 2 == 0);
	}
	delete bv;
}

TEST(BitVectorTest, InitialZeroTest) {
	mrmc::vector::BitVector bvA(32);

	for (int i = 0; i < 32; ++i) {
		ASSERT_FALSE(bvA.get(i));
	}
}

TEST(BitVectorTest, ResizeTest) {
	mrmc::vector::BitVector bvA(32);
	
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
	mrmc::vector::BitVector bvA(32);
	mrmc::vector::BitVector bvB(32);

	for (int i = 0; i < 32; ++i) {
		bvA.set(i, i % 2 == 0);
		bvB.set(i, i % 2 == 1);
	}

	mrmc::vector::BitVector bvN = ~bvB;

	for (int i = 0; i < 32; ++i) {
		ASSERT_EQ(bvA.get(i), bvN.get(i));
	}
}

TEST(BitVectorTest, OperatorAndTest) {
	mrmc::vector::BitVector bvA(32);
	mrmc::vector::BitVector bvB(32);

	for (int i = 0; i < 32; ++i) {
		bvA.set(i, i % 2 == 0);
		bvB.set(i, i % 2 == 1);
	}

	mrmc::vector::BitVector bvN = bvA & bvB;

	for (int i = 0; i < 32; ++i) {
		ASSERT_FALSE(bvN.get(i));
	}
}

TEST(BitVectorTest, OperatorOrTest) {
	mrmc::vector::BitVector bvA(32);
	mrmc::vector::BitVector bvB(32);

	for (int i = 0; i < 32; ++i) {
		bvA.set(i, i % 2 == 0);
		bvB.set(i, i % 2 == 1);
	}

	mrmc::vector::BitVector bvN = bvA | bvB;

	for (int i = 0; i < 32; ++i) {
		ASSERT_TRUE(bvN.get(i));
	}
}

TEST(BitVectorTest, OperatorXorTest) {
	mrmc::vector::BitVector bvA(32);
	mrmc::vector::BitVector bvB(32);

	for (int i = 0; i < 32; ++i) {
		bvA.set(i, true);
		bvB.set(i, i % 2 == 1);
	}

	mrmc::vector::BitVector bvN = bvA ^ bvB;
	mrmc::vector::BitVector bvO = ~bvB;
	mrmc::vector::BitVector bvP = bvA ^ bvA;

	for (int i = 0; i < 32; ++i) {
		ASSERT_EQ(bvN.get(i), bvO.get(i));
		// A XOR A = 0
		ASSERT_FALSE(bvP.get(i));
	}
}
