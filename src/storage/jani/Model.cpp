#include "src/storage/jani/Model.h"

#include "src/storage/expressions/ExpressionManager.h"

#include "src/storage/jani/AutomatonComposition.h"
#include "src/storage/jani/ParallelComposition.h"
#include "src/storage/jani/RenameComposition.h"

#include "src/utility/macros.h"
#include "src/exceptions/WrongFormatException.h"

namespace storm {
    namespace jani {
        
        static const std::string SILENT_ACTION_NAME = "";
        
        Model::Model() {
            // Intentionally left empty.
        }
        
        Model::Model(std::string const& name, ModelType const& modelType, uint64_t version, boost::optional<std::shared_ptr<storm::expressions::ExpressionManager>> const& expressionManager) : name(name), modelType(modelType), version(version), composition(nullptr) {
            // Use the provided manager or create a new one.
            if (expressionManager) {
                this->expressionManager = expressionManager.get();
            } else {
                this->expressionManager = std::make_shared<storm::expressions::ExpressionManager>();
            }
            
            // Add a prefined action that represents the silent action.
            silentActionIndex = addAction(storm::jani::Action(SILENT_ACTION_NAME));
        }
        
        uint64_t Model::getJaniVersion() const {
            return version;
        }
        
        ModelType const& Model::getModelType() const {
            return modelType;
        }
        
        std::string const& Model::getName() const {
            return name;
        }
        
        uint64_t Model::addAction(Action const& action) {
            auto it = actionToIndex.find(action.getName());
            STORM_LOG_THROW(it == actionToIndex.end(), storm::exceptions::WrongFormatException, "Action with name '" << action.getName() << "' already exists");
            actionToIndex.emplace(action.getName(), actions.size());
            actions.push_back(action);
            return actions.size() - 1;
        }
        
        Action const& Model::getAction(uint64_t index) const {
            return actions[index];
        }
        
        bool Model::hasAction(std::string const& name) const {
            return actionToIndex.find(name) != actionToIndex.end();
        }
        
        uint64_t Model::getActionIndex(std::string const& name) const {
            auto it = actionToIndex.find(name);
            STORM_LOG_THROW(it != actionToIndex.end(), storm::exceptions::WrongFormatException, "Unable to retrieve index of unknown action '" << name << "'.");
            return it->second;
        }
        
        uint64_t Model::addConstant(Constant const& constant) {
            auto it = constantToIndex.find(constant.getName());
            STORM_LOG_THROW(it == constantToIndex.end(), storm::exceptions::WrongFormatException, "Cannot add constant with name '" << constant.getName() << "', because a constant with that name already exists.");
            constantToIndex.emplace(constant.getName(), constants.size());
            constants.push_back(constant);
            return constants.size() - 1;
        }
        
        void Model::addBooleanVariable(BooleanVariable const& variable) {
            globalVariables.addBooleanVariable(variable);
        }
        
        void Model::addBoundedIntegerVariable(BoundedIntegerVariable const& variable) {
            globalVariables.addBoundedIntegerVariable(variable);
        }
        
        void Model::addUnboundedIntegerVariable(UnboundedIntegerVariable const& variable) {
            globalVariables.addUnboundedIntegerVariable(variable);
        }
        
        VariableSet const& Model::getVariables() const {
            return globalVariables;
        }
        
        storm::expressions::ExpressionManager& Model::getExpressionManager() {
            return *expressionManager;
        }
        
        storm::expressions::ExpressionManager const& Model::getExpressionManager() const {
            return *expressionManager;
        }
        
        uint64_t Model::addAutomaton(Automaton const& automaton) {
            auto it = automatonToIndex.find(automaton.getName());
            STORM_LOG_THROW(it == automatonToIndex.end(), storm::exceptions::WrongFormatException, "Automaton with name '" << automaton.getName() << "' already exists.");
            automatonToIndex.emplace(automaton.getName(), automata.size());
            automata.push_back(automaton);
            return automata.size() - 1;
        }
        
        std::shared_ptr<Composition> Model::getStandardSystemComposition() const {
            std::set<std::string> fullSynchronizationAlphabet = getActionNames(false);
            
            std::shared_ptr<Composition> current;
            current = std::make_shared<AutomatonComposition>(this->automata.front().getName());
            for (uint64_t index = 1; index < automata.size(); ++index) {
                current = std::make_shared<ParallelComposition>(current, std::make_shared<AutomatonComposition>(automata[index].getName()), fullSynchronizationAlphabet);
            }
            return current;
        }
        
        Composition const& Model::getSystemComposition() const {
            return *composition;
        }
        
        std::set<std::string> Model::getActionNames(bool includeSilent) const {
            std::set<std::string> result;
            for (auto const& entry : actionToIndex) {
                if (includeSilent || entry.second != silentActionIndex) {
                    result.insert(entry.first);
                }
            }
            return result;
        }
        
        bool Model::checkValidity(bool logdbg) const {
            // TODO switch to exception based return value.
            
            if (version == 0) {
                if(logdbg) STORM_LOG_DEBUG("Jani version is unspecified");
                return false;
            }
            
            if(modelType == ModelType::UNDEFINED) {
                if(logdbg) STORM_LOG_DEBUG("Model type is unspecified");
                return false;
            }
            
            if(automata.empty()) {
                if(logdbg) STORM_LOG_DEBUG("No automata specified");
                return false;
            }
            // All checks passed.
            return true;
            
        }
        
    }
}