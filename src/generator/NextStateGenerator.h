#ifndef STORM_GENERATOR_NEXTSTATEGENERATOR_H_
#define STORM_GENERATOR_NEXTSTATEGENERATOR_H_

#include <vector>
#include <cstdint>

#include <boost/variant.hpp>

#include "src/storage/expressions/Expression.h"
#include "src/storage/BitVectorHashMap.h"
#include "src/storage/expressions/ExpressionEvaluator.h"

#include "src/generator/VariableInformation.h"
#include "src/generator/CompressedState.h"
#include "src/generator/StateBehavior.h"

#include "src/utility/ConstantsComparator.h"

namespace storm {
    namespace expressions {
        class Expression;
        class ExpressionManager;
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
            NextStateGeneratorOptions(bool buildAllRewardModels = false, bool buildAllLabels = false);
            
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
            std::set<std::string> const& getLabelNames() const;
            std::vector<storm::expressions::Expression> const& getExpressionLabels() const;
            std::vector<std::pair<LabelOrExpression, bool>> const& getTerminalStates() const;
            bool hasTerminalStates() const;
            void clearTerminalStates();
            bool isBuildChoiceLabelsSet() const;
            bool isBuildAllRewardModelsSet() const;
            bool isBuildAllLabelsSet() const;
            
            NextStateGeneratorOptions& setBuildAllRewardModels();
            NextStateGeneratorOptions& addRewardModel(std::string const& rewardModelName);
            NextStateGeneratorOptions& setBuildAllLabels();
            NextStateGeneratorOptions& addLabel(storm::expressions::Expression const& expression);
            NextStateGeneratorOptions& addLabel(std::string const& labelName);
            NextStateGeneratorOptions& addTerminalExpression(storm::expressions::Expression const& expression, bool value);
            NextStateGeneratorOptions& addTerminalLabel(std::string const& label, bool value);
            NextStateGeneratorOptions& setBuildChoiceLabels(bool newValue);
            
        private:
            /// A flag that indicates whether all reward models are to be built. In this case, the reward model names are
            /// to be ignored.
            bool buildAllRewardModels;
            
            /// The names of the reward models to generate.
            std::vector<std::string> rewardModelNames;

            /// A flag that indicates whether all labels are to be built. In this case, the label names are to be ignored.
            bool buildAllLabels;
            
            /// A set of labels to build.
            std::set<std::string> labelNames;
            
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

            NextStateGenerator(storm::expressions::ExpressionManager const& expressionManager, VariableInformation const& variableInformation, NextStateGeneratorOptions const& options);
            
            uint64_t getStateSize() const;
            virtual ModelType getModelType() const = 0;
            virtual bool isDeterministicModel() const = 0;
            virtual bool isDiscreteTimeModel() const = 0;
            virtual std::vector<StateType> getInitialStates(StateToIdCallback const& stateToIdCallback) = 0;
            
            void load(CompressedState const& state);
            virtual StateBehavior<ValueType, StateType> expand(StateToIdCallback const& stateToIdCallback) = 0;
            bool satisfies(storm::expressions::Expression const& expression) const;
            
            virtual std::size_t getNumberOfRewardModels() const = 0;
            virtual RewardModelInformation getRewardModelInformation(uint64_t const& index) const = 0;
            
            storm::expressions::SimpleValuation toValuation(CompressedState const& state) const;
            
            virtual storm::models::sparse::StateLabeling label(storm::storage::BitVectorHashMap<StateType> const& states, std::vector<StateType> const& initialStateIndices = {}, std::vector<StateType> const& deadlockStateIndices = {}) = 0;
            
            NextStateGeneratorOptions const& getOptions() const;
            
        protected:
            /*!
             * Creates the state labeling for the given states using the provided labels and expressions.
             */
            storm::models::sparse::StateLabeling label(storm::storage::BitVectorHashMap<StateType> const& states, std::vector<StateType> const& initialStateIndices, std::vector<StateType> const& deadlockStateIndices, std::vector<std::pair<std::string, storm::expressions::Expression>> labelsAndExpressions);
            
            /// The options to be used for next-state generation.
            NextStateGeneratorOptions options;
            
            /// The expression manager used for evaluating expressions.
            std::shared_ptr<storm::expressions::ExpressionManager const> expressionManager;
            
            /// The expressions that define terminal states.
            std::vector<std::pair<storm::expressions::Expression, bool>> terminalStates;
            
            /// Information about how the variables are packed.
            VariableInformation variableInformation;
            
            /// An evaluator used to evaluate expressions.
            storm::expressions::ExpressionEvaluator<ValueType> evaluator;
            
            /// The currently loaded state.
            CompressedState const* state;
            
            /// A comparator used to compare constants.
            storm::utility::ConstantsComparator<ValueType> comparator;
        };
    }
}

#endif /* STORM_GENERATOR_NEXTSTATEGENERATOR_H_ */