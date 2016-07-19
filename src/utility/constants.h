#ifndef STORM_UTILITY_CONSTANTS_H_
#define STORM_UTILITY_CONSTANTS_H_

#ifdef max
#	undef max
#endif

#ifdef min
#	undef min
#endif

#include <limits>
#include <cstdint>
#include "src/storage/sparse/StateType.h"
#include "src/settings/SettingsManager.h"
#include "src/settings/modules/GeneralSettings.h"

#include "src/adapters/CarlAdapter.h"

namespace storm {
    
    // Forward-declare MatrixEntry class.
    namespace storage {
        template<typename IndexType, typename ValueType> class MatrixEntry;
    }
    
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

        template<>
        bool isOne(storm::RationalNumber const& a) {
            return carl::isOne(a);
        }

        template<>
        bool isZero(storm::RationalNumber const& a) {
            return carl::isZero(a);
        }

        template<>
        storm::RationalNumber infinity() {
            // FIXME: this should be treated more properly.
            return storm::RationalNumber(-1);
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

        template<typename SourceType, typename TargetType>
        TargetType convertNumber(SourceType const& number);

        template<>
        double convertNumber(double const& number){
            return number;
        }

        template<typename ValueType>
        ValueType abs(ValueType const& number) {
            return carl::abs(number);
        }

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

        template<>
        RationalFunction convertNumber(double const& number){
            return RationalFunction(carl::rationalize<RationalNumber>(number));
        }
//
//        template<>
//        storm::RationalNumber abs(storm::RationalNumber const& number) {
//            return carl::abs(number);
//        }

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


    }
}

#endif /* STORM_UTILITY_CONSTANTS_H_ */