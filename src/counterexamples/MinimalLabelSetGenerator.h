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
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace counterexamples {
        
        /*!
         * This class provides functionality to generate a minimal counterexample to a probabilistic reachability
         * property in terms of used labels.
         */
        template <class T>
        class MinimalLabelSetGenerator {

        public:
            
            static std::unordered_set<uint_fast64_t> getMinimalLabelSet(storm::models::Mdp<T> const& labeledMdp, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, T lowerProbabilityBound, bool checkThresholdFeasible = false) {
#ifdef HAVE_GUROBI
                // (0) Check whether the MDP is indeed labeled.
                if (!labeledMdp.hasChoiceLabels()) {
                    throw storm::exceptions::InvalidArgumentException() << "Minimal label set generation is impossible for unlabeled model.";
                }
                
                // (1) TODO: check whether its possible to exceed the threshold if checkThresholdFeasible is set.
                
                // (2) Identify relevant and problematic states.
                storm::storage::SparseMatrix<bool> backwardTransitions = labeledMdp.getBackwardTransitions();
                storm::storage::BitVector relevantStates = storm::utility::graph::performProbGreater0E(labeledMdp, backwardTransitions, phiStates, psiStates);
                relevantStates &= ~psiStates;
                storm::storage::BitVector problematicStates = storm::utility::graph::performProbGreater0E(labeledMdp, backwardTransitions, phiStates, psiStates);
                problematicStates.complement();
                problematicStates &= relevantStates;
                
                // (3) Determine set of relevant labels.
                std::unordered_set<uint_fast64_t> relevantLabels;
                storm::storage::SparseMatrix<T> const& transitionMatrix = labeledMdp.getTransitionMatrix();
                std::vector<uint_fast64_t> const& nondeterministicChoiceIndices = labeledMdp.getNondeterministicChoiceIndices();
                std::vector<std::list<uint_fast64_t>> const& choiceLabeling = labeledMdp.getChoiceLabeling();
                // Now traverse all choices of all relevant states and check whether there is a relevant target state.
                // If so, the associated labels become relevant.
                for (auto state : relevantStates) {
                    for (uint_fast64_t row = nondeterministicChoiceIndices[state]; row < nondeterministicChoiceIndices[state + 1]; ++row) {
                        for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator successorIt = transitionMatrix.constColumnIteratorBegin(row); successorIt != transitionMatrix.constColumnIteratorEnd(row); ++successorIt) {
                            // If there is a relevant successor, we need to add the labels of the current choice.
                            if (relevantStates.get(*successorIt)) {
                                for (auto const& label : choiceLabeling[row]) {
                                    relevantLabels.insert(label);
                                }
                            }
                        }
                    }
                }
                LOG4CPLUS_INFO(logger, "Found " << relevantLabels.size() << " relevant labels.");
                
                // (3) Encode resulting system as MILP problem.
                //  (3.1) Initialize MILP solver and model.
                // GRBEnv env;
                // GRBModel model = GRBModel(env);
                
                //  (3.2) Create variables.
                
                // Create variables for involved labels.
                std::vector<std::pair<uint_fast64_t, GRBVar>> labelVariables;
                std::stringstream variableNameBuffer;
                for (auto const& label : relevantLabels) {
                    variableNameBuffer.clear();
                    variableNameBuffer << "label" << label;
                    // labelVariables.emplace_back(label, model.addVar(0.0, 1.0, 0.0, GRB_BINARY, variableNameBuffer.str()));
                }
                
                //  (3.3) Construct objective function.
                //  (3.4) Construct constraint system.
                // (4) Read off result from MILP variables.
                // (5) Potentially verify whether the resulting system exceeds the given threshold.
                // (6) Return result.
                
                // FIXME: Return fake result for the time being.
                return relevantLabels;
                
#else
                throw storm::exceptions::NotImplementedException() << "This functionality is unavailable if StoRM is compiled without support for Gurobi.";
#endif
            }
            
        };
        
    }
}

#endif /* STORM_COUNTEREXAMPLES_MINIMALLABELSETGENERATOR_MDP_H_ */
