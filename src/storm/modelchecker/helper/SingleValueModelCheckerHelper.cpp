#include "SingleValueModelCheckerHelper.h"


namespace storm {
    namespace modelchecker {
        namespace helper {
            
            template <typename ValueType, storm::dd::DdType DdType>
            void SingleValueModelCheckerHelper<ValueType, DdType>::setOptimizationDirection(storm::solver::OptimizationDirection const& direction) {
                _optimizationDirection = direction;
            }
            
            template <typename ValueType, storm::dd::DdType DdType>
            void SingleValueModelCheckerHelper<ValueType, DdType>::clearOptimizationDirection() {
                _optimizationDirection = boost::none;
            }
            
            template <typename ValueType, storm::dd::DdType DdType>
            bool SingleValueModelCheckerHelper<ValueType, DdType>::isOptimizationDirectionSet() const {
                return _optimizationDirection.is_initialized();
            }
            
            template <typename ValueType, storm::dd::DdType DdType>
            storm::solver::OptimizationDirection const& SingleValueModelCheckerHelper<ValueType, DdType>::getOptimizationDirection() const {
                STORM_LOG_ASSERT(isOptimizationDirectionSet(), "Requested optimization direction but none was set.");
                return _optimizationDirection.get();
            }
            
            template <typename ValueType, storm::dd::DdType DdType>
            bool SingleValueModelCheckerHelper<ValueType, DdType>::minimize() const {
                return storm::solver::minimize(getOptimizationDirection());
            }
            
            template <typename ValueType, storm::dd::DdType DdType>
            bool SingleValueModelCheckerHelper<ValueType, DdType>::maximize() const {
                return storm::solver::maximize(getOptimizationDirection());
            }
            
            template <typename ValueType, storm::dd::DdType DdType>
            boost::optional<storm::solver::OptimizationDirection> SingleValueModelCheckerHelper<ValueType, DdType>::getOptionalOptimizationDirection() const {
                return _optimizationDirection;
            }
            
            template <typename ValueType, storm::dd::DdType DdType>
            void SingleValueModelCheckerHelper<ValueType, DdType>::setValueThreshold(storm::logic::ComparisonType const& comparisonType, ValueType const& threshold) {
                _valueThreshold = std::make_pair(comparisonType, threshold);
            }
            
            template <typename ValueType, storm::dd::DdType DdType>
            void SingleValueModelCheckerHelper<ValueType, DdType>::clearValueThreshold() {
                _valueThreshold = boost::none;
            }
            
            template <typename ValueType, storm::dd::DdType DdType>
            bool SingleValueModelCheckerHelper<ValueType, DdType>::isValueThresholdSet() const {
                return _valueThreshold.is_initialized();
            }
            
            template <typename ValueType, storm::dd::DdType DdType>
            storm::logic::ComparisonType const& SingleValueModelCheckerHelper<ValueType, DdType>::getValueThresholdComparisonType() const {
                STORM_LOG_ASSERT(isValueThresholdSet(), "Value Threshold comparison type was requested but not set before.");
                return _valueThreshold->first;
            }
            
            template <typename ValueType, storm::dd::DdType DdType>
            ValueType const& SingleValueModelCheckerHelper<ValueType, DdType>::getValueThresholdValue() const {
                STORM_LOG_ASSERT(isValueThresholdSet(), "Value Threshold comparison type was requested but not set before.");
                return _valueThreshold->second;
            }
 
            template class SingleValueModelCheckerHelper<double, storm::dd::DdType::None>;
            template class SingleValueModelCheckerHelper<storm::RationalNumber, storm::dd::DdType::None>;
            template class SingleValueModelCheckerHelper<storm::RationalFunction, storm::dd::DdType::None>;
            
            template class SingleValueModelCheckerHelper<double, storm::dd::DdType::Sylvan>;
            template class SingleValueModelCheckerHelper<storm::RationalNumber, storm::dd::DdType::Sylvan>;
            template class SingleValueModelCheckerHelper<storm::RationalFunction, storm::dd::DdType::Sylvan>;
            
            template class SingleValueModelCheckerHelper<double, storm::dd::DdType::CUDD>;
            
        }
    }
}