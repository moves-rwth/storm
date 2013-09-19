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
         * This class provides functionality to generate a minimal counterexample to a probabilistic reachability
         * property in terms of used labels.
         */
        template <class T>
        class MinimalLabelSetGenerator {
        private:
            /*!
             * A helper class that provides the functionality to compute a hash value for pairs of state indices.
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
             * A helper struct storing which states are relevant or problematic.
             */
            struct StateInformation {
                storm::storage::BitVector relevantStates;
                storm::storage::BitVector problematicStates;
            };
            
            /*!
             * A helper struct capturing information about relevant and problematic choices of states and which labels
             * are relevant.
             */
            struct ChoiceInformation {
                std::unordered_map<uint_fast64_t, std::list<uint_fast64_t>> relevantChoicesForRelevantStates;
                std::unordered_map<uint_fast64_t, std::list<uint_fast64_t>> problematicChoicesForProblematicStates;
                std::unordered_set<uint_fast64_t> allRelevantLabels;
            };

            /*!
             * A helper struct capturing information about the variables of the MILP formulation.
             */
            struct VariableInformation {
                std::unordered_map<uint_fast64_t, uint_fast64_t> labelToVariableIndexMap;
                std::unordered_map<uint_fast64_t, std::list<uint_fast64_t>> stateToChoiceVariablesIndexMap;
                std::unordered_map<uint_fast64_t, uint_fast64_t> stateToProbabilityVariableIndexMap;
                std::unordered_map<uint_fast64_t, uint_fast64_t> problematicStateToVariableIndexMap;
                std::unordered_map<std::pair<uint_fast64_t, uint_fast64_t>, uint_fast64_t, PairHash> problematicTransitionToVariableIndexMap;
                uint_fast64_t nextFreeVariableIndex = 0;
            };

            /*!
             * Determines the relevant and the problematic states of the given MDP with respect to the given phi and psi
             * state sets. The relevant states are those for which there exists at least one scheduler that attains a
             * non-zero probability of satisfying phi until psi. Problematic states are relevant states that have at
             * least one scheduler such that the probability of satisfying phi until psi is zero.
             *
             * @param labeledMdp The MDP whose states to search.
             * @param phiStates A bit vector characterizing all states satisfying phi.
             * @param psiStates A bit vector characterizing all states satisfying psi.
             * @return A structure that stores the relevant and problematic states.
             */
            static struct StateInformation determineRelevantAndProblematicStates(storm::models::Mdp<T> const& labeledMdp, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates) {
                StateInformation result;
                storm::storage::SparseMatrix<bool> backwardTransitions = labeledMdp.getBackwardTransitions();
                result.relevantStates = storm::utility::graph::performProbGreater0E(labeledMdp, backwardTransitions, phiStates, psiStates);
                result.relevantStates &= ~psiStates;
                storm::storage::BitVector problematicStates = storm::utility::graph::performProbGreater0E(labeledMdp, backwardTransitions, phiStates, psiStates);
                result.problematicStates.complement();
                result.problematicStates &= result.relevantStates;
                LOG4CPLUS_DEBUG(logger, "Found " << phiStates.getNumberOfSetBits() << " filter states (" << phiStates.toString() << ").");
                LOG4CPLUS_DEBUG(logger, "Found " << psiStates.getNumberOfSetBits() << " target states (" << psiStates.toString() << ").");
                LOG4CPLUS_DEBUG(logger, "Found " << result.relevantStates.getNumberOfSetBits() << " relevant states (" << result.relevantStates.toString() << ").");
                LOG4CPLUS_DEBUG(logger, "Found " << result.problematicStates.getNumberOfSetBits() << " problematic states (" << result.problematicStates.toString() << ").");
                return result;
            }
            
            /*!
             * Determines the relevant and problematic choices of the given MDP with respect to the given parameters.
             *
             * @param labeledMdp The MDP whose choices to search.
             * @param stateInformation The relevant and problematic states of the model.
             * @param psiStates A bit vector characterizing the psi states in the model.
             * @return A structure that stores the relevant and problematic choices in the model as well as the set
             * of relevant labels.
             */
            static struct ChoiceInformation determineRelevantAndProblematicChoices(storm::models::Mdp<T> const& labeledMdp, StateInformation const& stateInformation, storm::storage::BitVector const& psiStates) {
                // Create result and shortcuts to needed data for convenience.
                ChoiceInformation result;
                storm::storage::SparseMatrix<T> const& transitionMatrix = labeledMdp.getTransitionMatrix();
                std::vector<uint_fast64_t> const& nondeterministicChoiceIndices = labeledMdp.getNondeterministicChoiceIndices();
                std::vector<std::list<uint_fast64_t>> const& choiceLabeling = labeledMdp.getChoiceLabeling();
                
                // Now traverse all choices of all relevant states and check whether there is a relevant target state.
                // If so, the associated labels become relevant. Also, if a choice of relevant state has at least one
                // relevant successor, the choice is considered to be relevant.
                for (auto state : stateInformation.relevantStates) {
                    result.relevantChoicesForRelevantStates.emplace(state, std::list<uint_fast64_t>());
                    if (stateInformation.problematicStates.get(state)) {
                        result.problematicChoicesForProblematicStates.emplace(state, std::list<uint_fast64_t>());
                    }
                    for (uint_fast64_t row = nondeterministicChoiceIndices[state]; row < nondeterministicChoiceIndices[state + 1]; ++row) {
                        bool currentChoiceRelevant = false;
                        bool allSuccessorsProblematic = true;
                        for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator successorIt = transitionMatrix.constColumnIteratorBegin(row); successorIt != transitionMatrix.constColumnIteratorEnd(row); ++successorIt) {
                            // If there is a relevant successor, we need to add the labels of the current choice.
                            if (stateInformation.relevantStates.get(*successorIt) || psiStates.get(*successorIt)) {
                                for (auto const& label : choiceLabeling[row]) {
                                    result.relevantLabels.insert(label);
                                }
                                if (!currentChoiceRelevant) {
                                    currentChoiceRelevant = true;
                                    result.relevantChoicesForRelevantStates[state].emplace_back(row);
                                }
                            }
                            if (!stateInformation.problematicStates.get(*successorIt)) {
                                allSuccessorsProblematic = false;
                            }
                        }
                        
                        // If all successors of a problematic state are problematic themselves, we record this choice
                        // as being problematic.
                        if (stateInformation.problematicStates.get(state) && allSuccessorsProblematic) {
                            result.problematicChoicesForProblematicStates[state].emplace_back(row);
                        }
                    }
                }
                LOG4CPLUS_DEBUG(logger, "Found " << result.relevantLabels.size() << " relevant labels.");
                return result;
            }
            
            /*!
             * Creates a Gurobi environment and model and returns pointers to them.
             *
             * @return A pair of two pointers to a Gurobi environment and model, respectively.
             */
            static std::pair<GRBenv*, GRBmodel*> getGurobiEnvironmentAndModel() {
                GRBenv* env = nullptr;
                int error = GRBloadenv(&env, "storm_gurobi.log");
                if (error || env == NULL) {
                    LOG4CPLUS_ERROR(logger, "Could not initialize Gurobi (" << GRBgeterrormsg(env) << ").");
                    throw storm::exceptions::InvalidStateException() << "Could not initialize Gurobi (" << GRBgeterrormsg(env) << ").";
                }
                GRBmodel* model = nullptr;
                error = GRBnewmodel(env, &model, "minimal_label_milp", 0, nullptr, nullptr, nullptr, nullptr, nullptr);
                if (error) {
                    LOG4CPLUS_ERROR(logger, "Could not initialize Gurobi model (" << GRBgeterrormsg(env) << ").");
                    throw storm::exceptions::InvalidStateException() << "Could not initialize Gurobi model (" << GRBgeterrormsg(env) << ").";
                }
                return std::make_pair(env, model);
            }

            /*!
             * Creates the variables for the labels of the model.
             *
             * @param env The Gurobi environment.
             * @param model The Gurobi model.
             * @param relevantLabels The set of relevant labels of the model.
             * @param nextFreeVariableIndex A reference to the next free variable index. Note: when creating new
             * variables, this value is increased.
             * @return A mapping from labels to variable indices.
             */
            static std::unordered_map<uint_fast64_t, uint_fast64_t> createLabelVariables(GRBenv* env, GRBmodel* model, std::unordered_set<uint_fast64_t> const& relevantLabels, uint_fast64_t& nextFreeVariableIndex) {
                int error = 0;
                std::stringstream variableNameBuffer;
                std::unordered_map<uint_fast64_t, uint_fast64_t> resultingMap;
                for (auto const& label : relevantLabels) {
                    variableNameBuffer.str("");
                    variableNameBuffer.clear();
                    variableNameBuffer << "label" << label;
                    error = GRBaddvar(model, 0, nullptr, nullptr, 1.0, 0.0, 1.0, GRB_BINARY, variableNameBuffer.str().c_str());
                    if (error) {
                        LOG4CPLUS_ERROR(logger, "Could not create Gurobi variable (" << GRBgeterrormsg(env) << ").");
                        throw storm::exceptions::InvalidStateException() << "Could not create Gurobi variable (" << GRBgeterrormsg(env) << ").";
                    }
                    resultingMap[label] = nextFreeVariableIndex;
                    ++nextFreeVariableIndex;
                }
                return resultingMap;
            }
            
            /*!
             * Creates the variables for the relevant choices in the model.
             *
             * @param env The Gurobi environment.
             * @param model The Gurobi model.
             * @param stateInformation The information about the states of the model.
             * @param choiceInformation The information about the choices of the model.
             * @param nextFreeVariableIndex A reference to the next free variable index. Note: when creating new
             * variables, this value is increased.
             * @return A mapping from states to a list of choice variable indices.
             */
            static std::unordered_map<uint_fast64_t, std::list<uint_fast64_t>> createSchedulerVariables(GRBenv* env, GRBmodel* model, StateInformation const& stateInformation, ChoiceInformation const& choiceInformation, uint_fast64_t& nextFreeVariableIndex) {
                int error = 0;
                std::stringstream variableNameBuffer;
                std::unordered_map<uint_fast64_t, std::list<uint_fast64_t>> resultingMap;
                for (auto state : stateInformation.relevantStates) {
                    resultingMap.emplace(state, std::list<uint_fast64_t>());
                    std::list<uint_fast64_t> const& relevantChoicesForState = choiceInformation.relevantChoicesForRelevantStates[state];
                    for (uint_fast64_t row : relevantChoicesForState) {
                        variableNameBuffer.str("");
                        variableNameBuffer.clear();
                        variableNameBuffer << "choice" << row << "in" << state;
                        error = GRBaddvar(model, 0, nullptr, nullptr, 0.0, 0.0, 1.0, GRB_BINARY, variableNameBuffer.str().c_str());
                        if (error) {
                            LOG4CPLUS_ERROR(logger, "Could not create Gurobi variable (" << GRBgeterrormsg(env) << ").");
                            throw storm::exceptions::InvalidStateException() << "Could not create Gurobi variable (" << GRBgeterrormsg(env) << ").";
                        }
                        resultingMap[state].emplace_back(nextFreeVariableIndex);
                        ++nextFreeVariableIndex;
                    }
                }
                return resultingMap;
            }
            
            /*!
             * Creates the variables for the probabilities in the model.
             *
             * @param env The Gurobi environment.
             * @param model The Gurobi model.
             * @param stateInformation The information about the states in the model.
             * @param nextFreeVariableIndex A reference to the next free variable index. Note: when creating new
             * variables, this value is increased.
             * @return A mapping from states to the index of the corresponding probability variables.
             */
            static std::unordered_map<uint_fast64_t, uint_fast64_t> createProbabilityVariables(GRBenv* env, GRBmodel* model, StateInformation const& stateInformation, uint_fast64_t& nextFreeVariableIndex) {
                int error = 0;
                std::stringstream variableNameBuffer;
                std::unordered_map<uint_fast64_t, uint_fast64_t> resultingMap;
                for (auto state : stateInformation.relevantStates) {
                    variableNameBuffer.str("");
                    variableNameBuffer.clear();
                    variableNameBuffer << "p" << state;
                    error = GRBaddvar(model, 0, nullptr, nullptr, 0.0, 0.0, 1.0, GRB_CONTINUOUS, variableNameBuffer.str().c_str());
                    if (error) {
                        LOG4CPLUS_ERROR(logger, "Could not create Gurobi variable (" << GRBgeterrormsg(env) << ").");
                        throw storm::exceptions::InvalidStateException() << "Could not create Gurobi variable (" << GRBgeterrormsg(env) << ").";
                    }
                    resultingMap[state] = nextFreeVariableIndex;
                    ++nextFreeVariableIndex;
                }
                return resultingMap;
            }
            
            /*!
             * Creates the variables for the problematic states in the model.
             *
             * @param env The Gurobi environment.
             * @param model The Gurobi model.
             * @param labeledMdp The labeled MDP.
             * @param stateInformation The information about the states in the model.
             * @param nextFreeVariableIndex A reference to the next free variable index. Note: when creating new
             * variables, this value is increased.
             * @return A mapping from problematic states to the index of the corresponding variables.
             */
            static std::unordered_map<uint_fast64_t, uint_fast64_t> createProblematicStateVariables(GRBenv* env, GRBmodel* model, storm::models::Mdp<T> const& labeledMdp, StateInformation const& stateInformation, ChoiceInformation const& choiceInformation, uint_fast64_t& nextFreeVariableIndex) {
                int error = 0;
                std::stringstream variableNameBuffer;
                std::unordered_map<uint_fast64_t, uint_fast64_t> resultingMap;
                for (auto state : stateInformation.problematicStates) {
                    // First check whether there is not already a variable for this state and advance to the next state
                    // in this case.
                    if (resultingMap.find(state) == resultingMap.end()) {
                        variableNameBuffer.str("");
                        variableNameBuffer.clear();
                        variableNameBuffer << "r" << state;
                        error = GRBaddvar(model, 0, nullptr, nullptr, 0.0, 0.0, 1.0, GRB_CONTINUOUS, variableNameBuffer.str().c_str());
                        if (error) {
                            LOG4CPLUS_ERROR(logger, "Could not create Gurobi variable (" << GRBgeterrormsg(env) << ").");
                            throw storm::exceptions::InvalidStateException() << "Could not create Gurobi variable (" << GRBgeterrormsg(env) << ").";
                        }
                        resultingMap[state] = nextFreeVariableIndex;
                        ++nextFreeVariableIndex;
                    }
                    
                    std::list<uint_fast64_t> const& relevantChoicesForState = choiceInformation.relevantChoicesForRelevantStates[state];
                    for (uint_fast64_t row : choiceInformation.relevantChoicesForState) {
                        for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator successorIt = labeledMdp.getTransitionMatrix().constColumnIteratorBegin(row); successorIt != labeledMdp.getTransitionMatrix().constColumnIteratorEnd(row); ++successorIt) {
                            if (stateInformation.relevantStates.get(*successorIt)) {
                                if (resultingMap.find(*successorIt) == resultingMap.end()) {
                                    variableNameBuffer.str("");
                                    variableNameBuffer.clear();
                                    variableNameBuffer << "r" << *successorIt;
                                    error = GRBaddvar(model, 0, nullptr, nullptr, 0.0, 0.0, 1.0, GRB_CONTINUOUS, variableNameBuffer.str().c_str());
                                    if (error) {
                                        LOG4CPLUS_ERROR(logger, "Could not create Gurobi variable (" << GRBgeterrormsg(env) << ").");
                                        throw storm::exceptions::InvalidStateException() << "Could not create Gurobi variable (" << GRBgeterrormsg(env) << ").";
                                    }
                                    resultingMap[state] = nextFreeVariableIndex;
                                    ++nextFreeVariableIndex;
                                }
                            }
                        }
                    }
                }
                return resultingMap;
            }
            
            /*!
             * Creates the variables for the problematic choices in the model.
             *
             * @param env The Gurobi environment.
             * @param model The Gurobi model.
             * @param labeledMdp The labeled MDP.
             * @param stateInformation The information about the states in the model.
             * @param choiceInformation The information about the choices in the model.
             * @param nextFreeVariableIndex A reference to the next free variable index. Note: when creating new
             * variables, this value is increased.
             * @return A mapping from problematic choices to the index of the corresponding variables.
             */
            static std::unordered_map<std::pair<uint_fast64_t, uint_fast64_t>, uint_fast64_t, PairHash> createProblematicChoiceVariables(GRBenv* env, GRBmodel* model, storm::models::Mdp<T> const& labeledMdp, StateInformation const& stateInformation, ChoiceInformation const& choiceInformation, uint_fast64_t& nextFreeVariableIndex) {
                int error = 0;
                std::stringstream variableNameBuffer;
                std::unordered_map<std::pair<uint_fast64_t, uint_fast64_t>, uint_fast64_t, PairHash> resultingMap;
                for (auto state : stateInformation.problematicStates) {
                    std::list<uint_fast64_t> const& relevantChoicesForState = choiceInformation.relevantChoicesForRelevantStates[state];
                    for (uint_fast64_t row : choiceInformation.relevantChoicesForState) {
                        for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator successorIt = labeledMdp.getTransitionMatrix().constColumnIteratorBegin(row); successorIt != labeledMdp.getTransitionMatrix().constColumnIteratorEnd(row); ++successorIt) {
                            if (stateInformation.relevantStates.get(*successorIt)) {
                                variableNameBuffer.str("");
                                variableNameBuffer.clear();
                                variableNameBuffer << "t" << state << "to" << *successorIt;
                                error = GRBaddvar(model, 0, nullptr, nullptr, 0.0, 0.0, 1.0, GRB_BINARY, variableNameBuffer.str().c_str());
                                if (error) {
                                    LOG4CPLUS_ERROR(logger, "Could not create Gurobi variable (" << GRBgeterrormsg(env) << ").");
                                    throw storm::exceptions::InvalidStateException() << "Could not create Gurobi variable (" << GRBgeterrormsg(env) << ").";
                                }
                                resultingMap[std::make_pair(state, *successorIt)] = nextFreeVariableIndex;
                                ++nextFreeVariableIndex;
                            }
                        }
                    }
                }
                return resultingMap;
            }
            
            /*!
             * Creates all variables needed to encode the problem as an MILP problem and returns a struct containing
             * information about the variables that were created. This implicitly establishes the objective function
             * passed to the solver.
             *
             * @param env The Gurobi environment.
             * @param model The Gurobi model.
             * @param labeledMdp The labeled MDP.
             * @param stateInformation The information about the states in the model.
             * @param choiceInformation The information about the choices in the model.
             */
            static VariableInformation createVariables(GRBenv* env, GRBmodel* model, storm::models::Mdp<T> const& labeledMdp, StateInformation const& stateInformation, ChoiceInformation const& choiceInformation) {
                // Create a struct that stores all information about variables.
                VariableInformation result;
                
                // Create variables for involved labels.
                result.labelToVariableIndexMap = createLabelVariables(env, model, choiceInformation.relevantLabes, result.nextFreeVariableIndex);
                
                // Create scheduler variables for relevant states and their actions.
                result.stateToChoiceVariablesIndexMap = createSchedulerVariables(env, model, stateInformation, choiceInformation, result.nextFreeVariableIndex);
                
                // Create variables for probabilities for all relevant states.
                result.stateToProbabilityVariableIndexMap = createProbabilityVariables(env, model, stateInformation, result.nextFreeVariableIndex);
                                
                // Create variables for problematic states.
                result.problematicStateToVariableIndexMap = createProblematicStateVariables(env, model, stateInformation, result.nextFreeVariableIndex);
                
                // Create variables for problematic choices.
                result.problematicTransitionToVariableIndexMap = createProblematicChoiceVariables(env, model, stateInformation, choiceInformation, result.nextFreeVariableIndex);
                
                LOG4CPLUS_INFO(logger, "Successfully created " << result.nextFreeVariableIndex << " Gurobi variables.");
                
                // Finally, return variable information struct.
                return result;
            }
            
            /*!
             * Updates the Gurobi model to incorporate any prior changes.
             *
             * @param env The Gurobi environment.
             * @param model The Gurobi model.
             */
            static void updateModel(GRBenv* env, GRBmodel* model) {
                int error = GRBupdatemodel(model);
                if (error) {
                    LOG4CPLUS_ERROR(logger, "Unable to update Gurobi model (" << GRBgeterrormsg(env) << ").");
                    throw storm::exceptions::InvalidStateException() << "Unable to update Gurobi model (" << GRBgeterrormsg(env) << ").";
                }
            }
            
            /*!
             * Asserts a constraint in the MILP problem that makes sure the reachability probability in the subsystem
             * exceeds the given threshold.
             *
             * @param env The Gurobi environment.
             * @param model The Gurobi model.
             * @param labeledMdp The labeled MDP.
             * @param variableInformation A struct with information about the variables of the model.
             * @param probabilityThreshold The probability that the subsystem must exceed.
             */
            static void assertProbabilityGreaterThanThreshold(GRBenv* env, GRBmodel* model, storm::models::Mdp<T> const& labeledMdp, VariableInformation const& variableInformation, T probabilityThreshold) {
                int error = 0;
                storm::storage::BitVector const& initialStates = labeledMdp.getLabeledStates("init");
                if (initialStates.getNumberOfSetBits() != 1) {
                    LOG4CPLUS_ERROR(logger, "Must have exactly one initial state, but got " << initialStates.getNumberOfSetBits() << "instead.");
                    throw storm::exceptions::InvalidStateException() << "Must have exactly one initial state, but got " << initialStates.getNumberOfSetBits() << "instead.";
                }
                for (auto initialState : initialStates) {
                    int variableIndex = static_cast<int>(variableInformation.stateToProbabilityVariableIndexMap[initialState]);
                    double coefficient = 1.0;
                    error = GRBaddconstr(model, 1, &variableIndex, &coefficient, GRB_GREATER_EQUAL, probabilityThreshold + 1e-6, nullptr);
                    if (error) {
                        LOG4CPLUS_ERROR(logger, "Unable to assert constraint (" << GRBgeterrormsg(env) << ").");
                        throw storm::exceptions::InvalidStateException() << "Unable to assert constraint (" << GRBgeterrormsg(env) << ").";
                    }
                }
            }
            
            /*!
             * Asserts constraints that make sure the selected policy is valid, i.e. chooses at most one action in each state.
             *
             * @param env The Gurobi environment.
             * @param model The Gurobi model.
             * @param stateInformation The information about the states in the model.
             * @param variableInformation A struct with information about the variables of the model.
             */
            static void assertValidPolicy(GRBenv* env, GRBmodel* model, StateInformation const& stateInformation, VariableInformation const& variableInformation) {
                int error = 0;
                for (auto state : stateInformation.relevantStates) {
                    std::list<uint_fast64_t> const& choiceVariableIndices = variableInformation.stateToChoiceVariablesIndexMap[state];
                    std::vector<int> variables;
                    std::vector<double> coefficients(choiceVariableIndices.size(), 1);
                    variables.reserve(choiceVariableIndices.size());
                    for (auto choiceVariableIndex : choiceVariableIndices) {
                        variables.push_back(static_cast<int>(choiceVariableIndex));
                    }
                    error = GRBaddconstr(model, choiceVariableIndices.size(), &variables[0], &coefficients[0], GRB_LESS_EQUAL, 1, nullptr);
                    if (error) {
                        LOG4CPLUS_ERROR(logger, "Unable to assert constraint (" << GRBgeterrormsg(env) << ").");
                        throw storm::exceptions::InvalidStateException() << "Unable to assert constraint (" << GRBgeterrormsg(env) << ").";
                    }
                }
            }
            
            /*!
             * Asserts constraints that make sure the labels are included in the solution set if the policy selects a
             * choice that is labeled with the label in question.
             *
             * @param env The Gurobi environment.
             * @param model The Gurobi model.
             * @param labeledMdp The labeled MDP.
             * @param stateInformation The information about the states in the model.
             * @param choiceInformation The information about the choices in the model.
             * @param variableInformation A struct with information about the variables of the model.
             */
            static void assertChoicesImplyLabels(GRBenv* env, GRBmodel* model, storm::models::Mdp<T> const& labeledMdp, StateInformation const& stateInformation, ChoiceInformation const& choiceInformation, VariableInformation const& variableInformation) {
                int error = 0;
                for (auto state : stateInformation.relevantStates) {
                    std::list<uint_fast64_t> const& choiceVariableIndices = variableInformation.stateToChoiceVariablesIndexMap[state];
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
            }
            
            static void buildConstraintSystem(GRBenv* env, GRBmodel* model, storm::models::Mdp<T> const& labeledMdp, StateInformation const& stateInformation, ChoiceInformation const& choiceInformation, VariableInformation const& variableInformation, T probabilityThreshold) {
                // Assert that the reachability probability in the subsystem exceeds the given threshold.
                assertProbabilityGreaterThanThreshold(env, model, labeledMdp, variableInformation, probabilityThreshold);
                
                // Add constraints that assert the policy takes at most one action in each state.
                assertValidPolicy(env, model, stateInformation, variableInformation);
                
                assertChoicesImplyLabels(env, model, labeledMdp, stateInformation, choiceInformation, variableInformation);
            }
            
            // computeLabelSetFromSolution
            
        public:
            
            static std::unordered_set<uint_fast64_t> getMinimalLabelSet(storm::models::Mdp<T> const& labeledMdp, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, T probabilityThreshold, bool checkThresholdFeasible = false) {
#ifdef HAVE_GUROBI
                // (0) Check whether the MDP is indeed labeled.
                if (!labeledMdp.hasChoiceLabels()) {
                    throw storm::exceptions::InvalidArgumentException() << "Minimal label set generation is impossible for unlabeled model.";
                }
                
                // (1) TODO: check whether its possible to exceed the threshold if checkThresholdFeasible is set.
                
                // (2) Identify relevant and problematic states.
                StateInformation stateInformation = determineRelevantAndProblematicStates(labeledMdp, phiStates, psiStates);
                
                // (3) Determine sets of relevant labels and problematic choices.
                ChoiceInformation choiceInformation = determineRelevantAndProblematicChoices(labeledMdp, stateInformation);
                
                // (4) Encode resulting system as MILP problem.
                //  (4.1) Initialize MILP solver and model.
                std::pair<GRBenv*, GRBmodel*> environmentModelPair = getGurobiEnvironmentAndModel();
                
                //  (4.2) Create variables.
                VariableInformation variableInformation = createVariables(environmentModelPair.first, environmentModelPair.second, labeledMdp, stateInformation, choiceInformation);
                
                // Update model.
                updateModel(environmentModelPair.first, environmentModelPair.second);
 
                // Create all constraints.
                buildConstraintSystem(environmentModelPair.first, environmentModelPair.second, labeledMdp, stateInformation, choiceInformation, variableInformation, probabilityThreshold);
                

                

                
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
                                coefficients.push_back(-successorIt.value());
                            } else if (psiStates.get(successorIt.column())) {
                                rightHandSide += successorIt.value();
                            }
                        }
                        
                        coefficients.push_back(1);
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
                for (auto stateListPair : problematicChoicesForProblematicStates) {
                    for (auto problematicChoice : stateListPair.second) {
                        uint_fast64_t currentChoiceVariableIndex = stateToStartingIndexMap[stateListPair.first];
                        for (auto relevantChoice : relevantChoicesForRelevantStates[stateListPair.first]) {
                            if (relevantChoice == problematicChoice) {
                                break;
                            }
                            ++currentChoiceVariableIndex;
                        }
                        
                        std::vector<int> variables;
                        std::vector<double> coefficients;
                        
                        variables.push_back(currentChoiceVariableIndex);
                        coefficients.push_back(1);
                        
                        for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator successorIt = transitionMatrix.constColumnIteratorBegin(problematicChoice); successorIt != transitionMatrix.constColumnIteratorEnd(problematicChoice); ++successorIt) {
                            variables.push_back(problematicTransitionVariables[std::make_pair(stateListPair.first, *successorIt)]);
                            coefficients.push_back(-1);
                        }
                        
                        error = GRBaddconstr(model, variables.size(), &variables[0], &coefficients[0], GRB_LESS_EQUAL, 0, nullptr);
                        if (error) {
                            LOG4CPLUS_ERROR(logger, "Unable to assert constraint (" << GRBgeterrormsg(env) << ").");
                            throw storm::exceptions::InvalidStateException() << "Unable to assert constraint (" << GRBgeterrormsg(env) << ").";
                        }
                    }
                }
                for (auto state : problematicStates) {
                    for (auto problematicChoice : problematicChoicesForProblematicStates[state]) {
                        for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator successorIt = transitionMatrix.constColumnIteratorBegin(problematicChoice); successorIt != transitionMatrix.constColumnIteratorEnd(problematicChoice); ++successorIt) {
                            std::vector<int> variables;
                            std::vector<double> coefficients;
                            
                            variables.push_back(problematicStateVariablesToIndexMap[state]);
                            coefficients.push_back(1);
                            variables.push_back(problematicStateVariablesToIndexMap[*successorIt]);
                            coefficients.push_back(-1);
                            variables.push_back(problematicTransitionVariables[std::make_pair(state, *successorIt)]);
                            coefficients.push_back(1);
                            
                            error = GRBaddconstr(model, variables.size(), &variables[0], &coefficients[0], GRB_LESS_EQUAL, 1 - 1e-6, nullptr);
                            if (error) {
                                LOG4CPLUS_ERROR(logger, "Unable to assert constraint (" << GRBgeterrormsg(env) << ").");
                                throw storm::exceptions::InvalidStateException() << "Unable to assert constraint (" << GRBgeterrormsg(env) << ").";
                            }
                        }
                    }
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
                
                error = GRBoptimize(model);
                if (error) {
                    LOG4CPLUS_ERROR(logger, "Unable to optimize Gurobi model (" << GRBgeterrormsg(env) << ").");
                    throw storm::exceptions::InvalidStateException() << "Unable to optimize Gurobi model (" << GRBgeterrormsg(env) << ").";
                }
                
                std::vector<double> solution(labelToIndexMap.size());
                error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, labelToIndexMap.size(), &solution[0]);
                if (error) {
                    LOG4CPLUS_ERROR(logger, "Unable to get Gurobi solution (" << GRBgeterrormsg(env) << ").");
                    throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution (" << GRBgeterrormsg(env) << ").";
                }
                
                for (auto labelIndexPair : labelToIndexMap) {
                    std::cout << "label: " << labelIndexPair.first << " with value " << solution[labelIndexPair.second] << std::endl;
                }

                double reachabilityProbability = 0;
                error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, stateToProbabilityVariableIndex[0], 1, &reachabilityProbability);
                std::cout << "prob: " << reachabilityProbability << std::endl;
                
                //  (3.4) Construct constraint system.
                // (4) Read off result from MILP variables.
                
                // (5) Shutdown MILP solver.
                GRBfreemodel(environmentModelPair.second);
                GRBfreeenv(environmentModelPair.first);
                
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
