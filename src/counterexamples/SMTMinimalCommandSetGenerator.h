/*
 * SMTMinimalCommandSetGenerator.h
 *
 *  Created on: 01.10.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_COUNTEREXAMPLES_SMTMINIMALCOMMANDSETGENERATOR_MDP_H_
#define STORM_COUNTEREXAMPLES_SMTMINIMALCOMMANDSETGENERATOR_MDP_H_

// To detect whether the usage of Z3 is possible, this include is neccessary.
#include "storm-config.h"

namespace storm {
    namespace counterexamples {
        
        /*!
         * This class provides functionality to generate a minimal counterexample to a probabilistic reachability
         * property in terms of used labels.
         */
        template <class T>
        class MinimalLabelSetGenerator {
#ifdef STORM_HAVE_Z3
        private:
            
#endif
            
        public:
            static std::unordered_set<uint_fast64_t> getMinimalCommandSet(storm::ir::Program const& program, storm::models::Mdp<T> const& labeledMdp, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, double probabilityThreshold, bool checkThresholdFeasible = false) {
#ifdef STORM_HAVE_Z3
                // (0) Check whether the MDP is indeed labeled.
                if (!labeledMdp.hasChoiceLabels()) {
                    throw storm::exceptions::InvalidArgumentException() << "Minimal command set generation is impossible for unlabeled model.";
                }
                
                
                
                return std::unordered_set<uint_fast64_t>();
                
#else
                throw storm::exceptions::NotImplementedException() << "This functionality is unavailable since StoRM has been compiled without support for Z3.";
#endif
            }
            
        }
        
    } // namespace counterexamples
} // namespace storm

#endif /* STORM_COUNTEREXAMPLES_SMTMINIMALCOMMANDSETGENERATOR_MDP_H_ */
