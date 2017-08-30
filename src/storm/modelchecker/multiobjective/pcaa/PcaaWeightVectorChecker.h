#pragma once

#include <vector>

#include "storm/storage/Scheduler.h"
#include "storm/models/sparse/Mdp.h"
#include "storm/models/sparse/MarkovAutomaton.h"
#include "storm/modelchecker/multiobjective/SparseMultiObjectivePreprocessorReturnType.h"
#include "storm/modelchecker/multiobjective/Objective.h"


namespace storm {
    namespace modelchecker {
        namespace multiobjective {
            
            /*!
             * Helper Class that takes a weight vector and ...
             * - computes the optimal value w.r.t. the weighted sum of the individual objectives
             * - extracts the scheduler that induces this optimum
             * - computes for each objective the value induced by this scheduler
             */
            template <typename ModelType>
            class PcaaWeightVectorChecker {
            public:
                typedef typename ModelType::ValueType ValueType;
                
                /*
                 * Creates a weight vector checker.
                 *
                 * @param objectives The (preprocessed) objectives
                 *
                 */
                
                PcaaWeightVectorChecker(ModelType const& model, std::vector<Objective<ValueType>> const& objectives);
                
                virtual ~PcaaWeightVectorChecker() = default;
                
                virtual void check(std::vector<ValueType> const& weightVector) = 0;
                
                /*!
                 * Retrieves the results of the individual objectives at the initial state of the given model.
                 * Note that check(..) has to be called before retrieving results. Otherwise, an exception is thrown.
                 * Also note that there is no guarantee that the under/over approximation is in fact correct
                 * as long as the underlying solution methods are unsound (e.g., standard value iteration).
                 */
                virtual std::vector<ValueType> getUnderApproximationOfInitialStateResults() const = 0;
                virtual std::vector<ValueType> getOverApproximationOfInitialStateResults() const = 0;
                
                /*!
                 * Sets the precision of this weight vector checker. After calling check() the following will hold:
                 * Let h_lower and h_upper be two hyperplanes such that
                 * * the normal vector is the provided weight-vector where the entry for minimizing objectives is negated
                 * * getUnderApproximationOfInitialStateResults() lies on h_lower and
                 * * getOverApproximationOfInitialStateResults() lies on h_upper.
                 * Then the distance between the two hyperplanes is at most weightedPrecision
                 */
                void setWeightedPrecision(ValueType const& value);
                
                /*!
                 * Returns the precision of this weight vector checker.
                 */
                ValueType const& getWeightedPrecision() const;
                
                /*!
                 * Retrieves a scheduler that induces the current values (if such a scheduler was generated).
                 * Note that check(..) has to be called before retrieving the scheduler. Otherwise, an exception is thrown.
                 */
                virtual storm::storage::Scheduler<ValueType> computeScheduler() const;
                
                
            protected:
                
                // The (preprocessed) model
                ModelType const& model;
                // The (preprocessed) objectives
                std::vector<Objective<ValueType>> const& objectives;
                // The precision of this weight vector checker.
                ValueType weightedPrecision;
                
            };
            
            template<typename ModelType>
            class WeightVectorCheckerFactory {
            public:

                template<typename VT = typename ModelType::ValueType, typename std::enable_if<std::is_same<ModelType, storm::models::sparse::Mdp<VT>>::value, int>::type = 0>
                static std::unique_ptr<PcaaWeightVectorChecker<ModelType>> create(ModelType const& model,
                                                   std::vector<Objective<typename ModelType::ValueType>> const& objectives,
                                                   storm::storage::BitVector const& possibleECActions,
                                                   storm::storage::BitVector const& possibleBottomStates);
                
                template<typename VT = typename ModelType::ValueType, typename std::enable_if<std::is_same<ModelType, storm::models::sparse::MarkovAutomaton<VT>>::value, int>::type = 0>
                static std::unique_ptr<PcaaWeightVectorChecker<ModelType>> create(ModelType const& model,
                                                   std::vector<Objective<typename ModelType::ValueType>> const& objectives,
                                                   storm::storage::BitVector const& possibleECActions,
                                                   storm::storage::BitVector const& possibleBottomStates);
            private:

                static bool onlyCumulativeOrTotalRewardObjectives(std::vector<Objective<typename ModelType::ValueType>> const& objectives);
            };

        }
    }
}

