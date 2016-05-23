#pragma once

#include "src/utility/macros.h"

#include "src/storage/jani/Action.h"
#include "src/storage/jani/ModelType.h"
#include "src/storage/jani/Automaton.h"

namespace storm {
    namespace jani {

        class Model {
        public:
            /*!
             * Creates an empty model with the given type.
             */
            Model(std::string const& name, ModelType const& modelType, uint64_t version = 1);

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
            uint64_t addAction(Action const& act);

            /*!
             *  Does some simple checks to determine whether the model is supported by Prism.
             *  Mainly checks abscence of features the parser supports.
             *
             *  Throws UnsupportedModelException if something is wrong
             */
            // TODO add engine as argument to check this for several engines.
            void checkSupported() const;

             /*!
             *  Checks if the model is valid JANI, which should be verified before any further operations are applied to a model.
             */
             bool checkValidity(bool logdbg = true) const;
        private:
            /// The model name
            std::string name;
            /// The list of automata
            std::vector<Automaton> automata;
            /// A mapping from names to automata indices
            std::unordered_map<std::string, size_t> automatonToIndex;
            /// The list with actions
            std::vector<Action> actions;
            /// A mapping from names to action indices
            std::unordered_map<std::string, uint64_t> actionToIndex;

            /// The type of the model.
            ModelType modelType;
            /// The JANI-version used to specify the model.
            uint64_t version;


        };
    }
}

