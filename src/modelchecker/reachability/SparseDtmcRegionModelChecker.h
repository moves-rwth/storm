#ifndef STORM_MODELCHECKER_REACHABILITY_SPARSEDTMCREGIONMODELCHECKER_H_
#define STORM_MODELCHECKER_REACHABILITY_SPARSEDTMCREGIONMODELCHECKER_H_

#include <type_traits>

#include "src/storage/sparse/StateType.h"
#include "src/models/sparse/Dtmc.h"
#include "src/models/sparse/Mdp.h"
#include "src/utility/constants.h"
#include "utility/regions.h"
#include "src/solver/Smt2SmtSolver.h"
#include "SparseDtmcEliminationModelChecker.h"

namespace storm {
    namespace modelchecker {
        
        template<typename ParametricType, typename ConstantType>
        class SparseDtmcRegionModelChecker {
        public:
            
            //The type of variables and bounds depends on the template arguments
            typedef typename std::conditional<(std::is_same<ParametricType,storm::RationalFunction>::value), storm::Variable,std::nullptr_t>::type VariableType;
            typedef typename std::conditional<(std::is_same<ParametricType,storm::RationalFunction>::value), storm::RationalFunction::CoeffType,std::nullptr_t>::type BoundType;
            
            /*!
             * The possible results for a single region
             */
            enum class RegionCheckResult { 
                UNKNOWN, /*!< the result is unknown */
                ALLSAT, /*!< the formula is satisfied for all parameters in the given region */
                ALLUNSAT, /*!< the formula is violated for all parameters in the given region */
                INCONCLUSIVE /*!< the formula is satisfied for some parameters but also violated for others */
            };
            
            class ParameterRegion{
            public:

                ParameterRegion(std::map<VariableType, BoundType> lowerBounds, std::map<VariableType, BoundType> upperBounds);
                
                void setCheckResult(RegionCheckResult checkResult) {
                    this->checkResult = checkResult;
                }

                RegionCheckResult getCheckResult() const {
                    return checkResult;
                }
                
                
                std::set<VariableType> getVariables() const;
                BoundType const& getLowerBound(VariableType const& variable) const;
                BoundType const& getUpperBound(VariableType const& variable) const;
                
                /*
                 * Returns a vector of all possible combinations of lower and upper bounds of the given variables.
                 * The first entry of the returned vector will map every variable to its lower bound
                 * The second entry will map every variable to its lower bound, except the first one (i.e. *consVariables.begin())
                 * ...
                 * The last entry will map every variable to its upper bound
                 */
                std::vector<std::map<VariableType, BoundType>> getVerticesOfRegion(std::set<VariableType> const& consideredVariables) const;
                
                std::string getCheckResultAsString() const;
                std::string getRegionAsString() const;
                
            private:
                
                std::map<VariableType, BoundType> const lowerBounds;
                std::map<VariableType, BoundType> const upperBounds;
                RegionCheckResult checkResult;
                
            };
            
            explicit SparseDtmcRegionModelChecker(storm::models::sparse::Dtmc<ParametricType> const& model);

            /*!
             * Checks if the given formula can be handled by this
             * @param formula the formula to be checked
             */
            bool canHandle(storm::logic::Formula const& formula) const;
            
            /*!
             * Specifies the considered formula.
             * A few preprocessing steps are performed.
             * If another formula has been specified before, all 'context' regarding the old formula is lost.
             * 
             * @param formula the formula to be considered.
             */
            void specifyFormula(storm::logic::Formula const& formula);

            /*!
             * Checks whether the given formula holds for all possible parameters that satisfy the given parameter regions
             * ParameterRegions should contain all parameters.
             */
            bool checkRegion(storm::logic::Formula const& formula, std::vector<ParameterRegion> parameterRegions);
            
            /*!
             * Prints statistical information (mostly running times) to the given stream.
             */
            void printStatisticsToStream(std::ostream& outstream);
        private:
            
            typedef typename storm::modelchecker::SparseDtmcEliminationModelChecker<ParametricType>::FlexibleSparseMatrix FlexibleMatrix;
            
            /*!
             * Represents the current state of this
             */
           // enum class RegionCheckerState{
         //       NOFORMULA, /*!< there is no formula */
         //       INITIALIZED, /*!< a formula has been specified. Ready to get regions*/
        //    };
            
   #ifdef STORM_HAVE_CARL
                /*!
                 * Instantiates the matrix, i.e., evaluate the occurring functions according to the given substitutions of the variables.
                 * One row of the given flexible matrix will be one rowgroup in the returned matrix, consisting of one row for every substitution.
                 * The returned matrix can be seen as the transition matrix of an MDP with the action labeling given by the returned vector of sets.
                 * Only the rows selected by the given filter are considered. (filter should have size==matrix.getNumberOfRows())
                 * An exception is thrown if there is a transition from a selected state to an unselected state
                 * If one step probabilities are given, a new state is added which can be considered as target state.
                 * The "missing" probability can be redirected to a sink state
                 * By convention, the target state will have index filter.getNumberOfSetBits() and the sink state will be the state with the highest index (so right after the target state)
                 * 
                 * @param matrix the considered flexible matrix
                 * @param substitutions A list of mappings, each assigning a constant value to every variable
                 * @param filter selects the rows of this flexibleMatrix, that will be considered
                 * @param addSinkState adds a state with a self loop to which the "missing" probability will lead
                 * @param oneStepProbabilities if given, a new state is added to which there are transitions for all non-zero entries in this vector
                 * @param addSelfLoops if set, zero valued selfloops will be added in every row
                 * 
                 * @return A matrix with constant (double) entries and a choice labeling
                 */
                std::pair<storm::storage::SparseMatrix<double>,std::vector<boost::container::flat_set<uint_fast64_t>>> instantiateFlexibleMatrix(FlexibleMatrix const& matrix, std::vector<std::map<storm::Variable, storm::RationalFunction::CoeffType>> const& substitutions, storm::storage::BitVector const& filter, bool addSinkState=true, std::vector<ParametricType> const& oneStepProbabilities=std::vector<ParametricType>(), bool addSelfLoops=true) const;
                //todo add const keyword
            
            //eliminates some of the states according to different strategies.
            void eliminateStates(storm::storage::BitVector& subsystem, FlexibleMatrix& flexibleMatrix, std::vector<ParametricType>& oneStepProbabilities, FlexibleMatrix& flexibleBackwardTransitions, storm::storage::BitVector const& initialStates, storm::storage::SparseMatrix<ParametricType> const& forwardTransitions, boost::optional<std::vector<std::size_t>> const& statePriorities = {});
            
            void formulateModelWithSMT(storm::solver::Smt2SmtSolver& solver, std::vector<storm::RationalFunction::PolyType>& stateProbVars, storm::storage::BitVector const& subsystem, FlexibleMatrix const& flexibleMatrix, std::vector<storm::RationalFunction> const& oneStepProbabilities);
            
            void restrictProbabilityVariables(storm::solver::Smt2SmtSolver& solver, std::vector<storm::RationalFunction::PolyType> const& stateProbVars, storm::storage::BitVector const& subsystem, FlexibleMatrix const& flexibleMatrix, std::vector<storm::RationalFunction> const& oneStepProbabilities, ParameterRegion const& region, storm::logic::ComparisonType const& compTypeOfProperty);
#endif         
            template <typename ValueType>
            bool valueIsInBoundOfFormula(ValueType value);
            
            //eliminates all states for which the outgoing transitions are constant.
            void eliminateStatesConstSucc(
                    storm::storage::BitVector& subsys,
                    FlexibleMatrix& flexTransitions,
                     FlexibleMatrix& flexBackwardTransitions,
                    std::vector<ParametricType>& oneStepProbs,
                    storm::storage::sparse::state_type const& initState
            );
            
            //Computes the reachability probability function by performing state elimination
            ParametricType computeReachProbFunction(
                storm::storage::BitVector const& subsys,
                FlexibleMatrix const& flexTransitions,
                FlexibleMatrix const& flexBackwardTransitions,
                storm::storage::SparseMatrix<ParametricType> const& spTransitions,
                storm::storage::SparseMatrix<ParametricType> const& spBackwardTransitions,
                std::vector<ParametricType> const& oneStepProbs,
                storm::storage::sparse::state_type const& initState
            );
            
            
            
            
            
            // The model this model checker is supposed to analyze.
            storm::models::sparse::Dtmc<ParametricType> const& model;

            //classes that provide auxilliary functions
            // Instance of an elimination model checker to access its functions
            storm::modelchecker::SparseDtmcEliminationModelChecker<ParametricType> eliminationModelChecker;
            // comparators that can be used to compare constants.
            storm::utility::ConstantsComparator<ParametricType> parametricTypeComparator;
            storm::utility::ConstantsComparator<ConstantType> constantTypeComparator;
            

            //the following members depend on the currently specified formula:
            
            //the currently specified formula
            std::unique_ptr<storm::logic::ProbabilityOperatorFormula> probabilityOperatorFormula;
            
            // The ingredients of the model where constant transitions have been eliminated as much as possible
            // the probability matrix
            FlexibleMatrix flexibleTransitions;
            storm::storage::SparseMatrix<ParametricType>  sparseTransitions;
            //the corresponding backward transitions
            FlexibleMatrix flexibleBackwardTransitions;
            storm::storage::SparseMatrix<ParametricType>  sparseBackwardTransitions;
            // the propabilities to go to a state with probability 1 in one step (belongs to flexibleTransitions)
            std::vector<ParametricType> oneStepProbabilities;
            // the initial state
            storm::storage::sparse::state_type initialState;
            // the set of states that have not been eliminated
            storm::storage::BitVector subsystem;
            
            // The  function for the reachability probability in the initial state 
            ParametricType reachProbFunction;
            
            
            // runtimes and other information for statistics. 
            uint_fast64_t numOfCheckedRegions;
            std::chrono::high_resolution_clock::duration timePreprocessing;
            std::chrono::high_resolution_clock::duration timeInitialStateElimination;
        };
        
    } // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_REACHABILITY_SPARSEDTMCREGIONMODELCHECKER_H_ */
