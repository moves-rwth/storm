#pragma once

#include <vector>

#include "storm/modelchecker/multiobjective/preprocessing/SparseMultiObjectivePreprocessorResult.h"
#include "storm/models/sparse/Mdp.h"

namespace storm {
    
    class Environment;
    namespace storage {
        class BitVector;
    }
    
    namespace modelchecker {
        namespace multiobjective {
            
            template <class ModelType>
            class MultiObjectiveSchedulerEvaluator {
            public:
                
                typedef typename ModelType::ValueType ValueType;
                
                MultiObjectiveSchedulerEvaluator(preprocessing::SparseMultiObjectivePreprocessorResult<ModelType>& preprocessorResult);

                /*!
                 * Instantiates the given model with the provided scheduler and checks the objectives individually under that scheduler.
                 */
                void check(Environment const& env);
                
                // Retrieve the results after calling check.
                std::vector<std::vector<ValueType>> const& getResults() const;
                std::vector<ValueType> const& getResultForObjective(uint64_t objIndex) const;
                storm::storage::BitVector const& getSchedulerIndependentStates(uint64_t objIndex) const;
                std::vector<ValueType> getInitialStateResults() const;
                ModelType const& getModel() const;
                std::vector<Objective<ValueType>> const& getObjectives() const;
                bool hasCurrentSchedulerBeenChecked() const;
                std::vector<uint64_t> const& getScheduler() const;
                uint64_t const& getChoiceAtState(uint64_t state) const;
                void setChoiceAtState(uint64_t state, uint64_t choice);
                
                
            private:
                
                void initializeSchedulerIndependentStates();
                
                ModelType const& model;
                // In case the model is a markov automaton, we transform it to an mdp
                std::shared_ptr<storm::models::sparse::Mdp<ValueType>> mdp;
                
                std::vector<Objective<ValueType>> const& objectives;
                
                // Indicates for each objective the set of states for which the result is fix (i.e. independent of the scheduler).
                std::vector<storm::storage::BitVector> schedulerIndependentStates;
                
                // Stores the results from the last call to check
                std::vector<std::vector<ValueType>> results;
                std::vector<uint64_t> currSched;
                bool currSchedHasBeenChecked;
            };
        }
    }
}