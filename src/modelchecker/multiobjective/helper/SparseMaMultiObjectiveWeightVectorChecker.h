#ifndef STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMAMULTIOBJECTIVEWEIGHTVECTORCHECKER_H_
#define STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMAMULTIOBJECTIVEWEIGHTVECTORCHECKER_H_

#include <vector>
#include <type_traits>

#include "src/modelchecker/multiobjective/helper/SparseMultiObjectiveWeightVectorChecker.h"
#include "src/solver/LinearEquationSolver.h"
#include "src/solver/MinMaxLinearEquationSolver.h"
#include "src/utility/NumberTraits.h"

namespace storm {
    namespace modelchecker {
        namespace helper {
            
            /*!
             * Helper Class that takes preprocessed multi objective data and a weight vector and ...
             * - computes the maximal expected reward w.r.t. the weighted sum of the rewards of the individual objectives
             * - extracts the scheduler that induces this maximum
             * - computes for each objective the value induced by this scheduler
             */
            template <class SparseMaModelType>
            class SparseMaMultiObjectiveWeightVectorChecker : public SparseMultiObjectiveWeightVectorChecker<SparseMaModelType> {
            public:
                typedef typename SparseMaModelType::ValueType ValueType;
                typedef SparseMultiObjectivePreprocessorData<SparseMaModelType> PreprocessorData;
                
                SparseMaMultiObjectiveWeightVectorChecker(PreprocessorData const& data);
                
            private:
                
                /*
                 * Stores (digitized) time bounds in descending order
                 */
                typedef std::map<uint_fast64_t, storm::storage::BitVector, std::greater<uint_fast64_t>> TimeBoundMap;
                
                /*
                 * Stores the ingredients of a sub model
                 */
                struct SubModel {
                    storm::storage::BitVector states; // The states that are part of this sub model
                    storm::storage::BitVector choices; // The choices that are part of this sub model
                    
                    storm::storage::SparseMatrix<ValueType> toMS; // Transitions to Markovian states
                    storm::storage::SparseMatrix<ValueType> toPS; // Transitions to probabilistic states
                    
                    std::vector<ValueType> weightedRewardVector;
                    std::vector<std::vector<ValueType>> objectiveRewardVectors;
                    
                    std::vector<ValueType> weightedSolutionVector;
                    std::vector<std::vector<ValueType>> objectiveSolutionVectors;
                    
                    std::vector<ValueType> auxChoiceValues; //stores auxiliary values for every choice
                    
                    uint_fast64_t getNumberOfStates() const { return toMS.getRowGroupCount(); };
                    uint_fast64_t getNumberOfChoices() const { return toMS.getRowCount(); };
                };
      
                /*
                 * Stores the data that is relevant to invoke the minMaxSolver and retrieve the result.
                 */
                struct MinMaxSolverData {
                    std::unique_ptr<storm::solver::MinMaxLinearEquationSolver<ValueType>> solver; // the solver itself
                    
                    storm::storage::SparseMatrix<ValueType> matrix; // the considered matrix
                    std::vector<uint_fast64_t> toPSChoiceMapping;  // maps rows of the considered matrix to choices of the PS SubModel
                    std::vector<uint_fast64_t> fromPSStateMapping; // maps states of the PS SubModel to row groups of the considered matrix
                    
                    std::vector<ValueType> b;
                    std::vector<ValueType> x;
                };
                
                struct LinEqSolverData {
                    storm::solver::GeneralLinearEquationSolverFactory<ValueType> factory;
                    std::unique_ptr<storm::solver::LinearEquationSolver<ValueType>> solver;
                    
                    std::vector<ValueType> b;
                };
                
                /*!
                 *
                 * @param weightVector the weight vector of the current check
                 * @param weightedRewardVector the weighted rewards considering the unbounded objectives. Will be invalidated after calling this.
                 */
                virtual void boundedPhase(std::vector<ValueType> const& weightVector, std::vector<ValueType>& weightedRewardVector) override;
                
                /*!
                 * Retrieves the data for a submodel of the data->preprocessedModel
                 * @param createMS if true, the submodel containing the Markovian states is created.
                 *                 if false, the submodel containing the probabilistic states is created.
                 */
                SubModel createSubModel(bool createMS, std::vector<ValueType> const& weightedRewardVector) const;
                
                /*!
                 * Retrieves the delta used for digitization
                 */
                template <typename VT = ValueType, typename std::enable_if<storm::NumberTraits<VT>::SupportsExponential, int>::type = 0>
                VT getDigitizationConstant() const;
                template <typename VT = ValueType, typename std::enable_if<!storm::NumberTraits<VT>::SupportsExponential, int>::type = 0>
                VT getDigitizationConstant() const;
                
                /*!
                 * Digitizes the given matrix and vectors w.r.t. the given digitization constant and the given rate vector.
                 */
                template <typename VT = ValueType, typename std::enable_if<storm::NumberTraits<VT>::SupportsExponential, int>::type = 0>
                void digitize(SubModel& subModel, VT const& digitizationConstant) const;
                template <typename VT = ValueType, typename std::enable_if<!storm::NumberTraits<VT>::SupportsExponential, int>::type = 0>
                void digitize(SubModel& subModel, VT const& digitizationConstant) const;
                
                /* 
                 * Fills the given maps with the digitized time bounds. Also sets the offsetsToLowerBound / offsetsToUpperBound values
                 * according to the digitization error
                 */
                template <typename VT = ValueType, typename std::enable_if<storm::NumberTraits<VT>::SupportsExponential, int>::type = 0>
                void digitizeTimeBounds(TimeBoundMap& lowerTimeBounds, TimeBoundMap& upperTimeBounds, VT const& digitizationConstant);
                template <typename VT = ValueType, typename std::enable_if<!storm::NumberTraits<VT>::SupportsExponential, int>::type = 0>
                void digitizeTimeBounds(TimeBoundMap& lowerTimeBounds, TimeBoundMap& upperTimeBounds, VT const& digitizationConstant);
                
                
                /*!
                 * Computes a reduction of the PStoPS submodel which does not contain end components in which no choice with reward is taken.
                 * With the reduction, a MinMaxSolver is initialized.
                 */
                std::unique_ptr<MinMaxSolverData> initMinMaxSolverData(SubModel const& PS) const;
                
                /*
                 * Updates the reward vectors within the split model,
                 * the reward vector of the reduced PStoPS model, and
                 * objectives that are considered at the current time epoch.
                 */
                void updateDataToCurrentEpoch(SubModel& MS, SubModel& PS, MinMaxSolverData& minMax, storm::storage::BitVector& consideredObjectives, uint_fast64_t const& currentEpoch, std::vector<ValueType> const& weightVector, TimeBoundMap::iterator& lowerTimeBoundIt, TimeBoundMap const& lowerTimeBounds, TimeBoundMap::iterator& upperTimeBoundIt, TimeBoundMap const& upperTimeBounds);
                
                /*
                 * Performs a step for the probabilistic states, that is
                 * * Compute an optimal scheduler for the weighted reward sum
                 * * Compute the values for the individual objectives w.r.t. that scheduler
                 * 
                 * The resulting values represent the rewards at probabilistic states that are obtained at the current time epoch.
                 */
                void performPSStep(SubModel& PS, SubModel const& MS, MinMaxSolverData& minMax, LinEqSolverData& linEq, std::vector<uint_fast64_t>& optimalChoicesAtCurrentEpoch,  storm::storage::BitVector const& consideredObjectives) const;
                
                /*
                 * Performs a step for the Markovian states, that is
                 * * Compute values for the weighted reward sum as well as for the individual objectives
                 *
                 * The resulting values represent the rewards at Markovian states that are obtained after one (digitized) time unit has passed.
                 */
                void performMSStep(SubModel& MS, SubModel const& PS, storm::storage::BitVector const& consideredObjectives) const;
                
            };
            
        }
    }
}

#endif /* STORM_MODELCHECKER_MULTIOBJECTIVE_HELPER_SPARSEMAMULTIOBJECTIVEWEIGHTEDVECTORCHECKER_H_ */
