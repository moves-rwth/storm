#pragma once

#include "storm/storage/jani/ModelType.h"
#include "storm/builder/jit/Choice.h"

namespace storm {
    namespace builder {
        namespace jit {
         
            template <typename IndexType, typename ValueType>
            class StateBehaviour {
            public:
                typedef std::vector<Choice<IndexType, ValueType>> ContainerType;
                
                StateBehaviour();
                
                void addChoice(Choice<IndexType, ValueType>&& choice);
                Choice<IndexType, ValueType>& addChoice(bool markovian = false);
                ContainerType const& getChoices() const;
                
                /*!
                 * Adds the given state reward to the behavior of the state.
                 */
                void addStateReward(ValueType const& stateReward);
                
                /*!
                 * Adds the given state rewards to the behavior of the state.
                 */
                void addStateRewards(std::vector<ValueType>&& stateRewards);
                                
                /*!
                 * Retrieves the rewards for this state.
                 */
                std::vector<ValueType> const& getStateRewards() const;

                /*!
                 * Reduces this behaviour to one that is suitable for the provided model type.
                 */
                void reduce(storm::jani::ModelType const& modelType);
                
                /*!
                 * Determines whether the state behaviour has Markovian as well as probabilistic choices. Note that this
                 * only yields the desired result after the state behaviour has been reduced.
                 */
                bool isHybrid() const;
                
                /*!
                 * Determines whether the behaviour has Markovian choices only. Note that this only yields the desired
                 * result after the state behaviour has been reduced.
                 */
                bool isMarkovian() const;
                
                /*!
                 * Retrieves whether the state behaviour has any Markovian choices.
                 */
                bool isMarkovianOrHybrid() const;
                
                bool isExpanded() const;
                void setExpanded();
                
                bool empty() const;
                std::size_t size() const;
                void clear();
                
            private:
                /*!
                 * Reduces the choices of this state to make it a valid DTMC/CTMC behaviour.
                 */
                void reduceDeterministic(storm::jani::ModelType const& modelType);

                /*!
                 * Reduces the choices of this state to make it a valid MA behaviour.
                 */
                void reduceMarkovAutomaton();

                /// The actual choices of this behaviour.
                ContainerType choices;
                
                /// The state rewards (under the different, selected reward models) of the state.
                std::vector<ValueType> stateRewards;
                
                bool compressed;
                bool expanded;
            };
            
        }
    }
}
