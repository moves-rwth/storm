#include "src/generator/NextStateGenerator.h"

#include "src/adapters/CarlAdapter.h"

#include "src/logic/Formulas.h"

#include "src/storage/expressions/ExpressionManager.h"
#include "src/storage/expressions/SimpleValuation.h"

#include "src/models/sparse/StateLabeling.h"

#include "src/utility/macros.h"
#include "src/exceptions/InvalidSettingsException.h"

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
            return boost::get<std::string>(labelOrExpression);
        }
        
        bool LabelOrExpression::isExpression() const {
            return labelOrExpression.which() == 1;
        }
        
        storm::expressions::Expression const& LabelOrExpression::getExpression() const {
            return boost::get<storm::expressions::Expression>(labelOrExpression);
        }
        
        NextStateGeneratorOptions::NextStateGeneratorOptions(bool buildAllRewardModels, bool buildAllLabels) : buildAllRewardModels(buildAllRewardModels), buildAllLabels(buildAllLabels), buildChoiceLabels(false) {
            // Intentionally left empty.
        }
        
        NextStateGeneratorOptions::NextStateGeneratorOptions(storm::logic::Formula const& formula) : NextStateGeneratorOptions() {
            this->preserveFormula(formula);
            this->setTerminalStatesFromFormula(formula);
        }
        
        NextStateGeneratorOptions::NextStateGeneratorOptions(std::vector<std::shared_ptr<storm::logic::Formula const>> const& formulas) : NextStateGeneratorOptions() {
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
        
        std::set<std::string> const& NextStateGeneratorOptions::getLabelNames() const {
            return labelNames;
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
        
        bool NextStateGeneratorOptions::isBuildAllRewardModelsSet() const {
            return buildAllRewardModels;
        }
        
        bool NextStateGeneratorOptions::isBuildAllLabelsSet() const {
            return buildAllLabels;
        }

        NextStateGeneratorOptions& NextStateGeneratorOptions::setBuildAllRewardModels() {
            buildAllRewardModels = true;
            return *this;
        }
        
        NextStateGeneratorOptions& NextStateGeneratorOptions::addRewardModel(std::string const& rewardModelName) {
            STORM_LOG_THROW(!buildAllRewardModels, storm::exceptions::InvalidSettingsException, "Cannot add reward model, because all reward models are built anyway.");
            rewardModelNames.emplace_back(rewardModelName);
            return *this;
        }

        NextStateGeneratorOptions& NextStateGeneratorOptions::setBuildAllLabels() {
            buildAllLabels = true;
            return *this;
        }

        NextStateGeneratorOptions& NextStateGeneratorOptions::addLabel(storm::expressions::Expression const& expression) {
            expressionLabels.emplace_back(expression);
            return *this;
        }
        
        NextStateGeneratorOptions& NextStateGeneratorOptions::addLabel(std::string const& labelName) {
            STORM_LOG_THROW(!buildAllLabels, storm::exceptions::InvalidSettingsException, "Cannot add label, because all labels are built anyway.");
            labelNames.insert(labelName);
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
        NextStateGenerator<ValueType, StateType>::NextStateGenerator(storm::expressions::ExpressionManager const& expressionManager, VariableInformation const& variableInformation, NextStateGeneratorOptions const& options) : options(options), expressionManager(expressionManager.getSharedPointer()), variableInformation(variableInformation), evaluator(expressionManager), state(nullptr) {
            // Intentionally left empty.
        }
        
        template<typename ValueType, typename StateType>
        NextStateGenerator<ValueType, StateType>::NextStateGenerator(storm::expressions::ExpressionManager const& expressionManager, NextStateGeneratorOptions const& options) : options(options), expressionManager(expressionManager.getSharedPointer()), variableInformation(), evaluator(expressionManager), state(nullptr) {
            // Intentionally left empty.
        }
        
        template<typename ValueType, typename StateType>
        NextStateGeneratorOptions const& NextStateGenerator<ValueType, StateType>::getOptions() const {
            return options;
        }
        
        template<typename ValueType, typename StateType>
        uint64_t NextStateGenerator<ValueType, StateType>::getStateSize() const {
            return variableInformation.getTotalBitOffset(true);
        }
        
        template<typename ValueType, typename StateType>
        void NextStateGenerator<ValueType, StateType>::load(CompressedState const& state) {
            // Since almost all subsequent operations are based on the evaluator, we load the state into it now.
            unpackStateIntoEvaluator(state, variableInformation, evaluator);
            
            // Also, we need to store a pointer to the state itself, because we need to be able to access it when expanding it.
            this->state = &state;
        }
        
        template<typename ValueType, typename StateType>
        bool NextStateGenerator<ValueType, StateType>::satisfies(storm::expressions::Expression const& expression) const {
            if (expression.isTrue()) {
                return true;
            }
            return evaluator.asBool(expression);
        }
        
        template<typename ValueType, typename StateType>
        storm::models::sparse::StateLabeling NextStateGenerator<ValueType, StateType>::label(storm::storage::BitVectorHashMap<StateType> const& states, std::vector<StateType> const& initialStateIndices, std::vector<StateType> const& deadlockStateIndices, std::vector<std::pair<std::string, storm::expressions::Expression>> labelsAndExpressions) {
            
            for (auto const& expression : this->options.getExpressionLabels()) {
                std::stringstream stream;
                stream << expression;
                labelsAndExpressions.push_back(std::make_pair(stream.str(), expression));
            }
            
            // Make the labels unique.
            std::sort(labelsAndExpressions.begin(), labelsAndExpressions.end(), [] (std::pair<std::string, storm::expressions::Expression> const& a, std::pair<std::string, storm::expressions::Expression> const& b) { return a.first < b.first; } );
            auto it = std::unique(labelsAndExpressions.begin(), labelsAndExpressions.end(), [] (std::pair<std::string, storm::expressions::Expression> const& a, std::pair<std::string, storm::expressions::Expression> const& b) { return a.first == b.first; } );
            labelsAndExpressions.resize(std::distance(labelsAndExpressions.begin(), it));
            
            // Prepare result.
            storm::models::sparse::StateLabeling result(states.size());
            
            // Initialize labeling.
            for (auto const& label : labelsAndExpressions) {
                result.addLabel(label.first);
            }
            for (auto const& stateIndexPair : states) {
                unpackStateIntoEvaluator(stateIndexPair.first, variableInformation, this->evaluator);
                
                for (auto const& label : labelsAndExpressions) {
                    // Add label to state, if the corresponding expression is true.
                    if (evaluator.asBool(label.second)) {
                        result.addLabelToState(label.first, stateIndexPair.second);
                    }
                }
            }
            
            if (!result.containsLabel("init")) {
                // Also label the initial state with the special label "init".
                result.addLabel("init");
                for (auto index : initialStateIndices) {
                    result.addLabelToState("init", index);
                }
            }
            if (!result.containsLabel("deadlock")) {
                result.addLabel("deadlock");
                for (auto index : deadlockStateIndices) {
                    result.addLabelToState("deadlock", index);
                }
            }
            
            return result;
        }
        
        template<typename ValueType, typename StateType>
        storm::expressions::SimpleValuation NextStateGenerator<ValueType, StateType>::toValuation(CompressedState const& state) const {
            return unpackStateIntoValuation(state, variableInformation, *expressionManager);
        }

        template class NextStateGenerator<double>;

#ifdef STORM_HAVE_CARL
        template class NextStateGenerator<storm::RationalNumber>;
        template class NextStateGenerator<storm::RationalFunction>;
#endif
    }
}
