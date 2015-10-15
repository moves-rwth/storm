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
                //This enum helps to store how a parameter will be substituted.
                enum class TypeOfBound { 
                    LOWER,
                    UPPER,
                    CHOSEOPTIMAL
                }; 

                //A class that represents a function and how it should be substituted (i.e. which variables should be replaced with lower and which with upper bounds of the region)
                //The substitution is given as an index in funcSubData.substitutions (allowing to instantiate the substitutions more easily).
                class FunctionSubstitution {
                public:
                    FunctionSubstitution(ParametricType const& fun, std::size_t const& sub) : hash(computeHash(fun,sub)), function(fun), substitution(sub) {
                        //intentionally left empty
                    }

                    FunctionSubstitution(ParametricType&& fun, std::size_t&& sub) : hash(computeHash(fun,sub)), function(std::move(fun)), substitution(std::move(sub)) {
                        //intentionally left empty
                    }

                    FunctionSubstitution(FunctionSubstitution const& other) : hash(other.hash), function(other.function), substitution(other.substitution){
                        //intentionally left empty
                    }

                    FunctionSubstitution(FunctionSubstitution&& other) : hash(std::move(other.hash)), function(std::move(other.function)), substitution(std::move(other.substitution)){
                        //intentionally left empty
                    }

                    FunctionSubstitution() = default;

                    ~FunctionSubstitution() = default;

                    bool operator==(FunctionSubstitution const& other) const {
                        return this->hash==other.hash && this->substitution==other.substitution && this->function==other.function;
                    }

                    ParametricType const& getFunction() const{
                        return this->function;
                    }

                    std::size_t const& getSubstitution() const{
                        return this->substitution;
                    }

                    std::size_t const& getHash() const{
                        return this->hash;
                    }

                private:

                    static std::size_t computeHash(ParametricType const& fun, std::size_t const& sub) {
                        std::size_t seed = 0;
                        boost::hash_combine(seed, fun);
                        boost::hash_combine(seed, sub);
                        return seed;
                    }

                    std::size_t hash;
                    ParametricType function;
                    std::size_t substitution;
                };

                class FuncSubHash{
                    public:
                        std::size_t operator()(FunctionSubstitution const& fs) const {
                            return fs.getHash();
                        }
                };

                typedef typename std::unordered_map<FunctionSubstitution, ConstantType, FuncSubHash>::value_type FunctionEntry;

                void initializeProbabilities(ParametricSparseModelType const& parametricModel, std::vector<std::size_t> const& newIndices, std::vector<std::size_t>& rowSubstitutions);
                void initializeRewards(ParametricSparseModelType const& parametricModel, std::vector<std::size_t> const& newIndices, std::vector<std::size_t> const& rowSubstitutions);
                void initializePlayer1Matrix(ParametricSparseModelType const& parametricModel);
                void instantiate(ParameterRegion<ParametricType> const& region, bool computeLowerBounds);
                void invokeSolver(bool computeLowerBounds);

                //Some designated states in the original model
                storm::storage::BitVector targetStates, maybeStates;
                //The last result of the solving the equation system. Also serves as first guess for the next call.
                //Note: eqSysResult.size==maybeStates.numberOfSetBits
                std::vector<ConstantType> eqSysResult;
                //The index which represents the result for the initial state in the eqSysResult vector
                std::size_t eqSysInitIndex;
                //A flag that denotes whether we compute probabilities or rewards
                bool computeRewards;
                //Player 1 represents the nondeterminism of the given mdp (so, this is irrelevant if we approximate values of a DTMC)
                storm::solver::SolveGoal player1Goal;
                storm::storage::SparseMatrix<storm::storage::sparse::state_type> player1Matrix;
                
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
                    std::vector<std::map<VariableType, TypeOfBound>> substitutions;
                } funcSubData;
                struct MatrixData {
                    storm::storage::SparseMatrix<ConstantType> matrix; //The matrix itself.
                    std::vector<std::pair<typename storm::storage::SparseMatrix<ConstantType>::iterator, ConstantType&>> assignment; // Connection of matrix entries with placeholders
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

