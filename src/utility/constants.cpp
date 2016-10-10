#include "src/utility/constants.h"

#include "src/storage/sparse/StateType.h"
#include "src/storage/SparseMatrix.h"
#include "src/settings/SettingsManager.h"
#include "src/settings/modules/GeneralSettings.h"

#include "src/adapters/CarlAdapter.h"

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
        bool isConstant(ValueType const& a) {
            return true;
        }
        
        template<typename ValueType>
        bool isInteger(ValueType const& number) {
            ValueType iPart;
            ValueType result = std::modf(number, &iPart);
            return result = zero<ValueType>();
        }

        template<>
        bool isInteger(int const& number) {
            return true;
        }

        template<>
        bool isInteger(uint_fast64_t const& number) {
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
        
        template<typename ValueType>
        ValueType abs(ValueType const& number) {
            return std::fabs(number);
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
        RationalNumber convertNumber(double const& number){
            return carl::rationalize<RationalNumber>(number);
        }

        template<>
        RationalNumber convertNumber(RationalNumber const& number){
            return number;
        }

        template<>
        RationalFunction convertNumber(double const& number){
            return RationalFunction(carl::rationalize<RationalNumber>(number));
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
        RationalNumber abs(storm::RationalNumber const& number) {
            return carl::abs(number);
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
        
        template double one();
        template double zero();
        template double infinity();

        template double pow(double const& value, uint_fast64_t exponent);

        template double simplify(double value);

        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, double> simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, double> matrixEntry);
        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, double>& simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, double>& matrixEntry);
        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, double>&& simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, double>&& matrixEntry);

        template double abs(double const& number);
        template bool isInteger(double const& number);
        
        template bool isOne(float const& value);
        template bool isZero(float const& value);
        template bool isConstant(float const& value);
        
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

#ifdef STORM_HAVE_CARL
        // Instantiations for rational number.
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
        
        template storm::RationalNumber abs(storm::RationalNumber const& number);

        template storm::RationalNumber pow(storm::RationalNumber const& value, uint_fast64_t exponent);
        
        template storm::RationalNumber simplify(storm::RationalNumber value);
        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, storm::RationalNumber> simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, storm::RationalNumber> matrixEntry);
        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, storm::RationalNumber>& simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, storm::RationalNumber>& matrixEntry);
        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, storm::RationalNumber>&& simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, storm::RationalNumber>&& matrixEntry);
        
        // Instantiations for rational function.
        template bool isOne(RationalFunction const& value);
        template bool isZero(RationalFunction const& value);
        template bool isConstant(RationalFunction const& value);
        
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
        
        template Interval one();
        template Interval zero();
        
        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, RationalFunction> simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, RationalFunction> matrixEntry);
        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, RationalFunction>& simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, RationalFunction>& matrixEntry);
        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, RationalFunction>&& simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, RationalFunction>&& matrixEntry);
#endif
        
    }
}
