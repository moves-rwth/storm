#include "storm/modelchecker/helper/SingleValueModelCheckerHelper.h"
#include "storm/automata/DeterministicAutomaton.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/solver/SolveGoal.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Mdp.h"


namespace storm {

    namespace modelchecker {
        namespace helper {

            /*!
             * Helper class for LTL model checking
             * @tparam ValueType the type a value can have
             * @tparam Nondeterministic true if there is nondeterminism in the Model (MDP)
             */
            template<typename ValueType, bool Nondeterministic>
            class SparseLTLHelper: public SingleValueModelCheckerHelper<ValueType, storm::models::ModelRepresentation::Sparse> {

            public:

                /*!
                 * The type of the product automaton (DTMC or MDP) that is used during the computation.
                 */
                using productModelType = typename std::conditional<Nondeterministic, storm::models::sparse::Mdp<ValueType>, storm::models::sparse::Dtmc<ValueType>>::type;


                /*!
                 * Initializes the helper for a discrete time model (i.e. DTMC, MDP)
                 * @param the transition matrix of the model
                 * @param the number of states of the model
                 */
                SparseLTLHelper(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::size_t numberOfSates);


                /*!
                 * Computes the LTL probabilities
                 * @param the LTL formula
                 * @param the atomic propositions and satisfaction sets
                 * @return a value for each state
                 */
                std::vector<ValueType> computeLTLProbabilities(Environment const &env, storm::logic::Formula const& formula, std::map<std::string, storm::storage::BitVector>& apSatSets);

                /*!
                 * Computes the (maximizing) probabilities for the constructed DA product
                 * @param the DA to build the product with
                 * @param the atomic propositions and satisfaction sets
                 * @param a flag indicating whether qualitative model checking is performed
                 * @return a value for each state
                 */
                std::vector<ValueType> computeDAProductProbabilities(Environment const& env, storm::automata::DeterministicAutomaton const& da, std::map<std::string, storm::storage::BitVector>& apSatSets);


            private:

                /*!
                 * Compute a set S of states that admit a probability 1 strategy of satisfying the given acceptance condition (in DNF).
                 * More precisely, let
                 *   accEC be the set of states that are contained in end components that satisfy the acceptance condition
                 *  and let
                 *   P1acc be the set of states that satisfy Pmax=1[ F accEC ].
                 * This function then computes a set that contains accEC and is contained by P1acc.
                 * However, if the acceptance condition consists of 'true', the whole state space can be returned.
                 * @param the acceptance condition (in DNF)
                 * @param the transition matrix of the model
                 * @param the reversed transition relation
                 */
                static storm::storage::BitVector computeAcceptingECs(automata::AcceptanceCondition const& acceptance, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions);

                /**
                * Compute a set S of states that are contained in BSCCs that satisfy the given acceptance conditon.
                * @tparam the acceptance condition
                * @tparam the transition matrix of the model
                */
                static storm::storage::BitVector computeAcceptingBCCs(automata::AcceptanceCondition const& acceptance, storm::storage::SparseMatrix<ValueType> const& transitionMatrix);


                storm::storage::SparseMatrix<ValueType> const& _transitionMatrix;
                std::size_t _numberOfStates;

            };
        }
    }
}