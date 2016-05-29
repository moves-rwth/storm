#pragma once

#include <memory>

#include "src/storage/jani/Action.h"
#include "src/storage/jani/ModelType.h"
#include "src/storage/jani/Automaton.h"
#include "src/storage/jani/Constant.h"
#include "src/storage/jani/Composition.h"

namespace storm {
    namespace expressions {
        class ExpressionManager;
    }
    
    namespace jani {
        
        class Exporter;
        
        class Model {
        public:
            friend class Exporter;
            
            /*!
             * Creates an uninitialized model.
             */
            Model();
            
            /*!
             * Creates an empty model with the given type.
             */
            Model(std::string const& name, ModelType const& modelType, uint64_t version = 1, boost::optional<std::shared_ptr<storm::expressions::ExpressionManager>> const& expressionManager = boost::none);
            
            /*!
             * Retrieves the JANI-version of the model.
             */
            uint64_t getJaniVersion() const;
            
            /*!
             * Retrieves the type of the model.
             */
            ModelType const& getModelType() const;
            
            /*!
             * Retrievest the name of the model.
             */
            std::string const& getName() const;
            
            /**
             * Checks whether the model has an action with the given name.
             *
             * @param name The name to look for.
             */
            bool hasAction(std::string const& name) const;
            
            /**
             * Get the index of the action
             * @param the name of the model
             * @return the index of the (unique) action with the given name, undefined if no such action is present.
             */
            uint64_t getActionIndex(std::string const& name) const;
            
            /**
             * Adds an action to the model.
             *
             * @return the index for this action.
             */
            uint64_t addAction(Action const& action);
            
            /*!
             * Retrieves the action with the given index.
             */
            Action const& getAction(uint64_t index) const;
            
            /*!
             * Adds the given constant to the model.
             */
            uint64_t addConstant(Constant const& constant);
            
            /*!
             * Retrieves whether the model has a constant with the given name.
             */
            bool hasConstant(std::string const& name) const;
            
            /*!
             * Retrieves the constants of the model.
             */
            std::vector<Constant> const& getConstants() const;

            /*!
             * Retrieves the constants of the model.
             */
            std::vector<Constant>& getConstants();

            /*!
             * Retrieves the constant with the given name (if any).
             */
            Constant const& getConstant(std::string const& name) const;
            
            /*!
             * Adds the given boolean variable to this model.
             */
            void addBooleanVariable(BooleanVariable const& variable);
            
            /*!
             * Adds the given bounded integer variable to this model.
             */
            void addBoundedIntegerVariable(BoundedIntegerVariable const& variable);
            
            /*!
             * Adds the given unbounded integer variable to this model.
             */
            void addUnboundedIntegerVariable(UnboundedIntegerVariable const& variable);

            /*!
             * Retrieves the variables of this automaton.
             */
            VariableSet& getGlobalVariables();

            /*!
             * Retrieves the variables of this automaton.
             */
            VariableSet const& getGlobalVariables() const;
            
            /*!
             * Retrieves the manager responsible for the expressions in the JANI model.
             */
            storm::expressions::ExpressionManager& getExpressionManager();

            /*!
             * Retrieves the manager responsible for the expressions in the JANI model.
             */
            storm::expressions::ExpressionManager const& getExpressionManager() const;
            
            /*!
             * Adds the given automaton to the automata of this model.
             */
            uint64_t addAutomaton(Automaton const& automaton);
            
            /*!
             * Retrieves the automata of the model.
             */
            std::vector<Automaton>& getAutomata();

            /*!
             * Retrieves the automata of the model.
             */
            std::vector<Automaton> const& getAutomata() const;

            /*!
             * Sets the system composition expression of the JANI model.
             */
            void setSystemComposition(std::shared_ptr<Composition> const& composition);
            
            /*!
             * Gets the system composition as the standard, fully-synchronizing parallel composition.
             */
            std::shared_ptr<Composition> getStandardSystemComposition() const;
            
            /*!
             * Retrieves the system composition expression.
             */
            Composition const& getSystemComposition() const;
            
            /*!
             * Retrieves the set of action names.
             */
            std::set<std::string> getActionNames(bool includeSilent = true) const;
            
            /*!
             * Retrieves the name of the silent action.
             */
            std::string const& getSilentActionName() const;
            
            /*!
             * Defines the undefined constants of the model by the given expressions. The original model is not modified,
             * but instead a new model is created.
             */
            Model defineUndefinedConstants(std::map<storm::expressions::Variable, storm::expressions::Expression> const& constantDefinitions) const;
            
            /*!
             * Retrieves whether the model still has undefined constants.
             */
            bool hasUndefinedConstants() const;
            
            /*!
             * Retrieves all undefined constants of the model.
             */
            std::vector<std::reference_wrapper<Constant const>> getUndefinedConstants() const;
            
            /*!
             * Substitutes all constants in all expressions of the model. The original model is not modified, but
             * instead a new model is created.
             */
            Model substituteConstants() const;
            
            /*!
             *  Checks if the model is valid JANI, which should be verified before any further operations are applied to a model.
             */
            bool checkValidity(bool logdbg = true) const;
            
        private:
            /// The model name.
            std::string name;
            
            /// The type of the model.
            ModelType modelType;
            
            /// The JANI-version used to specify the model.
            uint64_t version;
            
            /// The manager responsible for the expressions in this model.
            std::shared_ptr<storm::expressions::ExpressionManager> expressionManager;
            
            /// The list of actions.
            std::vector<Action> actions;
            
            /// A mapping from names to action indices.
            std::unordered_map<std::string, uint64_t> actionToIndex;
            
            /// The index of the silent action.
            uint64_t silentActionIndex;
            
            /// The constants defined by the model.
            std::vector<Constant> constants;
            
            /// A mapping from names to constants.
            std::unordered_map<std::string, uint64_t> constantToIndex;
            
            /// The global variables of the model.
            VariableSet globalVariables;
            
            /// The list of automata.
            std::vector<Automaton> automata;
            
            /// A mapping from names to automata indices.
            std::unordered_map<std::string, size_t> automatonToIndex;
            
            /// An expression describing how the system is composed of the automata.
            std::shared_ptr<Composition> composition;
        };
    }
}

