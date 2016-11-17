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
#include <type_traits>

#include "src/storm/utility/region.h"
#include "src/storm/logic/Formulas.h"
#include "src/storm/models/sparse/Model.h"
#include "src/storm/solver/SolveGoal.h"
#include "src/storm/storage/TotalScheduler.h"
#include "src/storm/utility/ModelInstantiator.h"

namespace storm {
    namespace modelchecker{
        namespace region {
            template<typename ParametricSparseModelType, typename ConstantType>
            class SamplingModel {
            public:
                typedef typename ParametricSparseModelType::ValueType ParametricType;
                typedef typename storm::utility::region::VariableType<ParametricType> VariableType;
                typedef typename storm::utility::region::CoefficientType<ParametricType> CoefficientType;
                typedef typename std::conditional<(std::is_same<ParametricSparseModelType, storm::models::sparse::Dtmc<ParametricType>>::value),
                        storm::models::sparse::Dtmc<ConstantType>, 
                        storm::models::sparse::Mdp<ConstantType>
                >::type ConstantSparseModelType;
                
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
                 * Returns the reachability probabilities (or the expected rewards) for every state according to the current instantiation.
                 */
                std::vector<ConstantType> computeValues(std::map<VariableType, CoefficientType>const& point);
                
                /*!
                 * Instantiates the underlying model according to the given point
                 * Returns the reachability probability (or the expected rewards) of the initial state.
                 */
                ConstantType computeInitialStateValue(std::map<VariableType, CoefficientType>const& point);
                
                /*!
                 * Instantiates the underlying model according to the given point
                 * Returns true iff the formula (given upon construction of *this) is true in the initial state of the instantiated model
                 */
                bool checkFormulaOnSamplingPoint(std::map<VariableType, CoefficientType>const& point);

            private:
                
                void invokeSolver(ConstantSparseModelType const& instantiatedModel, bool allowEarlyTermination);
                
                //A flag that denotes whether we compute probabilities or rewards
                bool computeRewards;
                //The model type of the original (parametric) model
                storm::models::ModelType typeOfParametricModel;
                
                //Some designated states in the original model
                storm::storage::BitVector targetStates, maybeStates;
                
                struct SolverData{
                    //The result from the previous instantiation. Serve as first guess for the next call.
                    std::vector<ConstantType> result; //Note: result.size==maybeStates.numberOfSetBits
                    std::size_t initialStateIndex; //The index which represents the result for the initial state in the result vector
                    //The following is only relevant if we consider mdps:
                    std::unique_ptr<storm::solver::BoundedGoal<ConstantType>> solveGoal;
                    storm::storage::TotalScheduler lastScheduler; //best Scheduler from the previous instantiation. Serves as first guess for the next call.
                } solverData;
                
                storm::utility::ModelInstantiator<ParametricSparseModelType, ConstantSparseModelType> modelInstantiator;

            };
        } //namespace region
    }
}
#endif	/* STORM_MODELCHECKER_REGION_SAMPLINGMODEL_H */

