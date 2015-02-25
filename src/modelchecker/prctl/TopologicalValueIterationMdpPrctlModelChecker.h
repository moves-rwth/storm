#ifndef STORM_MODELCHECKER_PRCTL_TOPOLOGICALVALUEITERATIONSMDPPRCTLMODELCHECKER_H_
#define STORM_MODELCHECKER_PRCTL_TOPOLOGICALVALUEITERATIONSMDPPRCTLMODELCHECKER_H_

#include "src/modelchecker/prctl/SparseMdpPrctlModelChecker.h"
#include "src/solver/TopologicalValueIterationNondeterministicLinearEquationSolver.h"
#include "src/exceptions/InvalidPropertyException.h"
#include <cmath>

namespace storm {
    namespace modelchecker {
        
        /*
         * An implementation of the SparseMdpPrctlModelChecker interface that uses topoligical value iteration for solving
         * equation systems.
         */
        template <class ValueType>
        class TopologicalValueIterationMdpPrctlModelChecker : public SparseMdpPrctlModelChecker<ValueType> {
        public:
            /*!
             * Constructs a SparseMdpPrctlModelChecker with the given model.
             *
             * @param model The MDP to be checked.
             */
            explicit TopologicalValueIterationMdpPrctlModelChecker(storm::models::sparse::Mdp<ValueType> const& model) : SparseMdpPrctlModelChecker<ValueType>(model, std::shared_ptr<storm::solver::NondeterministicLinearEquationSolver<ValueType>>(new storm::solver::TopologicalValueIterationNondeterministicLinearEquationSolver<ValueType>())) {
                // Intentionally left empty.
            }
            
            /*!
             * Copy constructs a SparseMdpPrctlModelChecker from the given model checker. In particular, this means that the newly
             * constructed model checker will have the model of the given model checker as its associated model.
             */
            explicit TopologicalValueIterationMdpPrctlModelChecker(storm::modelchecker::TopologicalValueIterationMdpPrctlModelChecker<ValueType> const& modelchecker)
            : SparseMdpPrctlModelChecker<ValueType>(modelchecker) {
                // Intentionally left empty.
            }
        };
        
    } // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_PRCTL_TOPOLOGICALVALUEITERATIONSMDPPRCTLMODELCHECKER_H_ */
