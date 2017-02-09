#include "storm/utility/constants.h"

#include "storm/storage/sparse/StateType.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/GeneralSettings.h"

#include "storm/exceptions/InvalidArgumentException.h"

#include "storm/adapters/CarlAdapter.h"
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
        bool isConstant(ValueType const&) {
            return true;
        }

        template<typename ValueType>
        bool isInteger(ValueType const& number) {
            ValueType iPart;
            ValueType result = std::modf(number, &iPart);
            return result == zero<ValueType>();
        }
        
        template<typename ValueType>
        bool isInfinity(ValueType const& a) {
            return a == infinity<ValueType>();
        }

        template<>
        bool isInteger(int const&) {
            return true;
        }

        template<>
        bool isInteger(uint_fast64_t const&) {
            return true;
        }
        
        template<typename ValueType>
        std::string to_string(ValueType const& value) {
            std::stringstream ss;
            ss << value;
            return ss.str();
        }
        
        template<>
        std::string to_string(RationalFunction const& f) {
            std::stringstream ss;
            if (f.isConstant())  {
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
 
#ifdef STORM_HAVE_CARL
        template<>
        bool isOne(storm::RationalNumber const& a) {
            return carl::isOne(a);
        }

        template<>
        bool isZero(storm::RationalNumber const& a) {
            return carl::isZero(a);
        }
        template<>
        bool isOne(storm::RationalFunction const& a) {
            return a.isOne();
        }

        template<>
        bool isZero(storm::RationalFunction const& a) {
            return a.isZero();
        }

        template<>
        bool isConstant(storm::RationalFunction const& a) {
            return a.isConstant();
        }

        template<>
        bool isOne(storm::Polynomial const& a) {
            return a.isOne();
        }

        template<>
        bool isZero(storm::Polynomial const& a) {
            return a.isZero();
        }

        template<>
        bool isConstant(storm::Polynomial const& a) {
            return a.isConstant();
        }

        template<>
        storm::RationalFunction infinity() {
            // FIXME: this should be treated more properly.
            return storm::RationalFunction(-1.0);
        }

        template<>
        bool isInfinity(storm::RationalFunction const& a) {
            // FIXME: this should be treated more properly.
            return a == infinity<storm::RationalFunction>();
        }

        template<>
        storm::RationalNumber infinity() {
        	// FIXME: this should be treated more properly.
        	return storm::RationalNumber(-1);
        }
        
        template<>
        bool isInteger(storm::RationalNumber const& number) {
            return carl::isInteger(number);
        }

        template<>
        bool isInteger(storm::RationalFunction const& func) {
            return storm::utility::isConstant(func) && storm::utility::isOne(func.denominator());
        }
#endif
        
        template<typename ValueType>
        ValueType pow(ValueType const& value, uint_fast64_t exponent) {
            return std::pow(value, exponent);
        }

        template<typename ValueType>
        ValueType simplify(ValueType value) {
            // In the general case, we don't do anything here, but merely return the value. If something else is
            // supposed to happen here, the templated function can be specialized for this particular type.
            return value;
        }
        
        template<>
        double convertNumber(double const& number){
            return number;
        }
        
        template<>
        uint_fast64_t convertNumber(double const& number){
            return std::llround(number);
        }
        
        template<>
        double convertNumber(uint_fast64_t const& number){
            return number;
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
        
        template<>
        std::pair<storm::RationalFunction, storm::RationalFunction> minmax(std::vector<storm::RationalFunction> const&) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Minimum/maximum for rational functions is not defined.");
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
        
        template<>
        std::pair<storm::RationalNumber, storm::RationalNumber> minmax(std::vector<storm::RationalNumber> const& values) {
            assert(!values.empty());
            storm::RationalNumber min = values.front();
            storm::RationalNumber max = values.front();
            for (auto const& vt : values) {
                if (vt == storm::utility::infinity<storm::RationalNumber>()) {
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
        storm::RationalFunction minimum(std::vector<storm::RationalFunction> const&) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Minimum for rational functions is not defined.");
        }

        template<typename ValueType>
        ValueType minimum(std::vector<ValueType> const& values) {
            return minmax(values).first;
        }
        
        template<>
        storm::RationalFunction maximum(std::vector<storm::RationalFunction> const&) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Maximum for rational functions is not defined.");
        }
        
        template<typename ValueType>
        ValueType maximum(std::vector<ValueType> const& values) {
            return minmax(values).second;
        }

        template<>
        std::pair<storm::RationalFunction, storm::RationalFunction> minmax(std::map<uint64_t, storm::RationalFunction> const&) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Maximum/maximum for rational functions is not defined.");
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

        template<>
        std::pair<storm::RationalNumber, storm::RationalNumber> minmax(std::map<uint64_t, storm::RationalNumber> const& values) {
            assert(!values.empty());
            storm::RationalNumber min = values.begin()->second;
            storm::RationalNumber max = values.begin()->second;
            for (auto const& vt : values) {
                if (vt.second == storm::utility::infinity<storm::RationalNumber>()) {
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
        storm::RationalFunction minimum(std::map<uint64_t, storm::RationalFunction> const&) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Minimum for rational functions is not defined.");
        }
        
        template< typename K, typename ValueType>
        ValueType minimum(std::map<K, ValueType> const& values) {
            return minmax(values).first;
        }
        
        template<>
        storm::RationalFunction maximum(std::map<uint64_t, storm::RationalFunction> const&) {
            STORM_LOG_THROW(false, storm::exceptions::InvalidArgumentException, "Maximum for rational functions is not defined");
        }
        
        template<typename K, typename ValueType>
        ValueType maximum(std::map<K, ValueType> const& values) {
            return minmax(values).second;
        }
        
#ifdef STORM_HAVE_CARL
        template<>
        RationalFunction& simplify(RationalFunction& value);

        template<>
        RationalFunction&& simplify(RationalFunction&& value);

        template<>
        RationalFunction pow(RationalFunction const& value, uint_fast64_t exponent) {
            return carl::pow(value, exponent);
        }

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
        double convertNumber(RationalNumber const& number){
            return carl::toDouble(number);
        }

        template<>
        uint_fast64_t convertNumber(RationalNumber const& number){
            return carl::toInt<unsigned long>(number);
        }

        template<>
        RationalNumber convertNumber(RationalNumber const& number){
            return number;
        }
        
        template<>
        RationalNumber convertNumber(double const& number){
            return carl::rationalize<RationalNumber>(number);
        }

        template<>
        RationalNumber convertNumber(uint_fast64_t const& number){
            STORM_LOG_ASSERT(static_cast<carl::uint>(number) == number, "Rationalizing failed, because the number is too large.");
            return carl::rationalize<RationalNumber>(static_cast<carl::uint>(number));
        }

        template<>
        RationalNumber convertNumber(int_fast64_t const& number){
            STORM_LOG_ASSERT(static_cast<carl::sint>(number) == number, "Rationalizing failed, because the number is too large.");
            return carl::rationalize<RationalNumber>(static_cast<carl::sint>(number));
        }

        template<>
        RationalFunction convertNumber(double const& number){
            return RationalFunction(carl::rationalize<RationalNumber>(number));
        }

        template<>
        RationalFunction convertNumber(int_fast64_t const& number){
            STORM_LOG_ASSERT(static_cast<carl::sint>(number) == number, "Rationalizing failed, because the number is too large.");
            return RationalFunction(carl::rationalize<RationalNumber>(static_cast<carl::sint>(number)));
        }

        template<>
        RationalNumber convertNumber(std::string const& number) {
            return carl::rationalize<RationalNumber>(number);
        }

        template<>
        RationalFunction convertNumber(RationalNumber const& number) {
            return RationalFunction(number);
        }

        template<>
        uint_fast64_t convertNumber(RationalFunction const& func) {
            return carl::toInt<unsigned long>(func.nominatorAsNumber());
        }

        template<>
        RationalNumber sqrt(RationalNumber const& number) {
            return carl::sqrt(number);
        }

        template<>
        RationalNumber abs(storm::RationalNumber const& number) {
            return carl::abs(number);
        }

        template<>
        RationalNumber floor(storm::RationalNumber const& number) {
            return carl::floor(number);
        }

        template<>
        RationalNumber ceil(storm::RationalNumber const& number) {
            return carl::ceil(number);
        }

        template<>
        RationalNumber pow(RationalNumber const& value, uint_fast64_t exponent) {
            return carl::pow(value, exponent);
        }

#endif

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
        
        // Explicit instantiations.
        template bool isOne(double const& value);
        template bool isZero(double const& value);
        template bool isConstant(double const& value);
        template bool isInfinity(double const& value);

		template double one();
		template double zero();
		template double infinity();

        template double pow(double const& value, uint_fast64_t exponent);

        template double simplify(double value);

        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, double> simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, double> matrixEntry);
        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, double>& simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, double>& matrixEntry);
        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, double>&& simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, double>&& matrixEntry);

        template double sqrt(double const& number);
        template double abs(double const& number);

        template bool isInteger(double const& number);
        
        template bool isOne(float const& value);
        template bool isZero(float const& value);
        template bool isConstant(float const& value);
        template bool isInfinity(float const& value);

		template float one();
		template float zero();
		template float infinity();

        template float pow(float const& value, uint_fast64_t exponent);
        template bool isInteger(float const& number);

        template float simplify(float value);
        
        template std::string to_string(float const& value);
        template std::string to_string(double const& value);
        template std::string to_string(storm::RationalNumber const& value);
        template std::string to_string(storm::RationalFunction const& value);

        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, float> simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, float> matrixEntry);
        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, float>& simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, float>& matrixEntry);
        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, float>&& simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, float>&& matrixEntry);
        
        template bool isOne(int const& value);
        template bool isZero(int const& value);
        template bool isConstant(int const& value);
        template bool isInfinity(int const& value);

        template int one();
        template int zero();
        template int infinity();

        template int pow(int const& value, uint_fast64_t exponent);

        template bool isInteger(int const& number);

        template int simplify(int value);

        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, int> simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, int> matrixEntry);
        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, int>& simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, int>& matrixEntry);
        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, int>&& simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, int>&& matrixEntry);

        template bool isOne(storm::storage::sparse::state_type const& value);
        template bool isZero(storm::storage::sparse::state_type const& value);
        template bool isConstant(storm::storage::sparse::state_type const& value);
        template bool isInfinity(storm::storage::sparse::state_type const& value);

        template uint32_t one();
        template uint32_t zero();
        template uint32_t infinity();

        template storm::storage::sparse::state_type one();
        template storm::storage::sparse::state_type zero();
        template storm::storage::sparse::state_type infinity();

        template storm::storage::sparse::state_type pow(storm::storage::sparse::state_type const& value, uint_fast64_t exponent);

        template storm::storage::sparse::state_type simplify(storm::storage::sparse::state_type value);

        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, storm::storage::sparse::state_type> simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, storm::storage::sparse::state_type> matrixEntry);
        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, storm::storage::sparse::state_type>& simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, storm::storage::sparse::state_type>& matrixEntry);
        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, storm::storage::sparse::state_type>&& simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, storm::storage::sparse::state_type>&& matrixEntry);

        template std::pair<double, double> minmax(std::vector<double> const&);
        template double minimum(std::vector<double> const&);
        template double maximum(std::vector<double> const&);
        
        template std::pair<storm::RationalNumber, storm::RationalNumber> minmax(std::vector<storm::RationalNumber> const&);
        template storm::RationalNumber minimum(std::vector<storm::RationalNumber> const&);
        template storm::RationalNumber maximum(std::vector<storm::RationalNumber> const&);
        
        template storm::RationalFunction minimum(std::vector<storm::RationalFunction> const&);
        template storm::RationalFunction maximum(std::vector<storm::RationalFunction> const&);
        
        template std::pair<double, double> minmax(std::map<uint64_t, double> const&);
        template double minimum(std::map<uint64_t, double> const&);
        template double maximum(std::map<uint64_t, double> const&);

#ifdef STORM_HAVE_CARL
        // Instantiations for rational number.
        template std::pair<storm::RationalNumber, storm::RationalNumber> minmax(std::map<uint64_t, storm::RationalNumber> const&);
        template storm::RationalNumber minimum(std::map<uint64_t, storm::RationalNumber> const&);
        template storm::RationalNumber maximum(std::map<uint64_t, storm::RationalNumber> const&);
        
        template bool isOne(storm::RationalNumber const& value);
        template bool isZero(storm::RationalNumber const& value);
        template bool isConstant(storm::RationalNumber const& value);

        template storm::RationalNumber one();
        template storm::RationalNumber zero();
        template storm::RationalNumber infinity();

        template double convertNumber(storm::RationalNumber const& number);
        template uint_fast64_t convertNumber(storm::RationalNumber const& number);
        template storm::RationalNumber convertNumber(double const& number);
        template storm::RationalNumber convertNumber(storm::RationalNumber const& number);
        RationalNumber convertNumber(std::string const& number);
        
        template storm::RationalNumber sqrt(storm::RationalNumber const& number);
        template storm::RationalNumber abs(storm::RationalNumber const& number);
        template storm::RationalNumber floor(storm::RationalNumber const& number);
        template storm::RationalNumber ceil(storm::RationalNumber const& number);

        template storm::RationalNumber pow(storm::RationalNumber const& value, uint_fast64_t exponent);

        template storm::RationalNumber simplify(storm::RationalNumber value);
        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, storm::RationalNumber> simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, storm::RationalNumber> matrixEntry);
        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, storm::RationalNumber>& simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, storm::RationalNumber>& matrixEntry);
        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, storm::RationalNumber>&& simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, storm::RationalNumber>&& matrixEntry);
        
        // Instantiations for rational function.
        template bool isOne(RationalFunction const& value);
        template bool isZero(RationalFunction const& value);
        template bool isConstant(RationalFunction const& value);
        template bool isInfinity(RationalFunction const& value);

        template RationalFunction one();
        template RationalFunction zero();
        template storm::RationalFunction infinity();

        template RationalFunction pow(RationalFunction const& value, uint_fast64_t exponent);
        template RationalFunction simplify(RationalFunction value);
        template RationalFunction& simplify(RationalFunction& value);
        template RationalFunction&& simplify(RationalFunction&& value);

        template Polynomial one();
        template Polynomial zero();

        template bool isOne(Interval const& value);
        template bool isZero(Interval const& value);
        template bool isConstant(Interval const& value);
        template bool isInfinity(Interval const& value);

        template Interval one();
        template Interval zero();
        
        template storm::RationalFunction minimum(std::map<uint64_t, storm::RationalFunction> const&);
        template storm::RationalFunction maximum(std::map<uint64_t, storm::RationalFunction> const&);

        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, RationalFunction> simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, RationalFunction> matrixEntry);
        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, RationalFunction>& simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, RationalFunction>& matrixEntry);
        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, RationalFunction>&& simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, RationalFunction>&& matrixEntry);
#endif
        
    }
}
