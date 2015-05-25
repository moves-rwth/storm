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

#include "src/settings/SettingsManager.h"
#include "src/adapters/CarlAdapter.h"

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
        
#ifdef STORM_HAVE_CARL
        template<>
        storm::RationalFunction infinity();
#endif
        
        template<typename ValueType>
        ValueType pow(ValueType const& value, uint_fast64_t exponent);
        
#ifdef STORM_HAVE_CARL
        template<>
        RationalFunction pow(RationalFunction const& value, uint_fast64_t exponent);
#endif
        
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
        };
        
        // For floats we specialize this class and consider the comparison modulo some predefined precision.
        template<>
        class ConstantsComparator<float> {
        public:
            ConstantsComparator();
            
            ConstantsComparator(float precision);
            
            bool isOne(float const& value) const;
            
            bool isZero(float const& value) const;
            
            bool isEqual(float const& value1, float const& value2) const;
            
            bool isConstant(float const& value) const;
            
        private:
            // The precision used for comparisons.
            float precision;
        };
        
        // For doubles we specialize this class and consider the comparison modulo some predefined precision.
        template<>
        class ConstantsComparator<double> {
        public:
            ConstantsComparator();
            
            ConstantsComparator(double precision);
            
            bool isOne(double const& value) const;
            
            bool isZero(double const& value) const;
            
            bool isInfinity(double const& value) const;
            
            bool isEqual(double const& value1, double const& value2) const;
            
            bool isConstant(double const& value) const;
            
        private:
            // The precision used for comparisons.
            double precision;
        };
        
#ifdef STORM_HAVE_CARL
        template<>
        RationalFunction& simplify(RationalFunction& value);
        
        template<>
        RationalFunction&& simplify(RationalFunction&& value);

        template<>
        class ConstantsComparator<storm::RationalFunction> {
        public:
            bool isOne(storm::RationalFunction const& value) const;
            
            bool isZero(storm::RationalFunction const& value) const;
            
            bool isEqual(storm::RationalFunction const& value1, storm::RationalFunction const& value2) const;
            
            bool isConstant(storm::RationalFunction const& value) const;
        };
        
        template<>
        class ConstantsComparator<storm::Polynomial> {
        public:
            bool isOne(storm::Polynomial const& value) const;
            
            bool isZero(storm::Polynomial const& value) const;
            
            bool isEqual(storm::Polynomial const& value1, storm::Polynomial const& value2) const;
            
            bool isConstant(storm::Polynomial const& value) const;
        };
#endif
        
        template<typename IndexType, typename ValueType>
        storm::storage::MatrixEntry<IndexType, ValueType>& simplify(storm::storage::MatrixEntry<IndexType, ValueType>& matrixEntry);

        template<typename IndexType, typename ValueType>
        storm::storage::MatrixEntry<IndexType, ValueType>&& simplify(storm::storage::MatrixEntry<IndexType, ValueType>&& matrixEntry);
    }
}

#endif /* STORM_UTILITY_CONSTANTSCOMPARATOR_H_ */