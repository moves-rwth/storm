#include "storm/builder/ExplicitModelBuilderResult.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidOperationException.h"

#include "storm/adapters/CarlAdapter.h"
#include "storm/models/sparse/StandardRewardModel.h"

namespace storm {
    namespace builder {
        
        template <typename ValueType, typename RewardModelType>
        ExplicitModelBuilderResult<ValueType, RewardModelType>::ExplicitModelBuilderResult(std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>> model, std::shared_ptr<storm::storage::sparse::StateValuations> stateValuations,std::shared_ptr<storm::storage::sparse::ChoiceOrigins> choiceOrigins) : model(model), stateValuations(stateValuations), choiceOrigins(choiceOrigins) {
            // Intentionally left empty
        }
            
        template <typename ValueType, typename RewardModelType>
        std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>>& ExplicitModelBuilderResult<ValueType, RewardModelType>::getModel() {
            STORM_LOG_THROW(model, storm::exceptions::InvalidOperationException, "Retrieving the model failed since it is not set.");
            return model;
        }
        
        template <typename ValueType, typename RewardModelType>
        std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>> const& ExplicitModelBuilderResult<ValueType, RewardModelType>::getModel() const {
            STORM_LOG_THROW(model, storm::exceptions::InvalidOperationException, "Retrieving the model failed since it is not set.");
            return model;
        }
            
        template <typename ValueType, typename RewardModelType>
        bool ExplicitModelBuilderResult<ValueType, RewardModelType>::hasStateValuations() {
            return static_cast<bool>(stateValuations);
        }
        
        template <typename ValueType, typename RewardModelType>
        std::shared_ptr<storm::storage::sparse::StateValuations>& ExplicitModelBuilderResult<ValueType, RewardModelType>::getStateValuations() {
            STORM_LOG_THROW(stateValuations, storm::exceptions::InvalidOperationException, "Retrieving the state valuations failed since they are not set.");
            return stateValuations;
        }
        
        template <typename ValueType, typename RewardModelType>
        std::shared_ptr<storm::storage::sparse::StateValuations> const& ExplicitModelBuilderResult<ValueType, RewardModelType>::getStateValuations() const {
            STORM_LOG_THROW(stateValuations, storm::exceptions::InvalidOperationException, "Retrieving the state valuations failed since they are not set.");
            return stateValuations;
        }
            
        template <typename ValueType, typename RewardModelType>
        bool ExplicitModelBuilderResult<ValueType, RewardModelType>::hasChoiceOrigins() {
            return static_cast<bool>(choiceOrigins);
        }
        
        template <typename ValueType, typename RewardModelType>
        std::shared_ptr<storm::storage::sparse::ChoiceOrigins>& ExplicitModelBuilderResult<ValueType, RewardModelType>::getChoiceOrigins() {
            STORM_LOG_THROW(choiceOrigins, storm::exceptions::InvalidOperationException, "Retrieving the choice origins failed since they are not set.");
            return choiceOrigins;
        }
        
        template <typename ValueType, typename RewardModelType>
        std::shared_ptr<storm::storage::sparse::ChoiceOrigins> const& ExplicitModelBuilderResult<ValueType, RewardModelType>::getChoiceOrigins() const {
            STORM_LOG_THROW(choiceOrigins, storm::exceptions::InvalidOperationException, "Retrieving the choice origins failed since they are not set.");
            return choiceOrigins;
        }
            
        // Explicitly instantiate the class.
        template class ExplicitModelBuilderResult<double, storm::models::sparse::StandardRewardModel<double>>;

#ifdef STORM_HAVE_CARL
        template class ExplicitModelBuilderResult<RationalNumber, storm::models::sparse::StandardRewardModel<RationalNumber>>;
        template class ExplicitModelBuilderResult<RationalFunction, storm::models::sparse::StandardRewardModel<RationalFunction>>;
        template class ExplicitModelBuilderResult<double, storm::models::sparse::StandardRewardModel<storm::Interval>>;
#endif
        
    }
}