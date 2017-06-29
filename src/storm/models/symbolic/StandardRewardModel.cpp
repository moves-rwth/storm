#include "storm/models/symbolic/StandardRewardModel.h"

#include "storm/storage/dd/DdManager.h"
#include "storm/storage/dd/Add.h"
#include "storm/storage/dd/Bdd.h"

#include "storm/adapters/RationalFunctionAdapter.h"

namespace storm {
    namespace models {
        namespace symbolic {
            template <storm::dd::DdType Type, typename ValueType>
            StandardRewardModel<Type, ValueType>::StandardRewardModel(boost::optional<storm::dd::Add<Type, ValueType>> const& stateRewardVector, boost::optional<storm::dd::Add<Type, ValueType>> const& stateActionRewardVector, boost::optional<storm::dd::Add<Type, ValueType>> const& transitionRewardMatrix) : optionalStateRewardVector(stateRewardVector), optionalStateActionRewardVector(stateActionRewardVector), optionalTransitionRewardMatrix(transitionRewardMatrix) {
                // Intentionally left empty.
            }
            
            template <storm::dd::DdType Type, typename ValueType>
            bool StandardRewardModel<Type, ValueType>::empty() const {
                return !(this->hasStateRewards() || this->hasStateActionRewards() || this->hasTransitionRewards());
            }
            
            template <storm::dd::DdType Type, typename ValueType>
            bool StandardRewardModel<Type, ValueType>::hasStateRewards() const {
                return static_cast<bool>(this->optionalStateRewardVector);
            }
            
            template <storm::dd::DdType Type, typename ValueType>
            bool StandardRewardModel<Type, ValueType>::hasOnlyStateRewards() const {
                return this->hasStateRewards() && !this->hasStateActionRewards() && !this->hasTransitionRewards();
            }
            
            template <storm::dd::DdType Type, typename ValueType>
            storm::dd::Add<Type, ValueType> const& StandardRewardModel<Type, ValueType>::getStateRewardVector() const {
                return this->optionalStateRewardVector.get();
            }
            
            template <storm::dd::DdType Type, typename ValueType>
            storm::dd::Add<Type, ValueType>& StandardRewardModel<Type, ValueType>::getStateRewardVector() {
                return this->optionalStateRewardVector.get();
            }
            
            template <storm::dd::DdType Type, typename ValueType>
            boost::optional<storm::dd::Add<Type, ValueType>> const& StandardRewardModel<Type, ValueType>::getOptionalStateRewardVector() const {
                return this->optionalStateRewardVector;
            }
            
            template <storm::dd::DdType Type, typename ValueType>
            bool StandardRewardModel<Type, ValueType>::hasStateActionRewards() const {
                return static_cast<bool>(this->optionalStateActionRewardVector);
            }
            
            template <storm::dd::DdType Type, typename ValueType>
            storm::dd::Add<Type, ValueType> const& StandardRewardModel<Type, ValueType>::getStateActionRewardVector() const {
                return this->optionalStateActionRewardVector.get();
            }
            
            template <storm::dd::DdType Type, typename ValueType>
            storm::dd::Add<Type, ValueType>& StandardRewardModel<Type, ValueType>::getStateActionRewardVector() {
                return this->optionalStateActionRewardVector.get();
            }
            
            template <storm::dd::DdType Type, typename ValueType>
            boost::optional<storm::dd::Add<Type, ValueType>> const& StandardRewardModel<Type, ValueType>::getOptionalStateActionRewardVector() const {
                return this->optionalStateActionRewardVector;
            }
            
            template <storm::dd::DdType Type, typename ValueType>
            bool StandardRewardModel<Type, ValueType>::hasTransitionRewards() const {
                return static_cast<bool>(this->optionalTransitionRewardMatrix);
            }
            
            template <storm::dd::DdType Type, typename ValueType>
            storm::dd::Add<Type, ValueType> const& StandardRewardModel<Type, ValueType>::getTransitionRewardMatrix() const {
                return this->optionalTransitionRewardMatrix.get();
            }
            
            template <storm::dd::DdType Type, typename ValueType>
            storm::dd::Add<Type, ValueType>& StandardRewardModel<Type, ValueType>::getTransitionRewardMatrix() {
                return this->optionalTransitionRewardMatrix.get();
            }
            
            template <storm::dd::DdType Type, typename ValueType>
            boost::optional<storm::dd::Add<Type, ValueType>> const& StandardRewardModel<Type, ValueType>::getOptionalTransitionRewardMatrix() const {
                return this->optionalTransitionRewardMatrix;
            }
            
            template <storm::dd::DdType Type, typename ValueType>
            storm::dd::Add<Type, ValueType> StandardRewardModel<Type, ValueType>::getTotalRewardVector(storm::dd::Add<Type, ValueType> const& filterAdd, storm::dd::Add<Type, ValueType> const& transitionMatrix, std::set<storm::expressions::Variable> const& columnVariables) const {
                storm::dd::Add<Type, ValueType> result = transitionMatrix.getDdManager().template getAddZero<ValueType>();
                if (this->hasStateRewards()) {
                    result += filterAdd * optionalStateRewardVector.get();
                }
                if (this->hasStateActionRewards()) {
                    result += filterAdd * optionalStateActionRewardVector.get();
                }
                if (this->hasTransitionRewards()) {
                    result += (transitionMatrix * this->getTransitionRewardMatrix()).sumAbstract(columnVariables);
                }
                return result;
            }
            
            template <storm::dd::DdType Type, typename ValueType>
            storm::dd::Add<Type, ValueType> StandardRewardModel<Type, ValueType>::getTotalRewardVector(storm::dd::Add<Type, ValueType> const& transitionMatrix, std::set<storm::expressions::Variable> const& columnVariables) const {
                storm::dd::Add<Type, ValueType> result = transitionMatrix.getDdManager().template getAddZero<ValueType>();
                if (this->hasStateRewards()) {
                    result += optionalStateRewardVector.get();
                }
                if (this->hasStateActionRewards()) {
                    result += optionalStateActionRewardVector.get();
                }
                if (this->hasTransitionRewards()) {
                    result += (transitionMatrix * this->getTransitionRewardMatrix()).sumAbstract(columnVariables);
                }
                return result;
            }
            
            template <storm::dd::DdType Type, typename ValueType>
            storm::dd::Add<Type, ValueType> StandardRewardModel<Type, ValueType>::getTotalRewardVector(storm::dd::Add<Type, ValueType> const& transitionMatrix, std::set<storm::expressions::Variable> const& columnVariables, storm::dd::Add<Type, ValueType> const& weights, bool scaleTransAndActions) const {
                storm::dd::Add<Type, ValueType> result = transitionMatrix.getDdManager().template getAddZero<ValueType>();
                if (this->hasStateRewards()) {
                    result += optionalStateRewardVector.get();
                }
                if (this->hasStateActionRewards()) {
                    result += optionalStateActionRewardVector.get() * weights;
                }
                if (this->hasTransitionRewards()) {
                    if (scaleTransAndActions) {
                        result += weights * (transitionMatrix * this->getTransitionRewardMatrix()).sumAbstract(columnVariables);
                    } else {
                        result += (transitionMatrix * this->getTransitionRewardMatrix()).sumAbstract(columnVariables);
                    }
                }
                return result;
            }
            
            template <storm::dd::DdType Type, typename ValueType>
            StandardRewardModel<Type, ValueType>& StandardRewardModel<Type, ValueType>::operator*=(storm::dd::Add<Type, ValueType> const& filter) {
                if (this->hasStateRewards()) {
                    this->optionalStateRewardVector.get() *= filter;
                }
                if (this->hasStateActionRewards()) {
                    this->optionalStateActionRewardVector.get() *= filter;
                }
                if (this->hasTransitionRewards()) {
                    this->optionalTransitionRewardMatrix.get() *= filter;
                }
                
                return *this;
            }
            
            template <storm::dd::DdType Type, typename ValueType>
            StandardRewardModel<Type, ValueType> StandardRewardModel<Type, ValueType>::divideStateRewardVector(storm::dd::Add<Type, ValueType> const& divisor) const {
                boost::optional<storm::dd::Add<Type, ValueType>> modifiedStateRewardVector;
                if (this->hasStateRewards()) {
                    modifiedStateRewardVector = this->optionalStateRewardVector.get() / divisor;
                }
                return StandardRewardModel<Type, ValueType>(modifiedStateRewardVector, this->optionalStateActionRewardVector, this->optionalTransitionRewardMatrix);
            }
            
            template class StandardRewardModel<storm::dd::DdType::CUDD, double>;
            template class StandardRewardModel<storm::dd::DdType::Sylvan, double>;

            template class StandardRewardModel<storm::dd::DdType::Sylvan, storm::RationalNumber>;
            template class StandardRewardModel<storm::dd::DdType::Sylvan, storm::RationalFunction>;

        }
    }
}
