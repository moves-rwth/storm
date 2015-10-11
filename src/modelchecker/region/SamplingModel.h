/* 
 * File:   SamplingModel.h
 * Author: tim
 *
 * Created on August 7, 2015, 9:31 AM
 */

#ifndef STORM_MODELCHECKER_REGION_SAMPLINGMODEL_H
#define	STORM_MODELCHECKER_REGION_SAMPLINGMODEL_H

#include <unordered_map>
#include <memory>

#include "src/utility/region.h"
#include "src/logic/Formulas.h"
#include "src/models/sparse/Model.h"
#include "src/storage/SparseMatrix.h"
#include "src/solver/SolveGoal.h"

namespace storm {
    namespace modelchecker{
        namespace region {
            template<typename ParametricSparseModelType, typename ConstantType>
            class SamplingModel {
            public:
                typedef typename ParametricSparseModelType::ValueType ParametricType;
                typedef typename storm::utility::region::VariableType<ParametricType> VariableType;
                typedef typename storm::utility::region::CoefficientType<ParametricType> CoefficientType;
                
                /*!
                 * Creates a sampling model.
                 * The given model should have the state-labels
                 * * "target", labeled on states with reachability probability one (reachability reward zero)
                 * * "sink", labeled on states from which a target state can not be reached.
                 * The (single) initial state should be disjoint from these states. (otherwise the result would be independent of the parameters, anyway)
                 */
                SamplingModel(ParametricSparseModelType const& parametricModel, std::shared_ptr<storm::logic::OperatorFormula> formula);
                virtual ~SamplingModel();

                /*!
                 * Instantiates the underlying model according to the given point
                 */
                void instantiate(std::map<VariableType, CoefficientType>const& point);

                /*!
                 * Returns the reachability probabilities (or the expected rewards) for every state according to the current instantiation.
                 * Undefined behavior if model has not been instantiated first!
                 */
                std::vector<ConstantType> computeValues();
                
                /*!
                 * Returns the reachability probability (or the expected rewards) of the initial state.
                 * Undefined behavior if model has not been instantiated first!
                 */
                ConstantType computeInitialStateValue();

            private:

                typedef typename std::unordered_map<ParametricType, ConstantType>::value_type FunctionEntry;
                void initializeProbabilities(ParametricSparseModelType const& parametricModel, std::vector<std::size_t> const& newIndices);
                void initializeRewards(ParametricSparseModelType const& parametricModel, std::vector<std::size_t> const& newIndices);
                void invokeSolver();
                
                //Some designated states in the model
                storm::storage::BitVector targetStates, maybeStates;
                //The last result of the solving the equation system. Also serves as first guess for the next call.
                //Note: eqSysResult.size==maybeStates.numberOfSetBits
                std::vector<ConstantType> eqSysResult;
                //The index which represents the result for the initial state in the eqSysResult vector
                std::size_t eqSysInitIndex;
                //A flag that denotes whether we compute probabilities or rewards
                bool computeRewards;
                //The goal we want to accomplish when solving the eq sys.
                storm::solver::SolveGoal solveGoal;
                

                /* The data required for the equation system, i.e., a matrix and a vector.
                 * 
                 * We use a map to store one (unique) entry for every occurring function. 
                 * The map points to some ConstantType value which serves as placeholder. 
                 * When instantiating the model, the evaluated result of every function is stored in the corresponding placeholder.
                 * Finally, there is an assignment that connects every non-constant matrix entry
                 * with a pointer to the value that, on instantiation, needs to be written in that entry.
                 * 
                 * This way, it is avoided that the same function is evaluated multiple times.
                 */
                struct MatrixData {
                    storm::storage::SparseMatrix<ConstantType> matrix; //The matrix itself.
                    std::unordered_map<ParametricType, ConstantType> functions; // the occurring functions together with the corresponding placeholders for the result
                    std::vector<std::pair<typename storm::storage::SparseMatrix<ConstantType>::iterator, ConstantType*>> assignment; // Connection of matrix entries with placeholders
                } matrixData;
                struct VectorData {
                    std::vector<ConstantType> vector; //The vector itself.
                    std::unordered_map<ParametricType, ConstantType> functions; // the occurring functions together with the corresponding placeholders for the result
                    std::vector<std::pair<typename std::vector<ConstantType>::iterator, ConstantType*>> assignment; // Connection of vector entries with placeholders
                } vectorData;
                

            };
        } //namespace region
    }
}
#endif	/* STORM_MODELCHECKER_REGION_SAMPLINGMODEL_H */

