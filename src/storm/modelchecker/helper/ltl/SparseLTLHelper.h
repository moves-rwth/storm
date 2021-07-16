#include "storm/modelchecker/helper/SingleValueModelCheckerHelper.h"
#include "storm/modelchecker/results/ExplicitQualitativeCheckResult.h"
#include "storm/automata/DeterministicAutomaton.h"
#include "storm/storage/SparseMatrix.h"
#include "storm/solver/SolveGoal.h"
#include "storm/models/sparse/Dtmc.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/logic/ExtractMaximalStateFormulasVisitor.h"
#include "storm/transformer/DAProductBuilder.h"


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
                 * @pre before calling this, a computation call should have been performed during which scheduler production was enabled.
                 * @param TODO
                 * @return a new scheduler containing optimal choices for each state that yield the long run average values of the most recent call.
                 */
                storm::storage::Scheduler<ValueType> extractScheduler(storm::models::sparse::Model<ValueType> const& model);

                /*!
                 * todo computes Sat sets of AP
                 * @param
                 * @param
                 * @return
                 */
                static std::map<std::string, storm::storage::BitVector> computeApSets(std::map<std::string, std::shared_ptr<storm::logic::Formula const>> const& extracted, std::function<std::unique_ptr<CheckResult>(std::shared_ptr<storm::logic::Formula const> const& formula)> formulaChecker);

                /*!
                 * Computes the (maximizing) probabilities for the constructed DA product
                 * @param the DA to build the product with
                 * @param the atomic propositions and satisfaction sets
                 * @param a flag indicating whether qualitative model checking is performed
                 * @return a value for each state
                 */
                std::vector<ValueType> computeDAProductProbabilities(Environment const& env, storm::automata::DeterministicAutomaton const& da, std::map<std::string, storm::storage::BitVector>& apSatSets);

                /*!
                 * Computes the LTL probabilities
                 * @param the LTL formula
                 * @param the atomic propositions and satisfaction sets
                 * @return a value for each state
                 */
                std::vector<ValueType> computeLTLProbabilities(Environment const &env, storm::logic::Formula const& formula, std::map<std::string, storm::storage::BitVector>& apSatSets);

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
                storm::storage::BitVector computeAcceptingECs(automata::AcceptanceCondition const& acceptance, storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, typename transformer::DAProduct<productModelType>::ptr product);

                /**
                * Compute a set S of states that are contained in BSCCs that satisfy the given acceptance conditon.
                * @tparam the acceptance condition
                * @tparam the transition matrix of the model
                */
                storm::storage::BitVector computeAcceptingBCCs(automata::AcceptanceCondition const& acceptance, storm::storage::SparseMatrix<ValueType> const& transitionMatrix);


                storm::storage::SparseMatrix<ValueType> const& _transitionMatrix;
                std::size_t _numberOfStates;  //TODO just use _transitionMatrix.getRowGroupCount instead?

                // scheduler
                bool _randomScheduler = false;
                boost::optional<std::map <std::tuple<uint_fast64_t, uint_fast64_t, uint_fast64_t>, storm::storage::SchedulerChoice<ValueType>>> _productChoices;   // <s, q, len(_infSets)> --->  ReachChoice   and    <s, q, InfSet> --->  MecChoice

                boost::optional<std::vector<storm::storage::BitVector>> _infSets; // Save the InfSets of the Acceptance condition.
                boost::optional<std::vector<boost::optional<std::set<uint_fast64_t>>>> _accInfSets; // Save for each product state (which is assigned to an acceptingMEC), the infSets that need to be visited inf often to satisfy the acceptance condition. Remaining states belonging to no accepting EC, are assigned  len(_infSets) (REACH scheduler)
                // Memory structure
                boost::optional<std::vector<std::vector<storm::storage::BitVector>>> _memoryTransitions;  // The BitVector contains the model states that lead from startState <q, mec, infSet> to <q', mec', infSet'>. This is deterministic, because each state <s, q> is assigned to a unique MEC (scheduler).
                boost::optional<std::vector<uint_fast64_t>> _memoryInitialStates; // Save for each relevant state (initial or all) s its unique initial memory state (which memory state is reached from the initial state after reading s)

            };
        }
    }
}