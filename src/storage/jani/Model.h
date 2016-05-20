#pragma once

#include "src/utility/macros.h"

#include "src/storage/jani/ModelType.h"
#include "src/storage/jani/Automaton.h"

namespace storm {
    namespace jani {

        class Model {
        public:
            /*!
             * Creates an empty model with the given type.
             */
            Model(ModelType const& modelType, uint64_t version = 1);
            
            /*!
             *  Does some simple checks to determine whether the model is supported by Prism.
             *  Mainly checks abscence of features the parser supports.
             *
             *  Throws UnsupportedModelException if something is wrong
             */
            // TODO add engine as argument to check this for several engines.
            void checkSupported();

             /*!
             *  Checks if the model is valid JANI, which should be verified before any further operations are applied to a model.
             */
             bool checkValidity(bool logdbg = true);
        private:
            /// The list of automata
            std::vector<Automaton> automata;
            /// A mapping from names to automata indices
            std::map<std::string, size_t> automatonIndex;
            /// The type of the model.
            ModelType modelType;
            /// The JANI-version used to specify the model.
            uint64_t version;
            /// The model name
            std::string name;

        };
    }
}

