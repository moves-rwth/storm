#pragma once

#include <memory>
#include "storm/models/sparse/Model.h"
#include "storm/storage/sparse/StateValuations.h"
#include "storm/storage/sparse/ChoiceOrigins.h"

namespace storm {
    namespace builder {
        
        template<typename ValueType, typename RewardModelType = storm::models::sparse::StandardRewardModel<ValueType>>
        class ExplicitModelBuilderResult {
        public:
            ExplicitModelBuilderResult(std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>> model, std::shared_ptr<storm::storage::sparse::StateValuations> stateValuations,std::shared_ptr<storm::storage::sparse::ChoiceOrigins> choiceOrigins);
            
            std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>>& getModel();
            std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>> const& getModel() const;
            
            bool hasStateValuations();
            std::shared_ptr<storm::storage::sparse::StateValuations>& getStateValuations();
            std::shared_ptr<storm::storage::sparse::StateValuations> const& getStateValuations() const;
            
            bool hasChoiceOrigins();
            std::shared_ptr<storm::storage::sparse::ChoiceOrigins>& getChoiceOrigins();
            std::shared_ptr<storm::storage::sparse::ChoiceOrigins> const& getChoiceOrigins() const;
            
        private:
            std::shared_ptr<storm::models::sparse::Model<ValueType, RewardModelType>> model;
            std::shared_ptr<storm::storage::sparse::StateValuations> stateValuations;
            std::shared_ptr<storm::storage::sparse::ChoiceOrigins> choiceOrigins;
        };
    }
}