#pragma once

#include "storm/storage/BitVector.h"
#include "storm/solver/SmtSolver.h"
#include "storm/utility/solver.h"

namespace storm {
    namespace storage {
        namespace geometry {
            template<typename ValueType>
            class ReduceVertexCloud {
            public:
                ReduceVertexCloud(std::shared_ptr<storm::utility::solver::SmtSolverFactory>& smtSolverFactory, ValueType wiggle = storm::utility::convertNumber<ValueType>(0.001))
                : smtSolverFactory(smtSolverFactory), wiggle(wiggle)
                {

                }

                storm::storage::BitVector eliminate(std::vector<std::map<uint64_t, ValueType>> const& input, uint64_t maxdimension);

            private:
                std::shared_ptr<storm::utility::solver::SmtSolverFactory>& smtSolverFactory;
                ValueType wiggle;

            };

        }
    }
}