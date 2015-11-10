/* 
 * File:   ApproximationModel.h
 * Author: tim
 *
 * Created on August 7, 2015, 9:29 AM
 */

#ifndef STORM_MODELCHECKER_REGION_APPROXIMATIONMODEL_H
#define	STORM_MODELCHECKER_REGION_APPROXIMATIONMODEL_H

#include <unordered_map>
#include <memory>
#include <boost/functional/hash.hpp>

#include "src/utility/region.h"
#include "src/modelchecker/region/ParameterRegion.h"
#include "src/modelchecker/results/ExplicitQuantitativeCheckResult.h"
#include "src/logic/Formulas.h"
#include "src/models/sparse/Model.h"
#include "src/storage/SparseMatrix.h"
#include "src/solver/SolveGoal.h"
#include "src/modelchecker/region/RegionBoundary.h"

namespace storm {
    namespace modelchecker {
        namespace region {
            template<typename ParametricSparseModelType, typename ConstantType>
            class ApproximationModel{
            public:
                typedef typename ParametricSparseModelType::ValueType ParametricType;
                typedef typename storm::utility::region::VariableType<ParametricType> VariableType;
                typedef typename storm::utility::region::CoefficientType<ParametricType> CoefficientType;

                /*!
                 * Creates an Approximation model
                 * The given model should have the state-labels
                 * * "target", labeled on states with reachability probability one (reachability reward zero)
                 * * "sink", labeled on states from which a target state can not be reached.
                 * The (single) initial state should be disjoint from these states. (otherwise the result would be independent of the parameters, anyway)
                 * @note This will not check whether approximation is applicable
                 */
                ApproximationModel(ParametricSparseModelType const& parametricModel, std::shared_ptr<storm::logic::OperatorFormula> formula);
                virtual ~ApproximationModel();

                /*!
                 * Instantiates the approximation model w.r.t. the given region.
                 * Then computes and returns the approximated reachability probabilities or reward values for every state.
                 * If computeLowerBounds is true, the computed values will be a lower bound for the actual values. Otherwise, we get upper bounds,
                 */
                std::vector<ConstantType> computeValues(ParameterRegion<ParametricType> const& region, bool computeLowerBounds);

                /*!
                 * Instantiates the approximation model w.r.t. the given region.
                 * Then computes and returns the approximated reachability probabilities or reward value for the initial state.
                 * If computeLowerBounds is true, the computed value will be a lower bound for the actual value. Otherwise, we get an upper bound.
                 */
                ConstantType computeInitialStateValue(ParameterRegion<ParametricType> const& region, bool computeLowerBounds);

            private:

                typedef std::pair<ParametricType, std::size_t> FunctionSubstitution;
                typedef std::vector<storm::storage::sparse::state_type> Policy;
                class FuncSubHash{
                    public:
                        std::size_t operator()(FunctionSubstitution const& fs) const {
                       //     return fs.getHash();
                            std::size_t seed = 0;
                            boost::hash_combine(seed, fs.first);
                            boost::hash_combine(seed, fs.second);
                            return seed;
                            
                        }
                };
                typedef typename std::unordered_map<FunctionSubstitution, ConstantType, FuncSubHash>::value_type FunctionEntry;

                void initializeProbabilities(ParametricSparseModelType const& parametricModel, std::vector<std::size_t> const& newIndices);
                void initializeRewards(ParametricSparseModelType const& parametricModel, std::vector<std::size_t> const& newIndices);
                void initializePlayer1Matrix(ParametricSparseModelType const& parametricModel);
                void instantiate(ParameterRegion<ParametricType> const& region, bool computeLowerBounds);
                void invokeSolver(bool computeLowerBounds, Policy& policy);

                //A flag that denotes whether we compute probabilities or rewards
                bool computeRewards;
                
                //Some designated states in the original model
                storm::storage::BitVector targetStates, maybeStates;
                
                struct SolverData{
                    //The results from the previous instantiation. Serve as first guess for the next call.
                    std::vector<ConstantType> result; //Note: result.size==maybeStates.numberOfSetBits
                    Policy lastMinimizingPolicy, lastMaximizingPolicy;
                    std::size_t initialStateIndex; //The index which represents the result for the initial state in the result vector
                    //Player 1 represents the nondeterminism of the given mdp (so, this is irrelevant if we approximate values of a DTMC)
                    storm::solver::SolveGoal player1Goal;
                    storm::storage::SparseMatrix<storm::storage::sparse::state_type> player1Matrix;
                    Policy lastPlayer1Policy;
                } solverData;
                
                
                /* The data required for the equation system, i.e., a matrix and a vector.
                 * 
                 * We use a map to store one (unique) entry for every occurring pair of a non-constant function and substitution.
                 * The map points to some ConstantType value which serves as placeholder. 
                 * When instantiating the model, the evaluated result of every function + substitution is stored in the corresponding placeholder.
                 * For rewards, however, we might need a minimal and a maximal value which is why there are two paceholders.
                 * There is an assignment that connects every non-constant matrix (or: vector) entry
                 * with a pointer to the value that, on instantiation, needs to be written in that entry.
                 * 
                 * This way, it is avoided that the same function is evaluated multiple times.
                 */
                struct FuncSubData{
                    // the occurring (function,substitution)-pairs together with the corresponding placeholders for the result
                    std::unordered_map<FunctionSubstitution, ConstantType, FuncSubHash> functions; 
                    //Vector has one entry for every required substitution (=replacement of parameters with lower/upper bounds of region)
                    std::vector<std::map<VariableType, RegionBoundary>> substitutions;
                } funcSubData;
                struct MatrixData {
                    storm::storage::SparseMatrix<ConstantType> matrix; //The matrix itself.
                    std::vector<std::pair<typename storm::storage::SparseMatrix<ConstantType>::iterator, ConstantType&>> assignment; // Connection of matrix entries with placeholders
                    std::vector<std::size_t> rowSubstitutions; //used to obtain which row corresponds to which substitution (used to retrieve information from a scheduler)
                } matrixData;
                struct VectorData {
                    std::vector<ConstantType> vector; //The vector itself.
                    std::vector<std::pair<typename std::vector<ConstantType>::iterator, ConstantType&>> assignment; // Connection of vector entries with placeholders
                } vectorData;
                
            };
        } //namespace region
    }
}


#endif	/* STORM_MODELCHECKER_REGION_APPROXIMATIONMODEL_H */

