#include "ModelCheckerHelper.h"

#include "storm/utility/macros.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            
            template <typename ValueType, storm::dd::DdType DdType>
            void ModelCheckerHelper<ValueType, DdType>::setRelevantStates(StateSet const& relevantStates) {
                _relevantStates = relevantStates;
            }
            
            template <typename ValueType, storm::dd::DdType DdType>
            void ModelCheckerHelper<ValueType, DdType>::clearRelevantStates() {
                _relevantStates = boost::none;
            }
            
            template <typename ValueType, storm::dd::DdType DdType>
            bool ModelCheckerHelper<ValueType, DdType>::hasRelevantStates() const {
                return _relevantStates.is_initialized();
            }
            
            template <typename ValueType, storm::dd::DdType DdType>
            boost::optional<typename ModelCheckerHelper<ValueType, DdType>::StateSet> const& ModelCheckerHelper<ValueType, DdType>::getOptionalRelevantStates() const {
                return _relevantStates;
            }
            
            template <typename ValueType, storm::dd::DdType DdType>
            typename ModelCheckerHelper<ValueType, DdType>::StateSet const& ModelCheckerHelper<ValueType, DdType>::getRelevantStates() const {
                STORM_LOG_ASSERT(hasRelevantStates(), "Retrieving relevant states although none have been set.");
                return _relevantStates.get();
            }
            
            template class ModelCheckerHelper<double, storm::dd::DdType::None>;
            template class ModelCheckerHelper<storm::RationalNumber, storm::dd::DdType::None>;
            template class ModelCheckerHelper<storm::RationalFunction, storm::dd::DdType::None>;
            
            template class ModelCheckerHelper<double, storm::dd::DdType::Sylvan>;
            template class ModelCheckerHelper<storm::RationalNumber, storm::dd::DdType::Sylvan>;
            template class ModelCheckerHelper<storm::RationalFunction, storm::dd::DdType::Sylvan>;
            
            template class ModelCheckerHelper<double, storm::dd::DdType::CUDD>;
            
        }
    }
}