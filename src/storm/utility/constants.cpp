#include "storm/utility/constants.h"

#include <cmath>
#include <type_traits>

#include "storm/storage/SparseMatrix.h"
#include "storm/storage/sparse/StateType.h"

#include "storm/exceptions/InvalidArgumentException.h"

#include "storm/adapters/RationalFunctionAdapter.h"

#include "storm/utility/NumberTraits.h"

#include "storm/exceptions/NotSupportedException.h"
#include "storm/utility/macros.h"

namespace storm {
namespace utility {

template<typename ValueType>
ValueType one() {
    return ValueType(1);
}

template<typename ValueType>
ValueType zero() {
    return ValueType(0);
}

template<typename ValueType>
ValueType infinity() {
    return std::numeric_limits<ValueType>::infinity();
}

template<typename ValueType>
bool isOne(ValueType const& a) {
    return a == one<ValueType>();
}

template<typename ValueType>
bool isZero(ValueType const& a) {
    return a == zero<ValueType>();
}

template<typename ValueType>
bool isNan(ValueType const&) {
    return false;
}

template<>
bool isNan(double const& value) {
    return std::isnan(value);
}

template<typename ValueType>
bool isAlmostZero(ValueType const& a) {
    return a < convertNumber<ValueType>(1e-12) && a > -convertNumber<ValueType>(1e-12);
}

template<typename ValueType>
bool isAlmostOne(ValueType const& a) {
    return a < convertNumber<ValueType>(1.0 + 1e-12) && a > convertNumber<ValueType>(1.0 - 1e-12);
}

template<typename ValueType>
bool isConstant(ValueType const&) {
    return true;
}

template<typename ValueType>
bool isInfinity(ValueType const& a) {
    return a == infinity<ValueType>();
}

template<typename ValueType>
bool isInteger(ValueType const& number) {
    ValueType iPart;
    ValueType result = std::modf(number, &iPart);
    return result == zero<ValueType>();
}

template<>
bool isInteger(int const&) {
    return true;
}

template<>
bool isInteger(uint32_t const&) {
    return true;
}

template<>
bool isInteger(storm::storage::sparse::state_type const&) {
    return true;
}

template<typename TargetType, typename SourceType>
TargetType convertNumber(SourceType const& number) {
    return static_cast<TargetType>(number);
}

template<>
uint_fast64_t convertNumber(double const& number) {
    return std::llround(number);
}

template<>
int_fast64_t convertNumber(double const& number) {
    return std::llround(number);
}

template<>
double convertNumber(uint_fast64_t const& number) {
    return number;
}

template<>
double convertNumber(double const& number) {
    return number;
}

template<>
double convertNumber(long long const& number) {
    return static_cast<double>(number);
}

template<>
storm::storage::sparse::state_type convertNumber(long long const& number) {
    return static_cast<storm::storage::sparse::state_type>(number);
}

template<typename ValueType>
ValueType simplify(ValueType value) {
    // In the general case, we don't do anything here, but merely return the value. If something else is
    // supposed to happen here, the templated function can be specialized for this particular type.
    return value;
}

template<typename IndexType, typename ValueType>
storm::storage::MatrixEntry<IndexType, ValueType> simplify(storm::storage::MatrixEntry<IndexType, ValueType> matrixEntry) {
    simplify(matrixEntry.getValue());
    return matrixEntry;
}

template<typename IndexType, typename ValueType>
storm::storage::MatrixEntry<IndexType, ValueType>& simplify(storm::storage::MatrixEntry<IndexType, ValueType>& matrixEntry) {
    simplify(matrixEntry.getValue());
    return matrixEntry;
}

template<typename IndexType, typename ValueType>
storm::storage::MatrixEntry<IndexType, ValueType>&& simplify(storm::storage::MatrixEntry<IndexType, ValueType>&& matrixEntry) {
    simplify(matrixEntry.getValue());
    return std::move(matrixEntry);
}

template<typename ValueType>
std::pair<ValueType, ValueType> minmax(std::vector<ValueType> const& values) {
    assert(!values.empty());
    ValueType min = values.front();
    ValueType max = values.front();
    for (auto const& vt : values) {
        if (vt < min) {
            min = vt;
        }
        if (vt > max) {
            max = vt;
        }
    }
    return std::make_pair(min, max);
}

template<typename ValueType>
ValueType minimum(std::vector<ValueType> const& values) {
    assert(!values.empty());
    ValueType min = values.front();
    for (auto const& vt : values) {
        if (vt < min) {
            min = vt;
        }
    }
    return min;
}

template<typename ValueType>
ValueType maximum(std::vector<ValueType> const& values) {
    assert(!values.empty());
    ValueType max = values.front();
    for (auto const& vt : values) {
        if (vt > max) {
            max = vt;
        }
    }
    return max;
}

template<typename K, typename ValueType>
std::pair<ValueType, ValueType> minmax(std::map<K, ValueType> const& values) {
    assert(!values.empty());
    ValueType min = values.begin()->second;
    ValueType max = values.begin()->second;
    for (auto const& vt : values) {
        if (vt.second < min) {
            min = vt.second;
        }
        if (vt.second > max) {
            max = vt.second;
        }
    }
    return std::make_pair(min, max);
}

template<typename K, typename ValueType>
ValueType minimum(std::map<K, ValueType> const& values) {
    return minmax(values).first;
}

template<typename K, typename ValueType>
ValueType maximum(std::map<K, ValueType> const& values) {
    return minmax(values).second;
}

template<typename ValueType>
ValueType pow(ValueType const& value, int_fast64_t exponent) {
    return std::pow(value, exponent);
}

template<typename ValueType>
ValueType max(ValueType const& first, ValueType const& second) {
    return std::max(first, second);
}

template<typename ValueType>
ValueType min(ValueType const& first, ValueType const& second) {
    return std::min(first, second);
}

template<typename ValueType>
ValueType sqrt(ValueType const& number) {
    return std::sqrt(number);
}

template<typename ValueType>
ValueType abs(ValueType const& number) {
    return std::fabs(number);
}

template<typename ValueType>
ValueType floor(ValueType const& number) {
    return std::floor(number);
}

template<typename ValueType>
ValueType ceil(ValueType const& number) {
    return std::ceil(number);
}

template<typename ValueType>
ValueType round(ValueType const& number) {
    // Rounding towards infinity
    return floor<ValueType>(number + storm::utility::convertNumber<ValueType>(0.5));
}

template<typename ValueType>
ValueType log(ValueType const& number) {
    return std::log(number);
}

template<typename ValueType>
ValueType log10(ValueType const& number) {
    return std::log10(number);
}

template<typename ValueType>
typename NumberTraits<ValueType>::IntegerType trunc(ValueType const& number) {
    return static_cast<typename NumberTraits<ValueType>::IntegerType>(std::trunc(number));
}

template<typename IntegerType>
IntegerType mod(IntegerType const& first, IntegerType const& second) {
    return std::fmod(first, second);
}

template<typename IntegerType>
std::pair<IntegerType, IntegerType> divide(IntegerType const& dividend, IntegerType const& divisor) {
    return std::make_pair(dividend / divisor, mod(dividend, divisor));
}

template<typename ValueType>
std::string to_string(ValueType const& value) {
    std::stringstream ss;
    ss.precision(std::numeric_limits<ValueType>::max_digits10 + 2);
    ss << value;
    return ss.str();
}

#if defined(STORM_HAVE_CARL) && defined(STORM_HAVE_CLN)
template<>
storm::ClnRationalNumber infinity() {
    // FIXME: this should be treated more properly.
    return storm::ClnRationalNumber(100000000000);
}

template<>
bool isOne(storm::ClnRationalNumber const& a) {
    return carl::isOne(a);
}

template<>
bool isZero(storm::ClnRationalNumber const& a) {
    return carl::isZero(a);
}

template<>
bool isInteger(storm::ClnRationalNumber const& number) {
    return carl::isInteger(number);
}

template<>
std::pair<storm::ClnRationalNumber, storm::ClnRationalNumber> minmax(std::vector<storm::ClnRationalNumber> const& values) {
    assert(!values.empty());
    storm::ClnRationalNumber min = values.front();
    storm::ClnRationalNumber max = values.front();
    for (auto const& vt : values) {
        if (vt == storm::utility::infinity<storm::ClnRationalNumber>()) {
            max = vt;
        } else {
            if (vt < min) {
                min = vt;
            }
            if (vt > max) {
                max = vt;
            }
        }
    }
    return std::make_pair(min, max);
}

template<>
uint_fast64_t convertNumber(ClnRationalNumber const& number) {
    return carl::toInt<carl::uint>(number);
}

template<>
int_fast64_t convertNumber(ClnRationalNumber const& number) {
    return carl::toInt<carl::sint>(number);
}

template<>
ClnRationalNumber convertNumber(double const& number) {
    return carl::rationalize<ClnRationalNumber>(number);
}

template<>
ClnRationalNumber convertNumber(int const& number) {
    return carl::rationalize<ClnRationalNumber>(number);
}

template<>
ClnRationalNumber convertNumber(NumberTraits<ClnRationalNumber>::IntegerType const& number) {
    return ClnRationalNumber(number);
}

template<>
ClnRationalNumber convertNumber(uint_fast64_t const& number) {
    STORM_LOG_ASSERT(static_cast<carl::uint>(number) == number, "Rationalizing failed, because the number is too large.");
    return carl::rationalize<ClnRationalNumber>(static_cast<carl::uint>(number));
}

template<>
typename NumberTraits<ClnRationalNumber>::IntegerType convertNumber(uint_fast64_t const& number) {
    STORM_LOG_ASSERT(static_cast<unsigned long int>(number) == number, "Conversion failed, because the number is too large.");
    return NumberTraits<ClnRationalNumber>::IntegerType(static_cast<unsigned long int>(number));
}

template<>
typename NumberTraits<ClnRationalNumber>::IntegerType convertNumber(double const& number) {
    if (number < static_cast<double>(std::numeric_limits<uint64_t>::max())) {
        return NumberTraits<ClnRationalNumber>::IntegerType(static_cast<uint64_t>(number));
    } else {
        return carl::round(carl::rationalize<ClnRationalNumber>(number));
    }
}

template<>
ClnRationalNumber convertNumber(int_fast64_t const& number) {
    STORM_LOG_ASSERT(static_cast<carl::sint>(number) == number, "Rationalizing failed, because the number is too large.");
    return carl::rationalize<ClnRationalNumber>(static_cast<carl::sint>(number));
}

template<>
double convertNumber(ClnRationalNumber const& number) {
    return carl::toDouble(number);
}

template<>
ClnRationalNumber convertNumber(std::string const& number) {
    ClnRationalNumber result;
    if (carl::try_parse<ClnRationalNumber>(number, result)) {
        return result;
    }
    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Unable to parse '" << number << "' as a rational number.");
}

template<>
std::pair<ClnRationalNumber, ClnRationalNumber> asFraction(ClnRationalNumber const& number) {
    return std::make_pair(carl::getNum(number), carl::getDenom(number));
}

template<>
ClnRationalNumber sqrt(ClnRationalNumber const& number) {
    return carl::sqrt(number);
}

template<>
ClnRationalNumber abs(storm::ClnRationalNumber const& number) {
    return carl::abs(number);
}

template<>
ClnRationalNumber floor(storm::ClnRationalNumber const& number) {
    return carl::floor(number);
}

template<>
ClnRationalNumber ceil(storm::ClnRationalNumber const& number) {
    return carl::ceil(number);
}

template<>
ClnRationalNumber log(ClnRationalNumber const& number) {
    return carl::log(number);
}

template<>
ClnRationalNumber log10(ClnRationalNumber const& number) {
    return carl::log10(number);
}

template<>
typename NumberTraits<ClnRationalNumber>::IntegerType trunc(ClnRationalNumber const& number) {
    return cln::truncate1(number);
}

template<>
typename NumberTraits<ClnRationalNumber>::IntegerType mod(NumberTraits<ClnRationalNumber>::IntegerType const& first,
                                                          NumberTraits<ClnRationalNumber>::IntegerType const& second) {
    return carl::mod(first, second);
}

template<>
std::pair<typename NumberTraits<ClnRationalNumber>::IntegerType, typename NumberTraits<ClnRationalNumber>::IntegerType> divide(
    typename NumberTraits<ClnRationalNumber>::IntegerType const& dividend, typename NumberTraits<ClnRationalNumber>::IntegerType const& divisor) {
    std::pair<typename NumberTraits<ClnRationalNumber>::IntegerType, typename NumberTraits<ClnRationalNumber>::IntegerType> result;
    carl::divide(dividend, divisor, result.first, result.second);
    return result;
}

template<>
typename NumberTraits<ClnRationalNumber>::IntegerType pow(typename NumberTraits<ClnRationalNumber>::IntegerType const& value, int_fast64_t exponent) {
    STORM_LOG_THROW(exponent >= 0, storm::exceptions::InvalidArgumentException,
                    "Tried to compute the power 'x^y' as an integer, but the exponent 'y' is negative.");
    return carl::pow(value, exponent);
}

template<>
ClnRationalNumber pow(ClnRationalNumber const& value, int_fast64_t exponent) {
    if (exponent >= 0) {
        return carl::pow(value, exponent);
    } else {
        return storm::utility::one<ClnRationalNumber>() / carl::pow(value, -exponent);
    }
}

template<>
NumberTraits<ClnRationalNumber>::IntegerType numerator(ClnRationalNumber const& number) {
    return carl::getNum(number);
}

template<>
NumberTraits<ClnRationalNumber>::IntegerType denominator(ClnRationalNumber const& number) {
    return carl::getDenom(number);
}
#endif

#if defined(STORM_HAVE_CARL) && defined(STORM_HAVE_GMP)
template<>
storm::GmpRationalNumber infinity() {
    // FIXME: this should be treated more properly.
    return storm::GmpRationalNumber(100000000000);
}

template<>
bool isOne(storm::GmpRationalNumber const& a) {
    return carl::isOne(a);
}

template<>
bool isZero(storm::GmpRationalNumber const& a) {
    return carl::isZero(a);
}

template<>
bool isInteger(storm::GmpRationalNumber const& number) {
    return carl::isInteger(number);
}

template<>
std::pair<storm::GmpRationalNumber, storm::GmpRationalNumber> minmax(std::vector<storm::GmpRationalNumber> const& values) {
    assert(!values.empty());
    storm::GmpRationalNumber min = values.front();
    storm::GmpRationalNumber max = values.front();
    for (auto const& vt : values) {
        if (vt == storm::utility::infinity<storm::GmpRationalNumber>()) {
            max = vt;
        } else {
            if (vt < min) {
                min = vt;
            }
            if (vt > max) {
                max = vt;
            }
        }
    }
    return std::make_pair(min, max);
}

template<>
std::pair<storm::GmpRationalNumber, storm::GmpRationalNumber> minmax(std::map<uint64_t, storm::GmpRationalNumber> const& values) {
    assert(!values.empty());
    storm::GmpRationalNumber min = values.begin()->second;
    storm::GmpRationalNumber max = values.begin()->second;
    for (auto const& vt : values) {
        if (vt.second == storm::utility::infinity<storm::GmpRationalNumber>()) {
            max = vt.second;
        } else {
            if (vt.second < min) {
                min = vt.second;
            }
            if (vt.second > max) {
                max = vt.second;
            }
        }
    }
    return std::make_pair(min, max);
}

template<>
uint_fast64_t convertNumber(GmpRationalNumber const& number) {
    return carl::toInt<carl::uint>(number);
}

template<>
int_fast64_t convertNumber(GmpRationalNumber const& number) {
    return carl::toInt<carl::sint>(number);
}

template<>
GmpRationalNumber convertNumber(double const& number) {
    return carl::rationalize<GmpRationalNumber>(number);
}

template<>
GmpRationalNumber convertNumber(int const& number) {
    return carl::rationalize<GmpRationalNumber>(number);
}

template<>
GmpRationalNumber convertNumber(uint_fast64_t const& number) {
    STORM_LOG_ASSERT(static_cast<carl::uint>(number) == number, "Rationalizing failed, because the number is too large.");
    return carl::rationalize<GmpRationalNumber>(static_cast<carl::uint>(number));
}

template<>
GmpRationalNumber convertNumber(NumberTraits<GmpRationalNumber>::IntegerType const& number) {
    return GmpRationalNumber(number);
}

template<>
typename NumberTraits<GmpRationalNumber>::IntegerType convertNumber(uint_fast64_t const& number) {
    STORM_LOG_ASSERT(static_cast<unsigned long int>(number) == number, "Conversion failed, because the number is too large.");
    return NumberTraits<GmpRationalNumber>::IntegerType(static_cast<unsigned long int>(number));
}

template<>
typename NumberTraits<GmpRationalNumber>::IntegerType convertNumber(double const& number) {
    return NumberTraits<GmpRationalNumber>::IntegerType(number);
}

template<>
GmpRationalNumber convertNumber(int_fast64_t const& number) {
    STORM_LOG_ASSERT(static_cast<carl::sint>(number) == number, "Rationalizing failed, because the number is too large.");
    return carl::rationalize<GmpRationalNumber>(static_cast<carl::sint>(number));
}

template<>
double convertNumber(GmpRationalNumber const& number) {
    return carl::toDouble(number);
}

template<>
GmpRationalNumber convertNumber(std::string const& number) {
    GmpRationalNumber result;
    if (carl::try_parse<GmpRationalNumber>(number, result)) {
        return result;
    }
    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Unable to parse '" << number << "' as a rational number.");
}

template<>
std::pair<GmpRationalNumber, GmpRationalNumber> asFraction(GmpRationalNumber const& number) {
    return std::make_pair(carl::getNum(number), carl::getDenom(number));
}

template<>
GmpRationalNumber sqrt(GmpRationalNumber const& number) {
    return carl::sqrt(number);
}

template<>
GmpRationalNumber abs(storm::GmpRationalNumber const& number) {
    return carl::abs(number);
}

template<>
GmpRationalNumber floor(storm::GmpRationalNumber const& number) {
    return carl::floor(number);
}

template<>
GmpRationalNumber ceil(storm::GmpRationalNumber const& number) {
    return carl::ceil(number);
}

template<>
GmpRationalNumber log(GmpRationalNumber const& number) {
    return carl::log(number);
}

template<>
GmpRationalNumber log10(GmpRationalNumber const& number) {
    return carl::log10(number);
}

template<>
typename NumberTraits<GmpRationalNumber>::IntegerType trunc(GmpRationalNumber const& number) {
    return carl::getNum(number) / carl::getDenom(number);
}

template<>
typename NumberTraits<GmpRationalNumber>::IntegerType mod(typename NumberTraits<GmpRationalNumber>::IntegerType const& first,
                                                          typename NumberTraits<GmpRationalNumber>::IntegerType const& second) {
    return carl::mod(first, second);
}

template<>
std::pair<typename NumberTraits<GmpRationalNumber>::IntegerType, typename NumberTraits<GmpRationalNumber>::IntegerType> divide(
    typename NumberTraits<GmpRationalNumber>::IntegerType const& dividend, typename NumberTraits<GmpRationalNumber>::IntegerType const& divisor) {
    std::pair<typename NumberTraits<GmpRationalNumber>::IntegerType, typename NumberTraits<GmpRationalNumber>::IntegerType> result;
    carl::divide(dividend, divisor, result.first, result.second);
    return result;
}

template<>
typename NumberTraits<GmpRationalNumber>::IntegerType pow(typename NumberTraits<GmpRationalNumber>::IntegerType const& value, int_fast64_t exponent) {
    STORM_LOG_THROW(exponent >= 0, storm::exceptions::InvalidArgumentException,
                    "Tried to compute the power 'x^y' as an integer, but the exponent 'y' is negative.");
    return carl::pow(value, exponent);
}

template<>
GmpRationalNumber pow(GmpRationalNumber const& value, int_fast64_t exponent) {
    if (exponent >= 0) {
        return carl::pow(value, exponent);
    } else {
        return storm::utility::one<GmpRationalNumber>() / carl::pow(value, -exponent);
    }
}

template<>
typename NumberTraits<GmpRationalNumber>::IntegerType numerator(GmpRationalNumber const& number) {
    return carl::getNum(number);
}

template<>
typename NumberTraits<GmpRationalNumber>::IntegerType denominator(GmpRationalNumber const& number) {
    return carl::getDenom(number);
}
#endif

#if defined(STORM_HAVE_CARL) && defined(STORM_HAVE_GMP) && defined(STORM_HAVE_CLN)
template<>
storm::GmpRationalNumber convertNumber(storm::ClnRationalNumber const& number) {
    return carl::parse<storm::GmpRationalNumber>(to_string(number));
}

template<>
storm::ClnRationalNumber convertNumber(storm::GmpRationalNumber const& number) {
    return carl::parse<storm::ClnRationalNumber>(to_string(number));
}
#endif

#ifdef STORM_HAVE_CARL
template<>
storm::RationalFunction infinity() {
    // FIXME: this should be treated more properly.
    return storm::RationalFunction(convertNumber<RationalFunctionCoefficient>(100000000000));
}

template<>
bool isOne(storm::RationalFunction const& a) {
    return a.isOne();
}

template<>
bool isOne(storm::Polynomial const& a) {
    return a.isOne();
}

template<>
bool isZero(storm::RationalFunction const& a) {
    return a.isZero();
}

template<>
bool isZero(storm::Polynomial const& a) {
    return a.isZero();
}

template<>
bool isConstant(storm::RationalFunction const& a) {
    return a.isConstant();
}

template<>
bool isConstant(storm::Polynomial const& a) {
    return a.isConstant();
}

template<>
bool isInfinity(storm::RationalFunction const& a) {
    // FIXME: this should be treated more properly.
    return a == infinity<storm::RationalFunction>();
}

template<>
bool isInteger(storm::RationalFunction const& func) {
    return storm::utility::isConstant(func) && storm::utility::isOne(func.denominator());
}

template<>
RationalFunction convertNumber(double const& number) {
    return RationalFunction(carl::rationalize<RationalFunctionCoefficient>(number));
}

template<>
RationalFunction convertNumber(int_fast64_t const& number) {
    STORM_LOG_ASSERT(static_cast<carl::sint>(number) == number, "Rationalizing failed, because the number is too large.");
    return RationalFunction(carl::rationalize<RationalFunctionCoefficient>(static_cast<carl::sint>(number)));
}

#if defined(STORM_HAVE_CLN)
template<>
RationalFunction convertNumber(ClnRationalNumber const& number) {
    return RationalFunction(convertNumber<storm::RationalFunctionCoefficient>(number));
}

template<>
ClnRationalNumber convertNumber(RationalFunction const& number) {
    storm::RationalFunctionCoefficient tmp = number.nominatorAsNumber() / number.denominatorAsNumber();
    return convertNumber<ClnRationalNumber>(tmp);
}
#endif

#if defined(STORM_HAVE_GMP)
template<>
RationalFunction convertNumber(GmpRationalNumber const& number) {
    return RationalFunction(convertNumber<storm::RationalFunctionCoefficient>(number));
}

template<>
GmpRationalNumber convertNumber(RationalFunction const& number) {
    return convertNumber<GmpRationalNumber>(number.nominatorAsNumber() / number.denominatorAsNumber());
}
#endif

template<>
carl::uint convertNumber(RationalFunction const& func) {
    return carl::toInt<carl::uint>(convertNumber<RationalFunctionCoefficient>(func));
}

template<>
carl::sint convertNumber(RationalFunction const& func) {
    return carl::toInt<carl::sint>(convertNumber<RationalFunctionCoefficient>(func));
}

template<>
double convertNumber(RationalFunction const& func) {
    return carl::toDouble(convertNumber<RationalFunctionCoefficient>(func));
}

template<>
RationalFunction convertNumber(RationalFunction const& number) {
    return number;
}

template<>
RationalFunction convertNumber(std::string const& number) {
    return RationalFunction(convertNumber<RationalFunctionCoefficient>(number));
}

template<>
RationalFunction convertNumber(storm::storage::sparse::state_type const& number) {
    return RationalFunction(convertNumber<RationalFunctionCoefficient>(number));
}

template<>
RationalFunction& simplify(RationalFunction& value);

template<>
RationalFunction&& simplify(RationalFunction&& value);

template<>
RationalFunction simplify(RationalFunction value) {
    value.simplify();
    return value;
}

template<>
RationalFunction& simplify(RationalFunction& value) {
    value.simplify();
    return value;
}

template<>
RationalFunction&& simplify(RationalFunction&& value) {
    value.simplify();
    return std::move(value);
}

template<>
bool isAlmostZero(storm::RationalFunction const& a) {
    return a.isConstant() && isAlmostZero(convertNumber<RationalFunctionCoefficient>(a));
}

template<>
bool isAlmostOne(storm::RationalFunction const& a) {
    return a.isConstant() && isAlmostOne(convertNumber<RationalFunctionCoefficient>(a));
}

template<>
std::pair<storm::RationalFunction, storm::RationalFunction> minmax(std::vector<storm::RationalFunction> const&) {
    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Minimum/maximum for rational functions is not defined.");
}

template<>
storm::RationalFunction minimum(std::vector<storm::RationalFunction> const&) {
    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Minimum for rational functions is not defined.");
}

template<>
storm::RationalFunction maximum(std::vector<storm::RationalFunction> const&) {
    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Maximum for rational functions is not defined.");
}

template<>
std::pair<storm::RationalFunction, storm::RationalFunction> minmax(std::map<uint64_t, storm::RationalFunction> const&) {
    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Maximum/maximum for rational functions is not defined.");
}

template<>
storm::RationalFunction minimum(std::map<uint64_t, storm::RationalFunction> const&) {
    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Minimum for rational functions is not defined.");
}

template<>
storm::RationalFunction maximum(std::map<uint64_t, storm::RationalFunction> const&) {
    STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Maximum for rational functions is not defined");
}

template<>
RationalFunction pow(RationalFunction const& value, int_fast64_t exponent) {
    if (exponent >= 0) {
        return carl::pow(value, exponent);
    } else {
        return storm::utility::one<RationalFunction>() / carl::pow(value, -exponent);
    }
}

template<>
std::string to_string(RationalFunction const& f) {
    std::stringstream ss;
    if (f.isConstant()) {
        if (f.denominator().isOne()) {
            ss << f.nominatorAsNumber();
        } else {
            ss << f.nominatorAsNumber() << "/" << f.denominatorAsNumber();
        }
    } else if (f.denominator().isOne()) {
        ss << f.nominatorAsPolynomial().coefficient() * f.nominatorAsPolynomial().polynomial();
    } else {
        ss << "(" << f.nominatorAsPolynomial() << ")/(" << f.denominatorAsPolynomial() << ")";
    }
    return ss.str();
}

#endif

template<>
double convertNumber(std::string const& value) {
    return convertNumber<double>(convertNumber<storm::RationalNumber>(value));
}

// Explicit instantiations.

// double
template double one();
template double zero();
template double infinity();
template bool isOne(double const& value);
template bool isZero(double const& value);
template bool isAlmostZero(double const& value);
template bool isAlmostOne(double const& value);
template bool isConstant(double const& value);
template bool isInfinity(double const& value);
template bool isInteger(double const& number);
template double simplify(double value);
template storm::storage::MatrixEntry<storm::storage::sparse::state_type, double> simplify(
    storm::storage::MatrixEntry<storm::storage::sparse::state_type, double> matrixEntry);
template storm::storage::MatrixEntry<storm::storage::sparse::state_type, double>& simplify(
    storm::storage::MatrixEntry<storm::storage::sparse::state_type, double>& matrixEntry);
template storm::storage::MatrixEntry<storm::storage::sparse::state_type, double>&& simplify(
    storm::storage::MatrixEntry<storm::storage::sparse::state_type, double>&& matrixEntry);
template std::pair<double, double> minmax(std::vector<double> const&);
template double minimum(std::vector<double> const&);
template double maximum(std::vector<double> const&);
template std::pair<double, double> minmax(std::map<uint64_t, double> const&);
template double minimum(std::map<uint64_t, double> const&);
template double maximum(std::map<uint64_t, double> const&);
template double pow(double const& value, int_fast64_t exponent);
template double max(double const& first, double const& second);
template double min(double const& first, double const& second);
template double sqrt(double const& number);
template double abs(double const& number);
template double floor(double const& number);
template double ceil(double const& number);
template double round(double const& number);
template double log(double const& number);
template double log10(double const& number);
template typename NumberTraits<double>::IntegerType trunc(double const& number);
template double mod(double const& first, double const& second);
template std::string to_string(double const& value);

// int
template int one();
template int zero();
template int infinity();
template bool isOne(int const& value);
template bool isZero(int const& value);
template bool isConstant(int const& value);
template bool isInfinity(int const& value);

// uint32_t
template uint32_t one();
template uint32_t zero();
template uint32_t infinity();
template bool isOne(uint32_t const& value);
template bool isZero(uint32_t const& value);
template bool isConstant(uint32_t const& value);
template bool isInfinity(uint32_t const& value);

// storm::storage::sparse::state_type
template storm::storage::sparse::state_type one();
template storm::storage::sparse::state_type zero();
template storm::storage::sparse::state_type infinity();
template bool isOne(storm::storage::sparse::state_type const& value);
template bool isZero(storm::storage::sparse::state_type const& value);
template bool isConstant(storm::storage::sparse::state_type const& value);
template bool isInfinity(storm::storage::sparse::state_type const& value);

// other instantiations
template unsigned long convertNumber(long const&);
template double convertNumber(long const&);

#if defined(STORM_HAVE_CLN)
// Instantiations for (CLN) rational number.
template storm::ClnRationalNumber one();
template NumberTraits<storm::ClnRationalNumber>::IntegerType one();
template storm::ClnRationalNumber zero();
template bool isZero(NumberTraits<storm::ClnRationalNumber>::IntegerType const& value);
template bool isConstant(storm::ClnRationalNumber const& value);
template bool isInfinity(storm::ClnRationalNumber const& value);
template bool isNan(storm::ClnRationalNumber const& value);
template bool isAlmostZero(storm::ClnRationalNumber const& value);
template bool isAlmostOne(storm::ClnRationalNumber const& value);
template storm::NumberTraits<ClnRationalNumber>::IntegerType convertNumber(storm::NumberTraits<ClnRationalNumber>::IntegerType const& number);
template storm::ClnRationalNumber convertNumber(storm::ClnRationalNumber const& number);
template storm::ClnRationalNumber simplify(storm::ClnRationalNumber value);
template storm::storage::MatrixEntry<storm::storage::sparse::state_type, storm::ClnRationalNumber> simplify(
    storm::storage::MatrixEntry<storm::storage::sparse::state_type, storm::ClnRationalNumber> matrixEntry);
template storm::storage::MatrixEntry<storm::storage::sparse::state_type, storm::ClnRationalNumber>& simplify(
    storm::storage::MatrixEntry<storm::storage::sparse::state_type, storm::ClnRationalNumber>& matrixEntry);
template storm::storage::MatrixEntry<storm::storage::sparse::state_type, storm::ClnRationalNumber>&& simplify(
    storm::storage::MatrixEntry<storm::storage::sparse::state_type, storm::ClnRationalNumber>&& matrixEntry);
template std::pair<storm::ClnRationalNumber, storm::ClnRationalNumber> minmax(std::map<uint64_t, storm::ClnRationalNumber> const&);
template storm::ClnRationalNumber minimum(std::map<uint64_t, storm::ClnRationalNumber> const&);
template storm::ClnRationalNumber maximum(std::map<uint64_t, storm::ClnRationalNumber> const&);
template storm::ClnRationalNumber minimum(std::vector<storm::ClnRationalNumber> const&);
template storm::ClnRationalNumber maximum(std::vector<storm::ClnRationalNumber> const&);
template storm::ClnRationalNumber max(storm::ClnRationalNumber const& first, storm::ClnRationalNumber const& second);
template storm::ClnRationalNumber min(storm::ClnRationalNumber const& first, storm::ClnRationalNumber const& second);
template storm::ClnRationalNumber round(storm::ClnRationalNumber const& number);
template std::string to_string(storm::ClnRationalNumber const& value);
#endif

#if defined(STORM_HAVE_GMP)
// Instantiations for (GMP) rational number.
template storm::GmpRationalNumber one();
template NumberTraits<storm::GmpRationalNumber>::IntegerType one();
template storm::GmpRationalNumber zero();
template bool isZero(NumberTraits<storm::GmpRationalNumber>::IntegerType const& value);
template bool isConstant(storm::GmpRationalNumber const& value);
template bool isInfinity(storm::GmpRationalNumber const& value);
template bool isNan(storm::GmpRationalNumber const& value);
template bool isAlmostZero(storm::GmpRationalNumber const& value);
template bool isAlmostOne(storm::GmpRationalNumber const& value);
template storm::NumberTraits<GmpRationalNumber>::IntegerType convertNumber(storm::NumberTraits<GmpRationalNumber>::IntegerType const& number);
template storm::GmpRationalNumber convertNumber(storm::GmpRationalNumber const& number);
template storm::GmpRationalNumber simplify(storm::GmpRationalNumber value);
template storm::storage::MatrixEntry<storm::storage::sparse::state_type, storm::GmpRationalNumber> simplify(
    storm::storage::MatrixEntry<storm::storage::sparse::state_type, storm::GmpRationalNumber> matrixEntry);
template storm::storage::MatrixEntry<storm::storage::sparse::state_type, storm::GmpRationalNumber>& simplify(
    storm::storage::MatrixEntry<storm::storage::sparse::state_type, storm::GmpRationalNumber>& matrixEntry);
template storm::storage::MatrixEntry<storm::storage::sparse::state_type, storm::GmpRationalNumber>&& simplify(
    storm::storage::MatrixEntry<storm::storage::sparse::state_type, storm::GmpRationalNumber>&& matrixEntry);
template storm::GmpRationalNumber minimum(std::map<uint64_t, storm::GmpRationalNumber> const&);
template storm::GmpRationalNumber maximum(std::map<uint64_t, storm::GmpRationalNumber> const&);
template storm::GmpRationalNumber minimum(std::vector<storm::GmpRationalNumber> const&);
template storm::GmpRationalNumber maximum(std::vector<storm::GmpRationalNumber> const&);
template storm::GmpRationalNumber max(storm::GmpRationalNumber const& first, storm::GmpRationalNumber const& second);
template storm::GmpRationalNumber min(storm::GmpRationalNumber const& first, storm::GmpRationalNumber const& second);
template storm::GmpRationalNumber round(storm::GmpRationalNumber const& number);
template std::string to_string(storm::GmpRationalNumber const& value);
#endif

#if defined(STORM_HAVE_CARL) && defined(STORM_HAVE_GMP) && defined(STORM_HAVE_CLN)
#endif

#ifdef STORM_HAVE_CARL
// Instantiations for rational function.
template RationalFunction one();
template RationalFunction zero();
template storm::storage::MatrixEntry<storm::storage::sparse::state_type, RationalFunction> simplify(
    storm::storage::MatrixEntry<storm::storage::sparse::state_type, RationalFunction> matrixEntry);
template storm::storage::MatrixEntry<storm::storage::sparse::state_type, RationalFunction>& simplify(
    storm::storage::MatrixEntry<storm::storage::sparse::state_type, RationalFunction>& matrixEntry);
template storm::storage::MatrixEntry<storm::storage::sparse::state_type, RationalFunction>&& simplify(
    storm::storage::MatrixEntry<storm::storage::sparse::state_type, RationalFunction>&& matrixEntry);

// Instantiations for polynomials.
template Polynomial one();
template Polynomial zero();

// Instantiations for intervals.
template Interval one();
template Interval zero();
template bool isOne(Interval const& value);
template bool isZero(Interval const& value);
template bool isConstant(Interval const& value);
template bool isInfinity(Interval const& value);
#endif

}  // namespace utility
}  // namespace storm
