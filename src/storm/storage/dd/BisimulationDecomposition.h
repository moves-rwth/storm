#pragma once

#include <memory>
#include <vector>

#include "storm/storage/dd/DdType.h"

#include "storm/storage/dd/bisimulation/Partition.h"

#include "storm/models/symbolic/Model.h"

#include "storm/logic/Formula.h"

namespace storm {
    namespace dd {
        
        template <storm::dd::DdType DdType, typename ValueType>
        class BisimulationDecomposition {
        public:
            enum class Status {
                Initialized, InComputation, FixedPoint
            };
            
            BisimulationDecomposition(storm::models::symbolic::Model<DdType, ValueType> const& model);
            BisimulationDecomposition(storm::models::symbolic::Model<DdType, ValueType> const& model, std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas);
            BisimulationDecomposition(storm::models::symbolic::Model<DdType, ValueType> const& model, bisimulation::Partition<DdType, ValueType> const& initialPartition);
            
            /*!
             * Computes the decomposition.
             */
            void compute();
            
            /*!
             * Retrieves the quotient model after the bisimulation decomposition was computed.
             */
            std::shared_ptr<storm::models::Model<ValueType>> getQuotient() const;
            
        private:
            // The status of the computation.
            Status status;
            
            // The model for which to compute the bisimulation decomposition.
            storm::models::symbolic::Model<DdType, ValueType> const& model;
            
            // The current partition in the partition refinement process. Initially set to the initial partition.
            bisimulation::Partition<DdType, ValueType> currentPartition;
        };
        
    }
}
