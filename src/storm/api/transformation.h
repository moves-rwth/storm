#pragma once

#include "storm/transformer/ContinuousToDiscreteTimeModelTransformer.h"
#include "storm/transformer/SymbolicToSparseTransformer.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidOperationException.h"
#include "storm/exceptions/NotSupportedException.h"

namespace storm {
    namespace api {
        
        /*!
         * Transforms the given continuous model to a discrete time model.
         * If such a transformation does not preserve one of the given formulas, an error is issued.
         */
        template <typename ValueType>
        std::shared_ptr<storm::models::sparse::Model<ValueType>> transformContinuousToDiscreteTimeSparseModel(std::shared_ptr<storm::models::sparse::Model<ValueType>> const& model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas) {
            
            storm::transformer::ContinuousToDiscreteTimeModelTransformer<ValueType> transformer;
            
            for (auto const& formula : formulas) {
                STORM_LOG_THROW(transformer.preservesFormula(*formula), storm::exceptions::InvalidOperationException, "Transformation to discrete time model does not preserve formula " << *formula << ".");
            }
            
            if (model->isOfType(storm::models::ModelType::Ctmc)) {
                return transformer.transform(*model->template as<storm::models::sparse::Ctmc<ValueType>>());
            } else if (model->isOfType(storm::models::ModelType::MarkovAutomaton)) {
                return transformer.transform(*model->template as<storm::models::sparse::MarkovAutomaton<ValueType>>());
            } else {
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Transformation of a " << model->getType() << " to a discrete time model is not supported");
            }
            return nullptr;
        }

        /*!
         * Transforms the given continuous model to a discrete time model IN PLACE.
         * This means that the input continuous time model is replaced by the new discrete time model.
         * If such a transformation does not preserve one of the given formulas, an error is issued.
         */
        template <typename ValueType>
        std::shared_ptr<storm::models::sparse::Model<ValueType>> transformContinuousToDiscreteTimeSparseModel(storm::models::sparse::Model<ValueType>&& model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas) {
            
            storm::transformer::ContinuousToDiscreteTimeModelTransformer<ValueType> transformer;
            
            for (auto const& formula : formulas) {
                STORM_LOG_THROW(transformer.preservesFormula(*formula), storm::exceptions::InvalidOperationException, "Transformation to discrete time model does not preserve formula " << *formula << ".");
            }
            
            if (model.isOfType(storm::models::ModelType::Ctmc)) {
                transformer.transform(std::move(*model.template as<storm::models::sparse::Ctmc<ValueType>>()));
            } else if (model.isOfType(storm::models::ModelType::MarkovAutomaton)) {
                transformer.transform(std::move(*model.template as<storm::models::sparse::MarkovAutomaton<ValueType>>()));
            } else {
                STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Transformation of a " << model.getType() << " to a discrete time model is not supported.");
            }
            return nullptr;
            
        }
        
        /*!
         * Transforms the given symbolic model to a sparse model.
         */
        template<storm::dd::DdType Type, typename ValueType>
        std::shared_ptr<storm::models::sparse::Model<ValueType>> transformSymbolicToSparseModel(std::shared_ptr<storm::models::symbolic::Model<Type, ValueType>> const& symbolicModel) {
            switch (symbolicModel->getType()) {
                case storm::models::ModelType::Dtmc:
                    return storm::transformer::SymbolicDtmcToSparseDtmcTransformer<Type, ValueType>().translate(*symbolicModel->template as<storm::models::symbolic::Dtmc<Type, ValueType>>());
                case storm::models::ModelType::Mdp:
                    return storm::transformer::SymbolicMdpToSparseMdpTransformer<Type, ValueType>::translate(*symbolicModel->template as<storm::models::symbolic::Mdp<Type, ValueType>>());
                case storm::models::ModelType::Ctmc:
                    return storm::transformer::SymbolicCtmcToSparseCtmcTransformer<Type, ValueType>::translate(*symbolicModel->template as<storm::models::symbolic::Ctmc<Type, ValueType>>());
                default:
                    STORM_LOG_THROW(false, storm::exceptions::NotSupportedException, "Transformation of symbolic " << symbolicModel->getType() << " to sparse model is not supported.");
            }
            return nullptr;
        }
        
    }
}
