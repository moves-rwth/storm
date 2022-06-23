// author: Roman Andriushchenko

#include "storm-synthesis/synthesis/Counterexample.h"

#include <queue>
#include <deque>

#include "storm/storage/BitVector.h"
#include "storm/exceptions/UnexpectedException.h"

#include "storm/storage/sparse/JaniChoiceOrigins.h"
#include "storm/storage/sparse/StateValuations.h"

#include "storm/utility/builder.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/storage/sparse/ModelComponents.h"
#include "storm/models/sparse/StateLabeling.h"

#include "storm/solver/OptimizationDirection.h"

#include "storm/api/verification.h"
#include "storm/logic/Bound.h"
#include "storm/modelchecker/CheckTask.h"
#include "storm/modelchecker/hints/ExplicitModelCheckerHint.h"

#include "storm/environment/Environment.h"
#include "storm/environment/solver/SolverEnvironment.h"



namespace storm {
    namespace synthesis {

        template <typename ValueType, typename StateType>
        std::shared_ptr<storm::modelchecker::ExplicitQualitativeCheckResult> CounterexampleGenerator<ValueType,StateType>::labelStates(
            storm::models::sparse::Mdp<ValueType> const& mdp,
            storm::logic::Formula const& label
        ) {
            std::shared_ptr<storm::models::sparse::Mdp<ValueType>> mdp_shared = std::make_shared<storm::models::sparse::Mdp<ValueType>>(mdp);
            bool onlyInitialStatesRelevant = false;
            storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> task(label, onlyInitialStatesRelevant);
            std::unique_ptr<storm::modelchecker::CheckResult> result_ptr = storm::api::verifyWithSparseEngine<ValueType>(mdp_shared, task);
            std::shared_ptr<storm::modelchecker::ExplicitQualitativeCheckResult> mdp_target = std::make_shared<storm::modelchecker::ExplicitQualitativeCheckResult>(result_ptr->asExplicitQualitativeCheckResult());
            return mdp_target;
        }

        template <typename ValueType, typename StateType>
        CounterexampleGenerator<ValueType,StateType>::CounterexampleGenerator (
            storm::models::sparse::Mdp<ValueType> const& quotient_mdp,
            uint_fast64_t hole_count,
            std::vector<std::set<uint_fast64_t>> const& mdp_holes,
            std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulae
            ) : quotient_mdp(quotient_mdp), hole_count(hole_count), mdp_holes(mdp_holes) {

            // create label formulae for our own labels
            std::shared_ptr<storm::logic::Formula const> const& target_label_formula = std::make_shared<storm::logic::AtomicLabelFormula>(this->target_label);
            std::shared_ptr<storm::logic::Formula const> const& until_label_formula = std::make_shared<storm::logic::AtomicLabelFormula>(this->until_label);

            // process all formulae
            for(auto formula: formulae) {

                // store formula type and optimality type
                assert(formula->isOperatorFormula());
                storm::logic::OperatorFormula const& of = formula->asOperatorFormula();
                
                assert(of.hasOptimalityType());
                storm::solver::OptimizationDirection ot = of.getOptimalityType();
                bool is_safety = ot == storm::solver::OptimizationDirection::Minimize;
                this->formula_safety.push_back(is_safety);

                bool is_reward = formula->isRewardOperatorFormula();
                this->formula_reward.push_back(is_reward);
                if(!is_reward) {
                    this->formula_reward_name.push_back("");
                } else {
                    STORM_LOG_THROW(formula->asRewardOperatorFormula().hasRewardModelName(), storm::exceptions::InvalidArgumentException, "Name of the reward model must be specified.");
                    this->formula_reward_name.push_back(formula->asRewardOperatorFormula().getRewardModelName());
                }

                // extract predicate for until and target states and identify such states
                storm::logic::Formula const& osf = of.getSubformula();
                if(!osf.isUntilFormula() && !osf.isEventuallyFormula()) {
                    throw storm::exceptions::NotImplementedException() << "Only until or reachability formulae supported.";
                }

                std::shared_ptr<storm::logic::Formula const> modified_subformula;
                if(osf.isUntilFormula()) {
                    storm::logic::UntilFormula const& uf = osf.asUntilFormula();
                    
                    auto mdp_until = this->labelStates(this->quotient_mdp,uf.getLeftSubformula());
                    this->mdp_untils.push_back(mdp_until);

                    auto mdp_target = this->labelStates(this->quotient_mdp, uf.getRightSubformula());
                    this->mdp_targets.push_back(mdp_target);

                    modified_subformula = std::make_shared<storm::logic::UntilFormula>(until_label_formula, target_label_formula);
                } else if(osf.isEventuallyFormula()) {
                    storm::logic::EventuallyFormula const& ef = osf.asEventuallyFormula();

                    this->mdp_untils.push_back(NULL);

                    auto mdp_target = this->labelStates(this->quotient_mdp,ef.getSubformula());
                    this->mdp_targets.push_back(mdp_target);

                    modified_subformula = std::make_shared<storm::logic::EventuallyFormula>(target_label_formula, ef.getContext());
                }

                // integrate formula into original context
                std::shared_ptr<storm::logic::Formula> modified_formula;
                if(!is_reward) {
                    modified_formula = std::make_shared<storm::logic::ProbabilityOperatorFormula>(modified_subformula, of.getOperatorInformation());
                } else {
                    modified_formula = std::make_shared<storm::logic::RewardOperatorFormula>(modified_subformula, this->formula_reward_name.back(), of.getOperatorInformation());
                }
                this->formula_modified.push_back(modified_formula);     
            }
        }

        template <typename ValueType, typename StateType>
        void CounterexampleGenerator<ValueType,StateType>::prepareDtmc(
            storm::models::sparse::Dtmc<ValueType> const& dtmc,
            std::vector<uint_fast64_t> const& state_map
            ) {
            
            // Clear up previous DTMC metadata
            this->hole_wave.clear();
            this->wave_states.clear();

            // Get DTMC info
            this->dtmc = std::make_shared<storm::models::sparse::Dtmc<ValueType>>(dtmc);
            this->state_map = state_map;
            uint_fast64_t dtmc_states = this->dtmc->getNumberOfStates();
            StateType initial_state = *(this->dtmc->getInitialStates().begin());
            storm::storage::SparseMatrix<ValueType> const& transition_matrix = this->dtmc->getTransitionMatrix();

            // Mark all holes as unregistered
            for(uint_fast64_t index = 0; index < this->hole_count; index++) {
                this->hole_wave.push_back(0);
            }

            // Associate states of a DTMC with relevant holes and store their count
            std::vector<std::set<uint_fast64_t>> dtmc_holes(dtmc_states);
            std::vector<uint_fast64_t> unregistered_holes_count(dtmc_states, 0);
            for(StateType state = 0; state < dtmc_states; state++) {
                dtmc_holes[state] = this->mdp_holes[state_map[state]];
                unregistered_holes_count[state] = dtmc_holes[state].size();
            }

            // Prepare to explore
            // wave increases by one when new holes of a blocking candidate are registered
            uint_fast64_t current_wave = 0;
            // true if the state was reached during exploration (expanded states + both horizons)
            storm::storage::BitVector reachable_flag(dtmc_states, false);
            // non-blocking horizon
            std::stack<StateType> state_horizon;
            // horizon containing, for a current wave, only blocking states
            std::vector<StateType> state_horizon_blocking;
            // blocking state containing currently the least number of unregistered holes + flag if the value was set
            bool blocking_candidate_set = false;
            StateType blocking_candidate;

            // Round 0: encounter initial state first (important)
            this->wave_states.push_back(std::vector<StateType>());
            reachable_flag.set(initial_state);
            if(unregistered_holes_count[initial_state] == 0) {
                // non-blocking
                state_horizon.push(initial_state);
            } else {
                // blocking
                state_horizon_blocking.push_back(initial_state);
                blocking_candidate_set = true;
                blocking_candidate = initial_state;
            }

            // Explore the state space
            while(true) {
                // Expand the non-blocking horizon
                while(!state_horizon.empty()) {
                    StateType state = state_horizon.top();
                    state_horizon.pop();
                    this->wave_states.back().push_back(state);

                    // Reach successors
                    for(auto entry: transition_matrix.getRow(state)) {
                        StateType successor = entry.getColumn();
                        if(reachable_flag[successor]) {
                            // already reached
                            continue;
                        }
                        // new state reached
                        reachable_flag.set(successor);
                        if(unregistered_holes_count[successor] == 0) {
                            // non-blocking
                            state_horizon.push(successor);
                        } else {
                            // blocking
                            state_horizon_blocking.push_back(successor);
                            if(!blocking_candidate_set || unregistered_holes_count[successor] < unregistered_holes_count[blocking_candidate]) {
                                // new blocking candidate
                                blocking_candidate_set = true;
                                blocking_candidate = successor;
                            }
                        }
                    }
                }

                // Non-blocking horizon exhausted
                if(!blocking_candidate_set) {
                    // Nothing more to expand
                    break;
                }
                
                // Start a new wave
                current_wave++;
                this->wave_states.push_back(std::vector<StateType>());
                blocking_candidate_set = false;
                
                // Register all unregistered holes of this blocking state
                for(uint_fast64_t hole: dtmc_holes[blocking_candidate]) {
                    if(this->hole_wave[hole] == 0) {
                        hole_wave[hole] = current_wave;
                        // std::cout << "[storm] hole " << hole << " expanded in wave " << current_wave << std::endl;
                    }
                }

                // Recompute number of unregistered holes in each state
                for(StateType state = 0; state < dtmc_states; state++) {
                    unregistered_holes_count[state] = 0;
                    for(uint_fast64_t hole: dtmc_holes[state]) {
                        if(this->hole_wave[hole] == 0) {
                            unregistered_holes_count[state]++;
                        }
                    }
                }
                
                // Unblock the states from the blocking horizon
                std::vector<StateType> old_blocking_horizon;
                old_blocking_horizon.swap(state_horizon_blocking);
                for(StateType state: old_blocking_horizon) {
                    if(unregistered_holes_count[state] == 0) {
                        // state unblocked
                        state_horizon.push(state);
                    } else {
                        // still blocking
                        state_horizon_blocking.push_back(state);
                        if(!blocking_candidate_set || unregistered_holes_count[state] < unregistered_holes_count[blocking_candidate]) {
                            // new blocking candidate
                            blocking_candidate_set = true;
                            blocking_candidate = state;
                        }
                    }
                }
            }
        }

        template <typename ValueType, typename StateType>
        void CounterexampleGenerator<ValueType,StateType>::prepareSubdtmc (
            uint_fast64_t formula_index,
            std::shared_ptr<storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType> const> mdp_bounds,
            std::vector<StateType> const& mdp_quotient_state_map,
            std::vector<std::vector<std::pair<StateType,ValueType>>> & matrix_subdtmc,
            storm::models::sparse::StateLabeling & labeling_subdtmc,
            std::unordered_map<std::string,storm::models::sparse::StandardRewardModel<ValueType>> & reward_models_subdtmc
            ) {

            // Get DTMC info
            StateType dtmc_states = dtmc->getNumberOfStates();
            
            // Introduce expanded state space
            uint_fast64_t sink_state_false = dtmc_states;
            uint_fast64_t sink_state_true = dtmc_states+1;

            // Label target states of a DTMC
            std::shared_ptr<storm::modelchecker::ExplicitQualitativeCheckResult const> mdp_target = this->mdp_targets[formula_index];
            std::shared_ptr<storm::modelchecker::ExplicitQualitativeCheckResult const> mdp_until = this->mdp_untils[formula_index];
            labeling_subdtmc.addLabel(this->target_label);
            labeling_subdtmc.addLabel(this->until_label);
            for(StateType state = 0; state < dtmc_states; state++) {
                StateType mdp_state = this->state_map[state];
                if((*mdp_target)[mdp_state]) {
                    labeling_subdtmc.addLabelToState(this->target_label, state);
                }
                if(mdp_until != NULL && (*mdp_until)[mdp_state]) {
                    labeling_subdtmc.addLabelToState(this->until_label, state);
                }
            }
            // Associate true sink with the target label
            labeling_subdtmc.addLabelToState(this->target_label, sink_state_true);

            // Map MDP bounds onto the state space of a quotient MDP
            bool have_bounds = mdp_bounds != NULL;
            std::vector<ValueType> quotient_mdp_bounds;
            if(have_bounds) {
                auto const& mdp_values = mdp_bounds->getValueVector();
                quotient_mdp_bounds.resize(this->quotient_mdp.getNumberOfStates());
                uint_fast64_t mdp_states = mdp_values.size();
                for(StateType state = 0; state < mdp_states; state++) {
                    quotient_mdp_bounds[mdp_quotient_state_map[state]] = mdp_values[state];
                }
            }

            

            // Construct transition matrix (as well as the reward model) for the subdtmc
            if(!this->formula_reward[formula_index]) {
                // Probability formula: no reward models
                double default_bound = this->formula_safety[formula_index] ? 0 : 1;
                for(StateType state = 0; state < dtmc_states; state++) {
                    StateType mdp_state = this->state_map[state];
                    std::vector<std::pair<StateType,ValueType>> r;
                    double probability = have_bounds ? quotient_mdp_bounds[mdp_state] : default_bound;
                    r.emplace_back(sink_state_false, 1-probability);
                    r.emplace_back(sink_state_true, probability);
                    matrix_subdtmc.push_back(r);
                }
            } else {
                // Reward formula: one reward model
                assert(mdp_bounds != NULL);
                assert(dtmc->hasRewardModel(this->formula_reward_name[formula_index]));

                std::vector<ValueType> state_rewards_subdtmc(dtmc_states+2);
                double default_reward = 0;
                for(StateType state = 0; state < dtmc_states; state++) {
                    StateType mdp_state = this->state_map[state];
                    double reward = have_bounds ? quotient_mdp_bounds[mdp_state] : default_reward;
                    state_rewards_subdtmc[state] = reward;

                    std::vector<std::pair<StateType,ValueType>> r;
                    r.emplace_back(sink_state_true, 1);
                    matrix_subdtmc.push_back(r);
                }
                storm::models::sparse::StandardRewardModel<ValueType> reward_model_subdtmc(state_rewards_subdtmc, boost::none, boost::none);
                reward_models_subdtmc.emplace(this->formula_reward_name[formula_index], reward_model_subdtmc);
            }

            // Add self-loops to sink states
            for(StateType state = sink_state_false; state <= sink_state_true; state++) {
                std::vector<std::pair<StateType,ValueType>> r;
                r.emplace_back(state, 1);
                matrix_subdtmc.push_back(r);
            }
        }

        template <typename ValueType, typename StateType>
        bool CounterexampleGenerator<ValueType,StateType>::expandAndCheck (
            uint_fast64_t index,
            ValueType formula_bound,
            std::vector<std::vector<std::pair<StateType,ValueType>>> & matrix_subdtmc,
            storm::models::sparse::StateLabeling const& labeling_subdtmc,
            std::unordered_map<std::string,storm::models::sparse::StandardRewardModel<ValueType>> & reward_models_subdtmc,
            std::vector<StateType> const& to_expand
        ) {
            
            // Get DTMC info
            uint_fast64_t dtmc_states = this->dtmc->getNumberOfStates();
            storm::storage::SparseMatrix<ValueType> const& transition_matrix = this->dtmc->getTransitionMatrix();
            StateType initial_state = *(this->dtmc->getInitialStates().begin());
            
            // Expand states from the new wave: 
            // - expand transition probabilities
            // std::cout << "expanded " << to_expand.size() << " states in this wave " << std::endl;
            for(StateType state : to_expand) {
                // std::cout << "holes in state " << state << " : ";
                /*for(auto hole: this->mdp_holes[this->state_map[state]]) {
                    std::cout << hole << ",";
                }*/
                // std::cout << std::endl;
                matrix_subdtmc[state].clear();
                for(auto entry: transition_matrix.getRow(state)) {
                    matrix_subdtmc[state].emplace_back(entry.getColumn(), entry.getValue());
                }
            }
            // std::cout << std::endl;

            if(this->formula_reward[index]) {
                // - expand state rewards
                storm::models::sparse::StandardRewardModel<ValueType> const& reward_model_dtmc = dtmc->getRewardModel(this->formula_reward_name[index]);
                assert(reward_model_dtmc.hasStateRewards() or reward_model_dtmc.hasStateActionRewards());
                storm::models::sparse::StandardRewardModel<ValueType> & reward_model_subdtmc = (reward_models_subdtmc.find(this->formula_reward_name[index]))->second;
                for(StateType state : to_expand) {
                    ValueType reward;
                    if(reward_model_dtmc.hasStateRewards()) {
                        reward = reward_model_dtmc.getStateReward(state);
                    } else {
                        reward = reward_model_dtmc.getStateActionReward(state);
                    }
                    reward_model_subdtmc.setStateReward(state, reward);
                }
            }

            // Construct sub-DTMC
            storm::storage::SparseMatrixBuilder<ValueType> transitionMatrixBuilder(0, 0, 0, false, false, 0);
            for(StateType state = 0; state < dtmc_states+2; state++) {
                for(auto row_entry: matrix_subdtmc[state]) {
                    transitionMatrixBuilder.addNextValue(state, row_entry.first, row_entry.second);
                }
            }
            storm::storage::SparseMatrix<ValueType> sub_matrix = transitionMatrixBuilder.build();
            assert(sub_matrix.isProbabilistic());
            storm::storage::sparse::ModelComponents<ValueType> components(sub_matrix, labeling_subdtmc, reward_models_subdtmc);
            std::shared_ptr<storm::models::sparse::Model<ValueType>> subdtmc = storm::utility::builder::buildModelFromComponents(storm::models::ModelType::Dtmc, std::move(components));
            // std::cout << "[storm] sub-dtmc has " << subdtmc->getNumberOfStates() << " states" << std::endl;

            
            // Construct MC task
            bool onlyInitialStatesRelevant = false;
            storm::modelchecker::CheckTask<storm::logic::Formula, ValueType> task(*(this->formula_modified[index]), onlyInitialStatesRelevant);
            if(this->hint_result != NULL) {
                // Add hints from previous wave
                storm::modelchecker::ExplicitModelCheckerHint<ValueType> hint;
                hint.setComputeOnlyMaybeStates(false);
                hint.setResultHint(boost::make_optional(this->hint_result->asExplicitQuantitativeCheckResult<ValueType>().getValueVector()));
                task.setHint(std::make_shared<storm::modelchecker::ExplicitModelCheckerHint<ValueType>>(hint));
            }
            storm::Environment env;
            // storm::SolverEnvironment & solver = env.solver();
            // std::cout << solver.getLinearEquationSolverType() << std::endl;
            // std::cout << solver.getPrecisionOfLinearEquationSolver() << std::endl;


            // Model check
            // std::unique_ptr<storm::modelchecker::CheckResult> result_ptr = storm::api::verifyWithSparseEngine<ValueType>(subdtmc, task);
            // storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType>& result = result_ptr->asExplicitQuantitativeCheckResult<ValueType>();
            this->timer_model_check.start();
            this->hint_result = storm::api::verifyWithSparseEngine<ValueType>(env, subdtmc, task);
            this->timer_model_check.stop();
            storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType>& result = this->hint_result->asExplicitQuantitativeCheckResult<ValueType>();
            bool satisfied;
            if(this->formula_safety[index]) {
                satisfied = result[initial_state] < formula_bound;
            } else {
                satisfied = result[initial_state] > formula_bound;
            }

            return satisfied;
        }

        template <typename ValueType, typename StateType>
        std::vector<uint_fast64_t> CounterexampleGenerator<ValueType,StateType>::constructConflict (
            uint_fast64_t formula_index,
            ValueType formula_bound,
            std::shared_ptr<storm::modelchecker::ExplicitQuantitativeCheckResult<ValueType> const> mdp_bounds,
            std::vector<StateType> const& mdp_quotient_state_map
            ) {
            this->timer_conflict.start();

            // Clear hint result
            this->hint_result = NULL;
            
            // Get DTMC info
            StateType dtmc_states = this->dtmc->getNumberOfStates();
            
            // Prepare to construct sub-DTMCs
            std::vector<std::vector<std::pair<StateType,ValueType>>> matrix_subdtmc;
            storm::models::sparse::StateLabeling labeling_subdtmc(dtmc_states+2);
            std::unordered_map<std::string, storm::models::sparse::StandardRewardModel<ValueType>> reward_models_subdtmc;
            this->prepareSubdtmc(
                formula_index, mdp_bounds, mdp_quotient_state_map, matrix_subdtmc, labeling_subdtmc, reward_models_subdtmc
            );

            // Explore subDTMCs wave by wave
            uint_fast64_t wave_last = this->wave_states.size()-1;
            uint_fast64_t wave = 0;

            /*std::cout << "[storm] hole-wave: ";
            for(uint_fast64_t hole = 0; hole < this->hole_count; hole++) {
                std::cout << this->hole_wave[hole] << ",";
            }
            std::cout << std::endl;*/
            while(true) {
                bool satisfied = this->expandAndCheck(
                    formula_index, formula_bound, matrix_subdtmc, labeling_subdtmc,
                    reward_models_subdtmc, this->wave_states[wave]
                );
                // std::cout << "[storm] wave " << wave << "/" << wave_last << " : " << satisfied << std::endl;
                if(!satisfied) {
                    break;
                }
                if(wave == wave_last) {
                    break;
                }
                wave++;
            }

            // Return a set of critical holes
            std::vector<uint_fast64_t> critical_holes;
            for(uint_fast64_t hole = 0; hole < this->hole_count; hole++) {
                uint_fast64_t wave_registered = this->hole_wave[hole];
                if(wave_registered > 0 && wave_registered <= wave) {
                    critical_holes.push_back(hole);
                }
            }
            this->timer_conflict.stop();

            return critical_holes;
        }

        template <typename ValueType, typename StateType>
        void CounterexampleGenerator<ValueType,StateType>::printProfiling() {
            std::cout << "[s] conflict: " << this->timer_conflict << std::endl;
            std::cout << "[s]     model checking: " << this->timer_model_check << std::endl;
        }
        



         // Explicitly instantiate functions and classes.
        template class CounterexampleGenerator<double, uint_fast64_t>;

    } // namespace research
} // namespace storm
