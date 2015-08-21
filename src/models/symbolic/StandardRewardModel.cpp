#include "src/models/symbolic/StandardRewardModel.h"

#include "src/storage/dd/CuddDdManager.h"
#include "src/storage/dd/CuddAdd.h"
#include "src/storage/dd/CuddBdd.h"

namespace storm {
    namespace models {
        namespace symbolic {
            template <storm::dd::DdType Type, typename ValueType>
            StandardRewardModel<Type, ValueType>::StandardRewardModel(boost::optional<storm::dd::Add<Type>> const& stateRewardVector, boost::optional<storm::dd::Add<Type>> const& stateActionRewardVector, boost::optional<storm::dd::Add<Type>> const& transitionRewardMatrix) : optionalStateRewardVector(stateRewardVector), optionalStateActionRewardVector(stateActionRewardVector), optionalTransitionRewardMatrix(transitionRewardMatrix) {
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
            storm::dd::Add<Type> const& StandardRewardModel<Type, ValueType>::getStateRewardVector() const {
                return this->optionalStateRewardVector.get();
            }
            
            template <storm::dd::DdType Type, typename ValueType>
            storm::dd::Add<Type>& StandardRewardModel<Type, ValueType>::getStateRewardVector() {
                return this->optionalStateRewardVector.get();
            }
            
            template <storm::dd::DdType Type, typename ValueType>
            boost::optional<storm::dd::Add<Type>> const& StandardRewardModel<Type, ValueType>::getOptionalStateRewardVector() const {
                return this->optionalStateRewardVector;
            }
            
            template <storm::dd::DdType Type, typename ValueType>
            bool StandardRewardModel<Type, ValueType>::hasStateActionRewards() const {
                return static_cast<bool>(this->optionalStateActionRewardVector);
            }
            
            template <storm::dd::DdType Type, typename ValueType>
            storm::dd::Add<Type> const& StandardRewardModel<Type, ValueType>::getStateActionRewardVector() const {
                return this->optionalStateActionRewardVector.get();
            }
            
            template <storm::dd::DdType Type, typename ValueType>
            storm::dd::Add<Type>& StandardRewardModel<Type, ValueType>::getStateActionRewardVector() {
                return this->optionalStateActionRewardVector.get();
            }
            
            template <storm::dd::DdType Type, typename ValueType>
            boost::optional<storm::dd::Add<Type>> const& StandardRewardModel<Type, ValueType>::getOptionalStateActionRewardVector() const {
                return this->optionalStateActionRewardVector;
            }
            
            template <storm::dd::DdType Type, typename ValueType>
            bool StandardRewardModel<Type, ValueType>::hasTransitionRewards() const {
                return static_cast<bool>(this->optionalTransitionRewardMatrix);
            }
            
            template <storm::dd::DdType Type, typename ValueType>
            storm::dd::Add<Type> const& StandardRewardModel<Type, ValueType>::getTransitionRewardMatrix() const {
                return this->optionalTransitionRewardMatrix.get();
            }
            
            template <storm::dd::DdType Type, typename ValueType>
            storm::dd::Add<Type>& StandardRewardModel<Type, ValueType>::getTransitionRewardMatrix() {
                return this->optionalTransitionRewardMatrix.get();
            }
            
            template <storm::dd::DdType Type, typename ValueType>
            boost::optional<storm::dd::Add<Type>> const& StandardRewardModel<Type, ValueType>::getOptionalTransitionRewardMatrix() const {
                return this->optionalTransitionRewardMatrix;
            }
            
            template <storm::dd::DdType Type, typename ValueType>
            storm::dd::Add<Type> StandardRewardModel<Type, ValueType>::getTotalRewardVector(storm::dd::Add<Type> const& transitionMatrix, std::set<storm::expressions::Variable> const& columnVariables) const {
                storm::dd::Add<Type> result = transitionMatrix.getDdManager()->getAddZero();
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
            storm::dd::Add<Type> StandardRewardModel<Type, ValueType>::getTotalRewardVector(storm::dd::Add<Type> const& transitionMatrix, std::set<storm::expressions::Variable> const& columnVariables, storm::dd::Add<Type> const& weights) const {
                storm::dd::Add<Type> result = transitionMatrix.getDdManager()->getAddZero();
                if (this->hasStateRewards()) {
                    result += optionalStateRewardVector.get();
                }
                if (this->hasStateActionRewards()) {
                    result += optionalStateActionRewardVector.get() * weights;
                }
                if (this->hasTransitionRewards()) {
                    result += (transitionMatrix * this->getTransitionRewardMatrix()).sumAbstract(columnVariables);
                }
                return result;
            }
            
            template <storm::dd::DdType Type, typename ValueType>
            StandardRewardModel<Type, ValueType>& StandardRewardModel<Type, ValueType>::operator*=(storm::dd::Add<Type> const& filter) {
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
            StandardRewardModel<Type, ValueType> StandardRewardModel<Type, ValueType>::divideStateRewardVector(storm::dd::Add<Type> const& divisor) const {
                boost::optional<storm::dd::Add<Type>> modifiedStateRewardVector;
                if (this->hasStateRewards()) {
                    modifiedStateRewardVector = this->optionalStateRewardVector.get() / divisor;
                }
                return StandardRewardModel<Type, ValueType>(modifiedStateRewardVector, this->optionalStateActionRewardVector, this->optionalTransitionRewardMatrix);
            }
            
            template class StandardRewardModel<storm::dd::DdType::CUDD, double>;
        }
    }
}