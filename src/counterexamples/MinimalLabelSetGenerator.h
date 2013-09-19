/*
 * MinimalLabelSetGenerator.h
 *
 *  Created on: 15.09.2013
 *      Author: Christian Dehnert
 */

#ifndef STORM_COUNTEREXAMPLES_MINIMALCOMMANDSETGENERATOR_MDP_H_
#define STORM_COUNTEREXAMPLES_MINIMALCOMMANDSETGENERATOR_MDP_H_

#ifdef HAVE_GUROBI
extern "C" {
#include "gurobi_c.h"
    
    int __stdcall GRBislp(GRBenv **, const char *, const char *, const char *, const char *);
}
#endif

#include "src/models/Mdp.h"
#include "src/exceptions/NotImplementedException.h"
#include "src/exceptions/InvalidArgumentException.h"
#include "src/exceptions/InvalidStateException.h"

namespace storm {
    namespace counterexamples {
        
        /*!
         * A helper class that provides the functionality to compute a hash value for pairs of indices.
         */
        class PairHash {
        public:
            std::size_t operator()(std::pair<uint_fast64_t, uint_fast64_t> const& pair) const {
                size_t seed = 0;
                boost::hash_combine(seed, pair.first);
                boost::hash_combine(seed, pair.second);
                return seed;
            }
        };
        
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
                std::unordered_map<uint_fast64_t, std::list<uint_fast64_t>> relevantChoicesForRelevantStates;
                std::unordered_set<uint_fast64_t> relevantLabels;
                storm::storage::SparseMatrix<T> const& transitionMatrix = labeledMdp.getTransitionMatrix();
                std::vector<uint_fast64_t> const& nondeterministicChoiceIndices = labeledMdp.getNondeterministicChoiceIndices();
                std::vector<std::list<uint_fast64_t>> const& choiceLabeling = labeledMdp.getChoiceLabeling();
                // Now traverse all choices of all relevant states and check whether there is a relevant target state.
                // If so, the associated labels become relevant.
                for (auto state : relevantStates) {
                    relevantChoicesForRelevantStates.emplace(state, std::list<uint_fast64_t>());
                    for (uint_fast64_t row = nondeterministicChoiceIndices[state]; row < nondeterministicChoiceIndices[state + 1]; ++row) {
                        bool currentChoiceRelevant = false;
                        for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator successorIt = transitionMatrix.constColumnIteratorBegin(row); successorIt != transitionMatrix.constColumnIteratorEnd(row); ++successorIt) {
                            // If there is a relevant successor, we need to add the labels of the current choice.
                            if (relevantStates.get(*successorIt) || psiStates.get(*successorIt)) {
                                for (auto const& label : choiceLabeling[row]) {
                                    relevantLabels.insert(label);
                                }
                                if (!currentChoiceRelevant) {
                                    currentChoiceRelevant = true;
                                    relevantChoicesForRelevantStates[state].emplace_back(row);
                                }
                            }
                        }
                    }
                }
                LOG4CPLUS_INFO(logger, "Found " << relevantLabels.size() << " relevant labels.");
                
                // Determine set of problematic transitions.
                std::unordered_map<uint_fast64_t, std::list<uint_fast64_t>> problematicChoicesForProblematicStates;
                for (auto state : problematicStates) {
                    std::unordered_map<uint_fast64_t, std::list<uint_fast64_t>> relevantChoicesForRelevantStates;
                    
                }
                
                // (3) Encode resulting system as MILP problem.
                //  (3.1) Initialize MILP solver and model.
                GRBenv* env = nullptr;
                int error = GRBloadenv(&env, "storm_gurobi.log");
                if (error || env == NULL) {
                    LOG4CPLUS_ERROR(logger, "Could not initialize Gurobi (" << GRBgeterrormsg(env) << ").");
                    throw storm::exceptions::InvalidStateException() << "Could not initialize Gurobi (" << GRBgeterrormsg(env) << ").";
                }
                GRBmodel* model = nullptr;
                error = GRBnewmodel(env, &model, "storm_milp", 0, nullptr, nullptr, nullptr, nullptr, nullptr);
                if (error) {
                    LOG4CPLUS_ERROR(logger, "Could not initialize Gurobi model (" << GRBgeterrormsg(env) << ").");
                    throw storm::exceptions::InvalidStateException() << "Could not initialize Gurobi model (" << GRBgeterrormsg(env) << ").";
                }
                
                //  (3.2) Create variables.
                
                // Prepare internal variables.
                std::stringstream variableNameBuffer;
                uint_fast64_t nextLabelIndex = 0;
                
                // Create variables for involved labels.
                std::unordered_map<uint_fast64_t, uint_fast64_t> labelToIndexMap;
                for (auto const& label : relevantLabels) {
                    // Reset stringstream properly to construct new variable name.
                    variableNameBuffer.str("");
                    variableNameBuffer.clear();
                    variableNameBuffer << "label" << label;
                    error = GRBaddvar(model, 0, nullptr, nullptr, 1.0, 0.0, 1.0, GRB_BINARY, variableNameBuffer.str().c_str());
                    if (error) {
                        LOG4CPLUS_ERROR(logger, "Could not create Gurobi variable (" << GRBgeterrormsg(env) << ").");
                        throw storm::exceptions::InvalidStateException() << "Could not create Gurobi variable (" << GRBgeterrormsg(env) << ").";
                    }
                    labelToIndexMap[label] = nextLabelIndex;
                    ++nextLabelIndex;
                }
                
                // Create scheduler variables for relevant states and their actions.
                std::unordered_map<uint_fast64_t, uint_fast64_t> stateToStartingIndexMap;
                for (auto state : relevantStates) {
                    std::list<uint_fast64_t> const& relevantChoicesForState = relevantChoicesForRelevantStates[state];
                    stateToStartingIndexMap[state] = nextLabelIndex;
                    for (uint_fast64_t row : relevantChoicesForState) {
                        // Reset stringstream properly to construct new variable name.
                        variableNameBuffer.str("");
                        variableNameBuffer.clear();
                        variableNameBuffer << "choice" << row << "in" << state;
                        error = GRBaddvar(model, 0, nullptr, nullptr, 0.0, 0.0, 1.0, GRB_BINARY, variableNameBuffer.str().c_str());
                        if (error) {
                            LOG4CPLUS_ERROR(logger, "Could not create Gurobi variable (" << GRBgeterrormsg(env) << ").");
                            throw storm::exceptions::InvalidStateException() << "Could not create Gurobi variable (" << GRBgeterrormsg(env) << ").";
                        }
                        ++nextLabelIndex;
                    }
                }
                
                // Create variables for probabilities for all relevant states.
                std::unordered_map<uint_fast64_t, uint_fast64_t> stateToProbabilityVariableIndex;
                for (auto state : relevantStates) {
                    // Reset stringstream properly to construct new variable name.
                    variableNameBuffer.str("");
                    variableNameBuffer.clear();
                    variableNameBuffer << "p" << state;
                    error = GRBaddvar(model, 0, nullptr, nullptr, 0.0, 0.0, 1.0, GRB_CONTINUOUS, variableNameBuffer.str().c_str());
                    if (error) {
                        LOG4CPLUS_ERROR(logger, "Could not create Gurobi variable (" << GRBgeterrormsg(env) << ").");
                        throw storm::exceptions::InvalidStateException() << "Could not create Gurobi variable (" << GRBgeterrormsg(env) << ").";
                    }
                    stateToProbabilityVariableIndex[state] = nextLabelIndex;
                    ++nextLabelIndex;
                }
                
                // Create variables for problematic states, successors of problematic states and transitions of problematic states.
                std::unordered_map<uint_fast64_t, uint_fast64_t> problematicStateVariables;
                std::unordered_map<std::pair<uint_fast64_t, uint_fast64_t>, uint_fast64_t, PairHash> problematicTransitionVariables;
                for (auto state : problematicStates) {
                    // First check whether there is not already a variable for this state and proceed with next state.
                    if (problematicStateVariables.find(state) == problematicStateVariables.end()) {
                        // Reset stringstream properly to construct new variable name.
                        variableNameBuffer.str("");
                        variableNameBuffer.clear();
                        variableNameBuffer << "r" << state;
                        std::cout << "Creating r variable" << std::endl;
                        error = GRBaddvar(model, 0, nullptr, nullptr, 0.0, 0.0, 1.0, GRB_CONTINUOUS, variableNameBuffer.str().c_str());
                        if (error) {
                            LOG4CPLUS_ERROR(logger, "Could not create Gurobi variable (" << GRBgeterrormsg(env) << ").");
                            throw storm::exceptions::InvalidStateException() << "Could not create Gurobi variable (" << GRBgeterrormsg(env) << ").";
                        }
                        problematicStateVariables[state] = nextLabelIndex;
                        ++nextLabelIndex;
                    }
                    
                    std::list<uint_fast64_t> const& relevantChoicesForState = relevantChoicesForRelevantStates[state];
                    for (uint_fast64_t row : relevantChoicesForState) {
                        for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator successorIt = transitionMatrix.constColumnIteratorBegin(row); successorIt != transitionMatrix.constColumnIteratorEnd(row); ++successorIt) {
                            if (relevantStates.get(*successorIt)) {
                                if (problematicStateVariables.find(*successorIt) == problematicStateVariables.end()) {
                                    // Reset stringstream properly to construct new variable name.
                                    variableNameBuffer.str("");
                                    variableNameBuffer.clear();
                                    variableNameBuffer << "r" << *successorIt;
                                    error = GRBaddvar(model, 0, nullptr, nullptr, 0.0, 0.0, 1.0, GRB_CONTINUOUS, variableNameBuffer.str().c_str());
                                    if (error) {
                                        LOG4CPLUS_ERROR(logger, "Could not create Gurobi variable (" << GRBgeterrormsg(env) << ").");
                                        throw storm::exceptions::InvalidStateException() << "Could not create Gurobi variable (" << GRBgeterrormsg(env) << ").";
                                    }
                                    problematicStateVariables[state] = nextLabelIndex;
                                    ++nextLabelIndex;
                                }
                                variableNameBuffer.str("");
                                variableNameBuffer.clear();
                                variableNameBuffer << "t" << state << "to" << *successorIt;
                                error = GRBaddvar(model, 0, nullptr, nullptr, 0.0, 0.0, 1.0, GRB_BINARY, variableNameBuffer.str().c_str());
                                if (error) {
                                    LOG4CPLUS_ERROR(logger, "Could not create Gurobi variable (" << GRBgeterrormsg(env) << ").");
                                    throw storm::exceptions::InvalidStateException() << "Could not create Gurobi variable (" << GRBgeterrormsg(env) << ").";
                                }
                                problematicTransitionVariables[std::make_pair(state, *successorIt)] = nextLabelIndex;
                                ++nextLabelIndex;
                            }
                        }
                    }
                }
                
                LOG4CPLUS_ERROR(logger, "Successfully created " << nextLabelIndex << " Gurobi variables.");
                
                // Update model to incorporate prior changes.
                error = GRBupdatemodel(model);
                if (error) {
                    LOG4CPLUS_ERROR(logger, "Unable to update Gurobi model (" << GRBgeterrormsg(env) << ").");
                    throw storm::exceptions::InvalidStateException() << "Unable to update Gurobi model (" << GRBgeterrormsg(env) << ").";
                }
                
                // Make sure we have exactly one initial state and assert that it's probability is above the given threshold.
                storm::storage::BitVector const& initialStates = labeledMdp.getLabeledStates("init");
                if (initialStates.getNumberOfSetBits() != 1) {
                    LOG4CPLUS_ERROR(logger, "Must have exactly one initial state, but got " << initialStates.getNumberOfSetBits() << "instead.");
                    throw storm::exceptions::InvalidStateException() << "Must have exactly one initial state, but got " << initialStates.getNumberOfSetBits() << "instead.";
                }
                for (auto initialState : initialStates) {
                    int variableIndex = static_cast<int>(stateToProbabilityVariableIndex[initialState]);
                    double coefficient = 1.0;
                    error = GRBaddconstr(model, 1, &variableIndex, &coefficient, GRB_GREATER_EQUAL, lowerProbabilityBound + 10e-6, nullptr);
                    if (error) {
                        LOG4CPLUS_ERROR(logger, "Unable to assert constraint (" << GRBgeterrormsg(env) << ").");
                        throw storm::exceptions::InvalidStateException() << "Unable to assert constraint (" << GRBgeterrormsg(env) << ").";
                    }
                }
                
                // Now add the constaints that ensure that the policy chooses at most one action in each state.
                for (auto state : relevantStates) {
                    uint_fast64_t startingIndex = stateToStartingIndexMap[state];
                    std::list<uint_fast64_t> const& relevantChoicesForState = relevantChoicesForRelevantStates[state];
                    std::vector<int> variables;
                    std::vector<double> coefficients(relevantChoicesForState.size(), 1);
                    variables.reserve(relevantChoicesForState.size());
                    for (auto choice : relevantChoicesForState) {
                        variables.push_back(static_cast<int>(startingIndex++));
                    }
                    error = GRBaddconstr(model, relevantChoicesForState.size(), &variables[0], &coefficients[0], GRB_LESS_EQUAL, 1, nullptr);
                    if (error) {
                        LOG4CPLUS_ERROR(logger, "Unable to assert constraint (" << GRBgeterrormsg(env) << ").");
                        throw storm::exceptions::InvalidStateException() << "Unable to assert constraint (" << GRBgeterrormsg(env) << ").";
                    }
                }
                
                // Add constraints that enforce that certain labels are put into the label set when the corresponding
                // transitions get selected.
                for (auto state : relevantStates) {
                    uint_fast64_t currentChoiceVariableIndex = stateToStartingIndexMap[state];
                    for (auto choice : relevantChoicesForRelevantStates[state]) {
                        int indices[2]; indices[0] = 0; indices[1] = currentChoiceVariableIndex;
                        double coefficients[2]; coefficients[0] = 1; coefficients[1] = -1;
                        for (auto label : choiceLabeling[choice]) {
                            indices[0] = labelToIndexMap[label];
                            error = GRBaddconstr(model, 2, indices, coefficients, GRB_GREATER_EQUAL, 0, nullptr);
                            if (error) {
                                LOG4CPLUS_ERROR(logger, "Unable to assert constraint (" << GRBgeterrormsg(env) << ").");
                                throw storm::exceptions::InvalidStateException() << "Unable to assert constraint (" << GRBgeterrormsg(env) << ").";
                            }
                        }
                        ++currentChoiceVariableIndex;
                    }
                }
                
                // Add constraints that make sure the reachability probability for states that do not choose any action
                // is zero.
                for (auto state : relevantStates) {
                    std::vector<double> coefficients(relevantChoicesForRelevantStates[state].size() + 1, -1);
                    coefficients[0] = 1;
                    std::vector<int> variables;
                    variables.reserve(relevantChoicesForRelevantStates[state].size() + 1);
                    variables.push_back(stateToProbabilityVariableIndex[state]);
                    
                    uint_fast64_t currentChoiceVariableIndex = stateToStartingIndexMap[state];
                    for (auto choice : relevantChoicesForRelevantStates[state]) {
                        variables.push_back(currentChoiceVariableIndex);
                        ++currentChoiceVariableIndex;
                    }
                    error = GRBaddconstr(model, relevantChoicesForRelevantStates[state].size() + 1, &variables[0], &coefficients[0], GRB_LESS_EQUAL, 0, nullptr);
                    if (error) {
                        LOG4CPLUS_ERROR(logger, "Unable to assert constraint (" << GRBgeterrormsg(env) << ").");
                        throw storm::exceptions::InvalidStateException() << "Unable to assert constraint (" << GRBgeterrormsg(env) << ").";
                    }
                }
                
                // Add constraints that encode the reachability probabilities for all states.
                for (auto state : relevantStates) {
                    std::vector<double> coefficients;
                    std::vector<int> variables;
                    
                    uint_fast64_t currentChoiceVariableIndex = stateToStartingIndexMap[state];
                    for (auto choice : relevantChoicesForRelevantStates[state]) {
                        variables.clear();
                        coefficients.clear();
                        variables.push_back(stateToProbabilityVariableIndex[state]);
                        coefficients.push_back(1.0);
                        
                        double rightHandSide = 1;
                        typename storm::storage::SparseMatrix<T>::Rows rows = transitionMatrix.getRows(choice, choice);
                        for (typename storm::storage::SparseMatrix<T>::ConstIterator successorIt = rows.begin(), successorIte = rows.end(); successorIt != successorIte; ++successorIt) {
                            if (relevantStates.get(successorIt.column())) {
                                variables.push_back(stateToProbabilityVariableIndex[successorIt.column()]);
                                coefficients.push_back(-1);
                            } else if (psiStates.get(successorIt.column())) {
                                rightHandSide += successorIt.value();
                            }
                        }
                        
                        coefficients.push_back(-1);
                        variables.push_back(currentChoiceVariableIndex);
                        
                        error = GRBaddconstr(model, variables.size(), &variables[0], &coefficients[0], GRB_LESS_EQUAL, rightHandSide, nullptr);
                        if (error) {
                            LOG4CPLUS_ERROR(logger, "Unable to assert constraint (" << GRBgeterrormsg(env) << ").");
                            throw storm::exceptions::InvalidStateException() << "Unable to assert constraint (" << GRBgeterrormsg(env) << ").";
                        }
                        
                        ++currentChoiceVariableIndex;
                    }
                }
                
                // Add constraints that ensure reachability of at least one unproblematic state.
                for (auto state : problematicStates) {
                    
                }
                
                // Update model to incorporate prior changes.
                error = GRBupdatemodel(model);
                if (error) {
                    LOG4CPLUS_ERROR(logger, "Unable to update Gurobi model (" << GRBgeterrormsg(env) << ").");
                    throw storm::exceptions::InvalidStateException() << "Unable to update Gurobi model (" << GRBgeterrormsg(env) << ").";
                }
                
                error = GRBwrite(model, "storm.lp");
                if (error) {
                    LOG4CPLUS_ERROR(logger, "Unable to write Gurobi model (" << GRBgeterrormsg(env) << ").");
                    throw storm::exceptions::InvalidStateException() << "Unable to write Gurobi model (" << GRBgeterrormsg(env) << ").";
                }
                
                //  (3.3) Construct objective function.
                //  (3.4) Construct constraint system.
                // (4) Read off result from MILP variables.
                
                // (5) Shutdown MILP solver.
                GRBfreemodel(model);
                GRBfreeenv(env);
                
                // (6) Potentially verify whether the resulting system exceeds the given threshold.
                // (7) Return result.
                
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
