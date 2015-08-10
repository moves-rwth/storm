
#ifndef PERMISSIVESCHEDULERS_H
#define	PERMISSIVESCHEDULERS_H

#include <unordered_map>
#include "expressions/Variable.h"
#include "StateActionPair.h"
#include "StateActionTargetTuple.h"

namespace storm {
    namespace storage {
        class PermissiveSchedulerPenalties {
            std::unordered_map<StateActionPair, double> mPenalties;
            
        public:
            double get(uint_fast64_t state, uint_fast64_t action) const {
                return get(StateActionPair(state, action));
              
            }
            
            
            double get(StateActionPair const& sap) const {
                auto it = mPenalties.find(sap);
                if(it == mPenalties.end()) {
                    return 1.0;
                }
                else {
                    return it->second;
                }
            }
            
            void set(uint_fast64_t state, uint_fast64_t action, double penalty) {
                assert(penalty >= 1.0);
                if(penalty == 1.0) {
                    auto it = mPenalties.find(std::make_pair(state, action));
                    if(it != mPenalties.end()) {
                        mPenalties.erase(it);
                    }
                } else {
                    mPenalties.emplace(std::make_pair(state, action), penalty);
                }
            }
            
            void clear() {
                mPenalties.clear();
            }
            
        };
        
        class MilpPermissiveSchedulerComputation {
        private:
           
            bool mCalledOptimizer = false;
            storm::solver::LpSolver& solver;
            std::shared_ptr<storm::models::sparse::Mdp<double>> mdp;
            std::unordered_map<StateActionPair, storm::expressions::Variable> multistrategyVariables;
            std::unordered_map<uint_fast64_t, storm::expressions::Variable> mProbVariables;
            std::unordered_map<uint_fast64_t, storm::expressions::Variable> mAlphaVariables;
            std::unordered_map<StateActionTarget, storm::expressions::Variable> mBetaVariables;
            std::unordered_map<uint_fast64_t, storm::expressions::Variable> mGammaVariables;
            BitVector const& mGoals;
            BitVector const& mSinks;
            
        public:
            
            MilpPermissiveSchedulerComputation(storm::solver::LpSolver& milpsolver, std::shared_ptr<storm::models::sparse::Mdp<double>> mdp, BitVector const& goalstates, BitVector const& sinkstates)
                : solver(milpsolver), mdp(mdp), mGoals(goalstates), mSinks(sinkstates)
            {
                
            }
            
                    
            void calculatePermissiveScheduler(double boundary, PermissiveSchedulerPenalties const& penalties, BitVector const& irrelevantStates = BitVector()) {
                createMILP(boundary, penalties, irrelevantStates);
                solver.optimize();
                mCalledOptimizer = true;
            }
            
            bool foundSolution() {
                assert(mCalledOptimizer);
                return !solver.isInfeasible();
            }
            
            BitVector getAllowedStateActionPairs() const {
                BitVector result(mdp->getNumberOfChoices(), true);
                for(auto const& entry : multistrategyVariables) {
                    if(!solver.getBinaryValue(entry.second)) {
                        result.set(mdp->getNondeterministicChoiceIndices()[entry.first.getState()]+entry.first.getAction(), false);
                    }
                }
                return result;
            }
            
                    
        private:
            
            /**
             * 
             */
            void createVariables(PermissiveSchedulerPenalties const& penalties, BitVector const& relevantStates) {
                // We need the unique initial state later, so we get that one before looping.
                assert(mdp->getInitialStates().getNumberOfSetBits() == 1);
                uint_fast64_t initialStateIndex = mdp->getInitialStates().getNextSetIndex(0);
                    
                storm::expressions::Variable var;
                for(uint_fast64_t s : relevantStates) {
                    // Create x_s variables
                    // Notice that only the initial probability appears in the objective.
                    if(s == initialStateIndex) {
                        var = solver.addLowerBoundedContinuousVariable("x_" + std::to_string(s), 0.0, -1.0);
                    } else {
                        var = solver.addLowerBoundedContinuousVariable("x_" + std::to_string(s), 0.0);
                    }
                    mProbVariables[s] = var;
                    // Create alpha_s variables
                    var = solver.addBinaryVariable("alp_" + std::to_string(s));
                    mAlphaVariables[s] = var;
                    // Create gamma_s variables
                    var = solver.addBoundedContinuousVariable("gam_" + std::to_string(s), 0.0, 1.0);
                    mGammaVariables[s] = var;
                    for(uint_fast64_t a = 0; a < mdp->getNumberOfChoices(s); ++a) {
                        auto stateAndAction = StateActionPair(s,a);
                        // Create y_(s,a) variables
                        
                        double penalty = penalties.get(stateAndAction);
                        var = solver.addBinaryVariable("y_"  + std::to_string(s) + "_" + std::to_string(a), -penalty);
                        multistrategyVariables[stateAndAction] = var;
                        
                        // Create beta_(s,a,t) variables
                        // Iterate over successors of s via a.
                        for(auto const& entry : mdp->getTransitionMatrix().getRow(mdp->getNondeterministicChoiceIndices()[s]+a)) {
                            if(entry.getValue() != 0) {
                                StateActionTarget sat = {s,a,entry.getColumn()};
                                var = solver.addBinaryVariable("beta_" + to_string(sat));
                                mBetaVariables[sat] = var;
                            }
                        }
                                
                    }
                }
            }
            
            
            
            void createConstraints(double boundary, storm::storage::BitVector const& relevantStates) {
                // (5) and (7) are omitted on purpose (-- we currenty do not support controllability of actions -- )
                
                // (1)
                assert(mdp->getInitialStates().getNumberOfSetBits() == 1);
                uint_fast64_t initialStateIndex = mdp->getInitialStates().getNextSetIndex(0);
                solver.addConstraint("c1", mProbVariables[initialStateIndex] >= solver.getConstant(boundary));
                for(uint_fast64_t s : relevantStates) {
                    std::string stateString =  std::to_string(s);
                    storm::expressions::Expression expr;
                    // (2)
                    for(uint_fast64_t a = 0; a < mdp->getNumberOfChoices(s); ++a) {
                        expr = expr + multistrategyVariables[StateActionPair(s,a)];
                    }   
                    solver.addConstraint("c2-" + stateString, solver.getConstant(1) <= expr);
                    // (5)
                    solver.addConstraint("c5-" + std::to_string(s), mProbVariables[s] <= mAlphaVariables[s]);
               
                    // (3) For the relevant states.
                    for(uint_fast64_t a = 0; a < mdp->getNumberOfChoices(s); ++a) {
                        std::string sastring(stateString + "_" + std::to_string(a));
                        expr = storm::expressions::Expression();        
                        for(auto const& entry : mdp->getTransitionMatrix().getRow(mdp->getNondeterministicChoiceIndices()[s]+a)) {
                            if(entry.getValue() != 0 && relevantStates.get(entry.getColumn())) {
                                expr = expr + solver.getConstant(entry.getValue()) * mProbVariables[entry.getColumn()];
                            } else if (entry.getValue() != 0 && mGoals.get(entry.getColumn())) {
                                expr = expr + solver.getConstant(entry.getValue());
                            }
                        }        
                        solver.addConstraint("c3-" + sastring, mProbVariables[s] < (solver.getConstant(1) - multistrategyVariables[StateActionPair(s,a)]) + expr);
                    }

                    for(uint_fast64_t a = 0; a < mdp->getNumberOfChoices(s); ++a) {    
                        // (6)
                        std::string sastring(stateString + "_" + std::to_string(a));
                        expr = storm::expressions::Expression();        
                        for(auto const& entry : mdp->getTransitionMatrix().getRow(mdp->getNondeterministicChoiceIndices()[s]+a)) {
                            if(entry.getValue() != 0) {
                                StateActionTarget sat = {s,a,entry.getColumn()};
                                expr = expr + mBetaVariables[sat];
                            }
                        }        
                        solver.addConstraint("c6-" + sastring, multistrategyVariables[StateActionPair(s,a)] == (solver.getConstant(1) - mAlphaVariables[s]) + expr);
                        
                        for(auto const& entry : mdp->getTransitionMatrix().getRow(mdp->getNondeterministicChoiceIndices()[s]+a)) {
                            if(entry.getValue() != 0) {
                                StateActionTarget sat = {s,a,entry.getColumn()};
                                std::string satstring = to_string(sat);
                                // (8)
                                solver.addConstraint("c8-" + satstring, mGammaVariables[entry.getColumn()] < mGammaVariables[s] + (solver.getConstant(1) - mBetaVariables[sat])); // With rewards, we have to change this.
                            }
                        }
                                
                    }
                }
            }
                
            
            /**
             *
             */
            void createMILP(double boundary, PermissiveSchedulerPenalties const& penalties, BitVector const& dontCareStates ) {
                BitVector irrelevant = mGoals | mSinks;
                BitVector relevantStates = ~irrelevant;
                // Notice that the separated construction of variables and 
                // constraints slows down the construction of the MILP. 
                // In the future, we might want to merge this.
                createVariables(penalties, relevantStates);
                createConstraints(boundary, relevantStates);
                
                
                solver.setModelSense(storm::solver::LpSolver::ModelSense::Minimize);
                
                
            }
            
            
            
            
            
        };
        
    }
}


#endif	/* PERMISSIVESCHEDULERS_H */

