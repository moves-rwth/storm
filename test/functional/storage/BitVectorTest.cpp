#include "gtest/gtest.h"
#include "src/storage/BitVector.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/exceptions/OutOfRangeException.h"

TEST(BitVectorTest, InitToZeroTest) {
	storm::storage::BitVector vector(32);
    
	for (uint_fast64_t i = 0; i < 32; ++i) {
		ASSERT_FALSE(vector.get(i));
	}
    
    ASSERT_TRUE(vector.empty());
    ASSERT_FALSE(vector.full());
}

TEST(BitVectorTest, InitToOneTest) {
	storm::storage::BitVector vector(32, true);
    
	for (uint_fast64_t i = 0; i < 32; ++i) {
		ASSERT_TRUE(vector.get(i));
	}
    ASSERT_FALSE(vector.empty());
    ASSERT_TRUE(vector.full());
}

TEST(BitVectorTest, InitFromIteratorTest) {
    std::vector<uint_fast64_t> valueVector = {0, 4, 10};
	storm::storage::BitVector vector(32, valueVector.begin(), valueVector.end());
    
    ASSERT_EQ(vector.size(), 32);
    
	for (uint_fast64_t i = 0; i < 32; ++i) {
        if (i == 0 || i == 4 || i == 10) {
            ASSERT_TRUE(vector.get(i));
        } else {
            ASSERT_FALSE(vector.get(i));
        }
	}
}

TEST(BitVectorTest, GetSetTest) {
	storm::storage::BitVector vector(32);

	for (uint_fast64_t i = 0; i < 32; ++i) {
		vector.set(i, i % 2 == 0);
	}

	for (uint_fast64_t i = 0; i < 32; ++i) {
		ASSERT_EQ(vector.get(i), i % 2 == 0);
	}
}

TEST(BitVectorTest, GetSetExceptionTest) {
	storm::storage::BitVector vector(32);
    
    ASSERT_THROW(vector.get(32), storm::exceptions::OutOfRangeException);
    ASSERT_THROW(vector.set(32), storm::exceptions::OutOfRangeException);
}

TEST(BitVectorTest, ResizeTest) {
	storm::storage::BitVector vector(32);
	
	for (uint_fast64_t i = 0; i < 32; ++i) {
		vector.set(i);
	}

	vector.resize(70);
    
    ASSERT_EQ(vector.size(), 70);
    ASSERT_EQ(vector.getNumberOfSetBits(), 32);

	for (uint_fast64_t i = 0; i < 32; ++i) {
		ASSERT_TRUE(vector.get(i));
	}
	bool result;
	for (uint_fast64_t i = 32; i < 70; ++i) {
		result = true;
		ASSERT_NO_THROW(result = vector.get(i));
		ASSERT_FALSE(result);
	}
    
    vector.resize(72, true);
    
    ASSERT_EQ(vector.size(), 72);
    ASSERT_EQ(vector.getNumberOfSetBits(), 34);
    
    for (uint_fast64_t i = 0; i < 32; ++i) {
		ASSERT_TRUE(vector.get(i));
	}
	for (uint_fast64_t i = 32; i < 70; ++i) {
		result = true;
		ASSERT_NO_THROW(result = vector.get(i));
		ASSERT_FALSE(result);
	}
    for (uint_fast64_t i = 70; i < 72; ++i) {
        ASSERT_TRUE(vector.get(i));
    }

    vector.resize(16, 0);
    ASSERT_EQ(vector.size(), 16);
    ASSERT_EQ(vector.getNumberOfSetBits(), 16);
    
    for (uint_fast64_t i = 0; i < 16; ++i) {
		ASSERT_TRUE(vector.get(i));
	}
    
    vector.resize(65, 1);
    ASSERT_EQ(vector.size(), 65);
    ASSERT_TRUE(vector.full());
}

TEST(BitVectorTest, OperatorAndTest) {
	storm::storage::BitVector vector1(32);
	storm::storage::BitVector vector2(32);
    
	for (int i = 0; i < 32; ++i) {
		vector1.set(i, i % 2 == 0);
		vector2.set(i, i % 2 == 1);
	}
    vector1.set(31);
    vector2.set(31);
    
	storm::storage::BitVector andResult = vector1 & vector2;
    
	for (uint_fast64_t i = 0; i < 31; ++i) {
		ASSERT_FALSE(andResult.get(i));
	}
    ASSERT_TRUE(andResult.get(31));
}

TEST(BitVectorTest, OperatorAndEqualTest) {
	storm::storage::BitVector vector1(32);
	storm::storage::BitVector vector2(32);
    
	for (int i = 0; i < 32; ++i) {
		vector1.set(i, i % 2 == 0);
		vector2.set(i, i % 2 == 1);
	}
    vector1.set(31);
    vector2.set(31);
    
	vector1 &= vector2;
    
	for (uint_fast64_t i = 0; i < 31; ++i) {
		ASSERT_FALSE(vector1.get(i));
	}
    ASSERT_TRUE(vector1.get(31));
}


TEST(BitVectorTest, OperatorOrTest) {
	storm::storage::BitVector vector1(32);
	storm::storage::BitVector vector2(32);
    
	for (uint_fast64_t i = 0; i < 32; ++i) {
		vector1.set(i, i % 2 == 0);
		vector2.set(i, i % 2 == 1);
	}
    vector1.set(31, false);
    vector2.set(31, false);
    
	storm::storage::BitVector orResult = vector1 | vector2;
    
	for (uint_fast64_t i = 0; i < 31; ++i) {
		ASSERT_TRUE(orResult.get(i));
	}
    ASSERT_FALSE(orResult.get(31));
}

TEST(BitVectorTest, OperatorOrEqualTest) {
	storm::storage::BitVector vector1(32);
	storm::storage::BitVector vector2(32);
    
	for (uint_fast64_t i = 0; i < 32; ++i) {
		vector1.set(i, i % 2 == 0);
		vector2.set(i, i % 2 == 1);
	}
    vector1.set(31, false);
    vector2.set(31, false);
    
    vector1 |= vector2;
    
	for (uint_fast64_t i = 0; i < 31; ++i) {
		ASSERT_TRUE(vector1.get(i));
	}
    ASSERT_FALSE(vector1.get(31));
}

TEST(BitVectorTest, OperatorXorTest) {
	storm::storage::BitVector vector1(32);
	storm::storage::BitVector vector2(32);
    
	for (uint_fast64_t i = 0; i < 32; ++i) {
		vector1.set(i);
		vector2.set(i, i % 2 == 1);
	}
    
	storm::storage::BitVector vector3 = vector1 ^ vector2;
	storm::storage::BitVector vector4 = ~vector2;
	storm::storage::BitVector vector5 = vector1 ^ vector1;
    
	for (uint_fast64_t i = 0; i < 32; ++i) {
		ASSERT_EQ(vector3.get(i), vector4.get(i));
		ASSERT_FALSE(vector5.get(i));
	}
}

TEST(BitVectorTest, OperatorModuloTest) {
    storm::storage::BitVector vector1(32);
	storm::storage::BitVector vector2(32);
    
	for (uint_fast64_t i = 0; i < 15; ++i) {
		vector2.set(i, i % 2 == 0);
    }
    
    vector1.set(2);
    vector1.set(5);
    vector1.set(6);
    
    storm::storage::BitVector moduloResult = vector1 % vector2;
    
    ASSERT_EQ(moduloResult.size(), 8);
    ASSERT_EQ(moduloResult.getNumberOfSetBits(), 2);
    
    for (uint_fast64_t i = 0; i < 8; ++i) {
        if (i == 1 || i == 3) {
            ASSERT_TRUE(moduloResult.get(i));
        } else {
            ASSERT_FALSE(moduloResult.get(i));
        }
    }
    
    storm::storage::BitVector vector3(31);
    
    for (uint_fast64_t i = 0; i < 15; ++i) {
		vector3.set(i, i % 2 == 0);
    }
    
    ASSERT_THROW(vector1 % vector3, storm::exceptions::InvalidArgumentException);
}

TEST(BitVectorTest, OperatorNotTest) {
	storm::storage::BitVector vector1(32);
	storm::storage::BitVector vector2(32);

	for (uint_fast64_t i = 0; i < 32; ++i) {
		vector1.set(i, i % 2 == 0);
		vector2.set(i, i % 2 == 1);
	}

	storm::storage::BitVector notResult = ~vector2;

	for (uint_fast64_t i = 0; i < 32; ++i) {
		ASSERT_EQ(vector1.get(i), notResult.get(i));
	}
}

TEST(BitVectorTest, ComplementTest) {
	storm::storage::BitVector vector1(32);
	storm::storage::BitVector vector2(32);
    
	for (uint_fast64_t i = 0; i < 32; ++i) {
		vector1.set(i, i % 2 == 0);
		vector2.set(i, i % 2 == 1);
	}
    
    vector2.complement();
    
	for (uint_fast64_t i = 0; i < 32; ++i) {
		ASSERT_EQ(vector1.get(i), vector2.get(i));
	}
}

TEST(BitVectorTest, ImpliesTest) {
    storm::storage::BitVector vector1(32);
	storm::storage::BitVector vector2(32, true);
    
	for (uint_fast64_t i = 0; i < 32; ++i) {
		vector1.set(i, i % 2 == 0);
	}
    vector2.set(31, false);
    vector2.set(30, false);
    
    storm::storage::BitVector impliesResult = vector1.implies(vector2);
    
    for (uint_fast64_t i = 0; i < 30; ++i) {
        ASSERT_TRUE(impliesResult.get(i));
    }
    ASSERT_FALSE(impliesResult.get(30));
    ASSERT_TRUE(impliesResult.get(31));
}

TEST(BitVectorTest, SubsetTest) {
    storm::storage::BitVector vector1(32);
	storm::storage::BitVector vector2(32, true);
    
	for (uint_fast64_t i = 0; i < 32; ++i) {
		vector1.set(i, i % 2 == 0);
	}

    ASSERT_TRUE(vector1.isSubsetOf(vector2));
                
    vector2.set(16, false);
    
    ASSERT_FALSE(vector1.isSubsetOf(vector2));
}

TEST(BitVectorTest, DisjointTest) {
    storm::storage::BitVector vector1(32);
	storm::storage::BitVector vector2(32);
    
	for (uint_fast64_t i = 0; i < 32; ++i) {
		vector1.set(i, i % 2 == 0);
        vector2.set(i, i % 2 == 1);
	}
    
    ASSERT_TRUE(vector1.isDisjointFrom(vector2));
    
    vector2.set(16, true);
    
    ASSERT_FALSE(vector1.isDisjointFrom(vector2));
}

TEST(BitVectorTest, EmptyTest) {
    storm::storage::BitVector vector(32);
    
    ASSERT_TRUE(vector.empty());
    
    vector.set(17, true);
    
    ASSERT_FALSE(vector.empty());
    
    vector.set(17, false);
    vector.set(18, false);
    
    ASSERT_TRUE(vector.empty());
}

TEST(BitVectorTest, FullTest) {
    storm::storage::BitVector vector(32, true);
    
    ASSERT_TRUE(vector.full());
    
    vector.set(17, false);
    
    ASSERT_FALSE(vector.full());
    
    vector.set(17, true);
    vector.set(18, true);
    
    ASSERT_TRUE(vector.full());
}

TEST(BitVectorTest, NumberOfSetBitsTest) {
    storm::storage::BitVector vector(32);
    
    for (uint_fast64_t i = 0; i < 32; ++i) {
		vector.set(i, i % 2 == 0);
	}

    ASSERT_EQ(vector.getNumberOfSetBits(), 16);
}

TEST(BitVectorTest, NumberOfSetBitsBeforeIndexTest) {
    storm::storage::BitVector vector(32);
    
    for (uint_fast64_t i = 0; i < 32; ++i) {
		vector.set(i, i % 2 == 0);
	}
    
    ASSERT_EQ(vector.getNumberOfSetBitsBeforeIndex(14), 7);
}

TEST(BitVectorTest, BeginEndTest) {
    storm::storage::BitVector vector(32);
    
    ASSERT_TRUE(vector.begin() == vector.end());
    
    vector.set(17);
    
    ASSERT_FALSE(vector.begin() == vector.end());
    
    vector.set(17, false);

    ASSERT_TRUE(vector.begin() == vector.end());
}

TEST(BitVectorTest, NextSetIndexTest) {
    storm::storage::BitVector vector(32);
    
    vector.set(14);
    vector.set(17);
    
    ASSERT_EQ(vector.getNextSetIndex(14), 14);
    ASSERT_EQ(vector.getNextSetIndex(15), 17);
    ASSERT_EQ(vector.getNextSetIndex(16), 17);
    ASSERT_EQ(vector.getNextSetIndex(17), 17);
    ASSERT_EQ(vector.getNextSetIndex(18), vector.size());
}

TEST(BitVectorTest, IteratorTest) {
    storm::storage::BitVector vector(32);
    
    for (uint_fast64_t i = 0; i < 32; ++i) {
		vector.set(i, i % 2 == 0);
	}
    
    for (auto bit : vector) {
        ASSERT_TRUE(bit % 2 == 0);
    }
}