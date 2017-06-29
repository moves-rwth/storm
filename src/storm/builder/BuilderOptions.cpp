#include "storm/builder/BuilderOptions.h"

#include "storm/logic/Formulas.h"

#include "storm/settings/SettingsManager.h"
#include "storm/settings/modules/IOSettings.h"

#include "storm/utility/macros.h"
#include "storm/exceptions/InvalidSettingsException.h"

namespace storm {
    namespace builder {
        
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
            return boost::get<std::string>(labelOrExpression);
        }
        
        bool LabelOrExpression::isExpression() const {
            return labelOrExpression.which() == 1;
        }
        
        storm::expressions::Expression const& LabelOrExpression::getExpression() const {
            return boost::get<storm::expressions::Expression>(labelOrExpression);
        }
        
        BuilderOptions::BuilderOptions(bool buildAllRewardModels, bool buildAllLabels) : buildAllRewardModels(buildAllRewardModels), buildAllLabels(buildAllLabels), buildChoiceLabels(false), buildStateValuations(false), buildChoiceOrigins(false), explorationChecks(false) {
            // Intentionally left empty.
        }
        
        BuilderOptions::BuilderOptions(storm::logic::Formula const& formula) : BuilderOptions() {
            this->preserveFormula(formula);
            this->setTerminalStatesFromFormula(formula);
        }
        
        BuilderOptions::BuilderOptions(std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas) : BuilderOptions() {
            if (!formulas.empty()) {
                for (auto const& formula : formulas) {
                    this->preserveFormula(*formula);
                }
                if (formulas.size() == 1) {
                    this->setTerminalStatesFromFormula(*formulas.front());
                }
            }
            
            explorationChecks = storm::settings::getModule<storm::settings::modules::IOSettings>().isExplorationChecksSet();
        }
        
        void BuilderOptions::preserveFormula(storm::logic::Formula const& formula) {
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
        
        void BuilderOptions::setTerminalStatesFromFormula(storm::logic::Formula const& formula) {
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
        
        std::vector<std::string> const& BuilderOptions::getRewardModelNames() const {
            return rewardModelNames;
        }
        
        std::set<std::string> const& BuilderOptions::getLabelNames() const {
            return labelNames;
        }
        
        std::vector<storm::expressions::Expression> const& BuilderOptions::getExpressionLabels() const {
            return expressionLabels;
        }
        
        std::vector<std::pair<LabelOrExpression, bool>> const& BuilderOptions::getTerminalStates() const {
            return terminalStates;
        }
        
        bool BuilderOptions::hasTerminalStates() const {
            return !terminalStates.empty();
        }
        
        void BuilderOptions::clearTerminalStates() {
            terminalStates.clear();
        }
        
        bool BuilderOptions::isBuildChoiceLabelsSet() const {
            return buildChoiceLabels;
        }
        
       bool BuilderOptions::isBuildStateValuationsSet() const {
            return buildStateValuations;
        }
        
        bool BuilderOptions::isBuildChoiceOriginsSet() const {
            return buildChoiceOrigins;
        }
        
        bool BuilderOptions::isBuildAllRewardModelsSet() const {
            return buildAllRewardModels;
        }
        
        bool BuilderOptions::isBuildAllLabelsSet() const {
            return buildAllLabels;
        }
        
        BuilderOptions& BuilderOptions::setBuildAllRewardModels() {
            buildAllRewardModels = true;
            return *this;
        }

        bool BuilderOptions::isExplorationChecksSet() const {
            return explorationChecks;
        }

        BuilderOptions& BuilderOptions::setExplorationChecks(bool newValue) {
            explorationChecks = newValue;
            return *this;
        }
        
        BuilderOptions& BuilderOptions::addRewardModel(std::string const& rewardModelName) {
            STORM_LOG_THROW(!buildAllRewardModels, storm::exceptions::InvalidSettingsException, "Cannot add reward model, because all reward models are built anyway.");
            rewardModelNames.emplace_back(rewardModelName);
            return *this;
        }
        
        BuilderOptions& BuilderOptions::setBuildAllLabels() {
            buildAllLabels = true;
            return *this;
        }
        
        BuilderOptions& BuilderOptions::addLabel(storm::expressions::Expression const& expression) {
            expressionLabels.emplace_back(expression);
            return *this;
        }
        
        BuilderOptions& BuilderOptions::addLabel(std::string const& labelName) {
            STORM_LOG_THROW(!buildAllLabels, storm::exceptions::InvalidSettingsException, "Cannot add label, because all labels are built anyway.");
            labelNames.insert(labelName);
            return *this;
        }
        
        BuilderOptions& BuilderOptions::addTerminalExpression(storm::expressions::Expression const& expression, bool value) {
            terminalStates.push_back(std::make_pair(LabelOrExpression(expression), value));
            return *this;
        }
        
        BuilderOptions& BuilderOptions::addTerminalLabel(std::string const& label, bool value) {
            terminalStates.push_back(std::make_pair(LabelOrExpression(label), value));
            return *this;
        }
        
        BuilderOptions& BuilderOptions::setBuildChoiceLabels(bool newValue) {
            buildChoiceLabels = newValue;
            return *this;
        }
        
        BuilderOptions& BuilderOptions::setBuildStateValuations(bool newValue) {
            buildStateValuations = newValue;
            return *this;
        }
 
        BuilderOptions& BuilderOptions::setBuildChoiceOrigins(bool newValue) {
            buildChoiceOrigins = newValue;
            return *this;
        }

    }
}
