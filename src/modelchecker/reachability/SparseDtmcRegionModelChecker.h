#ifndef STORM_MODELCHECKER_REACHABILITY_SPARSEDTMCREGIONMODELCHECKER_H_
#define STORM_MODELCHECKER_REACHABILITY_SPARSEDTMCREGIONMODELCHECKER_H_

//TODO remove useless includes

#include "src/storage/sparse/StateType.h"
#include "src/models/sparse/Dtmc.h"
#include "src/models/sparse/Mdp.h"
//#include "src/modelchecker/AbstractModelChecker.h"
#include "src/utility/constants.h"
//#include "src/solver/SmtSolver.h"
#include "src/solver/Smt2SmtSolver.h"
#include "SparseDtmcEliminationModelChecker.h"

namespace storm {
    namespace modelchecker {
        
        template<typename ParametricType, typename ConstantType>
        class SparseDtmcRegionModelChecker {
        public:
            explicit SparseDtmcRegionModelChecker(storm::models::sparse::Dtmc<ParametricType> const& model);
            
            virtual bool canHandle(storm::logic::Formula const& formula) const;

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
            typedef typename storm::modelchecker::SparseDtmcEliminationModelChecker<ParametricType>::FlexibleSparseMatrix FlexibleMatrix;
            
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
            
            void restrictProbabilityVariables(storm::solver::Smt2SmtSolver& solver, std::vector<storm::RationalFunction::PolyType> const& stateProbVars, storm::storage::BitVector const& subsystem, FlexibleMatrix const& flexibleMatrix, std::vector<storm::RationalFunction> const& oneStepProbabilities, std::vector<ParameterRegion> const& regions, storm::logic::ComparisonType const& compTypeOfProperty);
#endif         
                
            
            
            // The model this model checker is supposed to analyze.
            storm::models::sparse::Dtmc<ParametricType> const& model;
            
            // Instance of an elimination model checker to access its functions
            storm::modelchecker::SparseDtmcEliminationModelChecker<ParametricType> eliminationModelChecker;
            
            // A comparator that can be used to compare constants.
            storm::utility::ConstantsComparator<ParametricType> comparator;
            
        };
        
    } // namespace modelchecker
} // namespace storm

#endif /* STORM_MODELCHECKER_REACHABILITY_SPARSEDTMCREGIONMODELCHECKER_H_ */
