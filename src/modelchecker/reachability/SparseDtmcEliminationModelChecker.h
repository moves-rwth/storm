#ifndef STORM_MODELCHECKER_REACHABILITY_SPARSEDTMCELIMINATIONMODELCHECKER_H_
#define STORM_MODELCHECKER_REACHABILITY_SPARSEDTMCELIMINATIONMODELCHECKER_H_

#include "src/storage/sparse/StateType.h"
#include "src/models/sparse/Dtmc.h"
#include "src/modelchecker/AbstractModelChecker.h"
#include "src/utility/constants.h"
#include "src/solver/SmtSolver.h"
#include "src/solver/Smt2SmtSolver.h"

namespace storm {
    namespace modelchecker {
        
        template<typename ValueType>
        class SparseDtmcEliminationModelChecker : public AbstractModelChecker {
        public:
            explicit SparseDtmcEliminationModelChecker(storm::models::sparse::Dtmc<ValueType> const& model);
            
            // The implemented methods of the AbstractModelChecker interface.
            virtual bool canHandle(storm::logic::Formula const& formula) const override;
            virtual std::unique_ptr<CheckResult> computeUntilProbabilities(storm::logic::UntilFormula const& pathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeReachabilityRewards(storm::logic::ReachabilityRewardFormula const& rewardPathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> computeConditionalProbabilities(storm::logic::ConditionalPathFormula const& pathFormula, bool qualitative = false, boost::optional<storm::logic::OptimalityType> const& optimalityType = boost::optional<storm::logic::OptimalityType>()) override;
            virtual std::unique_ptr<CheckResult> checkBooleanLiteralFormula(storm::logic::BooleanLiteralFormula const& stateFormula) override;
            virtual std::unique_ptr<CheckResult> checkAtomicLabelFormula(storm::logic::AtomicLabelFormula const& stateFormula) override;

#ifdef STORM_HAVE_CARL
            struct ParameterRegion{
                storm::Variable variable;
                storm::RationalFunction::CoeffType lowerBound;
                storm::RationalFunction::CoeffType upperBound;
            };

            
            /*!
             * Checks whether the given formula holds for all possible parameters that satisfy the given parameter regions
             * ParameterRegions should contain all parameters (not mentioned parameters are assumed to be arbitrary reals)
             */
            bool checkRegion(storm::logic::Formula const& formula, std::vector<ParameterRegion> parameterRegions);
#endif            
            
            
        private:
            class FlexibleSparseMatrix {
            public:
                typedef uint_fast64_t index_type;
                typedef ValueType value_type;
                typedef std::vector<storm::storage::MatrixEntry<index_type, value_type>> row_type;
                typedef typename row_type::iterator iterator;
                typedef typename row_type::const_iterator const_iterator;
                
                FlexibleSparseMatrix() = default;
                FlexibleSparseMatrix(index_type rows);
                
                void reserveInRow(index_type row, index_type numberOfElements);
                
                row_type& getRow(index_type);
                row_type const& getRow(index_type) const;
                
                index_type getNumberOfRows() const;
                
                void print() const;
                
                /*!
                 * Checks whether the given state has a self-loop with an arbitrary probability in the given probability matrix.
                 *
                 * @param state The state for which to check whether it possesses a self-loop.
                 * @param matrix The matrix in which to look for the loop.
                 * @return True iff the given state has a self-loop with an arbitrary probability in the given probability matrix.
                 */
                bool hasSelfLoop(storm::storage::sparse::state_type state) const;
                
#ifdef STORM_HAVE_CARL
                /*!
                 * Instantiates the matrix, i.e., evaluate the occurring functions according to the given substitution of the variables.
                 * Only the rows selected by the given filter are considered. (filter should have size==this->getNumberOfRows())
                 * An exception is thrown if there is a transition from a selected state to an unselected state
                 * If one step probabilities are given, a new state is added which can be considered as target state.
                 * The "missing" probability can be redirected to a sink state
                 * By convention, the target state will have index filter.getNumberOfSetBits() and the sink state will be the state with the highest index (so right after the target state)
                 * 
                 * 
                 * @param substitutions A mapping that assigns a constant value to every variable
                 * @param filter selects the rows of this flexibleMatrix, that will be considered
                 * @param addSinkState adds a state with a self loop to which the "missing" probability will lead
                 * @param oneStepProbabilities if given, a new state is added to which there are transitions for all non-zero entries in this vector
                 * @param addSelfLoops if set, zero valued selfloops will be added in every row
                 * 
                 * @return A matrix with constant (double) entries
                 */
                storm::storage::SparseMatrix<double> instantiateAsDouble(std::map<storm::Variable, storm::RationalFunction::CoeffType> const& substitutions, storm::storage::BitVector const& filter, bool addSinkState=true, std::vector<ValueType> const& oneStepProbabilities=std::vector<ValueType>(), bool addSelfLoops=true) const;
                //todo add const keyword
#endif         
                
            private:
                std::vector<row_type> data;
            };
            
            ValueType computeReachabilityValue(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, std::vector<ValueType>& oneStepProbabilities, storm::storage::SparseMatrix<ValueType> const& backwardTransitions, storm::storage::BitVector const& initialStates, storm::storage::BitVector const& phiStates, storm::storage::BitVector const& psiStates, boost::optional<std::vector<ValueType>>& stateRewards, boost::optional<std::vector<std::size_t>> const& statePriorities = {});
            
            uint_fast64_t treatScc(FlexibleSparseMatrix& matrix, std::vector<ValueType>& oneStepProbabilities, storm::storage::BitVector const& entryStates, storm::storage::BitVector const& scc, storm::storage::SparseMatrix<ValueType> const& forwardTransitions, FlexibleSparseMatrix& backwardTransitions, bool eliminateEntryStates, uint_fast64_t level, uint_fast64_t maximalSccSize, std::vector<storm::storage::sparse::state_type>& entryStateQueue, boost::optional<std::vector<ValueType>>& stateRewards, boost::optional<std::vector<std::size_t>> const& statePriorities = {});
            
            static FlexibleSparseMatrix getFlexibleSparseMatrix(storm::storage::SparseMatrix<ValueType> const& matrix, bool setAllValuesToOne = false);
            
            void eliminateState(FlexibleSparseMatrix& matrix, std::vector<ValueType>& oneStepProbabilities, uint_fast64_t state, FlexibleSparseMatrix& backwardTransitions, boost::optional<std::vector<ValueType>>& stateRewards, bool removeForwardTransitions = true, bool constrained = false, storm::storage::BitVector const& predecessorConstraint = storm::storage::BitVector());
            
            std::vector<std::size_t> getStatePriorities(storm::storage::SparseMatrix<ValueType> const& transitionMatrix, storm::storage::SparseMatrix<ValueType> const& transitionMatrixTransposed, storm::storage::BitVector const& initialStates, std::vector<ValueType> const& oneStepProbabilities);
            
            //eliminates some of the states according to different strategies.
            void eliminateStates(storm::storage::BitVector& subsystem, FlexibleSparseMatrix& flexibleMatrix, std::vector<ValueType>& oneStepProbabilities, FlexibleSparseMatrix& flexibleBackwardTransitions, storm::storage::BitVector const& initialStates);
            
            void formulateModelWithSMT(storm::solver::Smt2SmtSolver& solver, std::vector<storm::RationalFunction::PolyType>& stateProbVars, storm::storage::BitVector const& subsystem, FlexibleSparseMatrix const& flexibleMatrix, std::vector<storm::RationalFunction> const& oneStepProbabilities);
            
            void restrictProbabilityVariables(storm::solver::Smt2SmtSolver& solver, std::vector<storm::RationalFunction::PolyType> const& stateProbVars, storm::storage::BitVector const& subsystem, FlexibleSparseMatrix const& flexibleMatrix, std::vector<storm::RationalFunction> const& oneStepProbabilities, std::vector<ParameterRegion> const& regions, storm::logic::ComparisonType const& compTypeOfProperty);
            
            // The model this model checker is supposed to analyze.
            storm::models::sparse::Dtmc<ValueType> const& model;
            
            // A comparator that can be used to compare constants.
            storm::utility::ConstantsComparator<ValueType> comparator;
        };
        
    } // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_REACHABILITY_SPARSEDTMCELIMINATIONMODELCHECKER_H_ */
