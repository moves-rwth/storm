#include <limits>
#include <span>
#include <vector>

#include "storm/storage/umb/model/ValueEncoding.h"

#include "storm/adapters/RationalNumberAdapter.h"
#include "storm/utility/constants.h"
#include "test/storm_gtest.h"

TEST(UmbTest, RationalEncoding) {
    auto const one = storm::utility::one<storm::RationalNumber>();
    auto const int64max = storm::utility::convertNumber<storm::RationalNumber, int64_t>(std::numeric_limits<int64_t>::max());
    auto const int64min = storm::utility::convertNumber<storm::RationalNumber, int64_t>(std::numeric_limits<int64_t>::min());
    auto const uint64max = storm::utility::convertNumber<storm::RationalNumber, uint64_t>(std::numeric_limits<uint64_t>::max());

    std::vector<storm::RationalNumber> values(17);
    // The first 8 values are chosen such that they can be represented with two 64-bit numbers.
    values[0] = storm::utility::zero<storm::RationalNumber>();
    values[1] = -storm::utility::zero<storm::RationalNumber>();
    values[2] = one;
    values[3] = -one;
    values[4] = storm::utility::convertNumber<storm::RationalNumber, std::string>("123/456");
    values[5] = -storm::utility::convertNumber<storm::RationalNumber, std::string>("123/456");
    values[6] = int64max / uint64max;
    values[7] = int64min / uint64max;

    auto const simpleRationals = std::span<storm::RationalNumber>(values.data(), 8);
    ASSERT_EQ(128ull, storm::umb::ValueEncoding::getMinimalRationalSize(simpleRationals, false));
    ASSERT_EQ(128ull, storm::umb::ValueEncoding::getMinimalRationalSize(simpleRationals, true));
    auto encoded1 = storm::umb::ValueEncoding::createUint64FromRationalRange(simpleRationals, 128ull);
    auto decoded1 = storm::umb::ValueEncoding::uint64ToRationalRangeView(encoded1, 128ull);
    ASSERT_EQ(simpleRationals.size(), decoded1.size());
    for (size_t i = 0; i < simpleRationals.size(); ++i) {
        EXPECT_EQ(simpleRationals[i], decoded1[i]) << " at index " << i;
    }

    // The following values are chosen such that they are not representable with two 64-bit numbers.
    values[8] = int64max + one;
    values[9] = one / (uint64max + one);
    values[10] = int64min - one;
    values[11] = one / (int64min - one);
    values[12] = (int64min - one) / (uint64max + one);
    values[13] = storm::utility::convertNumber<storm::RationalNumber, std::string>(
        "949667607787274453086419753000949667607787274453086419753000949667607787274453086419753000949667607787274453086419753000949667607787274453086419753000"
        "9496676077872744530864197530009496676077872744530864197530009496676077872744530864197530/"
        "780116505469339517040847240228739241622101546262265311616467711470010820006007800398204693387501962318501358930877102188539546463329577703105788853954"
        "134811616465520508472358467546262155762385699576193087775947700108638258539546825539241654967");
    values[14] = one / values[13];
    values[15] = -values[13];
    values[16] = -values[14];

    ASSERT_EQ(1616ull, storm::umb::ValueEncoding::getMinimalRationalSize(values, false));
    ASSERT_EQ(1664ull, storm::umb::ValueEncoding::getMinimalRationalSize(values, true));
    auto encoded2 = storm::umb::ValueEncoding::createUint64FromRationalRange(values, 1664ull);
    auto decoded2 = storm::umb::ValueEncoding::uint64ToRationalRangeView(encoded2, 1664ull);

    ASSERT_EQ(values.size(), decoded2.size());
    for (size_t i = 0; i < values.size(); ++i) {
        EXPECT_EQ(values[i], decoded2[i]) << " at index " << i;
    }
}
