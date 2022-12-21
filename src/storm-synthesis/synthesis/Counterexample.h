// author: Roman Andriushchenko

#ifndef STORM_SYNTHESIS_COUNTEREXAMPLE_H
#define STORM_SYNTHESIS_COUNTEREXAMPLE_H

#include "storm/storage/jani/Model.h"
#include "storm/logic/Formula.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"

#include "storm/models/sparse/Dtmc.h"
#include "storm/utility/Stopwatch.h"

namespace storm {
    namespace synthesis {

        template<typename ValueType = double, typename StateType = uint_fast64_t>
        class CounterexampleGenerator {
        public:

            /*!
             * Preprocess the quotient MDP and its bound on the reachability
             * probability before constructing counterexamples from various
             * deterministic sub-MDPs (DTMCs).
             * @param quotient_mdp The quotient MDP.
             * @param hole_count Total number of holes.
             * @param mdp_holes For each state of a quotient MDP, a set of
             *   indices of significant holes.
             * @param formulae Formulae to check, can be both
             *   probabilistic and reward-based.
             */
            CounterexampleGenerator(
                storm::models::sparse::Mdp<ValueType> const& quotient_mdp,
                uint_fast64_t hole_count,
                std::vector<std::set<uint_fast64_t>> const& mdp_holes,
                std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulae
                );

            /*!
             * Preprocess the DTMC by establishing the state expansion order (waves):
             * - explore the reachable state space wave by wave
             * - during each wave, expand only 'non-blocking' states (states with registered holes)
             * - if no non-blocking state remains, pick a blocking candidate with the least amount of unregistered holes
             * - register all holes in this blocking candidate, thus unblocking this state (and possibly many others)
             * - rinse and repeat
             * @param dtmc A deterministic MDP (DTMC).
             * @param state_map DTMC-MDP state mapping.
             
             */
            void prepareDtmc(
                storm::models::sparse::Dtmc<ValueType> const& dtmc,
                std::vector<uint_fast64_t> const& state_map
                );
            
            /*!
             * Construct a counterexample to a prepared DTMC and a formula with
             * the given index.
             * @param formula_index Formula index.
             * @param formula_bound Formula threshold for CE construction.
             * @param mdp_bounds MDP model checking result in the primary direction (NULL if not used).
             * @param mdp_quotient_state_mdp A mapping of MDP states to the states of a quotient MDP.
             * @return A list of holes relevant in the CE.
             */
            std::vector<uint_fast64_t> constructConflict(
                uint_fast64_t formula_index,
                ValueType formula_bound,
                std::shared_ptr<storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType> const> mdp_bounds,
                std::vector<StateType> const& mdp_quotient_state_map
                );

            /*!
             * TODO
             */
            void printProfiling();

        protected:

            /** Identify states of an MDP having some label. */
            std::shared_ptr<storm::modelchecker::ExplicitQualitativeCheckResult> labelStates(
                storm::models::sparse::Mdp<ValueType> const& mdp,
                storm::logic::Formula const& label
                );

            /**
             * Prepare data structures for sub-DTMC construction.
             * @param formula_index Formula index.
             * @param mdp_bounds MDP model checking result in the primary direction.
             * @param matrix_dtmc (output) Copy of the transition matrix of the DTMC.
             * @param matrix_subdtmc (output) Matrix of shortcuts.
             * @param labeling_subdtdmc (output) Labeling marking target states.
             * @param reward_model_subdtmc (output) If the reward property is
             *   investigated, this map will contain exactly one reward model
             *   for the initial sub-DTMC.
             */
            void prepareSubdtmc(
                uint_fast64_t formula_index,
                std::shared_ptr<storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType> const> mdp_bounds,
                std::vector<StateType> const& mdp_quotient_state_map,
                std::vector<std::vector<std::pair<StateType,ValueType>>> & matrix_subdtmc,
                storm::models::sparse::StateLabeling & labeling_subdtmc,
                std::unordered_map<std::string,storm::models::sparse::StandardRewardModel<ValueType>> & reward_models_subdtmc
                );

            /**
             * Expand new wave and model check resulting rerouting of a DTMC.
             * @param dtmc A DTMC.
             * @param labeling Prototype labeling.
             * @param matrix_dtmc Original transition matrix.
             * @param matrix_subdtmc Rerouting of the transition matrix wrt. unexpanded states.
             * @param reward_models_subdtmc Reward models for the initial sub-DTMC.
             * @param to_expand States expanded during this wave.
             * @return true if the rerouting still satisfies the formula
             */
            bool expandAndCheck(
                uint_fast64_t index,
                ValueType formula_bound,
                std::vector<std::vector<std::pair<StateType,ValueType>>> & matrix_subdtmc,
                storm::models::sparse::StateLabeling const& labeling_subdtmc,
                std::unordered_map<std::string,storm::models::sparse::StandardRewardModel<ValueType>> & reward_models_subdtmc,
                std::vector<StateType> const& to_expand
                );

            // Quotient MDP
            storm::models::sparse::Mdp<ValueType> const& quotient_mdp;
            // Number of significant holes
            uint_fast64_t hole_count;
            // Significant holes in MDP states
            std::vector<std::set<uint_fast64_t>> mdp_holes;

            // Formula bounds: safety (<,<=) or liveness (>,>=)
            std::vector<bool> formula_safety;
            // Formula types: probability (false) or reward-based (true)
            std::vector<bool> formula_reward;
            // Reward model names for reward formulae
            std::vector<std::string> formula_reward_name;
            
            // Until label for sub-dtmcs
            const std::string until_label = "__until__";
            // Target label for sub-dtmcs
            const std::string target_label = "__target__";
            // Modified operator formulae to apply to sub-dtmcs: P~?["__until" U "__target__"] or P~?[F "__target__"]
            std::vector<std::shared_ptr<storm::logic::Formula>> formula_modified;
            // Flags for until states
            std::vector<std::shared_ptr<storm::modelchecker::ExplicitQualitativeCheckResult const>> mdp_untils;
            // Flags for target states
            std::vector<std::shared_ptr<storm::modelchecker::ExplicitQualitativeCheckResult const>> mdp_targets;

            // DTMC under investigation
            std::shared_ptr<storm::models::sparse::Dtmc<ValueType>> dtmc;
            // DTMC to MDP state mapping
            std::vector<uint_fast64_t> state_map;
            // For each hole, a wave when it was registered (0 = unregistered).
            std::vector<uint_fast64_t> hole_wave;
            // For each wave, a set of states that were expanded.
            std::vector<std::vector<StateType>> wave_states;

            // Hint for future model checking.
            std::unique_ptr<storm::modelchecker::CheckResult> hint_result;

            // Profiling
            storm::utility::Stopwatch timer_conflict;
            storm::utility::Stopwatch timer_model_check;

        };

    } // namespace research
} // namespace storm

#endif  /* STORM_SYNTHESIS_COUNTEREXAMPLE_H */
