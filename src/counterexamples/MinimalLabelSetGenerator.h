/*
 * MinimalLabelSetGenerator.h
 *
 *  Created on: 15.09.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_COUNTEREXAMPLES_MINIMALCOMMANDSETGENERATOR_MDP_H_
#define STORM_COUNTEREXAMPLES_MINIMALCOMMANDSETGENERATOR_MDP_H_

#ifdef HAVE_GUROBI
#include <gurobi_c++.h>
#endif

#include "src/models/Mdp.h" 
#include "src/exceptions/NotImplementedException.h"
#include "src/storage/SparseMatrix.h"
#include "src/storage/BitVector.h"

namespace storm {
    namespace counterexamples {
        
        /*!
         * This class provides functionality to generate a minimal counterexample to a probabilistic reachability
         * property in terms of used labels.
         */
        template <class T>
        class MinimalLabelSetGenerator {

            static std::unordered_set<uint_fast64_t> getMinimalLabelSet(storm::models::Mdp<T> const& labeledMdp, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, T lowerProbabilityBound, bool checkThresholdFeasible = false) {
#ifdef HAVE_GUROBI
                // (1) Check whether its possible to exceed the threshold if checkThresholdFeasible is set.
                // (2) Identify relevant labels and states.
                // (3) Encode resulting system as MILP problem.
                //  (3.1) Initialize MILP solver.
                //  (3.2) Create variables.
                //  (3.3) Construct objective function.
                //  (3.4) Construct constraint system.
                // (4) Read off result from MILP variables.
                // (5) Potentially verify whether the resulting system exceeds the given threshold.
                // (6) Return result.
#else
                throw storm::exceptions::NotImplementedException() << "This functionality is unavailable if StoRM is compiled without support for Gurobi.";
#endif
            }
            
        };
        
    }
}

#endif /* STORM_COUNTEREXAMPLES_MINIMALLABELSETGENERATOR_MDP_H_ */
