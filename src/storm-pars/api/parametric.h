#pragma once

#include <memory>

#include "storm-pars/transformer/SparseParametricDtmcSimplifier.h"
#include "storm-pars/transformer/SparseParametricMdpSimplifier.h"

#include "storm/models/sparse/Model.h"
#include "storm/logic/Formula.h"
#include "storm/utility/macros.h"

namespace storm {
    namespace api {
        
        template <typename ValueType>
        bool simplifyParametricModel(std::shared_ptr<storm::models::sparse::Model<ValueType> const& inputModel, std::shared_ptr<storm::logic::Formula const> const& inputFormula, std::shared_ptr<storm::models::sparse::Model<ValueType>& outputModel, std::shared_ptr<storm::logic::Formula const>& outputFormula) {
            
            if (inputModel->isOfType(storm::models::ModelType::Dtmc)) {
                auto const& dtmc = *inputModel->template as<storm::models::sparse::Dtmc<ValueType>>();
                auto simplifier = storm::transformer::SparseParametricDtmcSimplifier<storm::models::sparse::Dtmc<ValueType>>(dtmc);
                if (simplifier.simplify(*inputFormula)) {
                    outputModel = simplifier.getSimplifiedModel();
                    outputFormula = simplifier.getSimplifiedFormula();
                    return true;
                }
            } else if (inputModel->isOfType(storm::models::ModelType::Mdp)) {
                auto const& mdp = *inputModel->template as<storm::models::sparse::Mdp<ValueType>>();
                auto simplifier = storm::transformer::SparseParametricMdpSimplifier<storm::models::sparse::Mdp<ValueType>>(mdp);
                if (simplifier.simplify(*inputFormula)) {
                    outputModel = simplifier.getSimplifiedModel();
                    outputFormula = simplifier.getSimplifiedFormula();
                    return true;
                }
            } else {
                STORM_LOG_ERROR("Unable to simplify model with the given type.");
            }

            // Reaching this point means that simplification was not successful.
            outputModel = nullptr;
            outputFormula = nullptr;
            return false;
            
        }
    }
}
