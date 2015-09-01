#ifndef MILPPERMISSIVESCHEDULERS_H
#define	MILPPERMISSIVESCHEDULERS_H


#include <memory>
#include <unordered_map>

#include "PermissiveSchedulerComputation.h"

#include "../storage/BitVector.h"
#include "../storage/StateActionPair.h"
#include "../storage/StateActionTargetTuple.h"
#include "../storage/expressions/Variable.h"
#include "../solver/LpSolver.h"



namespace storm {
    namespace ps {
        
class MilpPermissiveSchedulerComputation : public PermissiveSchedulerComputation {
        private:
           
            bool mCalledOptimizer = false;
            storm::solver::LpSolver& solver;
            std::unordered_map<storm::storage::StateActionPair, storm::expressions::Variable> multistrategyVariables;
            std::unordered_map<uint_fast64_t, storm::expressions::Variable> mProbVariables;
            std::unordered_map<uint_fast64_t, storm::expressions::Variable> mAlphaVariables;
            std::unordered_map<storm::storage::StateActionTarget, storm::expressions::Variable> mBetaVariables;
            std::unordered_map<uint_fast64_t, storm::expressions::Variable> mGammaVariables;
            
        public:
            
            MilpPermissiveSchedulerComputation(storm::solver::LpSolver& milpsolver, std::shared_ptr<storm::models::sparse::Mdp<double>> mdp, storm::storage::BitVector const& goalstates, storm::storage::BitVector const& sinkstates)
                :  PermissiveSchedulerComputation(mdp, goalstates, sinkstates), solver(milpsolver)
            {
                
            }
            
                    
            void calculatePermissiveScheduler(double boundary) override {
                createMILP(boundary, mPenalties);
                solver.optimize();
                mCalledOptimizer = true;
            }
            
            
            bool foundSolution() const override {
                assert(mCalledOptimizer);
                return !solver.isInfeasible();
            }
            
            MemorylessDeterministicPermissiveScheduler && getScheduler() const override {
                storm::storage::BitVector result(mdp->getNumberOfChoices(), true);
                for(auto const& entry : multistrategyVariables) {
                    if(!solver.getBinaryValue(entry.second)) {
                        result.set(mdp->getNondeterministicChoiceIndices()[entry.first.getState()]+entry.first.getAction(), false);
                    }
                }
                return std::move<MemorylessDeterministicPermissiveScheduler>(result);
            }
            
                    
        private:
            
            /**
             * 
             */
            void createVariables(PermissiveSchedulerPenalties const& penalties, storm::storage::BitVector const& relevantStates) {
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
                        auto stateAndAction = storage::StateActionPair(s,a);
                        
                        // Create y_(s,a) variables
                        double penalty = penalties.get(stateAndAction);
                        var = solver.addBinaryVariable("y_"  + std::to_string(s) + "_" + std::to_string(a), -penalty);
                        multistrategyVariables[stateAndAction] = var;
                        
                        // Create beta_(s,a,t) variables
                        // Iterate over successors of s via a.
                        for(auto const& entry : mdp->getTransitionMatrix().getRow(mdp->getNondeterministicChoiceIndices()[s]+a)) {
                            if(entry.getValue() != 0) {
                                storage::StateActionTarget sat = {s,a,entry.getColumn()};
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
                    storm::expressions::Expression expr = solver.getConstant(0.0);
                    // (2)
                    for(uint_fast64_t a = 0; a < mdp->getNumberOfChoices(s); ++a) {
                        expr = expr + multistrategyVariables[storage::StateActionPair(s,a)];
                    }   
                    solver.addConstraint("c2-" + stateString, solver.getConstant(1) <= expr);
                    // (5)
                    solver.addConstraint("c5-" + std::to_string(s), mProbVariables[s] <= mAlphaVariables[s]);
               
                    // (3) For the relevant states.
                    for(uint_fast64_t a = 0; a < mdp->getNumberOfChoices(s); ++a) {
                        std::string sastring(stateString + "_" + std::to_string(a));
                        expr = solver.getConstant(0.0);       
                        for(auto const& entry : mdp->getTransitionMatrix().getRow(mdp->getNondeterministicChoiceIndices()[s]+a)) {
                            if(entry.getValue() != 0 && relevantStates.get(entry.getColumn())) {
                                expr = expr + solver.getConstant(entry.getValue()) * mProbVariables[entry.getColumn()];
                            } else if (entry.getValue() != 0 && mGoals.get(entry.getColumn())) {
                                expr = expr + solver.getConstant(entry.getValue());
                            }
                        }        
                        solver.addConstraint("c3-" + sastring, mProbVariables[s] < (solver.getConstant(1) - multistrategyVariables[storage::StateActionPair(s,a)]) + expr);
                    }

                    for(uint_fast64_t a = 0; a < mdp->getNumberOfChoices(s); ++a) {    
                        // (6)
                        std::string sastring(stateString + "_" + std::to_string(a));
                        expr = solver.getConstant(0.0);     
                        for(auto const& entry : mdp->getTransitionMatrix().getRow(mdp->getNondeterministicChoiceIndices()[s]+a)) {
                            if(entry.getValue() != 0) {
                                storage::StateActionTarget sat = {s,a,entry.getColumn()};
                                expr = expr + mBetaVariables[sat];
                            }
                        }        
                        solver.addConstraint("c6-" + sastring, multistrategyVariables[storage::StateActionPair(s,a)] == (solver.getConstant(1) - mAlphaVariables[s]) + expr);
                        
                        for(auto const& entry : mdp->getTransitionMatrix().getRow(mdp->getNondeterministicChoiceIndices()[s]+a)) {
                            if(entry.getValue() != 0) {
                                storage::StateActionTarget sat = {s,a,entry.getColumn()};
                                std::string satstring = to_string(sat);
                                // (8)
                                assert(mGammaVariables.count(entry.getColumn()) > 0);
                                assert(mGammaVariables.count(s) > 0);
                                assert(mBetaVariables.count(sat) > 0);
                                if(relevantStates[entry.getColumn()]) {
                                    solver.addConstraint("c8-" + satstring, mGammaVariables[entry.getColumn()] < mGammaVariables[s] + (solver.getConstant(1) - mBetaVariables[sat]) + mProbVariables[s]); // With rewards, we have to change this.
                               }
                            }
                        }
                                
                    }
                }
            }
                
            
            /**
             *
             */
            void createMILP(double boundary, PermissiveSchedulerPenalties const& penalties) {
                storm::storage::BitVector irrelevant = mGoals | mSinks;
                storm::storage::BitVector relevantStates = ~irrelevant;
                // Notice that the separated construction of variables and 
                // constraints slows down the construction of the MILP. 
                // In the future, we might want to merge this.
                createVariables(penalties, relevantStates);
                createConstraints(boundary, relevantStates);
                
                
                solver.setOptimizationDirection(storm::OptimizationDirection::Minimize);
                
            }
        };
    }
}

#endif	/* MILPPERMISSIVESCHEDULERS_H */

