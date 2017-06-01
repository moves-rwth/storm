#pragma once

#include "storm/storage/bisimulation/DeterministicModelBisimulationDecomposition.h"
#include "storm/storage/bisimulation/NondeterministicModelBisimulationDecomposition.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/NotSupportedException.h"

namespace storm {
    namespace api {
        
        template <typename ModelType>
        std::shared_ptr<ModelType> performDeterministicSparseBisimulationMinimization(std::shared_ptr<ModelType> model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, storm::storage::BisimulationType type) {
            typename storm::storage::DeterministicModelBisimulationDecomposition<ModelType>::Options options;
            if (!formulas.empty()) {
                options = typename storm::storage::DeterministicModelBisimulationDecomposition<ModelType>::Options(*model, formulas);
            }
            options.setType(type);
            
            storm::storage::DeterministicModelBisimulationDecomposition<ModelType> bisimulationDecomposition(*model, options);
            bisimulationDecomposition.computeBisimulationDecomposition();
            return bisimulationDecomposition.getQuotient();
        }
        
        template<typename ModelType>
        std::shared_ptr<ModelType> performNondeterministicSparseBisimulationMinimization(std::shared_ptr<ModelType> model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, storm::storage::BisimulationType type) {
            typename storm::storage::DeterministicModelBisimulationDecomposition<ModelType>::Options options;
            if (!formulas.empty()) {
                options = typename storm::storage::NondeterministicModelBisimulationDecomposition<ModelType>::Options(*model, formulas);
            }
            options.setType(type);
            
            storm::storage::NondeterministicModelBisimulationDecomposition<ModelType> bisimulationDecomposition(*model, options);
            bisimulationDecomposition.computeBisimulationDecomposition();
            return bisimulationDecomposition.getQuotient();
        }
        
        template <typename ValueType>
        std::shared_ptr<storm::models::sparse::Model<ValueType>> performBisimulationMinimization(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas, storm::storage::BisimulationType type = storm::storage::BisimulationType::Strong) {
            
            STORM_LOG_THROW(model->isOfType(storm::models::ModelType::Dtmc) || model->isOfType(storm::models::ModelType::Ctmc) || model->isOfType(storm::models::ModelType::Mdp), storm::exceptions::NotSupportedException, "Bisimulation minimization is currently only available for DTMCs, CTMCs and MDPs.");

            model->reduceToStateBasedRewards();

            if (model->isOfType(storm::models::ModelType::Dtmc)) {
                return performDeterministicSparseBisimulationMinimization<storm::models::sparse::Dtmc<ValueType>>(model->template as<storm::models::sparse::Dtmc<ValueType>>(), formulas, type);
            } else if (model->isOfType(storm::models::ModelType::Ctmc)) {
                return performDeterministicSparseBisimulationMinimization<storm::models::sparse::Ctmc<ValueType>>(model->template as<storm::models::sparse::Ctmc<ValueType>>(), formulas, type);
            } else {
                return performNondeterministicSparseBisimulationMinimization<storm::models::sparse::Mdp<ValueType>>(model->template as<storm::models::sparse::Mdp<ValueType>>(), formulas, type);
            }
        }
        
    }
}
