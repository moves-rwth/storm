#pragma once

#include <string>
#include <vector>
#include <set>

#include <boost/variant.hpp>
#include <boost/optional.hpp>

#include "storm/storage/expressions/Expression.h"

namespace storm {
    namespace expressions {
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

    namespace builder {
        
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
        
        class BuilderOptions {
        public:
            /*!
             * Creates an object representing the default options.
             */
            BuilderOptions(bool buildAllRewardModels = false, bool buildAllLabels = false);
            
            /*!
             * Creates an object representing the suggested building options assuming that the given formula is the
             * only one to check. Additional formulas may be preserved by calling <code>preserveFormula</code>.
             *
             * @param formula The formula based on which to choose the building options.
             */
            BuilderOptions(storm::logic::Formula const& formula);
            
            /*!
             * Creates an object representing the suggested building options assuming that the given formulas are
             * the only ones to check. Additional formulas may be preserved by calling <code>preserveFormula</code>.
             *
             * @param formula Thes formula based on which to choose the building options.
             */
            BuilderOptions(std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas);
            
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
            bool isBuildChoiceOriginsSet() const;
            bool isBuildAllRewardModelsSet() const;
            bool isBuildAllLabelsSet() const;
            bool isExplorationChecksSet() const;
            
            BuilderOptions& setBuildAllRewardModels();
            BuilderOptions& addRewardModel(std::string const& rewardModelName);
            BuilderOptions& setBuildAllLabels();
            BuilderOptions& addLabel(storm::expressions::Expression const& expression);
            BuilderOptions& addLabel(std::string const& labelName);
            BuilderOptions& addTerminalExpression(storm::expressions::Expression const& expression, bool value);
            BuilderOptions& addTerminalLabel(std::string const& label, bool value);
            BuilderOptions& setBuildChoiceLabels(bool newValue);
            BuilderOptions& setBuildChoiceOrigins(bool newValue);
            BuilderOptions& setExplorationChecks(bool newValue);
            
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
            
            /// If one of these labels/expressions evaluates to the given bool, the builder can abort the exploration.
            std::vector<std::pair<LabelOrExpression, bool>> terminalStates;
            
            /// A flag indicating whether or not to build choice labels.
            bool buildChoiceLabels;
                         
            // A flag that indicates whether or not to generate the information from which parts of the model specification
            // each choice originates.
            bool buildChoiceOrigins;
            
            /// A flag that stores whether exploration checks are to be performed.
            bool explorationChecks;
        };
        
    }
}
