#pragma once

#include <map>
#include <boost/optional.hpp>

#include "storm/modelchecker/multiobjective/Objective.h"


namespace storm {
    
    class Environment;
    
    namespace modelchecker {
        namespace multiobjective {
            
            template <typename ModelType>
            class DeterministicSchedsObjectiveHelper {
            public:
                
                typedef typename ModelType::ValueType ValueType;
                DeterministicSchedsObjectiveHelper(ModelType const& model, Objective<ValueType> const& objective);
                
                /*!
                 * Returns states and values for states that are independent of the scheduler.
                 */
                std::map<uint64_t, ValueType> const& getSchedulerIndependentStateValues() const;

                /*!
                 * Returns offsets of each choice value (e.g., the reward) if non-zero.
                 * This does not include choices of states with independent state values
                 */
                std::map<uint64_t, ValueType> const& getChoiceValueOffsets() const;

                ValueType const& getUpperValueBoundAtState(Environment const& env, uint64_t state) const;
                ValueType const& getLowerValueBoundAtState(Environment const& env, uint64_t state) const;
                
                bool minimizing() const;
                
            private:
                
                mutable boost::optional<std::map<uint64_t, ValueType>> schedulerIndependentStateValues;
                mutable boost::optional<std::map<uint64_t, ValueType>> choiceValueOffsets;
                mutable boost::optional<std::vector<ValueType>> lowerResultBounds;
                mutable boost::optional<std::vector<ValueType>> upperResultBounds;
                
                
                ModelType const& model;
                Objective<ValueType> const& objective;
                
            };
        }
    }
}