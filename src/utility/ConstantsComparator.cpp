#include "src/utility/ConstantsComparator.h"

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
        
        template<>
        double simplify(double value) {
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
        
        template<typename IndexType, typename ValueType>
        storm::storage::MatrixEntry<IndexType, ValueType>& simplify(storm::storage::MatrixEntry<IndexType, ValueType>& matrixEntry) {
            simplify(matrixEntry.getValue());
            return matrixEntry;
        }
        
        template class ConstantsComparator<double>;
        
        template double one();
        template double zero();
        template double infinity();

        template double simplify(double value);
        
        template storm::storage::MatrixEntry<storm::storage::sparse::state_type, double>& simplify(storm::storage::MatrixEntry<storm::storage::sparse::state_type, double>& matrixEntry);
    }
}