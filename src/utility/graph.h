#ifndef STORM_UTILITY_GRAPH_H_
#define STORM_UTILITY_GRAPH_H_

#include <set>
#include <limits>

#include "utility/OsDetection.h"

#include "src/storage/sparse/StateType.h"
#include "src/storage/PartialScheduler.h"
#include "src/models/sparse/NondeterministicModel.h"
#include "src/models/sparse/DeterministicModel.h"
#include "src/storage/dd/DdType.h"

namespace storm {
    namespace storage {
        class BitVector;
        template<typename VT> class SparseMatrix;
    }
    
    namespace models {
        
        namespace symbolic {
            template<storm::dd::DdType T> class Model;
            template<storm::dd::DdType T> class DeterministicModel;
            template<storm::dd::DdType T> class NondeterministicModel;
        }
        
    }
    
    namespace dd {
        template<storm::dd::DdType T> class Bdd;
        template<storm::dd::DdType T> class Add;
        
    }
    
    
    namespace utility {
        namespace graph {
            
            /*!
             * Performs a forward depth-first search through the underlying graph structure to identify the states that
             * are reachable from the given set only passing through a constrained set of states until some target
             * have been reached.
             *
             * @param transitionMatrix The transition relation of the graph structure to search.
             * @param initialStates The set of states from which to start the search.
             * @param constraintStates The set of states that must not be left.
             * @param targetStates The target states that may not be passed.
             */
            template<typename T>
            storm::storage::BitVector getReachableStates(storm::storage::SparseMatrix<T> const& transitionMatrix, storm::storage::BitVector const& initialStates, storm::storage::BitVector const& constraintStates, storm::storage::BitVector const& targetStates);
            
            /*!
             * Performs a breadth-first search through the underlying graph structure to compute the distance from all
             * states to the starting states of the search.
             *
             * @param transitionMatrix The transition relation of the graph structure to search.
             * @param initialStates The set of states from which to start the search.
             * @return The distances of each state to the initial states of the sarch.
             */
            template<typename T>
            std::vector<std::size_t> getDistances(storm::storage::SparseMatrix<T> const& transitionMatrix, storm::storage::BitVector const& initialStates);
            
            /*!
             * Performs a backward depth-first search trough the underlying graph structure
             * of the given model to determine which states of the model have a positive probability
             * of satisfying phi until psi. The resulting states are written to the given bit vector.
             *
             * @param backwardTransitions The reversed transition relation of the graph structure to search.
             * @param phiStates A bit vector of all states satisfying phi.
             * @param psiStates A bit vector of all states satisfying psi.
             * @param useStepBound A flag that indicates whether or not to use the given number of maximal steps for the search.
             * @param maximalSteps The maximal number of steps to reach the psi states.
             * @return A bit vector with all indices of states that have a probability greater than 0.
             */
            template <typename T>
            storm::storage::BitVector performProbGreater0(storm::storage::SparseMatrix<T> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool useStepBound = false, uint_fast64_t maximalSteps = 0);
            
            /*!
             * Computes the set of states of the given model for which all paths lead to
             * the given set of target states and only visit states from the filter set
             * before. In order to do this, it uses the given set of states that
             * characterizes the states that possess at least one path to a target state.
             * The results are written to the given bit vector.
             *
             * @param backwardTransitions The reversed transition relation of the graph structure to search.
             * @param phiStates A bit vector of all states satisfying phi.
             * @param psiStates A bit vector of all states satisfying psi.
             * @param statesWithProbabilityGreater0 A reference to a bit vector of states that possess a positive
             * probability mass of satisfying phi until psi.
             * @return A bit vector with all indices of states that have a probability greater than 1.
             */
            template <typename T>
            storm::storage::BitVector performProb1(storm::storage::SparseMatrix<T> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, storm::storage::BitVector const& statesWithProbabilityGreater0);
            
            /*!
             * Computes the set of states of the given model for which all paths lead to
             * the given set of target states and only visit states from the filter set
             * before. In order to do this, it uses the given set of states that
             * characterizes the states that possess at least one path to a target state.
             * The results are written to the given bit vector.
             *
             * @param backwardTransitions The reversed transition relation of the graph structure to search.
             * @param phiStates A bit vector of all states satisfying phi.
             * @param psiStates A bit vector of all states satisfying psi.
             * @return A bit vector with all indices of states that have a probability greater than 1.
             */
            template <typename T>
            storm::storage::BitVector performProb1(storm::storage::SparseMatrix<T> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates);
            
            /*!
             * Computes the sets of states that have probability 0 or 1, respectively, of satisfying phi until psi in a
             * deterministic model.
             *
             * @param model The model whose graph structure to search.
             * @param phiStates The set of all states satisfying phi.
             * @param psiStates The set of all states satisfying psi.
             * @return A pair of bit vectors such that the first bit vector stores the indices of all states
             * with probability 0 and the second stores all indices of states with probability 1.
             */
            template <typename T>
            std::pair<storm::storage::BitVector, storm::storage::BitVector> performProb01(storm::models::sparse::DeterministicModel<T> const& model, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates);
            
            /*!
             * Computes the sets of states that have probability 0 or 1, respectively, of satisfying phi until psi in a
             * deterministic model.
             *
             * @param backwardTransitions The backward transitions of the model whose graph structure to search.
             * @param phiStates The set of all states satisfying phi.
             * @param psiStates The set of all states satisfying psi.
             * @return A pair of bit vectors such that the first bit vector stores the indices of all states
             * with probability 0 and the second stores all indices of states with probability 1.
             */
            template <typename T>
            std::pair<storm::storage::BitVector, storm::storage::BitVector> performProb01(storm::storage::SparseMatrix<T> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates);
            
            /*!
             * Computes the set of states that has a positive probability of reaching psi states after only passing
             * through phi states before.
             *
             * @param model The (symbolic) model for which to compute the set of states.
             * @param transitionMatrix The transition matrix of the model as a BDD.
             * @param phiStates The BDD containing all phi states of the model.
             * @param psiStates The BDD containing all psi states of the model.
             * @param stepBound If given, this number indicates the maximal amount of steps allowed.
             * @return All states with positive probability.
             */
            template <storm::dd::DdType Type>
            storm::dd::Bdd<Type> performProbGreater0(storm::models::symbolic::Model<Type> const& model, storm::dd::Bdd<Type> const& transitionMatrix, storm::dd::Bdd<Type> const& phiStates, storm::dd::Bdd<Type> const& psiStates, boost::optional<uint_fast64_t> const& stepBound = boost::optional<uint_fast64_t>());
            /*!
             * Computes the set of states that have a probability of one of reaching psi states after only passing
             * through phi states before.
             *
             * @param model The (symbolic) model for which to compute the set of states.
             * @param transitionMatrix The transition matrix of the model as a BDD.
             * @param phiStates The BDD containing all phi states of the model.
             * @param psiStates The BDD containing all psi states of the model.
             * @param statesWithProbabilityGreater0 The set of states with a positive probability of satisfying phi
             * until psi as a BDD.
             * @return All states with probability 1.
             */
            template <storm::dd::DdType Type>
            storm::dd::Bdd<Type> performProb1(storm::models::symbolic::Model<Type> const& model, storm::dd::Bdd<Type> const& transitionMatrix, storm::dd::Bdd<Type> const& phiStates, storm::dd::Bdd<Type> const& psiStates, storm::dd::Bdd<Type> const& statesWithProbabilityGreater0);
            
            /*!
             * Computes the set of states that have a probability of one of reaching psi states after only passing
             * through phi states before.
             *
             * @param model The (symbolic) model for which to compute the set of states.
             * @param transitionMatrix The transition matrix of the model as a BDD.
             * @param phiStates The BDD containing all phi states of the model.
             * @param psiStates The BDD containing all psi states of the model.
             * @return All states with probability 1.
             */
            template <storm::dd::DdType Type>
            storm::dd::Bdd<Type> performProb1(storm::models::symbolic::Model<Type> const& model, storm::dd::Bdd<Type> const& transitionMatrix, storm::dd::Bdd<Type> const& phiStates, storm::dd::Bdd<Type> const& psiStates) ;
            
            /*!
             * Computes the sets of states that have probability 0 or 1, respectively, of satisfying phi until psi in a
             * deterministic model.
             *
             * @param model The (symbolic) model for which to compute the set of states.
             * @param phiStates The BDD containing all  phi states of the model.
             * @param psiStates The BDD containing all psi states of the model.
             * @return A pair of BDDs that represent all states with probability 0 and 1, respectively.
             */
            template <storm::dd::DdType Type>
            std::pair<storm::dd::Bdd<Type>, storm::dd::Bdd<Type>> performProb01(storm::models::symbolic::DeterministicModel<Type> const& model, storm::dd::Bdd<Type> const& phiStates, storm::dd::Bdd<Type> const& psiStates);
            
            /*!
             * Computes the sets of states that have probability 0 or 1, respectively, of satisfying phi until psi in a
             * deterministic model.
             *
             * @param model The (symbolic) model for which to compute the set of states. This is used for retrieving the
             * manager and information about the meta variables.
             * @param phiStates The BDD containing all  phi states of the model.
             * @param psiStates The BDD containing all psi states of the model.
             * @return A pair of BDDs that represent all states with probability 0 and 1, respectively.
             */
            template <storm::dd::DdType Type>
            std::pair<storm::dd::Bdd<Type>, storm::dd::Bdd<Type>> performProb01(storm::models::symbolic::Model<Type> const& model, storm::dd::Add<Type> const& transitionMatrix, storm::dd::Bdd<Type> const& phiStates, storm::dd::Bdd<Type> const& psiStates);
            
            /*!
             * Computes a scheduler for the given states that chooses an action that stays completely in the very same set.
             * Note that this assumes that there is a legal choice for each of the states.
             *
             * @param states The set of states for which to compute the scheduler that stays in this very set.
             * @param transitionMatrix The transition matrix.
             */
            template <typename T>
            storm::storage::PartialScheduler computeSchedulerStayingInStates(storm::storage::BitVector const& states, storm::storage::SparseMatrix<T> const& transitionMatrix);

            /*!
             * Computes a scheduler for the given states that chooses an action that has at least one successor in the
             * given set of states. Note that this assumes that there is a legal choice for each of the states.
             *
             * @param states The set of states for which to compute the scheduler that chooses an action with a successor
             * in this very set.
             * @param transitionMatrix The transition matrix.
             */
            template <typename T>
            storm::storage::PartialScheduler computeSchedulerWithOneSuccessorInStates(storm::storage::BitVector const& states, storm::storage::SparseMatrix<T> const& transitionMatrix);
            
            /*!
             * Computes a scheduler for the given states that have a scheduler that has a probability greater 0.
             *
             * @param probGreater0EStates The states that have a scheduler achieving a probablity greater 0.
             * @param transitionMatrix The transition matrix of the system.
             */
            template <typename T>
            storm::storage::PartialScheduler computeSchedulerProbGreater0E(storm::storage::BitVector const& probGreater0EStates, storm::storage::SparseMatrix<T> const& transitionMatrix);

            /*!
             * Computes a scheduler for the given states that have a scheduler that has a probability 0.
             *
             * @param prob0EStates The states that have a scheduler achieving probablity 0.
             * @param transitionMatrix The transition matrix of the system.
             */
            template <typename T>
            storm::storage::PartialScheduler computeSchedulerProb0E(storm::storage::BitVector const& prob0EStates, storm::storage::SparseMatrix<T> const& transitionMatrix);

            /*!
             * Computes a scheduler for the given states that have a scheduler that has a probability 0.
             *
             * @param prob1EStates The states that have a scheduler achieving probablity 1.
             * @param transitionMatrix The transition matrix of the system.
             */
            template <typename T>
            storm::storage::PartialScheduler computeSchedulerProb1E(storm::storage::BitVector const& prob1EStates, storm::storage::SparseMatrix<T> const& transitionMatrix, storm::storage::SparseMatrix<T> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates);
            
            /*!
             * Computes the sets of states that have probability greater 0 of satisfying phi until psi under at least
             * one possible resolution of non-determinism in a non-deterministic model. Stated differently,
             * this means that these states have a probability greater 0 of satisfying phi until psi if the
             * scheduler tries to minimize this probability.
             *
             * @param model The model whose graph structure to search.
             * @param backwardTransitions The reversed transition relation of the model.
             * @param phiStates The set of all states satisfying phi.
             * @param psiStates The set of all states satisfying psi.
             * @param useStepBound A flag that indicates whether or not to use the given number of maximal steps for the search.
             * @param maximalSteps The maximal number of steps to reach the psi states.
             * @return A bit vector that represents all states with probability 0.
             */
            template <typename T>
            storm::storage::BitVector performProbGreater0E(storm::storage::SparseMatrix<T> const& transitionMatrix, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, storm::storage::SparseMatrix<T> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool useStepBound = false, uint_fast64_t maximalSteps = 0) ;
            
            template <typename T>
            storm::storage::BitVector performProb0A(storm::storage::SparseMatrix<T> const& transitionMatrix, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, storm::storage::SparseMatrix<T> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates);
            
            /*!
             * Computes the sets of states that have probability 0 of satisfying phi until psi under all
             * possible resolutions of non-determinism in a non-deterministic model. Stated differently,
             * this means that these states have probability 0 of satisfying phi until psi even if the
             * scheduler tries to maximize this probability.
             *
             * @param model The model whose graph structure to search.
             * @param backwardTransitions The reversed transition relation of the model.
             * @param phiStates The set of all states satisfying phi.
             * @param psiStates The set of all states satisfying psi.
             * @param useStepBound A flag that indicates whether or not to use the given number of maximal steps for the search.
             * @param maximalSteps The maximal number of steps to reach the psi states.
             * @return A bit vector that represents all states with probability 0.
             */
            template <typename T, typename RM>
            storm::storage::BitVector performProb0A(storm::models::sparse::NondeterministicModel<T, RM> const& model, storm::storage::SparseMatrix<T> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates) ;
            /*!
             * Computes the sets of states that have probability 1 of satisfying phi until psi under at least
             * one possible resolution of non-determinism in a non-deterministic model. Stated differently,
             * this means that these states have probability 1 of satisfying phi until psi if the
             * scheduler tries to maximize this probability.
             *
             * @param model The model whose graph structure to search.
             * @param backwardTransitions The reversed transition relation of the model.
             * @param phiStates The set of all states satisfying phi.
             * @param psiStates The set of all states satisfying psi.
             * @return A bit vector that represents all states with probability 1.
             */
            template <typename T>
            storm::storage::BitVector performProb1E(storm::storage::SparseMatrix<T> const& transitionMatrix, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, storm::storage::SparseMatrix<T> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates);
            
            /*!
             * Computes the sets of states that have probability 1 of satisfying phi until psi under at least
             * one possible resolution of non-determinism in a non-deterministic model. Stated differently,
             * this means that these states have probability 1 of satisfying phi until psi if the
             * scheduler tries to maximize this probability.
             *
             * @param model The model whose graph structure to search.
             * @param backwardTransitions The reversed transition relation of the model.
             * @param phiStates The set of all states satisfying phi.
             * @param psiStates The set of all states satisfying psi.
             * @return A bit vector that represents all states with probability 1.
             */
            template <typename T, typename RM>
            storm::storage::BitVector performProb1E(storm::models::sparse::NondeterministicModel<T, RM> const& model, storm::storage::SparseMatrix<T> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates);
            
            template <typename T>
            std::pair<storm::storage::BitVector, storm::storage::BitVector> performProb01Max(storm::storage::SparseMatrix<T> const& transitionMatrix, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, storm::storage::SparseMatrix<T> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates) ;

            /*!
             * Computes the sets of states that have probability 0 or 1, respectively, of satisfying phi
             * until psi in a non-deterministic model in which all non-deterministic choices are resolved
             * such that the probability is maximized.
             *
             * @param model The model whose graph structure to search.
             * @param phiStates The set of all states satisfying phi.
             * @param psiStates The set of all states satisfying psi.
             * @return A pair of bit vectors that represent all states with probability 0 and 1, respectively.
             */
            template <typename T, typename RM>
            std::pair<storm::storage::BitVector, storm::storage::BitVector> performProb01Max(storm::models::sparse::NondeterministicModel<T, RM> const& model, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates) ;
            
            /*!
             * Computes the sets of states that have probability greater 0 of satisfying phi until psi under any
             * possible resolution of non-determinism in a non-deterministic model. Stated differently,
             * this means that these states have a probability greater 0 of satisfying phi until psi if the
             * scheduler tries to maximize this probability.
             *
             * @param model The model whose graph structure to search.
             * @param backwardTransitions The reversed transition relation of the model.
             * @param phiStates The set of all states satisfying phi.
             * @param psiStates The set of all states satisfying psi.
             * @param useStepBound A flag that indicates whether or not to use the given number of maximal steps for the search.
             * @param maximalSteps The maximal number of steps to reach the psi states.
             * @return A bit vector that represents all states with probability 0.
             */
            template <typename T>
            storm::storage::BitVector performProbGreater0A(storm::storage::SparseMatrix<T> const& transitionMatrix, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, storm::storage::SparseMatrix<T> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, bool useStepBound = false, uint_fast64_t maximalSteps = 0);
            
            /*!
             * Computes the sets of states that have probability 0 of satisfying phi until psi under at least
             * one possible resolution of non-determinism in a non-deterministic model. Stated differently,
             * this means that these states have probability 0 of satisfying phi until psi if the
             * scheduler tries to minimize this probability.
             *
             * @param model The model whose graph structure to search.
             * @param backwardTransitions The reversed transition relation of the model.
             * @param phiStates The set of all states satisfying phi.
             * @param psiStates The set of all states satisfying psi.
             * @return A bit vector that represents all states with probability 0.
             */
            template <typename T, typename RM>
            storm::storage::BitVector performProb0E(storm::models::sparse::NondeterministicModel<T, RM> const& model, storm::storage::SparseMatrix<T> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates);
            template <typename T>
            storm::storage::BitVector performProb0E(storm::storage::SparseMatrix<T> const& transitionMatrix, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices,  storm::storage::SparseMatrix<T> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates) ;
            
            /*!
             * Computes the sets of states that have probability 1 of satisfying phi until psi under all
             * possible resolutions of non-determinism in a non-deterministic model. Stated differently,
             * this means that these states have probability 1 of satisfying phi until psi even if the
             * scheduler tries to minimize this probability.
             *
             * @param model The model whose graph structure to search.
             * @param backwardTransitions The reversed transition relation of the model.
             * @param phiStates The set of all states satisfying phi.
             * @param psiStates The set of all states satisfying psi.
             * @return A bit vector that represents all states with probability 0.
             */
            template <typename T, typename RM>
            storm::storage::BitVector performProb1A(storm::models::sparse::NondeterministicModel<T, RM> const& model, storm::storage::SparseMatrix<T> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates);

            template <typename T>
            storm::storage::BitVector performProb1A( storm::storage::SparseMatrix<T> const& transitionMatrix, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, storm::storage::SparseMatrix<T> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates);
            
            template <typename T>
            std::pair<storm::storage::BitVector, storm::storage::BitVector> performProb01Min(storm::storage::SparseMatrix<T> const& transitionMatrix, std::vector<uint_fast64_t> const& nondeterministicChoiceIndices, storm::storage::SparseMatrix<T> const& backwardTransitions, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates) ;

            /*!
             * Computes the sets of states that have probability 0 or 1, respectively, of satisfying phi
             * until psi in a non-deterministic model in which all non-deterministic choices are resolved
             * such that the probability is minimized.
             *
             * @param model The model whose graph structure to search.
             * @param phiStates The set of all states satisfying phi.
             * @param psiStates The set of all states satisfying psi.
             * @return A pair of bit vectors that represent all states with probability 0 and 1, respectively.
             */
            template <typename T, typename RM>
            std::pair<storm::storage::BitVector, storm::storage::BitVector> performProb01Min(storm::models::sparse::NondeterministicModel<T, RM> const& model, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates);
            
            /*!
             * Computes the set of states for which there exists a scheduler that achieves a probability greater than
             * zero of satisfying phi until psi.
             *
             * @param model The (symbolic) model for which to compute the set of states.
             * @param transitionMatrix The transition matrix of the model as a BDD.
             * @param phiStates The BDD containing all phi states of the model.
             * @param psiStates The BDD containing all psi states of the model.
             * @return A BDD representing all such states.
             */
            template <storm::dd::DdType Type>
            storm::dd::Bdd<Type> performProbGreater0E(storm::models::symbolic::NondeterministicModel<Type> const& model, storm::dd::Bdd<Type> const& transitionMatrix, storm::dd::Bdd<Type> const& phiStates, storm::dd::Bdd<Type> const& psiStates);
            
            /*!
             * Computes the set of states for which there does not exist a scheduler that achieves a probability greater
             * than zero of satisfying phi until psi.
             *
             * @param model The (symbolic) model for which to compute the set of states.
             * @param transitionMatrix The transition matrix of the model as a BDD.
             * @param phiStates The phi states of the model.
             * @param psiStates The psi states of the model.
             * @return A BDD representing all such states.
             */
            template <storm::dd::DdType Type>
            storm::dd::Bdd<Type> performProb0A(storm::models::symbolic::NondeterministicModel<Type> const& model, storm::dd::Bdd<Type> const& transitionMatrix, storm::dd::Bdd<Type> const& phiStates, storm::dd::Bdd<Type> const& psiStates);
            
            /*!
             * Computes the set of states for which all schedulers achieve a probability greater than zero of satisfying
             * phi until psi.
             *
             * @param model The (symbolic) model for which to compute the set of states.
             * @param transitionMatrix The transition matrix of the model as a BDD.
             * @param phiStates The BDD containing all phi states of the model.
             * @param psiStates The BDD containing all psi states of the model.
             * @return A BDD representing all such states.
             */
            template <storm::dd::DdType Type>
            storm::dd::Bdd<Type> performProbGreater0A(storm::models::symbolic::NondeterministicModel<Type> const& model, storm::dd::Bdd<Type> const& transitionMatrix, storm::dd::Bdd<Type> const& phiStates, storm::dd::Bdd<Type> const& psiStates);
            
            /*!
             * Computes the set of states for which there exists a scheduler that achieves probability zero of satisfying
             * phi until psi.
             *
             * @param model The (symbolic) model for which to compute the set of states.
             * @param transitionMatrix The transition matrix of the model as a BDD.
             * @param phiStates The BDD containing all phi states of the model.
             * @param psiStates The BDD containing all psi states of the model.
             * @return A BDD representing all such states.
             */
            template <storm::dd::DdType Type>
            storm::dd::Bdd<Type> performProb0E(storm::models::symbolic::NondeterministicModel<Type> const& model, storm::dd::Bdd<Type> const& transitionMatrix, storm::dd::Bdd<Type> const& phiStates, storm::dd::Bdd<Type> const& psiStates) ;
            
            /*!
             * Computes the set of states for which all schedulers achieve probability one of satisfying phi until psi.
             *
             * @param model The (symbolic) model for which to compute the set of states.
             * @param transitionMatrix The transition matrix of the model as a BDD.
             * @param phiStates The BDD containing all phi states of the model.
             * @param psiStates The BDD containing all psi states of the model.
             * @param statesWithProbabilityGreater0A The states of the model that have a probability greater zero under
             * all schedulers.
             * @return A BDD representing all such states.
             */
            template <storm::dd::DdType Type>
            storm::dd::Bdd<Type> performProb1A(storm::models::symbolic::NondeterministicModel<Type> const& model, storm::dd::Bdd<Type> const& transitionMatrix, storm::dd::Bdd<Type> const& phiStates, storm::dd::Bdd<Type> const& psiStates, storm::dd::Bdd<Type> const& statesWithProbabilityGreater0A);
            
            /*!
             * Computes the set of states for which there exists a scheduler that achieves probability one of satisfying
             * phi until psi.
             *
             * @param model The (symbolic) model for which to compute the set of states.
             * @param transitionMatrix The transition matrix of the model as a BDD.
             * @param phiStates The BDD containing all phi states of the model.
             * @param psiStates The BDD containing all psi states of the model.
             * @param statesWithProbabilityGreater0E The states of the model that have a scheduler that achieves a value
             * greater than zero.
             * @return A BDD representing all such states.
             */
            template <storm::dd::DdType Type>
            storm::dd::Bdd<Type> performProb1E(storm::models::symbolic::NondeterministicModel<Type> const& model, storm::dd::Bdd<Type> const& transitionMatrix, storm::dd::Bdd<Type> const& phiStates, storm::dd::Bdd<Type> const& psiStates, storm::dd::Bdd<Type> const& statesWithProbabilityGreater0E) ;
            
            template <storm::dd::DdType Type>
            std::pair<storm::dd::Bdd<Type>, storm::dd::Bdd<Type>> performProb01Max(storm::models::symbolic::NondeterministicModel<Type> const& model, storm::dd::Bdd<Type> const& phiStates, storm::dd::Bdd<Type> const& psiStates) ;
            
            template <storm::dd::DdType Type>
            std::pair<storm::dd::Bdd<Type>, storm::dd::Bdd<Type>> performProb01Min(storm::models::symbolic::NondeterministicModel<Type> const& model, storm::dd::Bdd<Type> const& phiStates, storm::dd::Bdd<Type> const& psiStates) ;
            
            /*!
             * Performs a topological sort of the states of the system according to the given transitions.
             *
             * @param matrix A square matrix representing the transition relation of the system.
             * @return A vector of indices that is a topological sort of the states.
             */
            template <typename T>
            std::vector<uint_fast64_t> getTopologicalSort(storm::storage::SparseMatrix<T> const& matrix) ;
            
            /*!
             * A class needed to compare the distances for two states in the Dijkstra search.
             */
            template<typename T>
            struct DistanceCompare {
                bool operator()(std::pair<T, uint_fast64_t> const& lhs, std::pair<T, uint_fast64_t> const& rhs) const {
                    return lhs.first > rhs.first || (lhs.first == rhs.first && lhs.second > rhs.second);
                }
            };
            
            /*!
             * Performs a Dijkstra search from the given starting states to determine the most probable paths to all other states
             * by only passing through the given state set.
             *
             * @param model The model whose state space is to be searched.
             * @param transitions The transitions wrt to which to compute the most probable paths.
             * @param startingStates The starting states of the Dijkstra search.
             * @param filterStates A set of states that must not be left on any path.
             */
            template <typename T>
            std::pair<std::vector<T>, std::vector<uint_fast64_t>> performDijkstra(storm::models::sparse::Model<T> const& model,
                                                                                  storm::storage::SparseMatrix<T> const& transitions,
                                                                                  storm::storage::BitVector const& startingStates,
                                                                                  storm::storage::BitVector const* filterStates = nullptr);
            
        } // namespace graph
    } // namespace utility
} // namespace storm

#endif /* STORM_UTILITY_GRAPH_H_ */
