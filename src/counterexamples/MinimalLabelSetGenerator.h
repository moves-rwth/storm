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
#ifdef HAVE_GUROBI
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
                std::unordered_map<uint_fast64_t, uint_fast64_t> initialStateToChoiceVariableIndexMap;
                std::unordered_map<uint_fast64_t, uint_fast64_t> stateToProbabilityVariableIndexMap;
                uint_fast64_t virtualInitialStateVariableIndex;
                std::unordered_map<uint_fast64_t, uint_fast64_t> problematicStateToVariableIndexMap;
                std::unordered_map<std::pair<uint_fast64_t, uint_fast64_t>, uint_fast64_t, PairHash> problematicTransitionToVariableIndexMap;
                uint_fast64_t nextFreeVariableIndex;

				VariableInformation() : nextFreeVariableIndex(0) {}
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
                result.problematicStates = storm::utility::graph::performProbGreater0E(labeledMdp, backwardTransitions, phiStates, psiStates);
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
                                    result.allRelevantLabels.insert(label);
                                }
                                if (!currentChoiceRelevant) {
                                    currentChoiceRelevant = true;
                                    result.relevantChoicesForRelevantStates[state].push_back(row);
                                }
                            }
                            if (!stateInformation.problematicStates.get(*successorIt)) {
                                allSuccessorsProblematic = false;
                            }
                        }
                        
                        // If all successors of a problematic state are problematic themselves, we record this choice
                        // as being problematic.
                        if (stateInformation.problematicStates.get(state) && allSuccessorsProblematic) {
                            result.problematicChoicesForProblematicStates[state].push_back(row);
                        }
                    }
                }
                LOG4CPLUS_DEBUG(logger, "Found " << result.allRelevantLabels.size() << " relevant labels.");
                return result;
            }
            
            /*!
             * Creates a Gurobi environment and model and returns pointers to them.
             *
             * @return A pair of two pointers to a Gurobi environment and model, respectively.
             */
            static std::pair<GRBenv*, GRBmodel*> createGurobiEnvironmentAndModel() {
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
             * Retrieves whether the given Gurobi model was optimized.
             *
             * @param env The Gurobi environment.
             * @param model The Gurobi model.
             * @return True if the model was optimized. Otherwise an exception is thrown.
             */
            static bool checkGurobiModelIsOptimized(GRBenv* env, GRBmodel* model) {
                int optimalityStatus = 0;
                int error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimalityStatus);
                if (optimalityStatus == GRB_INF_OR_UNBD) {
                    LOG4CPLUS_ERROR(logger, "Unable to get Gurobi solution from infeasible or unbounded model (" << GRBgeterrormsg(env) << ").");
                    throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution from infeasible or unbounded MILP (" << GRBgeterrormsg(env) << ").";
                } else if (optimalityStatus != GRB_OPTIMAL) {
                    LOG4CPLUS_ERROR(logger, "Unable to get Gurobi solution from unoptimized model (" << GRBgeterrormsg(env) << ").");
                    throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution from unoptimized model (" << GRBgeterrormsg(env) << ").";
                }
                return true;
            }
            
            /*!
             * Writes the given Gurobi model to a file.
             *
             * @param env The Gurobi environment.
             * @param model The Gurobi model.
             * @param filename The name of the file in which to write the model.
             */
            static void writeModelToFile(GRBenv* env, GRBmodel* model, std::string const& filename) {
                int error = GRBwrite(model, filename.c_str());
                if (error) {
                    LOG4CPLUS_ERROR(logger, "Unable to write Gurobi model (" << GRBgeterrormsg(env) << ").");
                    throw storm::exceptions::InvalidStateException() << "Unable to write Gurobi model (" << GRBgeterrormsg(env) << ").";
                }
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
             * Calls Gurobi to optimize the given model.
             *
             * @param env The Gurobi environment.
             * @param model The Gurobi model.
             */
            static void optimizeModel(GRBenv* env, GRBmodel* model) {
                int error = GRBoptimize(model);
                if (error) {
                    LOG4CPLUS_ERROR(logger, "Unable to optimize Gurobi model (" << GRBgeterrormsg(env) << ").");
                    throw storm::exceptions::InvalidStateException() << "Unable to optimize Gurobi model (" << GRBgeterrormsg(env) << ").";
                }
            }
            
            /*!
             * Destroys the given Gurobi model and environment to free ressources.
             *
             * @param env The Gurobi environment.
             * @param model The Gurobi model.
             */
            static void destroyGurobiModelAndEnvironment(GRBenv* env, GRBmodel* model) {
                GRBfreemodel(model);
                GRBfreeenv(env);
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
                    std::list<uint_fast64_t> const& relevantChoicesForState = choiceInformation.relevantChoicesForRelevantStates.at(state);
                    for (uint_fast64_t row : relevantChoicesForState) {
                        variableNameBuffer.str("");
                        variableNameBuffer.clear();
                        variableNameBuffer << "choice" << row << "in" << state;
                        error = GRBaddvar(model, 0, nullptr, nullptr, 0.0, 0.0, 1.0, GRB_BINARY, variableNameBuffer.str().c_str());
                        if (error) {
                            LOG4CPLUS_ERROR(logger, "Could not create Gurobi variable (" << GRBgeterrormsg(env) << ").");
                            throw storm::exceptions::InvalidStateException() << "Could not create Gurobi variable (" << GRBgeterrormsg(env) << ").";
                        }
                        resultingMap[state].push_back(nextFreeVariableIndex);
                        ++nextFreeVariableIndex;
                    }
                }
                return resultingMap;
            }
            
            /*!
             * Creates the variables needed for encoding the nondeterministic selection of one of the initial states
             * in the model.
             *
             * @param env The Gurobi environment.
             * @param model The Gurobi model.
             * @param labeledMdp The labeled MDP.
             * @param stateInformation The information about the states of the model.
             * @param nextFreeVariableIndex A reference to the next free variable index. Note: when creating new
             * variables, this value is increased.
             * @return A mapping from initial states to choice variable indices.
             */
            static std::unordered_map<uint_fast64_t, uint_fast64_t> createInitialChoiceVariables(GRBenv* env, GRBmodel* model, storm::models::Mdp<T> const& labeledMdp, StateInformation const& stateInformation, uint_fast64_t& nextFreeVariableIndex) {
                int error = 0;
                std::stringstream variableNameBuffer;
                std::unordered_map<uint_fast64_t, uint_fast64_t> resultingMap;
                for (auto initialState : labeledMdp.getLabeledStates("init")) {
                    // Only consider this initial state if it is relevant.
                    if (stateInformation.relevantStates.get(initialState)) {
                        variableNameBuffer.str("");
                        variableNameBuffer.clear();
                        variableNameBuffer << "init" << initialState;
                        error = GRBaddvar(model, 0, nullptr, nullptr, 0.0, 0.0, 1.0, GRB_BINARY, variableNameBuffer.str().c_str());
                        if (error) {
                            LOG4CPLUS_ERROR(logger, "Could not create Gurobi variable (" << GRBgeterrormsg(env) << ").");
                            throw storm::exceptions::InvalidStateException() << "Could not create Gurobi variable (" << GRBgeterrormsg(env) << ").";
                        }
                        resultingMap[initialState] = nextFreeVariableIndex;
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
             * @param maximizeProbability If set to true, the objective function is constructed in a way that a
             * label-minimal subsystem of maximal probability is computed. 
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
             * Creates the variables for the probabilities in the model.
             *
             * @param env The Gurobi environment.
             * @param model The Gurobi model.
             * @param nextFreeVariableIndex A reference to the next free variable index. Note: when creating new
             * variables, this value is increased.
             * @param maximizeProbability If set to true, the objective function is constructed in a way that a
             * label-minimal subsystem of maximal probability is computed.
             * @return The index of the variable for the probability of the virtual initial state.
             */
            static uint_fast64_t createVirtualInitialStateVariable(GRBenv* env, GRBmodel* model, uint_fast64_t& nextFreeVariableIndex, bool maximizeProbability = false) {
                int error = 0;
                std::stringstream variableNameBuffer;
                variableNameBuffer << "pinit";
                
                error = GRBaddvar(model, 0, nullptr, nullptr, maximizeProbability ? -0.5 : 0.0, 0.0, 1.0, GRB_CONTINUOUS, variableNameBuffer.str().c_str());
                if (error) {
                    LOG4CPLUS_ERROR(logger, "Could not create Gurobi variable (" << GRBgeterrormsg(env) << ").");
                    throw storm::exceptions::InvalidStateException() << "Could not create Gurobi variable (" << GRBgeterrormsg(env) << ").";
                }
                
                uint_fast64_t variableIndex = nextFreeVariableIndex;
                ++nextFreeVariableIndex;
                return variableIndex;
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
                    
                    std::list<uint_fast64_t> const& relevantChoicesForState = choiceInformation.relevantChoicesForRelevantStates.at(state);
                    for (uint_fast64_t row : relevantChoicesForState) {
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
                    std::list<uint_fast64_t> const& relevantChoicesForState = choiceInformation.relevantChoicesForRelevantStates.at(state);
                    for (uint_fast64_t row : relevantChoicesForState) {
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
                result.labelToVariableIndexMap = createLabelVariables(env, model, choiceInformation.allRelevantLabels, result.nextFreeVariableIndex);
                LOG4CPLUS_DEBUG(logger, "Created variables for labels.");
                
                // Create scheduler variables for relevant states and their actions.
                result.stateToChoiceVariablesIndexMap = createSchedulerVariables(env, model, stateInformation, choiceInformation, result.nextFreeVariableIndex);
                LOG4CPLUS_DEBUG(logger, "Created variables for nondeterministic choices.");

                // Create scheduler variables for nondeterministically choosing an initial state.
                result.initialStateToChoiceVariableIndexMap = createInitialChoiceVariables(env, model, labeledMdp, stateInformation, result.nextFreeVariableIndex);
                LOG4CPLUS_DEBUG(logger, "Created variables for the nondeterministic choice of the initial state.");
                
                // Create variables for probabilities for all relevant states.
                result.stateToProbabilityVariableIndexMap = createProbabilityVariables(env, model, stateInformation, result.nextFreeVariableIndex);
                LOG4CPLUS_DEBUG(logger, "Created variables for the reachability probabilities.");

                // Create a probability variable for a virtual initial state that nondeterministically chooses one of the system's real initial states as its target state.
                result.virtualInitialStateVariableIndex = createVirtualInitialStateVariable(env, model, result.nextFreeVariableIndex);
                LOG4CPLUS_DEBUG(logger, "Created variables for the virtual initial state.");

                // Create variables for problematic states.
                result.problematicStateToVariableIndexMap = createProblematicStateVariables(env, model, labeledMdp, stateInformation, choiceInformation, result.nextFreeVariableIndex);
                LOG4CPLUS_DEBUG(logger, "Created variables for the problematic states.");

                // Create variables for problematic choices.
                result.problematicTransitionToVariableIndexMap = createProblematicChoiceVariables(env, model, labeledMdp, stateInformation, choiceInformation, result.nextFreeVariableIndex);
                LOG4CPLUS_DEBUG(logger, "Created variables for the problematic choices.");

                LOG4CPLUS_INFO(logger, "Successfully created " << result.nextFreeVariableIndex << " Gurobi variables.");
                
                // Finally, return variable information struct.
                return result;
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
             * @return The total number of constraints that were created.
             */
            static uint_fast64_t assertProbabilityGreaterThanThreshold(GRBenv* env, GRBmodel* model, storm::models::Mdp<T> const& labeledMdp, VariableInformation const& variableInformation, T probabilityThreshold) {
                uint_fast64_t numberOfConstraintsCreated = 0;
                int error = 0;
                int variableIndex = static_cast<int>(variableInformation.virtualInitialStateVariableIndex);
                double coefficient = 1.0;
                error = GRBaddconstr(model, 1, &variableIndex, &coefficient, GRB_GREATER_EQUAL, probabilityThreshold + 1e-6, nullptr);
                if (error) {
                    LOG4CPLUS_ERROR(logger, "Unable to assert constraint (" << GRBgeterrormsg(env) << ").");
                    throw storm::exceptions::InvalidStateException() << "Unable to assert constraint (" << GRBgeterrormsg(env) << ").";
                }
                ++numberOfConstraintsCreated;
                return numberOfConstraintsCreated;
            }
            
            /*!
             * Asserts constraints that make sure the selected policy is valid, i.e. chooses at most one action in each state.
             *
             * @param env The Gurobi environment.
             * @param model The Gurobi model.
             * @param stateInformation The information about the states in the model.
             * @param variableInformation A struct with information about the variables of the model.
             * @return The total number of constraints that were created.
             */
            static uint_fast64_t assertValidPolicy(GRBenv* env, GRBmodel* model, StateInformation const& stateInformation, VariableInformation const& variableInformation) {
                // Assert that the policy chooses at most one action in each state of the system.
                uint_fast64_t numberOfConstraintsCreated = 0;
                int error = 0;
                for (auto state : stateInformation.relevantStates) {
                    std::list<uint_fast64_t> const& choiceVariableIndices = variableInformation.stateToChoiceVariablesIndexMap.at(state);
                    std::vector<int> variables;
                    std::vector<double> coefficients(choiceVariableIndices.size(), 1);
                    variables.reserve(choiceVariableIndices.size());
                    for (auto choiceVariableIndex : choiceVariableIndices) {
                        variables.push_back(static_cast<int>(choiceVariableIndex));
                    }
                    error = GRBaddconstr(model, variables.size(), &variables[0], &coefficients[0], GRB_LESS_EQUAL, 1, nullptr);
                    if (error) {
                        LOG4CPLUS_ERROR(logger, "Unable to assert constraint (" << GRBgeterrormsg(env) << ").");
                        throw storm::exceptions::InvalidStateException() << "Unable to assert constraint (" << GRBgeterrormsg(env) << ").";
                    }
                    ++numberOfConstraintsCreated;
                }
                
                // Now assert that the virtual initial state picks exactly one initial state from the system as its
                // successor state.
                std::vector<int> variables;
                variables.reserve(variableInformation.initialStateToChoiceVariableIndexMap.size());
                std::vector<double> coefficients(variableInformation.initialStateToChoiceVariableIndexMap.size(), 1);
                for (auto initialStateVariableIndexPair : variableInformation.initialStateToChoiceVariableIndexMap) {
                    variables.push_back(static_cast<int>(initialStateVariableIndexPair.second));
                }
                
                error = GRBaddconstr(model, variables.size(), &variables[0], &coefficients[0], GRB_EQUAL, 1, nullptr);
                if (error) {
                    LOG4CPLUS_ERROR(logger, "Unable to assert constraint (" << GRBgeterrormsg(env) << ").");
                    throw storm::exceptions::InvalidStateException() << "Unable to assert constraint (" << GRBgeterrormsg(env) << ").";
                }
                ++numberOfConstraintsCreated;
                
                return numberOfConstraintsCreated;
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
             * @return The total number of constraints that were created.
             */
            static uint_fast64_t assertChoicesImplyLabels(GRBenv* env, GRBmodel* model, storm::models::Mdp<T> const& labeledMdp, StateInformation const& stateInformation, ChoiceInformation const& choiceInformation, VariableInformation const& variableInformation) {
                uint_fast64_t numberOfConstraintsCreated = 0;
                int error = 0;
                std::vector<std::list<uint_fast64_t>> const& choiceLabeling = labeledMdp.getChoiceLabeling();
                for (auto state : stateInformation.relevantStates) {
                    std::list<uint_fast64_t>::const_iterator choiceVariableIndicesIterator = variableInformation.stateToChoiceVariablesIndexMap.at(state).begin();
                    for (auto choice : choiceInformation.relevantChoicesForRelevantStates.at(state)) {
                        int indices[2]; indices[0] = 0; indices[1] = *choiceVariableIndicesIterator;
                        double coefficients[2]; coefficients[0] = 1; coefficients[1] = -1;
                        for (auto label : choiceLabeling[choice]) {
                            indices[0] = variableInformation.labelToVariableIndexMap.at(label);
                            error = GRBaddconstr(model, 2, indices, coefficients, GRB_GREATER_EQUAL, 0, nullptr);
                            if (error) {
                                LOG4CPLUS_ERROR(logger, "Unable to assert constraint (" << GRBgeterrormsg(env) << ").");
                                throw storm::exceptions::InvalidStateException() << "Unable to assert constraint (" << GRBgeterrormsg(env) << ").";
                            }
                            ++numberOfConstraintsCreated;
                        }
                        ++choiceVariableIndicesIterator;
                    }
                }
                return numberOfConstraintsCreated;
            }
            
            /*!
             * Asserts constraints that make sure the reachability probability is zero for states in which the policy
             * does not pick any outgoing action.
             *
             * @param env The Gurobi environment.
             * @param model The Gurobi model.
             * @param stateInformation The information about the states in the model.
             * @param choiceInformation The information about the choices in the model.
             * @param variableInformation A struct with information about the variables of the model.
             * @return The total number of constraints that were created.
             */
            static uint_fast64_t assertZeroProbabilityWithoutChoice(GRBenv* env, GRBmodel* model, StateInformation const& stateInformation, ChoiceInformation const& choiceInformation, VariableInformation const& variableInformation) {
                uint_fast64_t numberOfConstraintsCreated = 0;
                int error = 0;
                for (auto state : stateInformation.relevantStates) {
                    std::list<uint_fast64_t> const& choiceVariableIndices = variableInformation.stateToChoiceVariablesIndexMap.at(state);

                    std::vector<double> coefficients(choiceVariableIndices.size() + 1, -1);
                    coefficients[0] = 1;
                    std::vector<int> variables;
                    variables.reserve(variableInformation.stateToChoiceVariablesIndexMap.at(state).size() + 1);
                    variables.push_back(static_cast<int>(variableInformation.stateToProbabilityVariableIndexMap.at(state)));
                    
                    variables.insert(variables.end(), choiceVariableIndices.begin(), choiceVariableIndices.end());
                    error = GRBaddconstr(model, choiceVariableIndices.size() + 1, &variables[0], &coefficients[0], GRB_LESS_EQUAL, 0, nullptr);
                    if (error) {
                        LOG4CPLUS_ERROR(logger, "Unable to assert constraint (" << GRBgeterrormsg(env) << ").");
                        throw storm::exceptions::InvalidStateException() << "Unable to assert constraint (" << GRBgeterrormsg(env) << ").";
                    }
                    ++numberOfConstraintsCreated;
                }
                return numberOfConstraintsCreated;
            }
            
            /*!
             * Asserts constraints that encode the correct reachability probabilties for all states.
             *
             * @param env The Gurobi environment.
             * @param model The Gurobi model.
             * @param labeledMdp The labeled MDP.
             * @param psiStates A bit vector characterizing the psi states in the model.
             * @param stateInformation The information about the states in the model.
             * @param choiceInformation The information about the choices in the model.
             * @param variableInformation A struct with information about the variables of the model.
             * @return The total number of constraints that were created.
             */
            static uint_fast64_t assertReachabilityProbabilities(GRBenv* env, GRBmodel* model, storm::models::Mdp<T> const& labeledMdp, storm::storage::BitVector const& psiStates, StateInformation const& stateInformation, ChoiceInformation const& choiceInformation, VariableInformation const& variableInformation) {
                uint_fast64_t numberOfConstraintsCreated = 0;
                int error = 0;
                for (auto state : stateInformation.relevantStates) {
                    std::vector<double> coefficients;
                    std::vector<int> variables;
                    
                    std::list<uint_fast64_t>::const_iterator choiceVariableIndicesIterator = variableInformation.stateToChoiceVariablesIndexMap.at(state).begin();
                    for (auto choice : choiceInformation.relevantChoicesForRelevantStates.at(state)) {
                        variables.clear();
                        coefficients.clear();
                        variables.push_back(static_cast<int>(variableInformation.stateToProbabilityVariableIndexMap.at(state)));
                        coefficients.push_back(1.0);
                        
                        double rightHandSide = 1;
                        typename storm::storage::SparseMatrix<T>::Rows rows = labeledMdp.getTransitionMatrix().getRows(choice, choice);
                        for (typename storm::storage::SparseMatrix<T>::ConstIterator successorIt = rows.begin(), successorIte = rows.end(); successorIt != successorIte; ++successorIt) {
                            if (stateInformation.relevantStates.get(successorIt.column())) {
                                variables.push_back(static_cast<int>(variableInformation.stateToProbabilityVariableIndexMap.at(successorIt.column())));
                                coefficients.push_back(-successorIt.value());
                            } else if (psiStates.get(successorIt.column())) {
                                rightHandSide += successorIt.value();
                            }
                        }
                        
                        coefficients.push_back(1);
                        variables.push_back(static_cast<int>(*choiceVariableIndicesIterator));
                        
                        error = GRBaddconstr(model, variables.size(), &variables[0], &coefficients[0], GRB_LESS_EQUAL, rightHandSide, nullptr);
                        if (error) {
                            LOG4CPLUS_ERROR(logger, "Unable to assert constraint (" << GRBgeterrormsg(env) << ").");
                            throw storm::exceptions::InvalidStateException() << "Unable to assert constraint (" << GRBgeterrormsg(env) << ").";
                        }
                        
                        ++numberOfConstraintsCreated;
                        ++choiceVariableIndicesIterator;
                    }
                }
                
                // Make sure that the virtual initial state is being assigned the probability from the initial state
                // that it selected as a successor state.
                std::vector<double> coefficients(3);
                coefficients[0] = 1;
                coefficients[1] = -1;
                coefficients[2] = 1;
                std::vector<int> variables(3);
                variables[0] = static_cast<int>(variableInformation.virtualInitialStateVariableIndex);
                
                for (auto initialStateVariableIndexPair : variableInformation.initialStateToChoiceVariableIndexMap) {
                    variables[1] = static_cast<int>(variableInformation.stateToProbabilityVariableIndexMap.at(initialStateVariableIndexPair.first));
                    variables[2] = static_cast<int>(initialStateVariableIndexPair.second);
                }
                
                error = GRBaddconstr(model, variables.size(), &variables[0], &coefficients[0], GRB_LESS_EQUAL, 1, nullptr);
                if (error) {
                    LOG4CPLUS_ERROR(logger, "Unable to assert constraint (" << GRBgeterrormsg(env) << ").");
                    throw storm::exceptions::InvalidStateException() << "Unable to assert constraint (" << GRBgeterrormsg(env) << ").";
                }
                
                return numberOfConstraintsCreated;
            }
            
            /*!
             * Asserts constraints that make sure an unproblematic state is reachable from each problematic state.
             *
             * @param env The Gurobi environment.
             * @param model The Gurobi model.
             * @param labeledMdp The labeled MDP.
             * @param stateInformation The information about the states in the model.
             * @param choiceInformation The information about the choices in the model.
             * @param variableInformation A struct with information about the variables of the model.
             * @return The total number of constraints that were created.
             */
            static uint_fast64_t assertUnproblematicStateReachable(GRBenv* env, GRBmodel* model, storm::models::Mdp<T> const& labeledMdp, StateInformation const& stateInformation, ChoiceInformation const& choiceInformation, VariableInformation const& variableInformation) {
                uint_fast64_t numberOfConstraintsCreated = 0;
                int error = 0;
                for (auto stateListPair : choiceInformation.problematicChoicesForProblematicStates) {
                    for (auto problematicChoice : stateListPair.second) {
                        std::list<uint_fast64_t>::const_iterator choiceVariableIndicesIterator = variableInformation.stateToChoiceVariablesIndexMap.at(stateListPair.first).begin();
                        for (auto relevantChoice : choiceInformation.relevantChoicesForRelevantStates.at(stateListPair.first)) {
                            if (relevantChoice == problematicChoice) {
                                break;
                            }
                            ++choiceVariableIndicesIterator;
                        }
                        
                        std::vector<int> variables;
                        std::vector<double> coefficients;
                        
                        variables.push_back(static_cast<int>(*choiceVariableIndicesIterator));
                        coefficients.push_back(1);
                        
                        for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator successorIt = labeledMdp.getTransitionMatrix().constColumnIteratorBegin(problematicChoice); successorIt != labeledMdp.getTransitionMatrix().constColumnIteratorEnd(problematicChoice); ++successorIt) {
                            variables.push_back(static_cast<int>(variableInformation.problematicTransitionToVariableIndexMap.at(std::make_pair(stateListPair.first, *successorIt))));
                            coefficients.push_back(-1);
                        }
                        
                        error = GRBaddconstr(model, variables.size(), &variables[0], &coefficients[0], GRB_LESS_EQUAL, 0, nullptr);
                        if (error) {
                            LOG4CPLUS_ERROR(logger, "Unable to assert constraint (" << GRBgeterrormsg(env) << ").");
                            throw storm::exceptions::InvalidStateException() << "Unable to assert constraint (" << GRBgeterrormsg(env) << ").";
                        }
                        ++numberOfConstraintsCreated;
                    }
                }
                
                for (auto state : stateInformation.problematicStates) {
                    for (auto problematicChoice : choiceInformation.problematicChoicesForProblematicStates.at(state)) {
                        for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator successorIt = labeledMdp.getTransitionMatrix().constColumnIteratorBegin(problematicChoice); successorIt != labeledMdp.getTransitionMatrix().constColumnIteratorEnd(problematicChoice); ++successorIt) {
                            std::vector<int> variables;
                            std::vector<double> coefficients;
                            
                            variables.push_back(static_cast<int>(variableInformation.problematicStateToVariableIndexMap.at(state)));
                            coefficients.push_back(1);
                            variables.push_back(static_cast<int>(variableInformation.problematicStateToVariableIndexMap.at(*successorIt)));
                            coefficients.push_back(-1);
                            variables.push_back(static_cast<int>(variableInformation.problematicTransitionToVariableIndexMap.at(std::make_pair(state, *successorIt))));
                            coefficients.push_back(1);
                            
                            error = GRBaddconstr(model, variables.size(), &variables[0], &coefficients[0], GRB_LESS_EQUAL, 1 - 1e-6, nullptr);
                            if (error) {
                                LOG4CPLUS_ERROR(logger, "Unable to assert constraint (" << GRBgeterrormsg(env) << ").");
                                throw storm::exceptions::InvalidStateException() << "Unable to assert constraint (" << GRBgeterrormsg(env) << ").";
                            }
                            ++numberOfConstraintsCreated;
                        }
                    }
                }
                return numberOfConstraintsCreated;
            }
            
            /*!
             * Asserts constraints that rule out many suboptimal policies.
             *
             * @param env The Gurobi environment.
             * @param model The Gurobi model.
             * @param labeledMdp The labeled MDP.
             * @param psiStates A bit vector characterizing the psi states in the model.
             * @param stateInformation The information about the states in the model.
             * @param choiceInformation The information about the choices in the model.
             * @param variableInformation A struct with information about the variables of the model.
             * @return The total number of constraints that were created.
             */
            static uint_fast64_t assertSchedulerCuts(GRBenv* env, GRBmodel* model, storm::models::Mdp<T> const& labeledMdp, storm::storage::BitVector const& psiStates, StateInformation const& stateInformation, ChoiceInformation const& choiceInformation, VariableInformation const& variableInformation) {
                storm::storage::SparseMatrix<bool> backwardTransitions = labeledMdp.getBackwardTransitions();
                uint_fast64_t numberOfConstraintsCreated = 0;
                int error = 0;
                std::vector<int> variables;
                std::vector<double> coefficients;
                
                for (auto state : stateInformation.relevantStates) {
                    // Assert that all states, that select an action, this action either has a non-zero probability to
                    // go to a psi state directly, or in the successor states, at least one action is selected as well.
                    std::list<uint_fast64_t>::const_iterator choiceVariableIndicesIterator = variableInformation.stateToChoiceVariablesIndexMap.at(state).begin();
                    for (auto choice : choiceInformation.relevantChoicesForRelevantStates.at(state)) {
                        bool psiStateReachableInOneStep = false;
                        for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator successorIt = labeledMdp.getTransitionMatrix().constColumnIteratorBegin(choice); successorIt != labeledMdp.getTransitionMatrix().constColumnIteratorEnd(choice); ++successorIt) {
                            if (psiStates.get(*successorIt)) {
                                psiStateReachableInOneStep = true;
                            }
                        }
                        
                        if (!psiStateReachableInOneStep) {
                            variables.clear();
                            coefficients.clear();
                            
                            variables.push_back(static_cast<int>(*choiceVariableIndicesIterator));
                            coefficients.push_back(1);
                            
                            for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator successorIt = labeledMdp.getTransitionMatrix().constColumnIteratorBegin(choice); successorIt != labeledMdp.getTransitionMatrix().constColumnIteratorEnd(choice); ++successorIt) {
                                if (state != *successorIt && stateInformation.relevantStates.get(*successorIt)) {
                                    std::list<uint_fast64_t> const& successorChoiceVariableIndices = variableInformation.stateToChoiceVariablesIndexMap.at(*successorIt);
                                    
                                    for (auto choiceVariableIndex : successorChoiceVariableIndices) {
                                        variables.push_back(static_cast<int>(choiceVariableIndex));
                                        coefficients.push_back(-1);
                                    }
                                }
                            }
                            
                            error = GRBaddconstr(model, variables.size(), &variables[0], &coefficients[0], GRB_LESS_EQUAL, 0, nullptr);
                            if (error) {
                                LOG4CPLUS_ERROR(logger, "Unable to assert constraint (" << GRBgeterrormsg(env) << ").");
                                throw storm::exceptions::InvalidStateException() << "Unable to assert constraint (" << GRBgeterrormsg(env) << ").";
                            }
                            ++numberOfConstraintsCreated;
                        }
                        
                        ++choiceVariableIndicesIterator;
                    }
                    
                    // For all states assert that there is either a selected incoming transition in the subsystem or the
                    // state is the chosen initial state if there is one selected action in the current state.
                    variables.clear();
                    coefficients.clear();
                    
                    for (auto choiceVariableIndex : variableInformation.stateToChoiceVariablesIndexMap.at(state)) {
                        variables.push_back(static_cast<int>(choiceVariableIndex));
                        coefficients.push_back(1);
                    }
                    
                    // Compute the set of predecessors.
                    std::unordered_set<uint_fast64_t> predecessors;
                    for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator predecessorIt = backwardTransitions.constColumnIteratorBegin(state); predecessorIt != backwardTransitions.constColumnIteratorEnd(state); ++predecessorIt) {
                        if (state != *predecessorIt) {
                            predecessors.insert(*predecessorIt);
                        }
                    }
                    
                    for (auto predecessor : predecessors) {
                        std::list<uint_fast64_t>::const_iterator choiceVariableIndicesIterator = variableInformation.stateToChoiceVariablesIndexMap.at(predecessor).begin();
                        for (auto relevantChoice : choiceInformation.relevantChoicesForRelevantStates.at(predecessor)) {
                            bool choiceTargetsCurrentState = false;
                            
                            // Check if the current choice targets the current state.
                            for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator successorIt = labeledMdp.getTransitionMatrix().constColumnIteratorBegin(relevantChoice); successorIt != labeledMdp.getTransitionMatrix().constColumnIteratorEnd(relevantChoice); ++successorIt) {
                                if (state == *successorIt) {
                                    choiceTargetsCurrentState = true;
                                    break;
                                }
                            }
                            
                            // If it does, we can add the choice to the sum.
                            if (choiceTargetsCurrentState) {
                                variables.push_back(static_cast<int>(*choiceVariableIndicesIterator));
                                coefficients.push_back(-1);
                            }
                            ++choiceVariableIndicesIterator;
                        }
                    }
                    
                    // If the current state is an initial state and is selected as a successor state by the virtual
                    // initial state, then this also justifies making a choice in the current state.
                    if (labeledMdp.getLabeledStates("init").get(state)) {
                        variables.push_back(static_cast<int>(variableInformation.initialStateToChoiceVariableIndexMap.at(state)));
                        coefficients.push_back(-1);
                    }
                    
                    error = GRBaddconstr(model, variables.size(), &variables[0], &coefficients[0], GRB_LESS_EQUAL, 0, nullptr);
                    if (error) {
                        LOG4CPLUS_ERROR(logger, "Unable to assert constraint (" << GRBgeterrormsg(env) << ").");
                        throw storm::exceptions::InvalidStateException() << "Unable to assert constraint (" << GRBgeterrormsg(env) << ").";
                    }
                    ++numberOfConstraintsCreated;
                }
                
                // Assert that at least one initial state selects at least one action.
                variables.clear();
                coefficients.clear();
                for (auto initialState : labeledMdp.getLabeledStates("init")) {
                    for (auto choiceVariableIndex : variableInformation.stateToChoiceVariablesIndexMap.at(initialState)) {
                        variables.push_back(static_cast<int>(choiceVariableIndex));
                        coefficients.push_back(1);
                    }
                }
                error = GRBaddconstr(model, variables.size(), &variables[0], &coefficients[0], GRB_GREATER_EQUAL, 1, nullptr);
                if (error) {
                    LOG4CPLUS_ERROR(logger, "Unable to assert constraint (" << GRBgeterrormsg(env) << ").");
                    throw storm::exceptions::InvalidStateException() << "Unable to assert constraint (" << GRBgeterrormsg(env) << ").";
                }
                ++numberOfConstraintsCreated;
                
                // Add constraints that ensure at least one choice is selected that targets a psi state.
                variables.clear();
                coefficients.clear();
                std::unordered_set<uint_fast64_t> predecessors;
                for (auto psiState : psiStates) {
                    // Compute the set of predecessors.
                    for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator predecessorIt = backwardTransitions.constColumnIteratorBegin(psiState); predecessorIt != backwardTransitions.constColumnIteratorEnd(psiState); ++predecessorIt) {
                        if (psiState != *predecessorIt) {
                            predecessors.insert(*predecessorIt);
                        }
                    }
                }
                
                for (auto predecessor : predecessors) {
                    std::list<uint_fast64_t>::const_iterator choiceVariableIndicesIterator = variableInformation.stateToChoiceVariablesIndexMap.at(predecessor).begin();
                    for (auto relevantChoice : choiceInformation.relevantChoicesForRelevantStates.at(predecessor)) {
                        bool choiceTargetsPsiState = false;
                        
                        // Check if the current choice targets the current state.
                        for (typename storm::storage::SparseMatrix<T>::ConstIndexIterator successorIt = labeledMdp.getTransitionMatrix().constColumnIteratorBegin(relevantChoice); successorIt != labeledMdp.getTransitionMatrix().constColumnIteratorEnd(relevantChoice); ++successorIt) {
                            if (psiStates.get(*successorIt)) {
                                choiceTargetsPsiState = true;
                                break;
                            }
                        }
                        
                        // If it does, we can add the choice to the sum.
                        if (choiceTargetsPsiState) {
                            variables.push_back(static_cast<int>(*choiceVariableIndicesIterator));
                            coefficients.push_back(1);
                        }
                        ++choiceVariableIndicesIterator;
                    }
                }
                
                error = GRBaddconstr(model, variables.size(), &variables[0], &coefficients[0], GRB_GREATER_EQUAL, 1, nullptr);
                if (error) {
                    LOG4CPLUS_ERROR(logger, "Unable to assert constraint (" << GRBgeterrormsg(env) << ").");
                    throw storm::exceptions::InvalidStateException() << "Unable to assert constraint (" << GRBgeterrormsg(env) << ").";
                }
                ++numberOfConstraintsCreated;
                
                return numberOfConstraintsCreated;
            }
            
            /*!
             * Builds a system of constraints that express that the reachability probability in the subsystem exceeeds
             * the given threshold.
             *
             * @param env The Gurobi environment.
             * @param model The Gurobi model.
             * @param labeledMdp The labeled MDP.
             * @param psiStates A bit vector characterizing all psi states in the model.
             * @param stateInformation The information about the states in the model.
             * @param choiceInformation The information about the choices in the model.
             * @param variableInformation A struct with information about the variables of the model.
             * @param probabilityThreshold The probability threshold the subsystem is required to exceed.
             * @param includeSchedulerCuts If set to true, additional constraints are asserted that reduce the set of
             * possible choices.
             */
            static void buildConstraintSystem(GRBenv* env, GRBmodel* model, storm::models::Mdp<T> const& labeledMdp, storm::storage::BitVector const& psiStates, StateInformation const& stateInformation, ChoiceInformation const& choiceInformation, VariableInformation const& variableInformation, T probabilityThreshold, bool includeSchedulerCuts = false) {
                // Assert that the reachability probability in the subsystem exceeds the given threshold.
                uint_fast64_t numberOfConstraints = assertProbabilityGreaterThanThreshold(env, model, labeledMdp, variableInformation, probabilityThreshold);
                LOG4CPLUS_DEBUG(logger, "Asserted that reachability probability exceeds threshold.");

                // Add constraints that assert the policy takes at most one action in each state.
                numberOfConstraints += assertValidPolicy(env, model, stateInformation, variableInformation);
                LOG4CPLUS_DEBUG(logger, "Asserted that policy is valid.");

                // Add constraints that assert the labels that belong to some taken choices are taken as well.
                numberOfConstraints += assertChoicesImplyLabels(env, model, labeledMdp, stateInformation, choiceInformation, variableInformation);
                LOG4CPLUS_DEBUG(logger, "Asserted that labels implied by choices are taken.");

                // Add constraints that encode that the reachability probability from states which do not pick any action
                // is zero.
                numberOfConstraints += assertZeroProbabilityWithoutChoice(env, model, stateInformation, choiceInformation, variableInformation);
                LOG4CPLUS_DEBUG(logger, "Asserted that reachability probability is zero if no choice is taken.");

                // Add constraints that encode the reachability probabilities for states.
                numberOfConstraints += assertReachabilityProbabilities(env, model, labeledMdp, psiStates, stateInformation, choiceInformation, variableInformation);
                LOG4CPLUS_DEBUG(logger, "Asserted constraints for reachability probabilities.");

                // Add constraints that ensure the reachability of an unproblematic state from each problematic state.
                numberOfConstraints += assertUnproblematicStateReachable(env, model, labeledMdp, stateInformation, choiceInformation, variableInformation);
                LOG4CPLUS_DEBUG(logger, "Asserted that unproblematic state reachable from problematic states.");

                // If required, assert additional constraints that reduce the number of possible policies.
                if (includeSchedulerCuts) {
                    numberOfConstraints += assertSchedulerCuts(env, model, labeledMdp, psiStates, stateInformation, choiceInformation, variableInformation);
                    LOG4CPLUS_DEBUG(logger, "Asserted scheduler cuts.");
                }
                
                LOG4CPLUS_INFO(logger, "Successfully created " << numberOfConstraints << " Gurobi constraints.");
            }
            
            /*!
             * Computes the set of labels that was used in the given optimized model.
             *
             * @param env The Gurobi environment.
             * @param model The Gurobi model.
             * @param variableInformation A struct with information about the variables of the model.
             */
            static std::unordered_set<uint_fast64_t> getUsedLabelsInSolution(GRBenv* env, GRBmodel* model, VariableInformation const& variableInformation) {
                int error = 0;

                // Check whether the model was optimized, so we can read off the solution.
                if (checkGurobiModelIsOptimized(env, model)) {
                    std::unordered_set<uint_fast64_t> result;
                    double value = 0;
                    
                    for (auto labelVariablePair : variableInformation.labelToVariableIndexMap) {
                        error = GRBgetdblattrelement(model, GRB_DBL_ATTR_X, labelVariablePair.second, &value);
                        if (error) {
                            LOG4CPLUS_ERROR(logger, "Unable to get Gurobi solution (" << GRBgeterrormsg(env) << ").");
                            throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution (" << GRBgeterrormsg(env) << ").";
                        }
                        
                        if (std::abs(value - 1) <= storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble()) {
                            result.insert(labelVariablePair.first);
                        } else if (std::abs(value) > storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble()) {
                            LOG4CPLUS_ERROR(logger, "Illegal value for binary variable in Gurobi solution (" << value << ").");
                            throw storm::exceptions::InvalidStateException() << "Illegal value for binary variable in Gurobi solution (" << value << ").";
                        }
                    }
                    return result;
                }
                
                LOG4CPLUS_ERROR(logger, "Unable to get Gurobi solution from unoptimized model.");
                throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution from unoptimized model.";
            }
            
            /*!
             * Computes a mapping from relevant states to choices such that a state is mapped to one of its choices if
             * it is selected by the subsystem computed by the solver.
             *
             * @param env The Gurobi environment.
             * @param model The Gurobi model.
             * @param labeledMdp The labeled MDP.
             * @param stateInformation The information about the states in the model.
             * @param choiceInformation The information about the choices in the model.
             * @param variableInformation A struct with information about the variables of the model.
             */
            static std::map<uint_fast64_t, uint_fast64_t> getChoices(GRBenv* env, GRBmodel* model, storm::models::Mdp<T> const& labeledMdp, StateInformation const& stateInformation, ChoiceInformation const& choiceInformation, VariableInformation const& variableInformation) {
                int error = 0;
                if (checkGurobiModelIsOptimized(env, model)) {
                    std::map<uint_fast64_t, uint_fast64_t> result;
                    double value = 0;

                    for (auto state : stateInformation.relevantStates) {
                        std::list<uint_fast64_t>::const_iterator choiceVariableIndicesIterator = variableInformation.stateToChoiceVariablesIndexMap.at(state).begin();
                        for (auto choice : choiceInformation.relevantChoicesForRelevantStates.at(state)) {
                            error = GRBgetdblattrelement(model, GRB_DBL_ATTR_X, *choiceVariableIndicesIterator, &value);
                            if (error) {
                                LOG4CPLUS_ERROR(logger, "Unable to get Gurobi solution (" << GRBgeterrormsg(env) << ").");
                                throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution (" << GRBgeterrormsg(env) << ").";
                            }
                            ++choiceVariableIndicesIterator;
                            
                            if (std::abs(value - 1) <= storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble()) {
                                result.emplace_hint(result.end(), state, choice);
                            } else if (std::abs(value) > storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble()) {
                                LOG4CPLUS_ERROR(logger, "Illegal value for binary variable in Gurobi solution (" << value << ").");
                                throw storm::exceptions::InvalidStateException() << "Illegal value for binary variable in Gurobi solution (" << value << ").";
                            }
                        }
                    }
                    
                    return result;
                }
                LOG4CPLUS_ERROR(logger, "Unable to get Gurobi solution from unoptimized model.");
                throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution from unoptimized model.";
            }
            
            /*!
             * Computes the reachability probability and the selected initial state in the given optimized Gurobi model.
             *
             * @param env The Gurobi environment.
             * @param model The Gurobi model.
             * @param labeledMdp The labeled MDP.
             * @param variableInformation A struct with information about the variables of the model.
             */
            static std::pair<uint_fast64_t, double> getReachabilityProbability(GRBenv* env, GRBmodel* model, storm::models::Mdp<T> const& labeledMdp, VariableInformation const& variableInformation) {
                int error = 0;
                // Check whether the model was optimized, so we can read off the solution.
                if (checkGurobiModelIsOptimized(env, model)) {
                    uint_fast64_t selectedInitialState = 0;
                    double value = 0;
                    for (auto initialStateVariableIndexPair : variableInformation.initialStateToChoiceVariableIndexMap) {
                        error = GRBgetdblattrelement(model, GRB_DBL_ATTR_X, initialStateVariableIndexPair.second, &value);
                        if (error) {
                            LOG4CPLUS_ERROR(logger, "Unable to get Gurobi solution (" << GRBgeterrormsg(env) << ").");
                            throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution (" << GRBgeterrormsg(env) << ").";
                        }
                        
                        if (std::abs(value - 1) <= storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble()) {
                            selectedInitialState = initialStateVariableIndexPair.first;
                            break;
                        } else if (std::abs(value) > storm::settings::Settings::getInstance()->getOptionByLongName("precision").getArgument(0).getValueAsDouble()) {
                            LOG4CPLUS_ERROR(logger, "Illegal value for binary variable in Gurobi solution (" << value << ").");
                            throw storm::exceptions::InvalidStateException() << "Illegal value for binary variable in Gurobi solution (" << value << ").";
                        }
                    }
                    
                    double reachabilityProbability = 0;
                    error = GRBgetdblattrelement(model, GRB_DBL_ATTR_X, variableInformation.virtualInitialStateVariableIndex, &reachabilityProbability);
                    if (error) {
                        LOG4CPLUS_ERROR(logger, "Unable to get Gurobi solution (" << GRBgeterrormsg(env) << ").");
                        throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution (" << GRBgeterrormsg(env) << ").";
                    }
                    return std::make_pair(selectedInitialState, reachabilityProbability);
                }
                
                LOG4CPLUS_ERROR(logger, "Unable to get Gurobi solution from unoptimized model.");
                throw storm::exceptions::InvalidStateException() << "Unable to get Gurobi solution from unoptimized model.";
            }
#endif

        public:
            
            static std::unordered_set<uint_fast64_t> getMinimalLabelSet(storm::models::Mdp<T> const& labeledMdp, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, T probabilityThreshold, bool checkThresholdFeasible = false, bool includeSchedulerCuts = false) {
#ifdef HAVE_GUROBI
                // (0) Check whether the MDP is indeed labeled.
                if (!labeledMdp.hasChoiceLabels()) {
                    throw storm::exceptions::InvalidArgumentException() << "Minimal label set generation is impossible for unlabeled model.";
                }
                
                // (1) FIXME: check whether its possible to exceed the threshold if checkThresholdFeasible is set.
                
                // (2) Identify relevant and problematic states.
                StateInformation stateInformation = determineRelevantAndProblematicStates(labeledMdp, phiStates, psiStates);
                
                // (3) Determine sets of relevant labels and problematic choices.
                ChoiceInformation choiceInformation = determineRelevantAndProblematicChoices(labeledMdp, stateInformation, psiStates);
                
                // (4) Encode resulting system as MILP problem.
                //  (4.1) Initialize Gurobi environment and model.
                std::pair<GRBenv*, GRBmodel*> environmentModelPair = createGurobiEnvironmentAndModel();
                
                //  (4.2) Create variables.
                VariableInformation variableInformation = createVariables(environmentModelPair.first, environmentModelPair.second, labeledMdp, stateInformation, choiceInformation);
                
                // Update model.
                updateModel(environmentModelPair.first, environmentModelPair.second);
 
                //  (4.3) Construct constraint system.
                buildConstraintSystem(environmentModelPair.first, environmentModelPair.second, labeledMdp, psiStates, stateInformation, choiceInformation, variableInformation, probabilityThreshold, includeSchedulerCuts);
                
                // Update model.
                updateModel(environmentModelPair.first, environmentModelPair.second);

                // writeModelToFile(environmentModelPair.first, environmentModelPair.second, "storm.lp");
                
                // (4.4) Optimize the model.
                optimizeModel(environmentModelPair.first, environmentModelPair.second);
                
                // (4.5) Read off result from variables.
                std::unordered_set<uint_fast64_t> usedLabelSet = getUsedLabelsInSolution(environmentModelPair.first, environmentModelPair.second, variableInformation);
                
                // Display achieved probability.
                std::pair<uint_fast64_t, double> initialStateProbabilityPair = getReachabilityProbability(environmentModelPair.first, environmentModelPair.second, labeledMdp, variableInformation);
                LOG4CPLUS_DEBUG(logger, "Achieved probability " << initialStateProbabilityPair.second << " in initial state " << initialStateProbabilityPair.first << ".");
                
                // (4.6) Shutdown Gurobi.
                destroyGurobiModelAndEnvironment(environmentModelPair.first, environmentModelPair.second);
                
                // (5) Return result.
                return usedLabelSet;
#else
                throw storm::exceptions::NotImplementedException() << "This functionality is unavailable if StoRM is compiled without support for Gurobi.";
#endif
            }
            
        };
        
    }
}

#endif /* STORM_COUNTEREXAMPLES_MINIMALLABELSETGENERATOR_MDP_H_ */
