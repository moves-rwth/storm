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
             * Helper class for todo...
             * @tparam ValueType the type a value can have
             * @tparam Nondeterministic true if there is nondeterminism in the Model (MDP)
             */
            template<typename ValueType, bool Nondeterministic>
            class SparseLTLHelper: public SingleValueModelCheckerHelper<ValueType, storm::models::ModelRepresentation::Sparse> {

            public:

                /*!
                 * The type of the product automaton model // todo
                 */
                using productModelType = typename std::conditional<Nondeterministic, storm::models::sparse::Mdp<ValueType>, storm::models::sparse::Dtmc<ValueType>>::type;


                /*!
                 * Initializes the helper for a discrete time (i.e. DTMC, MDP)
                 */
                SparseLTLHelper(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::size_t numberOfSates);


                /*!
                 * todo
                 * Computes maximizing(!) probabilties for DA product with MDP
                 * @return
                 */
                std::vector<ValueType> computeDAProductProbabilities(Environment const& env, storm::automata::DeterministicAutomaton const& da, std::map<std::string, storm::storage::BitVector>& apSatSets, bool qualitative);


                /*!
                 * Computes the ltl probabilities ...todo
                 * @return a value for each state
                 */
                std::vector<ValueType> computeLTLProbabilities(Environment const &env, storm::logic::Formula const& f, std::map<std::string, storm::storage::BitVector>& apSatSets);  //todo was brauchen wir hier aps und ..?


            private:

                /*! todo only relevant for MDP  -  enable_if_t ?
                 * Compute a set S of states that admit a probability 1 strategy of satisfying the given acceptance conditon.
                 * More precisely, let
                 *   accEC be the set of states that are contained in end components that satisfy the acceptance condition
                 *  and let
                 *   P1acc be the set of states that satisfy Pmax=1[ F accEC ].
                 * This function then computes a set that contains accEC and is contained by P1acc.
                 * However, if the acceptance condition consists of 'true', the whole state space can be returned.
                 */
                static storm::storage::BitVector computeSurelyAcceptingPmaxStates(automata::AcceptanceCondition const& acceptance, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions);

                //todo only for dtmc, different to mdp: no backward tm
                static storm::storage::BitVector computeAcceptingComponentStates(automata::AcceptanceCondition const& acceptance, storm::storage::SparseMatrix<ValueType> const& transitionMatrix);


                storm::storage::SparseMatrix<ValueType> const& _transitionMatrix;
                std::size_t _numberOfStates;

            };
        }
    }
}