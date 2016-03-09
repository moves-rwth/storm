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

        
#ifdef STORM_HAVE_CARL
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
        RationalNumber convertNumber(double const& number){
            return carl::rationalize<RationalNumber>(number);
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

        template bool isOne(float const& value);
        template bool isZero(float const& value);
        template bool isConstant(float const& value);
        
		template float one();
		template float zero();
		template float infinity();

		template float pow(float const& value, uint_fast64_t exponent);

		template float simplify(float value);

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
        
        template int simplify(int value);
        
        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, int> simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, int> matrixEntry);
        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, int>& simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, int>& matrixEntry);
        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, int>&& simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, int>&& matrixEntry);

        template bool isOne(storm::storage::sparse::state_type const& value);
        template bool isZero(storm::storage::sparse::state_type const& value);
        template bool isConstant(storm::storage::sparse::state_type const& value);

        template storm::storage::sparse::state_type one();
        template storm::storage::sparse::state_type zero();
        template storm::storage::sparse::state_type infinity();

        template storm::storage::sparse::state_type pow(storm::storage::sparse::state_type const& value, uint_fast64_t exponent);

        template storm::storage::sparse::state_type simplify(storm::storage::sparse::state_type value);

        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, storm::storage::sparse::state_type> simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, storm::storage::sparse::state_type> matrixEntry);
        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, storm::storage::sparse::state_type>& simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, storm::storage::sparse::state_type>& matrixEntry);
        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, storm::storage::sparse::state_type>&& simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, storm::storage::sparse::state_type>&& matrixEntry);

#ifdef STORM_HAVE_CARL
        template bool isOne(RationalFunction const& value);
        template bool isZero(RationalFunction const& value);
        template bool isConstant(RationalFunction const& value);
        
        template RationalFunction one();
        template RationalFunction zero();
        template storm::RationalFunction infinity();
        
        template RationalFunction pow(RationalFunction const& value, uint_fast64_t exponent);
        
        template Polynomial one();
        template Polynomial zero();
        template RationalFunction simplify(RationalFunction value);
        template RationalFunction& simplify(RationalFunction& value);
        template RationalFunction&& simplify(RationalFunction&& value);
        
        template double convertNumber(RationalNumber const& number);
        template RationalNumber convertNumber(double const& number);
        
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