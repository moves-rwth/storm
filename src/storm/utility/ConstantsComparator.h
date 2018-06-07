#ifndef STORM_UTILITY_CONSTANTSCOMPARATOR_H_
#define STORM_UTILITY_CONSTANTSCOMPARATOR_H_

#include "storm/adapters/RationalFunctionAdapter.h"

namespace storm {
    namespace utility {
        // A class that can be used for comparing constants.
        template<typename ValueType>
        class ConstantsComparator {
        public:
            // This needs to be in here, otherwise the template specializations are not used properly.
            ConstantsComparator() = default;
            
            bool isOne(ValueType const& value) const;
            
            bool isZero(ValueType const& value) const;
            
            bool isEqual(ValueType const& value1, ValueType const& value2) const;
            
            bool isConstant(ValueType const& value) const;
            
            bool isInfinity(ValueType const& value) const;
            
            bool isLess(ValueType const& value1, ValueType const& value2) const;
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
            
            bool isInfinity(float const& value) const;

            bool isLess(float const& value1, float const& value2) const;
            
        private:
            // The precision used for comparisons.
            float precision;
        };
        
        // For doubles we specialize this class and consider the comparison modulo some predefined precision.
        template<>
        class ConstantsComparator<double> {
        public:
            ConstantsComparator();
            
            ConstantsComparator(double precision, bool relative = false);
            
            bool isOne(double const& value) const;
            
            bool isZero(double const& value) const;
            
            bool isInfinity(double const& value) const;
            
            bool isEqual(double const& value1, double const& value2) const;
            
            bool isConstant(double const& value) const;
            
            bool isLess(double const& value1, double const& value2) const;
            
        private:
            // The precision used for comparisons.
            double precision;
            
            // Whether to use relative comparison for equality.
            bool relative;
        };
        
        // For rational numbers we specialize this class and consider the comparison modulo some predefined precision.
        template<>
        class ConstantsComparator<storm::RationalNumber> {
        public:
            ConstantsComparator();
            
            ConstantsComparator(storm::RationalNumber precision, bool relative);
            
            bool isOne(storm::RationalNumber const& value) const;
            
            bool isZero(storm::RationalNumber const& value) const;
            
            bool isEqual(storm::RationalNumber const& value1, storm::RationalNumber const& value2) const;
            
            bool isConstant(storm::RationalNumber const& value) const;
            
            bool isInfinity(storm::RationalNumber const& value) const;
            
            bool isLess(storm::RationalNumber const& value1, storm::RationalNumber const& value2) const;
            
        private:
            storm::RationalNumber precision;
            bool relative;
        };
    }
}

#endif /* STORM_UTILITY_CONSTANTSCOMPARATOR_H_ */
