#pragma once

#include "src/storage/jani/ModelType.h"
#include "src/builder/jit/Choice.h"

namespace storm {
    namespace builder {
        namespace jit {
         
            template <typename IndexType, typename ValueType>
            class StateBehaviour {
            public:
                typedef std::vector<Choice<IndexType, ValueType>> ContainerType;
                
                StateBehaviour();
                
                void addChoice(Choice<IndexType, ValueType>&& choice);
                Choice<IndexType, ValueType>& addChoice();
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
                
                void reduce(storm::jani::ModelType const& modelType);
                
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
