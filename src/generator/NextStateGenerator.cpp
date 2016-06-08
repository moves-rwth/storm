#include "src/generator/NextStateGenerator.h"

#include "src/adapters/CarlAdapter.h"

#include "src/logic/Formulas.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidArgumentException.h"

namespace storm {
    namespace generator {
        
        LabelOrExpression::LabelOrExpression(storm::expressions::Expression const& expression) : labelOrExpression(expression) {
            // Intentionally left empty.
        }
        
        LabelOrExpression::LabelOrExpression(std::string const& label) : labelOrExpression(label) {
            // Intentionally left empty.
        }
        
        bool LabelOrExpression::isLabel() const {
            return labelOrExpression.which() == 0;
        }
        
        std::string const& LabelOrExpression::getLabel() const {
            return boost::get<std::string const&>(labelOrExpression);
        }
        
        bool LabelOrExpression::isExpression() const {
            return labelOrExpression.which() == 1;
        }
        
        storm::expressions::Expression const& LabelOrExpression::getExpression() const {
            return boost::get<storm::expressions::Expression const&>(labelOrExpression);
        }
        
        NextStateGeneratorOptions::NextStateGeneratorOptions() : buildChoiceLabels(false) {
            // Intentionally left empty.
        }
        
        NextStateGeneratorOptions::NextStateGeneratorOptions(storm::logic::Formula const& formula) {
            this->preserveFormula(formula);
            this->setTerminalStatesFromFormula(formula);
        }
        
        NextStateGeneratorOptions::NextStateGeneratorOptions(std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas) {
            if (!formulas.empty()) {
                for (auto const& formula : formulas) {
                    this->preserveFormula(*formula);
                }
                if (formulas.size() == 1) {
                    this->setTerminalStatesFromFormula(*formulas.front());
                }
            }
        }
        
        void NextStateGeneratorOptions::preserveFormula(storm::logic::Formula const& formula) {
            // If we already had terminal states, we need to erase them.
            if (hasTerminalStates()) {
                clearTerminalStates();
            }
            
            // Determine the reward models we need to build.
            std::set<std::string> referencedRewardModels = formula.getReferencedRewardModels();
            for (auto const& rewardModelName : referencedRewardModels) {
                rewardModelNames.push_back(rewardModelName);
            }
            
            // Extract all the labels used in the formula.
            std::vector<std::shared_ptr<storm::logic::AtomicLabelFormula const>> atomicLabelFormulas = formula.getAtomicLabelFormulas();
            for (auto const& formula : atomicLabelFormulas) {
                addLabel(formula->getLabel());
            }
            
            // Extract all the expressions used in the formula.
            std::vector<std::shared_ptr<storm::logic::AtomicExpressionFormula const>> atomicExpressionFormulas = formula.getAtomicExpressionFormulas();
            for (auto const& formula : atomicExpressionFormulas) {
                addLabel(formula->getExpression());
            }
        }
        
        void NextStateGeneratorOptions::setTerminalStatesFromFormula(storm::logic::Formula const& formula) {
            if (formula.isAtomicExpressionFormula()) {
                addTerminalExpression(formula.asAtomicExpressionFormula().getExpression(), true);
            } else if (formula.isAtomicLabelFormula()) {
                addTerminalLabel(formula.asAtomicLabelFormula().getLabel(), true);
            } else if (formula.isEventuallyFormula()) {
                storm::logic::Formula const& sub = formula.asEventuallyFormula().getSubformula();
                if (sub.isAtomicExpressionFormula() || sub.isAtomicLabelFormula()) {
                    this->setTerminalStatesFromFormula(sub);
                }
            } else if (formula.isUntilFormula()) {
                storm::logic::Formula const& right = formula.asUntilFormula().getRightSubformula();
                if (right.isAtomicExpressionFormula() || right.isAtomicLabelFormula()) {
                    this->setTerminalStatesFromFormula(right);
                }
                storm::logic::Formula const& left = formula.asUntilFormula().getLeftSubformula();
                if (left.isAtomicExpressionFormula()) {
                    addTerminalExpression(left.asAtomicExpressionFormula().getExpression(), false);
                } else if (left.isAtomicLabelFormula()) {
                    addTerminalLabel(left.asAtomicLabelFormula().getLabel(), false);
                }
            } else if (formula.isProbabilityOperatorFormula()) {
                storm::logic::Formula const& sub = formula.asProbabilityOperatorFormula().getSubformula();
                if (sub.isEventuallyFormula() || sub.isUntilFormula()) {
                    this->setTerminalStatesFromFormula(sub);
                }
            }
        }
        
        std::vector<std::string> const& NextStateGeneratorOptions::getRewardModelNames() const {
            return rewardModelNames;
        }
        
        std::set<std::string> const& NextStateGeneratorOptions::getLabels() const {
            return labels;
        }
        
        std::vector<storm::expressions::Expression> const& NextStateGeneratorOptions::getExpressionLabels() const {
            return expressionLabels;
        }
        
        std::vector<std::pair<LabelOrExpression, bool>> const& NextStateGeneratorOptions::getTerminalStates() const {
            return terminalStates;
        }
        
        bool NextStateGeneratorOptions::hasTerminalStates() const {
            return !terminalStates.empty();
        }
        
        void NextStateGeneratorOptions::clearTerminalStates() {
            terminalStates.clear();
        }
        
        bool NextStateGeneratorOptions::isBuildChoiceLabelsSet() const {
            return buildChoiceLabels;
        }

        NextStateGeneratorOptions& NextStateGeneratorOptions::addRewardModel(std::string const& rewardModelName) {
            rewardModelNames.emplace_back(rewardModelName);
            return *this;
        }
        
        NextStateGeneratorOptions& NextStateGeneratorOptions::addLabel(storm::expressions::Expression const& expression) {
            expressionLabels.emplace_back(expression);
            return *this;
        }
        
        NextStateGeneratorOptions& NextStateGeneratorOptions::addLabel(std::string const& label) {
            labels.insert(label);
            return *this;
        }
        
        NextStateGeneratorOptions& NextStateGeneratorOptions::addTerminalExpression(storm::expressions::Expression const& expression, bool value) {
            terminalStates.push_back(std::make_pair(LabelOrExpression(expression), value));
            return *this;
        }

        NextStateGeneratorOptions& NextStateGeneratorOptions::addTerminalLabel(std::string const& label, bool value) {
            terminalStates.push_back(std::make_pair(LabelOrExpression(label), value));
            return *this;
        }
        
        NextStateGeneratorOptions& NextStateGeneratorOptions::setBuildChoiceLabels(bool newValue) {
            buildChoiceLabels = newValue;
            return *this;
        }
        
        RewardModelInformation::RewardModelInformation(std::string const& name, bool stateRewards, bool stateActionRewards, bool transitionRewards) : name(name), stateRewards(stateRewards), stateActionRewards(stateActionRewards), transitionRewards(transitionRewards) {
            // Intentionally left empty.
        }
        
        std::string const& RewardModelInformation::getName() const {
            return name;
        }
        
        bool RewardModelInformation::hasStateRewards() const {
            return stateRewards;
        }
        
        bool RewardModelInformation::hasStateActionRewards() const {
            return stateActionRewards;
        }
        
        bool RewardModelInformation::hasTransitionRewards() const {
            return transitionRewards;
        }
        
        template<typename ValueType, typename StateType>
        NextStateGenerator<ValueType, StateType>::NextStateGenerator(NextStateGeneratorOptions const& options) : options(options) {
            // Intentionally left empty.
        }
        
        template<typename ValueType, typename StateType>
        std::size_t NextStateGenerator<ValueType, StateType>::getNumberOfRewardModels() const {
            return this->options.getRewardModelNames().size();
        }
        
        template<typename ValueType, typename StateType>
        NextStateGeneratorOptions const& NextStateGenerator<ValueType, StateType>::getOptions() const {
            return options;
        }
        
        template class NextStateGenerator<double>;
        template class NextStateGenerator<storm::RationalFunction>;
        
    }
}