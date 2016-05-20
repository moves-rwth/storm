#pragma once

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
             * Checks whether the model is valid wrt. to the rules of JANI. Note that this does not make any statement
             * about whether we can handle the model.
             */
            bool isValid(bool logdbg = true) const;
            
        private:
            // The type of the model.
            ModelType modelType;
            
            // The JANI-version used to specify the model.
            uint64_t version;

        };
    }
}

