#include "src/utility/ConstantsComparator.h"

#include "src/storage/SparseMatrix.h"
#include "src/storage/sparse/StateType.h"

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
        
#ifdef STORM_HAVE_CARL
        template<>
        storm::RationalFunction infinity() {
            // FIXME: this does not work.
            return storm::RationalFunction(carl::rationalize<cln::cl_RA>(std::numeric_limits<double>::infinity()));
        }
#endif
        
        template<typename ValueType>
        ValueType pow(ValueType const& value, uint_fast64_t exponent) {
            return std::pow(value, exponent);
        }
        
        template<>
        double simplify(double value) {
            // In the general case, we don't to anything here, but merely return the value. If something else is
            // supposed to happen here, the templated function can be specialized for this particular type.
            return value;
        }

        template<>
        int simplify(int value) {
            // In the general case, we don't to anything here, but merely return the value. If something else is
            // supposed to happen here, the templated function can be specialized for this particular type.
            return value;
        }
        
        template<typename ValueType>
        bool ConstantsComparator<ValueType>::isOne(ValueType const& value) const {
            return value == one<ValueType>();
        }
        
        template<typename ValueType>
        bool ConstantsComparator<ValueType>::isZero(ValueType const& value) const {
            return value == zero<ValueType>();
        }
        
        template<typename ValueType>
        bool ConstantsComparator<ValueType>::isEqual(ValueType const& value1, ValueType const& value2) const {
            return value1 == value2;
        }
        
        ConstantsComparator<double>::ConstantsComparator() : precision(storm::settings::generalSettings().getPrecision()) {
            // Intentionally left empty.
        }
        
        ConstantsComparator<double>::ConstantsComparator(double precision) : precision(precision) {
            // Intentionally left empty.
        }
        
        bool ConstantsComparator<double>::isOne(double const& value) const {
            return std::abs(value - one<double>()) <= precision;
        }
        
        bool ConstantsComparator<double>::isZero(double const& value) const {
            return std::abs(value) <= precision;
        }
        
        bool ConstantsComparator<double>::isEqual(double const& value1, double const& value2) const {
            return std::abs(value1 - value2) <= precision;
        }
        
        bool ConstantsComparator<double>::isConstant(double const& value) const {
            return true;
        }
        
#ifdef STORM_HAVE_CARL
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
        
        bool ConstantsComparator<storm::RationalFunction>::isOne(storm::RationalFunction const& value) const {
            return value.isOne();
        }
        
        bool ConstantsComparator<storm::RationalFunction>::isZero(storm::RationalFunction const& value) const {
            return value.isZero();
        }
        
        bool ConstantsComparator<storm::RationalFunction>::isEqual(storm::RationalFunction const& value1, storm::RationalFunction const& value2) const {
            return value1 == value2;
        }
        
        bool ConstantsComparator<storm::RationalFunction>::isConstant(storm::RationalFunction const& value) const {
            return value.isConstant();
        }
        
        bool ConstantsComparator<storm::Polynomial>::isOne(storm::Polynomial const& value) const {
            return value.isOne();
        }
        
        bool ConstantsComparator<storm::Polynomial>::isZero(storm::Polynomial const& value) const {
            return value.isZero();
        }
        
        bool ConstantsComparator<storm::Polynomial>::isEqual(storm::Polynomial const& value1, storm::Polynomial const& value2) const {
            return value1 == value2;
        }
        
        bool ConstantsComparator<storm::Polynomial>::isConstant(storm::Polynomial const& value) const {
            return value.isConstant();
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
        
        template class ConstantsComparator<double>;
        
        template double one();
        template double zero();
        template double infinity();
        
        template double pow(double const& value, uint_fast64_t exponent);

        template double simplify(double value);

        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, double> simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, double> matrixEntry);
        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, double>& simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, double>& matrixEntry);
        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, double>&& simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, double>&& matrixEntry);
        
        template class ConstantsComparator<int>;
        
        template int one();
        template int zero();
        template int infinity();
        
        template int pow(int const& value, uint_fast64_t exponent);
        
        template int simplify(int value);
        
        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, int> simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, int> matrixEntry);
        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, int>& simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, int>& matrixEntry);
        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, int>&& simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, int>&& matrixEntry);
        
#ifdef STORM_HAVE_CARL
        template class ConstantsComparator<RationalFunction>;
        template class ConstantsComparator<Polynomial>;

        template RationalFunction one();
        template RationalFunction zero();
        template storm::RationalFunction infinity();
        
        template RationalFunction pow(RationalFunction const& value, uint_fast64_t exponent);
        
        template Polynomial one();
        template Polynomial zero();
        template RationalFunction simplify(RationalFunction value);
        template RationalFunction& simplify(RationalFunction& value);
        template RationalFunction&& simplify(RationalFunction&& value);
        
        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, RationalFunction> simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, RationalFunction> matrixEntry);
        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, RationalFunction>& simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, RationalFunction>& matrixEntry);
        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, RationalFunction>&& simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, RationalFunction>&& matrixEntry);
#endif
        
    }
}