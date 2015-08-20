#ifndef STORM_UTILITY_CONSTANTSCOMPARATOR_H_
#define STORM_UTILITY_CONSTANTSCOMPARATOR_H_

#ifdef max
#	undef max
#endif

#ifdef min
#	undef min
#endif

#include <limits>
#include <cstdint>


namespace storm {
    
    // Forward-declare MatrixEntry class.
    namespace storage {
        template<typename IndexType, typename ValueType> class MatrixEntry;
    }
    
    namespace utility {
        
        template<typename ValueType>
        ValueType one();
        
        template<typename ValueType>
        ValueType zero();
        
        template<typename ValueType>
        ValueType infinity();
     
        
        template<typename ValueType>
        ValueType pow(ValueType const& value, uint_fast64_t exponent);
        
        template<typename ValueType>
        ValueType simplify(ValueType value);
        
        // A class that can be used for comparing constants.
        template<typename ValueType>
        class ConstantsComparator {
        public:
            bool isOne(ValueType const& value) const;
            
            bool isZero(ValueType const& value) const;
            
            bool isEqual(ValueType const& value1, ValueType const& value2) const;
            
            bool isConstant(ValueType const& value) const;
            
            bool isInfinity(ValueType const& value) const;
        };
        
        template<typename IndexType, typename ValueType>
        storm::storage::MatrixEntry<IndexType, ValueType>& simplify(storm::storage::MatrixEntry<IndexType, ValueType>& matrixEntry);

        template<typename IndexType, typename ValueType>
        storm::storage::MatrixEntry<IndexType, ValueType>&& simplify(storm::storage::MatrixEntry<IndexType, ValueType>&& matrixEntry);
    }
}

#endif /* STORM_UTILITY_CONSTANTSCOMPARATOR_H_ */