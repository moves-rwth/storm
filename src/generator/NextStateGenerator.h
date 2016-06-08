#ifndef STORM_GENERATOR_NEXTSTATEGENERATOR_H_
#define STORM_GENERATOR_NEXTSTATEGENERATOR_H_

#include <vector>
#include <cstdint>

#include <boost/variant.hpp>

#include "src/storage/expressions/Expression.h"
#include "src/storage/BitVectorHashMap.h"

#include "src/generator/CompressedState.h"
#include "src/generator/StateBehavior.h"

namespace storm {
    namespace expressions {
        class Expression;
    }
    
    namespace models {
        namespace sparse {
            class StateLabeling;
        }
    }
    
    namespace logic {
        class Formula;
    }
    
    namespace generator {
        class LabelOrExpression {
        public:
            LabelOrExpression(storm::expressions::Expression const& expression);
            LabelOrExpression(std::string const& label);
            
            bool isLabel() const;
            std::string const& getLabel() const;
            bool isExpression() const;
            storm::expressions::Expression const& getExpression() const;
            
        private:
            /// An optional label for the expression.
            boost::variant<std::string, storm::expressions::Expression> labelOrExpression;
        };
        
        class NextStateGeneratorOptions {
        public:
            /*!
             * Creates an object representing the default options.
             */
            NextStateGeneratorOptions();
            
            /*!
             * Creates an object representing the suggested building options assuming that the given formula is the
             * only one to check. Additional formulas may be preserved by calling <code>preserveFormula</code>.
             *
             * @param formula The formula based on which to choose the building options.
             */
            NextStateGeneratorOptions(storm::logic::Formula const& formula);
            
            /*! 
             * Creates an object representing the suggested building options assuming that the given formulas are
             * the only ones to check. Additional formulas may be preserved by calling <code>preserveFormula</code>.
             *
             * @param formula Thes formula based on which to choose the building options.
             */
            NextStateGeneratorOptions(std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas);
            
            /*!
             * Changes the options in a way that ensures that the given formula can be checked on the model once it
             * has been built.
             *
             * @param formula The formula that is to be ''preserved''.
             */
            void preserveFormula(storm::logic::Formula const& formula);
            
            /*!
             * Analyzes the given formula and sets an expression for the states states of the model that can be
             * treated as terminal states. Note that this may interfere with checking properties different than the
             * one provided.
             *
             * @param formula The formula used to (possibly) derive an expression for the terminal states of the
             * model.
             */
            void setTerminalStatesFromFormula(storm::logic::Formula const& formula);
            
            std::vector<std::string> const& getRewardModelNames() const;
            std::set<std::string> const& getLabels() const;
            std::vector<storm::expressions::Expression> const& getExpressionLabels() const;
            std::vector<std::pair<LabelOrExpression, bool>> const& getTerminalStates() const;
            bool hasTerminalStates() const;
            void clearTerminalStates();
            bool isBuildChoiceLabelsSet() const;
            
            NextStateGeneratorOptions& addRewardModel(std::string const& rewardModelName);
            NextStateGeneratorOptions& addLabel(storm::expressions::Expression const& expression);
            NextStateGeneratorOptions& addLabel(std::string const& label);
            NextStateGeneratorOptions& addTerminalExpression(storm::expressions::Expression const& expression, bool value);
            NextStateGeneratorOptions& addTerminalLabel(std::string const& label, bool value);
            NextStateGeneratorOptions& setBuildChoiceLabels(bool newValue);
            
        private:
            /// The names of the reward models to generate.
            std::vector<std::string> rewardModelNames;

            /// A set of labels to build.
            std::set<std::string> labels;
            
            /// The expression that are to be used for creating the state labeling.
            std::vector<storm::expressions::Expression> expressionLabels;
            
            /// If one of these labels/expressions evaluates to the given bool, the state generator can abort the exploration.
            std::vector<std::pair<LabelOrExpression, bool>> terminalStates;
            
            /// A flag indicating whether or not to build choice labels.
            bool buildChoiceLabels;
        };
        
        enum class ModelType {
            DTMC,
            CTMC,
            MDP,
            MA
        };
        
        class RewardModelInformation {
        public:
            RewardModelInformation(std::string const& name, bool stateRewards, bool stateActionRewards, bool transitionRewards);
            
            std::string const& getName() const;
            bool hasStateRewards() const;
            bool hasStateActionRewards() const;
            bool hasTransitionRewards() const;
            
        private:
            std::string name;
            bool stateRewards;
            bool stateActionRewards;
            bool transitionRewards;
        };
        
        template<typename ValueType, typename StateType = uint32_t>
        class NextStateGenerator {
        public:
            typedef std::function<StateType (CompressedState const&)> StateToIdCallback;

            NextStateGenerator(NextStateGeneratorOptions const& options);
            
            virtual uint64_t getStateSize() const = 0;
            virtual ModelType getModelType() const = 0;
            virtual bool isDeterministicModel() const = 0;
            virtual std::vector<StateType> getInitialStates(StateToIdCallback const& stateToIdCallback) = 0;
            
            virtual void load(CompressedState const& state) = 0;
            virtual StateBehavior<ValueType, StateType> expand(StateToIdCallback const& stateToIdCallback) = 0;
            virtual bool satisfies(storm::expressions::Expression const& expression) const = 0;
            
            std::size_t getNumberOfRewardModels() const;
            virtual RewardModelInformation getRewardModelInformation(uint64_t const& index) const = 0;
            
            virtual storm::expressions::SimpleValuation toValuation(CompressedState const& state) const = 0;
            
            virtual storm::models::sparse::StateLabeling label(storm::storage::BitVectorHashMap<StateType> const& states, std::vector<StateType> const& initialStateIndices = {}) = 0;
            
            NextStateGeneratorOptions const& getOptions() const;
            
        protected:
            NextStateGeneratorOptions options;
        };
    }
}

#endif /* STORM_GENERATOR_NEXTSTATEGENERATOR_H_ */