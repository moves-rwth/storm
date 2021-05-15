#include "storm/modelchecker/helper/SingleValueModelCheckerHelper.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/solver/SolveGoal.h"


namespace storm {

    namespace automata {
        // fwd
        class DeterministicAutomaton;
    }

    namespace modelchecker {
        namespace helper {

            /*!
             * Helper class for todo...
             * @tparam ValueType the type a value can have
             * @tparam Nondeterministic true if there is nondeterminism in the Model (MDP)
             */
            template<typename ValueType, typename Model,  bool Nondeterministic> // todo remove Model
            class SparseLTLHelper: public SingleValueModelCheckerHelper<ValueType, storm::models::ModelRepresentation::Sparse> {

            public:
                /*!
                 * Initializes the helper for a discrete time (i.e. DTMC, MDP)
                 */
                SparseLTLHelper(Model const& model, storm::storage::SparseMatrix<ValueType> const& transitionMatrix);


                /*!
                 * todo
                 * @return
                 */
                std::vector<ValueType> computeDAProductProbabilities(Environment const& env, storm::solver::SolveGoal<ValueType>&& goal, storm::automata::DeterministicAutomaton const& da, std::map<std::string, storm::storage::BitVector>& apSatSets, bool qualitative);


                /*!
                 * Computes the ltl probabilities ...todo
                 * @return a value for each state
                 */
                std::vector<ValueType> computeLTLProbabilities(Environment const &env, storm::solver::SolveGoal<ValueType>&& goal, storm::logic::Formula const& f, std::map<std::string, storm::storage::BitVector>& apSatSets);  //todo was brauchen wir hier aps und ..?


            private:
                storm::storage::SparseMatrix<ValueType> const& _transitionMatrix;
                Model const& _model; // todo remove ?

            };
        }
    }
}